#ifndef CONSOLE_H_INCLUDED_C507037C
#define CONSOLE_H_INCLUDED_C507037C

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
#include "Mutex.h"

namespace GNE {
/**
 * Functions for providing very basic console support, meant for use in the
 * example and test programs, and in console-only servers using GNE.  These
 * functions's behavior is undefined when running in a Win32 application (not
 * a Win32 console) project, or in any non-console environment.\n
 * Functions that have m prefixed on them mean "multithreaded," because they
 * are thread safe.  Functions with l prefixed on them mean "location,"
 * because they perform their operations starting at the given coordinates
 * rather than the current cursor location.  When one of these functions are
 * called, the cursor is not returned to its original location (so you can't
 * mix mprintf and mlprintf calls well).\n
 * The upper-left coordinate of the screen is (0,0).  The lower-right
 * coordinate of the window is (width-1,height-1).  Use the function
 * getConsoleSize(int*, int*) to find the width and height if possible.
 * Note that the input functions are not thread-safe since there's only one
 * keyboard to be used ;).  It is okay, however, to be using the console
 * output functions at the same time you are using the input functions.
 */
namespace Console {
  /**
   * An ostream that works after the console part of GNE has been initialized.
   * Normally cout doesn't work after the console has been initalized.
   */
  extern std::ostream gout;

  /**
   * Similar thing with gout.
   * @see gout
   */
  extern std::istream gin;

  /**
   * Initalizes the console part of GNE.  This may be called multiple times.
   * Pass it your atexit function so the console will be shutdown
   * automatically on program exit.
   * @return true if the console could not be initalized.
   */
  bool initConsole(int (*atexit_ptr)(void (*func)(void)));

  /**
   * Shuts down the console part of GNE.  This may be called multiple times.
   */
  void shutdownConsole();

  /**
   * Returns non-zero if a key is waiting in the buffer to be received by
   * getch.  You cannot use this call while an lgetstring is being processed.
   * This is the same as kbhit(), but since kbhit is a macro in some cases it
   * messes up this function.
   * @see getch
   * @see lgetString
   */
  int kbhit();

  /**
   * Returns the next character in the input, blocking if no character is
   * ready to be returned.  You cannot use this call while an lgetstring
   * is being processed.
   * @return the next character
   * @see kbhit
   * @see lgetString
   */
  int getch();

  /**
   * Function to sync on printf.
   * @return number of characters written.
   */
  int mprintf(const char* format, ...);
  
  /**
   * Function to sync on printf, and print to a certain location.
   * @return number of characters written.
   * @see mprintf
   */
  int mlprintf(int x, int y, const char* format, ...);

  /**
   * A syncronized version of the ANSI function putchar.
   */
  void mputchar(int ch);

  /**
   * Like mputchar(), but with a specified location.
   */
  void mlputchar(int x, int y, int ch);

  /**
   * Gets input from the console.  This is a blocking call, because you
   * cannot have multiple inputs at the same time, since there is no way for
   * the user to choose which input to go to.  Also when you use this call,
   * no other threads can be using kbhit or getch.  It is recommended that
   * only one thread be in charge of input from the console.  It is okay,
   * however, to be using the console output functions at the same time you
   * are using the input functions.  When the user presses enter the input
   * is complete.
   * @param str a char* with size maxlen+1 where input will be stored.
   * @param maxlen the maximum number of characters the user can input.
   * @return the length of the string returned, from 0 <= x <= maxlen
   * @see kbhit
   * @see getch
   */
  int lgetString(int x, int y, char* str, int maxlen);

  /**
   * Same as lgetString(int, int, char*, int), but uses the current cursor
   * position as obtained from mgetpos for the positions.
   * @see lgetString
   * @see mgetpos
   */
  int getString(char* str, int maxlen);

  /**
   * Sets the title of the console window to the given parameter, where GNE
   * can, and where it makes sense (like Windows).  If the title cannot be
   * set, this function does nothing.
   * @param title the new window title
   */
  void setTitle(const char* title);

  /**
   * Gets the size of the console, in character columns and character rows.
   * If x or y are -1, then the width and height are unable to be determined.
   * The return values for this function may or may not change if the console
   * window is resized.
   * @param width integer where the number of columns is stored.
   * @param height integer where the number of rows is stored.
   */
  void mgetConsoleSize(int* width, int* height);

  /**
   * Returns the current position of the cursor.  As an "m" prefixed function,
   * this is safe to call from multiple threads, but note that if you are
   * using functions that relocate the cursor at the same time this function is
   * called, the "current position" probably will not be very helpful.  It is
   * best used when a single thread is doing console I/O.
   */
  void mgetPos(int* x, int* y);

  /**
   * The keycode the enter key gives from getch().
   * @see getch
   */
  extern int ENTER;

  /**
   * The keycode the backspace key gives from getch().
   * @see getch
   */
  extern int BACKSPACE;
}
}

#endif

