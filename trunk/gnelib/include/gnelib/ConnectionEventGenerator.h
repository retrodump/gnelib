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

#include "gneintern.h"
#include "ConditionVariable.h"
#include "Thread.h"

namespace GNE {
class ConnectionEventListener;

/**
 * A class used internally by GNE to generate the events in Connection.  Users
 * of GNE should not need to use or know about this class.  This class uses
 * nlPollGroup to check for events comming in on sockets.
 */
//##ModelId=3B07538002A1
class ConnectionEventGenerator : public Thread {
public:
  //##ModelId=3B07538100B9
  ConnectionEventGenerator();

  //##ModelId=3B07538100BA
  virtual ~ConnectionEventGenerator();

  /**
   * Register a Connection to recieve events generated by this class.
   * @param socket the low-level HawkNL socket for this connection.
   * @param conn the Connection class associated with the socket.
   */
  //##ModelId=3B07538100BD
  void reg(NLsocket socket, ConnectionEventListener* conn);

  /**
   * Removes a Connection.
   * @param socket the low-level HawkNL socket for this connection.
   */
  //##ModelId=3B07538100DD
  void unreg(NLsocket socket);

  /**
   * Tells the event generator to shutdown.  This function is called
   * internally on library cleanup, so you should not call it.
   */
  //##ModelId=3B07538100DF
  void shutDown();

protected:
 /**
   * The thread that listens for events.
   */
  //##ModelId=3B07538100BC
  void run();

private:
  //##ModelId=3AE358F701FE
  NLint group;

  //##ModelId=3AE35A87032A
  std::map<NLsocket, ConnectionEventListener*> connections;

  //##ModelId=3B0167440348
  NLsocket* sockBuf;

  //##ModelId=3B07538100B5
  ConditionVariable mapCtrl;
};

}
#endif /* CONNECTIONEVENTGENERATOR_H_INCLUDED_C51B7986 */



