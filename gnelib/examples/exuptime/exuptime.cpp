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

#include "../../src/gnelib.h"

int main() {
  GNE::init(GNE::NO_NET, atexit);
  Console::init(atexit);
  Console::setTitle("GNE Timer Example");
  
  Console::mprintf("The time in seconds, is:\n");
  Console::mprintf("Time is from some arbitrary point in the past.\n");
  Console::mprintf("In Windows, this is actually the system uptime, as it uses the rdtsc\n  instruction.");
  while (!Console::kbhit()) {
    Time t = Timer::getCurrentTime();
    Console::mlprintf(25, 0, "%i.%i  ", t.getSec(), t.getuSec());
  }

  return 0;
}