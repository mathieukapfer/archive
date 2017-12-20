#include "TraceDebug.h"
#ifdef ENABLE_LOG_SERVER
#include "LogServerThread.h"
#endif
#include <unistd.h>

TRACEDEBUG_REGISTER("[MAIN]")

void module1();
void module2();
void module_unknow();

int main(int argc, char **argv) {

#if 0 //def ENABLE_LOG_SERVER
  // Optional code to start tcp log server
  // Log can be catch with netcat on port 1501: 'nc localhost 1501'
  // ============= Begin section =============
  LogServerThread *logServer = LogServerThread::GetInstance();
  logServer->Start();
  // ============= end section  ==============
#endif

  LOG_INFO("start");

  while (1)
    {
    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);

    module1();
    module2();
    module_unknow();

    sleep(1);
  }
}

void module1() {
  {
    TRACEDEBUG_REGISTER("[MOD1]");

    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);
  }

  {
    TRACEDEBUG_REGISTER("[MOD1][SUB1]");

    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);
  }

  {
    TRACEDEBUG_REGISTER("[MOD1][SUB2]");

    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);
  }

}

void module2() {
   {
    TRACEDEBUG_REGISTER("[MOD2]");

    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);
  }

   {
    TRACEDEBUG_REGISTER("[MOD2][SUB1]");

    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);
  }

  {
    TRACEDEBUG_REGISTER("[MOD2][SUB2]");

    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);
  }

}

void module_unknow() {
  {
    TRACEDEBUG_REGISTER("[MOD_]");

    LOG_DEBUG("hello debug - %s",tracedebug_tag);
    LOG_INFO("hello info  - %s",tracedebug_tag);
  }

}
