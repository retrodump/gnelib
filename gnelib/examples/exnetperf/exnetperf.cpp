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

/**
 * exnetperf -- Shows how to use the statistical functions in GNE.  This
 * example should serve well for GNE to measure network performance, as best
 * as is possible with the GNE and HawkNL APIs.  This example uses event-
 * driven networking since that is what is most likely to be used once the
 * game has started, and the method that most likely has the largest impact
 * on performace.  (SyncConnection will most likely be used only while
 * connecting).
 * This example could also be used to test the bandwidth throttling.
 * Right now this example isn't completed yet, but it is getting close.
 */

#include "../../include/gnelib.h"
#include "rndalgor.h"
#include <iostream>
#include "StatsDisplay.h" //Our helper class for displaying the stats.

using namespace std;
using namespace GNE;
using namespace GNE::PacketParser;
using namespace GNE::Console;

void errorExit(const char* error);
int getPort(const char* prompt);
void doServer(int outRate, int inRate, int port);
void doClient(int outRate, int inRate, int port);

/**
 * A small wrapper around rndalgor from the jrnd library, which is not
 * thread-safe.
 */
class Randomizer {
public:
  Randomizer() {
    slongrand(time(NULL));
  }

  ~Randomizer() {}

  int getNum() {
    sync.acquire();
    int ret = longrand();
    sync.release();
    return ret;
  }

private:
  Mutex sync;
};

static StatsDisplay* sDisplay;
static Randomizer rng;

//The PerfTester class works on both the client and the server side.
class PerfTester : public ConnectionListener {
public:
  PerfTester() : packetsIn(0), packetsOut(0), conn(NULL) {}
  ~PerfTester() {}

  void onDisconnect() {
    sDisplay->delConn(conn);
    if (serverSide) {
      //see serverSide declaration for more info
      delete conn;
    }
    delete this;
  }

  void onConnect(SyncConnection& conn2) throw (Error) {
    serverSide = false;
    conn = conn2.getConnection();
    writePackets(); //Send an initial batch of data.
    sDisplay->addConn(conn, &packetsIn, &packetsOut);
  }

  void onNewConn(SyncConnection& conn2) throw (Error) {
    serverSide = true;
    conn = conn2.getConnection();
    writePackets();
    sDisplay->addConn(conn, &packetsIn, &packetsOut);
  }

  void onReceive() {
    //We don't need to do anything to the data we are being sent.  The low-
    //level routines will keep track of the stats for us, so we just delete
    //the packets we get.
    Packet* next = conn->stream().getNextPacket();
    while (next != NULL) {
      delete next;
      packetsIn++;
      next = conn->stream().getNextPacket();
    }
  }

  void onFailure(const Error& error) {
    mlprintf(0, 0, "Socket failure: %s   ", error.toString().c_str());
    //No need to disconnect, this has already happened on a failure.
  }

  void onError(const Error& error) {
    mlprintf(0, 0, "Socket error: %s   ", error.toString().c_str());
    conn->disconnect();//For simplicity we treat even normal errors as fatal.
  }

  void onConnectFailure(const Error& error) {
    mlprintf(0, 0, "Connection to server failed.   ");
    mlprintf(0, 1, "  GNE reported error: %s   ", error.toString().c_str());
  }

  void onDoneWriting() {
    writePackets();
  }

  //Try to send out some more packets of random size with random databytes.
  void writePackets() {
    //Find packetsize in 32-bit words
    //We pick a range of 2 to 100 because that is the typical GNE packet
    //size in game.  GNE is meant to combine the packets to optimize
    //bandwith.
    //int packetSize = rng.getNum() % 48 + 2;

    //Big packet test, to represent GNE transmitting files or initial game
    //information.
    int packetSize = CustomPacket::getMaxUserDataSize() / 4 - 1;

    //Create a packet with random data
    CustomPacket temp;
    for (int c = 0; c < packetSize; c++)
      temp.getData() << rng.getNum();

    conn->stream().writePacket(temp, true);
    packetsOut++;
  }

private:
  int packetsIn;
  int packetsOut;
  Connection* conn;

