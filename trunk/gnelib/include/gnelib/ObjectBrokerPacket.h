#ifndef _OBJECTBROKERPACKET_H_ADGNDF
#define _OBJECTBROKERPACKET_H_ADGNDF

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

#include "WrapperPacket.h"
#include <boost/shared_ptr.hpp>

namespace GNE {
  
/**
 * The three ObjectBrokerPackets are all identical except for their packet
 * IDs.  This base class abstracts the common functionality.
 */
class ObjectBrokerPacket : public WrapperPacket {
public: //typedefs
  /**
   * Smart, reference counted pointer to an ObjectBrokerPacket.
   * The object pointed to is destroyed when no more reference counted
   * pointers point to the object anymore.
   */
  typedef boost::shared_ptr<ObjectBrokerPacket> pointer;

  /**
   * Smart, weak reference to an ObjectBrokerPacket.
   * Weak references become null when all reference counted pointers to the
   * object dissapear, and the object has been destroyed.
   */
  typedef boost::weak_ptr<ObjectBrokerPacket> weak_pointer;

public:
  /**
   * Initializes a new ObjectBrokerPacket.  The given Packet is copied, if it
   * not NULL.  ObjectCreationPacket and ObjectUpdatePacket require a passed
   * packet, but ObjectDeathPacket does not.
   */
  ObjectBrokerPacket( int packetId, int objectId, const Packet* data );

  /**
   * Initializes a new ObjectBrokerPacket from the given one.
   */
  ObjectBrokerPacket( const ObjectBrokerPacket& o );

  virtual ~ObjectBrokerPacket();

  /**
   * Returns the object ID.
   */
  int getObjectId() const;

  /**
   * Sets the object ID for this packet.
   * @param the new ID, in the range of [0..65535] (inclusive).
   */
  void setObjectId( int newId );

  /**
   * Returns the current size of this packet in bytes.
   */
  virtual int getSize() const;

  /**
   * Writes the packet to the given RawPacket. 
   */
  virtual void writePacket(RawPacket& raw) const;

  /**
   * Reads this packet from the given RawPacket.
   */
  virtual void readPacket(RawPacket& raw);

protected:
  /**
   * This ctor is suitable for use only in the create method.
   */
  ObjectBrokerPacket();

private:
  guint16 objectId;
};

} //namespace GNE

#endif
