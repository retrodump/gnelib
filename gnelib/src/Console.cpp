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
#include "Console.h"

//Yes I know you don't include code files.  I'm lazy.  Shoot me, OK?
// OWE#$!()*%(!#%!5 THAT WAS A RETORICAL QUESTION!  THAT HURT !$#%(*!%
#include "conioport.h"

//##ModelId=3AF8A1F80226
Mutex Console::outSync;
//##ModelId=3AF8ADDB0280
bool Console::initialized = false;

//##ModelId=3AF8A1E2038E
bool Console::init(int (*atexit_ptr)(void (*func)(void))) {
  if (!initialized) {
    conio_init();
    assert(atexit_ptr);
    atexit_ptr(shutdown);
    initialized = true;
  }
  return false;
}

//##ModelId=3AF8A1ED037A
void Console::shutdown() {
  if (initialized) {
    conio_exit();
    initialized = false;
  }
}

//##ModelId=3AF8A3AE001E
void Console::gotoxy(int x, int y) {
  //outSync MUST be acquired.
  conio_gotoxy(x, y);
}

//##ModelId=3AF8AADE038E
int Console::kbhit() {
  return conio_kbhit();
}

//##ModelId=3AF8AADF0050
int Console::getch() {
  return conio_getch();
}

//##ModelId=3AF8A7EC026C
int Console::mprintf(char* format, ...) {
  va_list arg;
  
  va_start(arg, format);
  outSync.acquire();
  int ret = vprintf(format, arg);
  outSync.release();
  va_end(arg);

  return ret;
}

//##ModelId=3AF8A7ED00AA
int Console::mlprintf(int x, int y, char* format, ...) {
  va_list arg;
  
  va_start(arg, format);
  outSync.acquire();
  conio_gotoxy(x, y);
  int ret = vprintf(format, arg);
  outSync.release();
  va_end(arg);

  return ret;
}
