#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

/**
 * @file   thread.h
 * @author  <matk@guest-linux-debian-6.0.7>
 * @date   Mon Dec 22 14:57:06 2012
 * 
 * @brief  Help to manager posix thread
 *
 *         ========================================================================================================
 *         Typical 'Execute' method that define the thread process may have the form below :
 *         ========================================================================================================
 void Toto::Execute(void* arg) {       
 
 while(1) {                               <--- BEGIN of infinite loop
 
 // wait order                          
 if( aSem.Lock() < 0 || _quit) {        <--- wait event to start (usually based on semaphore notification)
 break;                               <--- IF semaphore is broken OR quit is false THEN leave the loop
 }                                           
 
 // do the job
 MyProcess();                           
 
 }                                        <--- END of infinite loop

 ,----      // exit properly the thread
 |          Exit();                                  <--- exit posix thread (release posix resources)
 |            
 |          LOG_EXIT_("END OF THREAD");
 |        }
 *     |    ========================================================================================================
 *     |    Typically, the descructor of a class that impement 'Thread' has the form below:
 *     |    ========================================================================================================
 |       Toto::~Toto() { 
 |
 |         LOG_ENTER_();
 |
 |         // leave the main loop
 |         stop();                                   <--- order to leave the thread loop a the next activation
 |         wakeUp();                                 <--- re activate to actually lead the loop
 |
 |         // join the main loop thread
 `--->     Join();                                   <--- join the thread and get its return code if any
 <--- BE SURE THE THREAD IS ABLE TO CALL 'Exit' to be definitly block here !
 };                    
 
 */

#include <pthread.h>
#include <string>

using namespace std;

class Thread {
 public:

  // const
  explicit Thread(char *name);
  explicit Thread(string name);

  // dest
  virtual ~Thread();

  // create a new thread with parameter
  int Start(void * arg);

  // create a new thread without parameter
  int Start();

  // join the process parent 
  void Join();

  // optional helper to leave the thread loop 
  virtual void Stop() {
    _quit = true;
  }

  // should be put at the end of thread loop to exit properly 
  void Exit();

  // ! experimental ! 
  // can be called by the descructor of daughter class that handle the process parent 
  void Kill();

 protected:
  virtual void Setup();
  virtual void Execute(void* arg);

  bool _quit;

 private:

  string _name;

  void Run(void * arg);
  static void * EntryPoint(void* arg);
  void Arg(void* a) {
    Arg_ = a;
  }
  pthread_t ThreadId_;
  void * Arg_;
  void * Arg() const {
    return Arg_;
  }
  void _logSyslog(char *format, ...);

};

#endif //THREAD_INCLUDED

