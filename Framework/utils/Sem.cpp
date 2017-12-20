#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <string.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Sem.h"
#include "SemCollection.h"
#include "sem_helper.h"

#include "TraceDebug.h"
#include "stdio.h"

#include <syscall.h>

// for public MUTEX initialisation right
bool Sem::hasRightToInitializePublicMutex = false;

void Sem::AllowPublicSemInitialisation() {
  hasRightToInitializePublicMutex = true;
}

int gettid() {
  int tid = syscall(SYS_gettid);
  return tid;
}

TRACEDEBUG_REGISTER("[KERNEL][SEM]");

#define IPC_ALL 0666

bool Sem::_initialized = false;
const char * Sem::_baseName = "/tmp/Sem";

Sem::Sem(const char *name, bool isPrivate, SemBehavour behavour) :
    _semTable(-1),
    _semNumber(-1),
    _behavour(behavour),
    _mutexCount(0),
    _semOp(NULL),
    _pid_owner(0),
    _tid_owner(0),
    _pid_waiter(0),
    _tid_waiter(0) {

  _name = name;

  Create(isPrivate);

  SemCollection::inst()->registerSem(this, isPrivate);

}

Sem::~Sem() {
  LOG_ENTER_("name: %s, semId: %d", _name, _semNumber );

  // Cleanup public sem if owner !
  // NOTE : private sem create/delete operation is managed by SemTable class
  if ((_isPrivate == false) && hasRightToInitializePublicMutex) {
    int res = 0;
    // operation on public sem
    if (_semTable > 0) {
      res = sem_remove(_semTable);

      if (res < 0) {
        LOG_ERROR("sem_remove error on name: %s, semId: %d", _name, _semTable );
      } else {
        LOG_INFO("sem_remove success on name: %s, semId: %d", _name, _semTable );
      }
    }

    // cleanup public key      
    char File[256];
    res = -1;

    /* Delete the associated file */
    snprintf(File, sizeof(File), "%s/%s", _baseName, _name);
    res = unlink(File);
    LOG_INFO("Delete associated file %s return %d", File, res);

  } else {
    SemTable::GetInstance()->ReleaseSem();
  }

  delete (_semOp);
}

void Sem::CleanupPublicSem() {
  LOG_ENTER_("%s", _name);

}

void Sem::Create(bool isPrivate) {
  key_t key;

  LOG_ENTER_("%s %s", _name, isPrivate?"":"(public)");

  _isPrivate = isPrivate;

  if (isPrivate) {
    // take one sem from private table
    SemTable::GetInstance()->GetSem(_semTable, _semNumber);
    LOG_INFO("Sem::Create: %s=%d, ", _name,  _semNumber);
  } else {
    // init file system stuff
    if (!_initialized) {
      CreateDirectory();
      _initialized = true;
    }

    key = File_To_Key();

    // get public sem
    _semTable = sem_get_public(key);
    _semNumber = 0;
  }

  if (_semTable < 0) {
    LOG_FATAL("Create failed for '%s', %s (errno = %d)", _name, strerror(errno), errno);
    exit(0);
  }
  
  // Init operation linker
  _semOp = new SemOp(_semTable, _semNumber);

  // initialize permission 
  if ((_isPrivate) || ((_isPrivate == false) && hasRightToInitializePublicMutex)) {
    if (sem_init_value(_semTable, _semNumber, 1) < 0) {
      LOG_ERROR("semctl call failed (errno %d)", errno);
    }
  } else {
    LOG_INFO("I have NOT right to initialize public sem");
  }

  // LOG_DEBUG("Create Sem %s - id %d", _name, _sem);

  // BASH : 
  // cmd to get sem info
  //  for id in `ipcs -s | cut -d ' ' -f 2 ` ; do ipcs -si $id; done   
  // cmd to rm all sem
  //  for id in `ipcs |  cut -d ' ' -f 2 ` ; do ipcrm -s$id; done
}

const void Sem::ResetValue() {
  LOG_ENTER();

  if (sem_init_value(_semTable, _semNumber, 0) < 0) {
    LOG_ERROR("semctl call failed (errno %d)", errno);
  }

}
/**
 * 
 * @param checkMutexOwer
 * @return 
 */