  //We use this because this class can be client or server.  In the client
  //side, the ClientConnection is on the stack and is automatically
  //destroyed but on the server side the ServerConnection is on the heap and
  //needs to be deleted.
  //Normially the recommended way is to have in onDisconnect report to the
  //main thread or some other responsible thread to control the creation or
  //deletion of the listener and connection, and have that thread check and
  //do the deletes, rather than use "delete conn; delete this;."  But in a
  //small program such as this, it is probably acceptable (at least much
  //easier to program!).
  bool serverSide;
};

class OurListener : public ServerConnectionListener {
public:
  OurListener() 
    : ServerConnectionListener() {
  }

  virtual ~OurListener() {}

  void onListenFailure(const Error& error, const Address& from, ConnectionListener* listener) {
    mlprintf(0, 0, "Connection error: %s   ", error.toString().c_str());
    mlprintf(0, 1, "  Error received from %s   ", from.toString().c_str());
    delete listener;
  }

  void getNewConnectionParams(int& inRate, int& outRate, ConnectionListener*& listener) {
    inRate = outRate = 0; //0 meaning no limits on rates.
    listener = new PerfTester();
  }

private:
};

void errorExit(const char* error) {
  shutdownConsole();
  cout << "Error: " << error << endl;
  exit(1);
}

int getPort(const char* prompt) {
  int port;
  gout << "Which port should we " << prompt << ": ";
  gin >> port;
  while ((port < 1) || (port > 65535)) {
    gout << "Ports range from 1 to 65535, please select one in this range: ";
    gin >> port;
  }
  return port;
}

int main() {
  if (initGNE(NL_IP, atexit)) {
    cout << "Unable to initialize GNE" << endl;
    exit(2);
  }
  enableStats();     //By default, stats are disabled.  This turns stat
                     //collection on.
  setUserVersion(1); //Sets our user protocol version number, used in
                     //the connection process by GNE to version check.
  if (initConsole(atexit)) {
    cout << "Unable to initialize GNE Console" << endl;
    exit(3);
  }
  setTitle("GNE Net Performance Tester");
  sDisplay = new StatsDisplay(500);

  gout << "GNE Net Performance Tester for " << GNE::VER_STR << endl;
  gout << "Local address: " << getLocalAddress() << endl;
  gout << "Should we act as the server, or the client?" << endl;
  gout << "Type 1 for client, 2 for server: ";
  int type;
  gin >> type;

  int port;
  if (type != 1) {
    setTitle("GNE Net Performance Tester -- Server");
    gout << "Reminder: ports <= 1024 on UNIX can only be used by the superuser." << endl;
    port = getPort("listen on");
    doServer(0, 0, port);
  } else {
    setTitle("GNE Net Performance Tester -- Client");
    port = getPort("connect to");
    doClient(0, 0, port);
  }

  delete sDisplay;
  return 0;
}

void doServer(int outRate, int inRate, int port) {
#ifdef _DEBUG
  //Generate debugging logs to server.log if in debug mode.
  initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL5, "server.log");
#endif
  OurListener server;
  if (server.open(port))
    errorExit("Cannot open server socket.");
  if (server.listen())
    errorExit("Cannot listen on server socket.");

  gout << "Server is listening on: " << server.getLocalAddress() << endl;
  gout << "Press a key to shutdown server." << endl;
  sDisplay->startDisplay();
  getch();
  //When the server class is destructed, it will stop listening and shutdown.
}

void doClient(int outRate, int inRate, int port) {
#ifdef _DEBUG
  initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL5, "client.log");
#endif
  string host;
  gout << "Enter hostname or IP address: ";
  gin >> host;

  Address address(host);
  address.setPort(port);
  if (!address)
    errorExit("Invalid address.");
  gout << "Connecting to: " << address << endl;
  gout << "Press a key to stop the testing. " << endl;

  ClientConnection client(outRate, inRate, new PerfTester());
  if (client.open(address, 0)) //localPort = 0, for any local port.
    errorExit("Cannot open client socket.");

  client.connect();
  client.join();

  sDisplay->startDisplay();
  getch(); //Wait for user keypress

  client.disconnectSendAll();
}