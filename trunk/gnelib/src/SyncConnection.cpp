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
#include "../include/gnelib/SyncConnection.h"
#include "../include/gnelib/Error.h"
#include "../include/gnelib/Mutex.h"
#include "../include/gnelib/ConditionVariable.h"
#include "../include/gnelib/Connection.h"
#include "../include/gnelib/ClientConnection.h"
#include "../include/gnelib/PacketStream.h"
#include "../include/gnelib/ConnectionListener.h"
#include "../include/gnelib/Packet.h"
#include "../include/gnelib/RawPacket.h"

namespace GNE {

//##ModelId=3BC3CB1703B6
SyncConnection::SyncConnection(Connection* target)
: currError(Error::NoError), conn(target), onDoneWritingEvent(false) {
  gnedbgo1(2, "Wrapping Connection %x into a SyncConnection.", conn);
  oldListener = conn->getListener();
  conn->setListener(this);
}

//##ModelId=3BC3CB1703B7
SyncConnection::~SyncConnection() throw (Error) {
  release();
}

//##ModelId=3BDB10A502A8
Connection* SyncConnection::getConnection() const {
  return conn;
}

/**
 * \todo Check out possibility of a dynamic cast.
 */
//##ModelId=3BC3CD0902E4
void SyncConnection::open(const Address& dest, int localPort) throw (Error) {
  assert(!isReleased());
  if (((ClientConnection*)conn)->open(dest, localPort))
    throw Error(Error::CouldNotOpenSocket);
}

/**
 * \todo Check out possibility of a dynamic cast.
 */
//##ModelId=3BC3CD090320
void SyncConnection::connect() throw (Error) {
  assert(!isReleased());
  ClientConnection* cli = (ClientConnection*)conn;
  cli->connect();  //If an error occurs, it will be thrown later.
  cli->join();
}

/**
 * \bug possible issue with events occuring after release but before
 *      disconnect.
 */
//##ModelId=3BC3CD6E02BD
void SyncConnection::disconnect() throw (Error) {
  assert(!isReleased());
  release();
  conn->disconnect();
}

//##ModelId=3BDB10A50316
void SyncConnection::release() throw (Error) {
  sync.acquire();
  if (oldListener != NULL) { //if not released
    gnedbgo1(2, "Releasing Connection %x", conn);
    conn->setListener(oldListener);
    oldListener = NULL;

    //Notify the old listener for onReceive and onDoneWriting if needed.
    if (conn->stream().isNextPacket())
      conn->onReceive();
    if (onDoneWritingEvent)
      conn->onDoneWriting();

    //Notify any receivers there was an error, and set release error if there
    //was no error already, otherwise don't change it.
    if (currError.getCode() == Error::NoError) {
      currError = Error(Error::SyncConnectionReleased);
    } else {
      recvNotify.broadcast();
      sync.release();
      throw currError;
    }
    recvNotify.broadcast();
  }
  sync.release();
}

//##ModelId=3BDB10A50317
bool SyncConnection::isReleased() const {
  return (oldListener == NULL);
}

/**
 * \todo consider timed waits in the future.
 * \todo This probably could be optimized quite a bit -- each packet gets
 *       copied twice!
 */
//##ModelId=3BC3CFE50014
SyncConnection& SyncConnection::operator >> (Packet& packet) throw (Error) {
  checkError();
  assert(!isReleased());
  recvNotify.acquire();
  while (!conn->stream().isNextPacket()) {
    recvNotify.wait();
    //Check if we were woken up due to an error.
    try {
      checkError();
    } catch (Error e) {
      recvNotify.release();
      throw;
    }
  }
  recvNotify.release();

  //Now that we have some data, do the actual receiving.
  Packet* recv = conn->stream().getNextPacket();
  assert(recv != NULL);  //There had better be some data!
  if (recv->getType() != packet.getType()) {
    gnedbgo2(1, "Packet type mismatch.  Got %d, expected %d.",
	     recv->getType(), packet.getType());
    throw Error(Error::PacketTypeMismatch);
  }

  //Copy the packet.
  //The original method was to use operator=() but doing it this way makes
  //things complicated in several ways, including how to overload it, and the
  //fact that you can't copy the children to children w/o another operator.
  //This should be optimized in the future.  This new method will be easier
  //to optimize later to allow better optimizations.
  RawPacket temp;
  recv->writePacket(temp);
  temp.reset();
  NLubyte dummy;
  temp >> dummy; //Skip the ID code.
  packet.readPacket(temp);

  return *this;
}

//##ModelId=3BC3DBB000AA
SyncConnection& SyncConnection::operator << (const Packet& packet) throw (Error) {
  checkError();
  assert(!isReleased());

  onDoneWritingEvent = false;
  conn->stream().writePacket(packet, true);

  return *this;
}

//##ModelId=3BDB10A50353
void SyncConnection::onNewConn(SyncConnection& newConn) {
  //newConn doesn't HAVE to be this, if there was a doubly-wrapped
  //SyncConnection, which can happen if we wrap then call connect (since
  //connect makes a new SyncConnection).
  assert(!isReleased());
  oldListener->onNewConn(newConn);
}

//##ModelId=3BDB10A6000A
void SyncConnection::onConnect(SyncConnection& conn) {
  assert(!isReleased());
  oldListener->onConnect(conn);
}

//##ModelId=3BDB10A60078
void SyncConnection::onConnectFailure(const Error& error) {
  setError(error);
}

//##ModelId=3BDB10A60122
void SyncConnection::onDisconnect() {
  //This should never happen.  An error should occur first, and at that time
  //we are released, and the onDisconnect event should be sent to the
  //original listener.
  assert(false);
}

//##ModelId=3BDB10A60154
void SyncConnection::onError(const Error& error) {
  conn->disconnect();
  setError(error);
}

//##ModelId=3BDB10A601FE
void SyncConnection::onFailure(const Error& error) {
  setError(error);
}

//##ModelId=3BDB10A6029E
void SyncConnection::onReceive() {
  //Acquire sync, so if we are releasing we handle this properly.
  recvNotify.signal();
  sync.acquire();
  //If we are released, then a receive event occured during that event,
  //so we need to pass the event thread to that onReceive.  The mutexes
  //will make sure onReceive is always properly called from async data,
  //and that it is not called multiple times (since there is only a max
  //of one thread per Connection, the mutex here stops multiple
  //onReceive calls).
  if (isReleased())
    conn->getListener()->onReceive();
  sync.release();
}

//##ModelId=3C1081BC015B
void SyncConnection::onDoneWriting() {
  onDoneWritingEvent = true;
}

//##ModelId=3BDB10A6029F
void SyncConnection::checkError() throw (Error) {
  sync.acquire();
  //Check for an error.  If there is, we know it won't change.
  bool err = (currError.getCode() != Error::NoError);
  sync.release();
  if (err)
    throw currError;
}

//##ModelId=3BDB10A602DA
void SyncConnection::setError(const Error& error) {
  try {
    if (!conn->isConnected()) //Release if conn was lost.
      release(); //will throw an error if one was already set.

    sync.acquire();
    currError = error;
    sync.release();
  } catch (Error e) {
    //If there was an error already set, release will throw it, so we don't
    //set the error.
  }
}

} //namespace GNE