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
#include "../include/gnelib/ChannelPacket.h"
#include "../include/gnelib/PacketParser.h"
#include "../include/gnelib/RawPacket.h"

namespace GNE {

const int ChannelPacket::ID = 5;

ChannelPacket::ChannelPacket() : channel(0), from(0) {
}

ChannelPacket::ChannelPacket( int channel, int from, const Packet& packet )
: WrapperPacket(ID, &packet), channel((guint8)channel), from((guint8)from) {
}

ChannelPacket::~ChannelPacket() {
}

int ChannelPacket::getChannel() const {
  return channel;
}

void ChannelPacket::setChannel( int chan ) {
  assert( chan >= 0 && chan <= 255 );
  channel = (guint8)chan;
}

int ChannelPacket::getSource() const {
  return from;
}

void ChannelPacket::setSource( int source ) {
  assert( source >= 0 && source <= 255 );
  from = (guint8)source;
}

Packet* ChannelPacket::makeClone() const {
  assert( getData() != NULL );
  return new ChannelPacket( channel, from, *getData() );
}

int ChannelPacket::getSize() const {
  return WrapperPacket::getSize() + RawPacket::getSizeOf( channel ) +
    RawPacket::getSizeOf( from );
}

void ChannelPacket::writePacket(RawPacket& raw) const {
  WrapperPacket::writePacket( raw );
  raw << channel << from;
}

void ChannelPacket::readPacket(RawPacket& raw) {
  WrapperPacket::readPacket( raw );
  raw >> channel >> from;
  assert( getData() != NULL );
}

Packet* ChannelPacket::create() {
  return new ChannelPacket;
}

} //namespace GNE
