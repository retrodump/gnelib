#ifndef THREAD_H_INCLUDED_C51E3746
#define THREAD_H_INCLUDED_C51E3746

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
#include "Mutex.h"

/**
 * A class resembling a thread.
 * Derive your own classes from this class that implement the run function.
 * Execution begins in the run method.  All methods of this class are thread-
 * safe, excluding run(), which should never be called directly by the user.
 * This class is a wrapper for pthreads, so please read man pages or other
 * documentation for the equivalent pthreads functions if you need to
 * understand the specifics on the semantics of these functions.
 * NOTE: All threads must either be detached or joined.  If you choose to
 * detach a thread you may not access that Thread object again.  If you need
 * to access it at a later time, keep a pointer, then at a later time call
 * join on this pointer, then delete at some later time.
 * \todo Priorities are not yet implemented.
 */
//##ModelId=3AE11D2B03DE
class Thread {
public:
  /**
   * Default ctor.
   * Creates the thread ready to be run, but is not yet started.
   * Same as calling Thread(Thread::DEF_NAME, Thread::DEF_PRI)
   */
  //##ModelId=3AE12014000A
  Thread();

  /**
   * Creates a new thread, ready to run but not yet running.
   * @param name2 the name of this thread
   * @param priority2 the priority this thread has
   */
  //##ModelId=3AE1224001CC
  Thread(std::string name2, int priority2);

  //##ModelId=3AE120140046
  virtual ~Thread();

  /**
   * Returns the Thread object that represents this calling thread.
   * @return the currently running thread, or NULL if no Thread object
   *         resembles this thread.
   */
  //##ModelId=3AE126EF00D2
  static Thread* currentThread();

  /**
   * The currently running thread sleeps for the time given in milliseconds.
   */
  //##ModelId=3AEEF76701E0
  static void sleep(int ms);

  /**
   * Returns the name of this thread.
   */
  //##ModelId=3AE126FD00DC
  std::string getName() const;

  /**
   * A blocking call that returns when this thread terminates, or returns
   * immediately if the thread has terminated.  A thread must always either
   * be joined or detached if system resources are to be recovered.  A
   * thread may not be joined after being detached.
   * @see detach
   */
  //##ModelId=3AE1DFB40186
  void join();

  /**
   * Detaches a running thread.  A thread must always either
   * be joined or detached if system resources are to be recovered.  A
   * thread may not be joined after being detached.  After detaching a thread,
   * it is undefined to access it any longer outside of its own actual thread.
   * In other words, at any moment after detach the Thread instance could be
   * be deleted because the thread terminated (or has already terminated).
   * But, of course a running thread could still access itself as it clearly
   * has not stopped and is still running.\n
   * If you care about reading the data from a thread after its completion,
   * do not use detach and instead use join.
   * @see join
   */
  //##ModelId=3AEE3C5D00BE
  void detach();

  /**
   * Returns the detached state of this thread.  Detached threads delete
   * themselves when they die.
   */
  //##ModelId=3AEE3C5D0168
  bool isDetached() const;

  /**
   * This function is the starting point for this thread.
   * Derive your own class from Thread, and implement this function.
   * When this function returns, the thread ends.
   */
  //##ModelId=3AE1DFBE006E
  virtual void run() = 0;

  /**
   * This function is called internally and automatically after run ends and
   * is used for internal library cleanup.  Do not call it directly.
   */
  //##ModelId=3AEE3C5D02E4
  void end();

  /**
   * Determine the running state of the thread.
   */
  //##ModelId=3AEE3C5D0384
  bool isRunning() const;

  /**
   * Returns if the thread has finished running after being started.
   */
  //##ModelId=3AEE3C5E0154
  bool hasEnded() const;

  /**
   * Starts this thread running.
   */
  //##ModelId=3AE1DFC70230
  void start();
  
  /**
   * Returns the priority of this class.
   */
  //##ModelId=3AE20B3402EE
  int getPriority() const;

  /**
   * Returns the underlying pthread_t for this thread.
   */
  //##ModelId=3AE20B3F02E4
  pthread_t getID() const;

  /**
   * The default priority of a thread.
   */
  //##ModelId=3AE1EA0B02E4
  static const int DEF_PRI;

  /**
   * The default name for a thread.
   */
  //##ModelId=3AE1F1CA00DC
  static const std::string DEF_NAME;

  /**
   * A "boosted priority" for a thread.  This is optimized to give some extra
   * priority for the network threads to reduce network latency in favor of a
   * little in-game fps.
   */
  //##ModelId=3AE359B501B8
  static const int HIGH_PRI;

private:
  /**
   * This is an internal-only function called by the underlying pthreads
   * functions -- do not call.
   */
  //##ModelId=3AECE4A00046
  static void remove(Thread*);

  //##ModelId=3AE11D30014A
  std::string name;

  //##ModelId=3AE11D540136
  pthread_t thread_id;

  //##ModelId=3AE11D5A01CC
  bool started;

  //##ModelId=3AEE3C5C01A4
  bool ended;

  //##ModelId=3AEE3C5C02EE
  bool detached;

  //##ModelId=3AE11D5F023A
  static std::map<pthread_t, Thread*> threads;

  //##ModelId=3AE1286000C8
  int priority;

  //##ModelId=3AEE3C5D0051
  Mutex sync;

  //##ModelId=3AEE3C5D008D
  static Mutex mapSync;

};

#endif /* THREAD_H_INCLUDED_C51E3746 */


