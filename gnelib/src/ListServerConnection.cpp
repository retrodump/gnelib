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
#include "ListServerConnection.h"

//##ModelId=3AE4AA420046
ListServerConnection::ListServerConnection(std::string address) 
: ClientConnection(0, 0, address) {
}

//##ModelId=3AE4AA420078
ListServerConnection::~ListServerConnection() {
}

//##ModelId=3AE4AAE000C8
void ListServerConnection::setGame(std::string gameName) {
}

//##ModelId=3AE4AB3A0000
void ListServerConnection::getGameList() {
}


