/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@webzone.net)
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
#include "ConnectionEventGenerator.h"
#include "ConnectionEventListener.h"
#include "ConditionVariable.h"
#include "Connection.h"

//##ModelId=3AE359220352
ConnectionEventGenerator::ConnectionEventGenerator() 
: running(true), group(NL_INVALID) {
  group = nlGroupCreate();
  assert(group != NL_INVALID);
  sockBuf = new NLsocket[NL_MAX_GROUP_SOCKETS];
}

//##ModelId=3AE359220384
ConnectionEventGenerator::~ConnectionEventGenerator() {
  nlGroupDestroy(group);
  delete[] sockBuf;
}

//##ModelId=3AE34D7900D2
void ConnectionEventGenerator::run() {
  while (running) {
    mapCtrl.acquire();
    while (connections.empty() && running) {
      mapCtrl.wait();
    }
    if (running) {
      int numsockets = nlPollGroup(group, NL_READ_STATUS, sockBuf, NL_MAX_GROUP_SOCKETS);
      for (numsockets--; numsockets >= 0; numsockets--) {
        connections[sockBuf[numsockets]]->onReceive();
      }
    }
    mapCtrl.release();
  }
}

//##ModelId=3AE4E158023A
void ConnectionEventGenerator::reg(NLsocket socket, ConnectionEventListener* conn) {
  mapCtrl.acquire();
  nlGroupAddSocket(group, socket);
  connections[socket] = conn;
  mapCtrl.release();
  mapCtrl.signal();
}

//##ModelId=3AE4E17D021C
void ConnectionEventGenerator::unreg(NLsocket socket) {
  assert(socket != NL_INVALID);
  assert(connections.find(socket) != connections.end());
  mapCtrl.acquire();
  nlGroupDeleteSocket(group, socket);
  connections.erase(socket);
  mapCtrl.release();
}

//##ModelId=3B00A4B703AC
void ConnectionEventGenerator::shutdown() {
  mapCtrl.signal();
  running = false;
}
