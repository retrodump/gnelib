#ifndef CONNECTIONEVENTGENERATOR_H_INCLUDED_C51B7986
#define CONNECTIONEVENTGENERATOR_H_INCLUDED_C51B7986

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

#include "ConditionVariable.h"
#include "Thread.h"
#include "SmartPointers.h"

#include <nl.h>
#include <map>

namespace GNE {
class ReceiveEventListener;

/**
 * @ingroup internal
 *
 * A class used internally by GNE to generate the events in Connection.  Users
 * of GNE should not need to use or know about this class.  This class uses
 * nlPollGroup to check for events comming in on sockets.
 */
class ConnectionEventGenerator : public Thread {
protected:
  ConnectionEventGenerator();

public:
  typedef SmartPtr<ConnectionEventGenerator> sptr;
  typedef WeakPtr<ConnectionEventGenerator> wptr;

  /**
   * Creates a new instance of a ConnectionEventGenerator managed by a
   * SmartPtr.
   */
  static sptr create();

  virtual ~ConnectionEventGenerator();

  /**
   * Register a socket to recieve events generated by this class.  If
   * the socket is already registered then this method has no effect (even
   * if the registered listener is NOT the one passed!)
   *
   * @param socket the low-level HawkNL socket for this connection.
   * @param conn the Connection class associated with the socket.
   */
  void reg(NLsocket socket, const SmartPtr<ReceiveEventListener>& listener);

  /**
   * Removes a socket.  If the socket is not registered, then no action takes
   * place.
   *
   * @param socket the low-level HawkNL socket for this connection.
   */
  void unreg(NLsocket socket);

  /**
   * Tells the event generator to shutdown.  This function is called
   * internally on library cleanup, so you should not call it.
   */
  void shutDown();

protected:
 /**
   * The thread that listens for events.
   */
  void run();

private:
  NLint group;

  typedef std::map<NLsocket, SmartPtr<ReceiveEventListener> > ConnectionsMap;
  typedef ConnectionsMap::iterator ConnectionsMapIter;

  ConnectionsMap connections;

  NLsocket* sockBuf;

  ConditionVariable mapCtrl;
};

}
#endif /* CONNECTIONEVENTGENERATOR_H_INCLUDED_C51B7986 */
