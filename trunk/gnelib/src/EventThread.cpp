/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@mail.rit.edu)
 * Project website: http://www.rit.edu/~jpw9607/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/EventThread.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/Thread.h"
#include "../include/gnelib/Timer.h"
#include "../include/gnelib/Time.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/ConditionVariable.h"

namespace GNE {

//##ModelId=3C106F0203D4
EventThread::EventThread(ConnectionListener* listener, Connection* conn)
: Thread("EventThr", Thread::HIGH_PRI), ourConn(conn), eventListener(listener),
started(false), onReceiveEvent(false), onTimeoutEvent(false),
onDisconnectEvent(false), onExitEvent(false), failure(NULL) {
  gnedbgo(5, "created");
}

//##ModelId=3C106F0203D6
EventThread::~EventThread() {
  eventSync.acquire();
  while (!eventQueue.empty()) {
    delete eventQueue.front();
    eventQueue.pop();
  }
  eventSync.release();
  delete failure;
  gnedbgo(5, "destroyed");
}

//##ModelId=3C106F0203D8
ConnectionListener* EventThread::getListener() const {
  return eventListener;
}

//##ModelId=3C106F0203DA
void EventThread::setListener(ConnectionListener* listener) {
  //Acquire listenSync to wait for the current event to complete.
  listenSync.acquire();

  //Acquire eventSync because of the wait loop check
  eventSync.acquire();
  eventListener = listener;

  //Signal the event thread in case it is waiting for a listener.
  eventSync.signal();
  eventSync.release();

  listenSync.release();
}

//##ModelId=3CC4E3380110
int EventThread::getTimeout() {
  return (timeout.getuSec() / 1000);
}

//##ModelId=3CC4E338011A
void EventThread::setTimeout(int ms) {
  timeSync.acquire();
  if (ms != 0) {
    timeout = Time(0, ms * 1000);
    nextTimeout = Timer::getAbsoluteTime() + timeout;
  } else {
    nextTimeout = timeout = Time();
  }
  timeSync.release();

  //Wake up the event thread if it is sleeping, which is needed if there is
  //no timeout currently and the event thread is waiting forever on eventSync.
  eventSync.signal();
}

//##ModelId=3C106F0203DC
void EventThread::onDisconnect() {
  gnedbgo(1, "onDisconnect Event triggered.");
  //We acquire the mutex to avoid the possiblity of a deadlock between the
  // test for the shutdown variable and the wait.
  eventSync.acquire();
  onDisconnectEvent = true;
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C70672C0037
void EventThread::onExit() {
  assert(failure == NULL); //only onFailure or onExit can happen.
  gnedbgo(1, "onExit Event triggered.");

  eventSync.acquire();
  onExitEvent = true;
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203DD
void EventThread::onFailure(const Error& error) {
  assert(!onExitEvent); //only onFailure or onExit can happen.
  gnedbgo1(1, "onFailure Event: %s", error.toString().c_str());

  eventSync.acquire();
  failure = new Error(error);
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203DF
void EventThread::onError(const Error& error) {
  gnedbgo1(1, "onError Event: %s", error.toString().c_str());

  eventSync.acquire();
  eventQueue.push(new Error(error));
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203E1
void EventThread::onReceive() {
  gnedbgo(4, "onReceive event triggered.");

  //reset the timeout counter
  resetTimeout();

  eventSync.acquire();
  onReceiveEvent = true;
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203E3
void EventThread::shutDown() {
  Thread::shutDown();

  eventSync.acquire();
  eventSync.signal();
  eventSync.release();
}

//##ModelId=3C106F0203E4
bool EventThread::hasStarted() {
  return started;
}

//##ModelId=3C106F0203E5
void EventThread::start() {
  Thread::start();
  started = true;
}

//##ModelId=3C106F0203E6
void EventThread::run() {
  while (!shutdown) {
    eventSync.acquire();
    //Wait while we have no listener and/or we have no events.
    while (eventListener == NULL || 
           (!onReceiveEvent && !failure
           && !onDisconnectEvent && eventQueue.empty() && !shutdown
           && !onExitEvent && !onTimeoutEvent) ) {
      //Calculate the time to wait
      if ( timeout == Time() ) {
        //wait "forever"
        eventSync.wait();
      } else {
        //wait until a timeout would occur
        eventSync.timedWait(nextTimeout);
        checkForTimeout();
      }
    }
    eventSync.release();
    if (!shutdown) {
      checkForTimeout();

      //Check for events, processing them if events are pending
      if (failure) {
        listenSync.acquire();
        eventListener->onFailure(*failure);
        listenSync.release();
        ourConn->disconnect();
        delete failure;
        failure = NULL;

      } else if (onExitEvent) {
        onExitEvent = false;
        listenSync.acquire();
        eventListener->onExit();
        listenSync.release();
        ourConn->disconnect();

      } else if (onDisconnectEvent) {
        listenSync.acquire();
        eventListener->onDisconnect();
        listenSync.release();
        return;  //terminate this thread since there are no other events to
                 //process -- onDisconnect HAS to be the last.

      } else if (onReceiveEvent) {
        //This is set to false before in case we get more packets during the
        //onReceive event.
        onReceiveEvent = false;
        listenSync.acquire();
        eventListener->onReceive();
        listenSync.release();

      } else if (onTimeoutEvent) {
        onTimeoutEvent = false;
        listenSync.acquire();
        eventListener->onTimeout();
        listenSync.release();

      } else {
        eventSync.acquire();

        //When we get here this is the only reason left why we were woken up!
        assert(!eventQueue.empty());
        Error* e = eventQueue.front();
        listenSync.acquire();
        eventListener->onError(*e);
        listenSync.release();
        delete e;
        eventQueue.pop();

        eventSync.release();
      }
    }
  }
}

//##ModelId=3CC4E3380124
void EventThread::checkForTimeout() {
  timeSync.acquire();
  Time t = nextTimeout;
  timeSync.release();

  if ( Timer::getAbsoluteTime() > t )
    onTimeout();
}

//##ModelId=3CC4E338012E
void EventThread::resetTimeout() {
  timeSync.acquire();
  if ( timeout != Time() ) {
    nextTimeout = Timer::getAbsoluteTime() + timeout;
  }
  timeSync.release();
}

//##ModelId=3CC4E3380138
void EventThread::onTimeout() {
  gnedbgo(4, "onTimeout event triggered.");

  //reset the timeout counter
  resetTimeout();

  eventSync.acquire();
  onTimeoutEvent = true;
  eventSync.signal();
  eventSync.release();
}

} //namespace GNE
