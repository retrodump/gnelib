#ifndef _OBJECTDEATHPACKET_H_ASBSD3
#define _OBJECTDEATHPACKET_H_ASBSD3

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

#include "ObjectBrokerPacket.h"

namespace GNE {

/**
 * ObjectDeathPacket is a class used by the ObjectBroker to signify when an
 * object is being removed from its control (usually because it is "dying").
 */
class ObjectDeathPacket : public ObjectBrokerPacket {
public: //typedefs
  /**
   * Smart, reference counted pointer to an ObjectDeathPacket.
   * The object pointed to is destroyed when no more reference counted
   * pointers point to the object anymore.
   */
  typedef boost::shared_ptr<ObjectDeathPacket> pointer;

  /**
   * Smart, weak reference to an ObjectDeathPacket.
   * Weak references become null when all reference counted pointers to the
   * object dissapear, and the object has been destroyed.
   */
  typedef boost::weak_ptr<ObjectDeathPacket> weak_pointer;

public:
  /**
   * Initializes a new ObjectDeathPacket.  The given Packet is copied.
   */
  ObjectDeathPacket( int objectId, const Packet* data );

  virtual ~ObjectDeathPacket();
  
  /**
   * The ID for this type of packet.
   */
  static const int ID;
  
  /**
   * Returns a newly allocated exact copy of this packet.
   */
  virtual Packet* makeClone() const;

  /**
   * Returns a new instance of this class suitable only to call readPacket on.
   */
  static Packet* create();

protected:
  ObjectDeathPacket();
};

} //namespace GNE 

#endif