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
#include "Counter.h"

namespace GNE {

//##ModelId=3B07538100EB
Counter::Counter() : count(0) {
}

//##ModelId=3B07538100EC
Counter::~Counter() {
}

//##ModelId=3B07538100EE
void Counter::timerCallback() {
  countSync.acquire();
  count++;
  countSync.release();
}

//##ModelId=3B0753810118
int Counter::getCount() {
	return count;
}

//##ModelId=3B0753810119
void Counter::setCount(int newCount) {
  countSync.acquire();
  count = newCount;
  countSync.release();
}

}