const int Sem::Lock(bool checkMutexOwer) {
  int res = 0;
  int tid_waiter = 0;
  int pid_waiter = 0;

  // use local variable to be thread safe
  tid_waiter = (int) gettid();
  pid_waiter = (int) getpid();

  LOG_DEBUG("ENTER Lock sem %s tried by tid:%d id:%d ", _name, tid_waiter, _semNumber);

  if ((tid_waiter == _tid_owner) && (pid_waiter == _pid_owner) && (_behavour == SEM_BEHAVOUR_MUTEX)
      && (checkMutexOwer == false)) {
    LOG_DEBUG("Process pid=%d tid=%d id:%d try to lock again the same MUTEX '%s' (useless)", _pid_owner, _tid_owner, _semNumber, _name);
    _mutexCount++;
    res = 0;
  } else {
    // save in instance attribute for debug purpose
    _tid_waiter = tid_waiter;
    _pid_waiter = pid_waiter;

    // do the sem operation
    res = _semOp->Lock();
    // stored PID and TID
    _tid_owner = (int) gettid();
    _pid_owner = (int) getpid();
    LOG_DEBUG("      Lock sem %s done by tid:%d id:%d", _name, (int) _tid_owner, _semNumber);
  }
  return res;
}

const void Sem::Unlock(bool checkMutexOwer) {
  int tid, pid;

  LOG_DEBUG("ENTER Unlock sem %s", _name);

  // check owner
  // use local variable to be thread safe
  tid = (int) gettid();
  pid = (int) getpid();

  if (_behavour == SEM_BEHAVOUR_MUTEX) {
    if ((tid == _tid_owner) && (pid == _pid_owner)) {
      if (_mutexCount > 0) {
        // do not unlock the MUTEX, but just decrease the counter 
        LOG_DEBUG("Decrease mutex count");
        _mutexCount--;
        return;
      }
    } else if (_isPrivate) {
      // If semaphore is private (same process), do additional check:
      // you are not the owner, some thing is not clear, log it !
      LOG_WARNING("Process pid=%d tid=%d id =%d try to unlock semaphore'%s' it doesn't have. ", pid, tid, _semNumber, _name);

      LOG_INFO("Owner is pid=%d tid=%d id = %d ", _pid_owner, _tid_owner, _semNumber);

    }
  }

  _tid_owner = 0;
  _pid_owner = 0;

  // do the sem operation
  _semOp->Unlock();

}
/**
 * @brief create directory 
 * Check if directory exist and if it's really a directory before creating it
 */
void Sem::CreateDirectory() {
  LOG_ENTER_();

  // check if directory already exist
  struct stat st;
  bool needCreation = false;
  if (stat(_baseName, &st) == 0) {
    // if it's not a real directory
    if ((st.st_mode & S_IFDIR) != 0) {
      LOG_INFO("directory %s already exist ", _baseName);
    } else {
      // else create it
      needCreation = true;
    }
  } else {
    LOG_ERROR("Can not stat directory %s", _baseName);
    needCreation = true;
  }
  // finally create directory
  if (needCreation) {
    // create directory with permission : is a directory + user R/W/X + group R/W/X + other R/W/X
    if (mkdir(_baseName, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO) == 0) {
      LOG_INFO("Creation of directory %s with all rigths :%d", _baseName, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO);
    } else {
      LOG_ERROR("Error while creating directory %s", _baseName);
    }
  }
  LOG_EXIT_("");
}

key_t Sem::File_To_Key() {
  char File[256];
  int Fd;
  snprintf(File, sizeof(File), "%s%s%s", _baseName, "/", _name);
  Fd = open(File, (O_RDWR | O_CREAT), 777);
  if (Fd > 0) close(Fd);
  return ftok(File, 1);
}

void Sem::Monitor() {

  log(" %-15s : (%6d,%6d) - (%6d,%6d) - %s(%d)\n", _name, _pid_owner, _tid_owner, _pid_waiter,
      _tid_waiter, (_behavour == 1 ? "TOKEN" : "MUTEX"), _mutexCount);

}

const char* Sem::GetName() {
  return _name;
}
