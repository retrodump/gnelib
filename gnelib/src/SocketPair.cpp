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
#include "../include/gnelib/SocketPair.h"
#include "../include/gnelib/Address.h"

namespace GNE {

//##ModelId=3BB2348D0348
SocketPair::SocketPair(NLsocket reliable, NLsocket unreliable)
: r(reliable), u(unreliable) {
}

//##ModelId=3BB2349B015E
SocketPair::~SocketPair() {
}

//##ModelId=3BB2351603A2
void SocketPair::disconnect() {
	if (r != NL_INVALID) {
		nlClose(r);
		r = NL_INVALID;
	}
	if (u != NL_INVALID) {
		nlClose(u);
		u = NL_INVALID;
	}
}

//##ModelId=3BB2CB41030C
Address SocketPair::getLocalAddress(bool reliable) const {
  NLaddress ret;
  if (reliable) {
		if (r != NL_INVALID)
			nlGetLocalAddr(r, &ret);
		else
			ret.valid = NL_FALSE;
  } else {
		if (u != NL_INVALID)
			nlGetLocalAddr(u, &ret);
		else
			ret.valid = NL_FALSE;
  }
  return Address(ret);
}

//##ModelId=3BB2CB41037A
Address SocketPair::getRemoteAddress(bool reliable) const {
  NLaddress ret;
  if (reliable) {
		if (r != NL_INVALID)
			nlGetRemoteAddr(r, &ret);
		else
			ret.valid = NL_FALSE;
  } else {
		if (u != NL_INVALID)
			nlGetRemoteAddr(u, &ret);
		else
			ret.valid = NL_FALSE;
  }
  return Address(ret);
}

//##ModelId=3B6B302400CA
int SocketPair::rawRead(bool reliable, const NLbyte* buf, int bufSize) const {
  NLsocket act;
  if (reliable)
    act = r;
  else
    act = u;
	assert(act != NL_INVALID);
  return int(nlRead(act, (NLvoid*)buf, (NLint)bufSize));
}

//##ModelId=3B6B302401D6
int SocketPair::rawWrite(bool reliable, const NLbyte* buf, int bufSize) const {
  NLsocket act;
  if (reliable)
    act = r;
  else
    act = u;
	assert(act != NL_INVALID);
  return int(nlWrite(act, (NLvoid*)buf, (NLint)bufSize));
}

} //Namespace GNE



