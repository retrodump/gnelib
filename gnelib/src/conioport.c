//This code comes with no warranty.  Most of the Linux portion of this code 
//belongs to George foot gfoot@users.sourceforge.net.  The Windows portion
//of this code is public domain.

#include "../include/gnelib/conioport.h"

#ifdef WIN32
// *** WINDOWS API CONSOLE CODE ***

#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

void conio_init (int* enterKey, int* backspaceKey) {
  *enterKey = 13;
  *backspaceKey = 8;
  system("cls");
}
void conio_exit () { system("cls"); }
int conio_kbhit () { return kbhit(); }
int conio_getch () { return getch(); }

void conio_gotoxy (int x, int y) {
  COORD pos = {x, y};
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void conio_getxy(int* x, int* y) {
  CONSOLE_SCREEN_BUFFER_INFO info;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
    *x = info.dwCursorPosition.X;
    *y = info.dwCursorPosition.Y;
  } else {
    *x = *y = -1;
  }
}

//conio_getsize returns x == y == -1 if it cannot get the size
void conio_getsize(int* x, int* y) {
  CONSOLE_SCREEN_BUFFER_INFO info;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
    *x = info.srWindow.Right + 1;
    *y = info.srWindow.Bottom + 1;
  } else {
    *x = *y = -1;
  }
}
//conio_settitle sets a title for the console window, whereever possible,
//else does nothing.
void conio_settitle(const char* title) {
  SetConsoleTitle(title);
}

void conio_putchar(int ch) {
  putchar(ch);
}

int conio_vprintf(const char* format, va_list ap) {
  return vprintf(format, ap);
}

#else
// *** Try to use ncurses ***
//Following insipired by similar code by George Foot.

#include <curses.h>

static int cached = 0, cached_value = ERR;

void conio_init (int* enterKey, int* backspaceKey) {
  initscr();
  cbreak();
  noecho();
  nonl();
  //intrflush (stdscr, FALSE);
  keypad (stdscr, TRUE);
  nodelay (stdscr, TRUE);
  scrollok (stdscr, TRUE);
  *enterKey = KEY_ENTER;
  *backspaceKey = KEY_BACKSPACE;
}

void conio_exit () {
  endwin();
}

int conio_kbhit () {
  if (cached) return 1;
  cached_value = getch();
  if (cached_value != ERR) cached = 1;
  return cached;
}

int conio_getch () {
  if (cached == 0) {
    nodelay (stdscr, FALSE); //turn getch into a blocking call
    cached_value = getch();
    nodelay (stdscr, TRUE);  //return to original mode
  }
  cached = 0;
  //We need to do conversions when KEY_ENTER and KEY_BACKSPACE don't work
  if (cached_value == '\r')
    cached_value = KEY_ENTER;
  else if (cached_value == 127)
    //Some UNIX & LINUX map backspace to ^? (127), like Mandrake.
    cached_value = KEY_BACKSPACE;
  return cached_value;
}

void conio_gotoxy (int x, int y) {
  move (y, x);
}

void conio_getxy(int* x, int* y) {
  getyx(stdscr, *y, *x);
}

//Following functions by Jason Winnebeck

void conio_getsize(int* x, int* y) {
  getmaxyx(stdscr, *y, *x);
}

void conio_settitle(const char* title) {
}

void conio_putchar(int ch) {
  echochar(ch);
}

int conio_vprintf(const char* format, va_list ap) {
  int ret = vw_printw(stdscr, format, ap);
  refresh();
  return ret;
}

#endif




