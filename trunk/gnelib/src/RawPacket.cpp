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
#include "RawPacket.h"

namespace GNE {

//##ModelId=3B0753810226
const int RawPacket::RAW_PACKET_LEN = 500;

//##ModelId=3B075381022A
RawPacket::RawPacket(NLbyte* buffer) : currLoc(0) {
  if (buffer) {
    data = buffer;
    ourBuffer = false;
  } else {
    data = new NLbyte[RAW_PACKET_LEN];
    ourBuffer = true;
  }
}

//##ModelId=3B075381022C
RawPacket::~RawPacket() {
  if (ourBuffer)
    delete[] data;
}

//##ModelId=3B075381022E
const NLbyte* RawPacket::getData() const {
  return data;
}

//##ModelId=3B0753810230
int RawPacket::getLength() const {
  return currLoc;
}

//##ModelId=3B0753810232
void RawPacket::reset() {
  currLoc = 0;
}

//##ModelId=3B0753810233
void RawPacket::writeRaw(const NLbyte* block, int length) {
  writeBlock(data, currLoc, block, length);
  assert(currLoc <= RAW_PACKET_LEN);
}

//##ModelId=3B0753810236
void RawPacket::readRaw(NLbyte* block, int length) {
  readBlock(data, currLoc, block, length);
  assert(currLoc <= RAW_PACKET_LEN);
}

//START OF WRITING OPERATORS

//##ModelId=3B0753810239
RawPacket& RawPacket::operator << (signed char x) {
  writeByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B075381023B
RawPacket& RawPacket::operator << (NLubyte x) {
  writeByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B075381023D
RawPacket& RawPacket::operator << (NLshort x) {
  writeShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B075381023F
RawPacket& RawPacket::operator << (NLushort x) {
  writeShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810241
RawPacket& RawPacket::operator << (NLint x) {
  writeLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810243
RawPacket& RawPacket::operator << (NLuint x) {
  writeLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810245
RawPacket& RawPacket::operator << (NLfloat x) {
  writeFloat(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810262
RawPacket& RawPacket::operator << (NLdouble x) {
  writeDouble(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//##ModelId=3B0753810264
RawPacket& RawPacket::operator << (const std::string& x) {
  writeString(data, currLoc, x.c_str());
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

//START OF READING OPERATORS

RawPacket& RawPacket::operator >> (signed char& x) {
  readByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (NLubyte& x) {
  readByte(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (NLshort& x) {
  readShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (NLushort& x) {
  readShort(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (NLint& x) {
  readLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (NLuint& x) {
  readLong(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (NLfloat& x) {
  readFloat(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (NLdouble& x) {
  readDouble(data, currLoc, x);
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

RawPacket& RawPacket::operator >> (std::string& x) {
  x = (char*)&data[currLoc];
  currLoc += x.length() + 1;
  assert(currLoc <= RAW_PACKET_LEN);
  return *this;
}

}