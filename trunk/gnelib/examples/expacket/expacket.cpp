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

/**
 * expacket -- Shows how to derive a new type of packet, and create the
 * proper parsing functions interacting with RawPacket to allow GNE to use
 * your packet.
 */

#include "../../src/gnelib.h"
#include <iostream>

using namespace std;

void rawPacketTest();

int main(int argc, char* argv[]) {
  GNE::init(GNE::NO_NET, atexit);
  Console::setTitle("GNE Packet Example");
  
  rawPacketTest();

  return 0;
}

void rawPacketTest() {
  cout << "Now writing a RawPacket..." << endl;
  char block[16] = {'a', 'b', 'a', 'a', 'c', 'a', 'd', 'a', 'e', '1', 'a',
    'a', 'a', '4', 'a', 'a'};
  RawPacket raw(NULL); //same as RawPacket raw().
  raw << (int)56 << "superlala" << (char)124 << (short)26 << (float)12.5 << (double)123.4;
  raw.writeRaw(block, 16);
  
  cout << "Raw length: " << raw.getLength() << endl;
  cout << " should be: " << (sizeof(int) + 10 + sizeof(char) + sizeof(short) +
                             sizeof(float) + sizeof(double) + 16) << endl;

  NLbyte* buffer = new NLbyte[raw.getLength()];
  memcpy(buffer, raw.getData(), raw.getLength());
  RawPacket raw2(buffer);
  cout << "Now reading from the same data..." << endl;

  int a;
  string b;
  char c;
  short d;
  float e;
  double f;
  memset(block, 0, 16);

  raw2 >> a >> b >> c >> d >> e >> f;
  raw2.readRaw(block, 16);

  cout << a << endl << b << endl << (int)c << endl << d << endl << e << endl << f << endl;
  for (int count=0; count<16; count++) {
    cout << block[count] << ' ';
  }
  cout << endl;
  cout << "Data was of length: " << raw2.getLength() << " (should be as above)" << endl;
  cout << "String length: " << b.length() << " (should be 9)" << endl;
}