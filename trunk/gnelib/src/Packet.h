#ifndef PACKET_H_INCLUDED_C51B374A
#define PACKET_H_INCLUDED_C51B374A

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
class RawPacket;

/**
 * The base packet class, used for dealing with all types of packets at a
 * fundamental level.  All types of packets must inherit from this class to
 * be recognized by GNE.  GNE already implmenets some types of packets.\n
 * When you create a new packet you MUST define your own versions of all
 * virtual functions or the program will fail.\n
 * See the example expacket on how to properly derive from a Packet class.
 */
//##ModelId=3AE450C3028A
class Packet {
public:
  /**
   * Constructs a packet with the given ID.  If you pass no ID, the ID for an
   * empty packet is assumed.  Normally sending blank packets are not useful,
   * however.  If you are wanting to use a blank packet for some purpose such
   * as a end-of-data marker or for sending some message that requires no
   * data, it is suggested that you simply derive a class from Packet that
   * adds no data, but has a unique ID so it can be "recognized" easier.
   * @param id a number from 0 to 255.
   */
  //##ModelId=3AE4D49C0168
  Packet(int id = 0);

  //##ModelId=3AE4D49C019A
  virtual ~Packet();

  /**
   * Returns a newly allocated exact copy of this packet.
   */
  //##ModelId=3B0723A6001E
  virtual Packet* makeClone() const;

  /**
   * Returns the type of this packet.
   */
  //##ModelId=3AE4DA920276
  int getType() const;

  /**
   * Returns the max size of this packet in bytes.  When overloading this
   * function, call getSize on the parent class then add the sizes of your
   * additional variables.\n
   * Note that "max size" is important, in case of strings.  Usually when we
   * are getting a packet's size, it is because we are checking to make sure
   * it can fit in the buffer.
   */
  //##ModelId=3B0723A600FA
  virtual int getSize() const;

  /**
   * Writes the packet to the given RawPacket.  You can continue writing more
   * packets to the RawPacket after this function.  You must make sure there
   * is enough space in the RawPacket to fit this new packet.  When
   * overloading this function, call writePacket on the parent class then
   * write your own variables.
   */
  //##ModelId=3B06EDF30302
  virtual void writePacket(RawPacket& raw) const;

  /**
   * Reads this packet from the given RawPacket.  This will overwrite all of
   * the values in this class.  When overloading this function, call
   * readPacket on the parent class then read your own variables.\n
   * Note that the parser has already read the packet type from raw so it can
   * create this packet, so it should not be re-read.  This object already
   * knows its type from creation.  This is only relevant to the implemenation
   * of the actual Packet class, and has no signifiance to derived classes,
   * or to the end user.
   */
  //##ModelId=3B06EE000352
  virtual void readPacket(RawPacket& raw);

  /**
   * The 32-bit timestamp for this packet.  You may use any system for
   * timestamping, for example seconds, milliseconds, or frames.  However,
   * whichever timestamping system you select, you MUST guarantee that a
   * packet that comes after another have a higher timestamp.  This is
   * so that late(obsolete) packets can be dropped, if they should be.
   */
  //##ModelId=3B06F74E00C8
  int timestamp;

private:
  //##ModelId=3AE4D15E021C
  NLubyte type;

};

#endif /* PACKET_H_INCLUDED_C51B374A */