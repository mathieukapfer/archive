#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "sem_helper.h"

#define IPC_ALL 0666

int sem_lock(int sem_table, int sem_number) {

  int res;
  struct sembuf op;

  op.sem_num = sem_number;
  op.sem_op = -1;
  op.sem_flg = 0;

  res = semop(/* Id    */sem_table,
  /* Sops  */&op,
              /* Nsops */1);

  return res;
}

int sem_unlock(int sem_table, int sem_number) {
  struct sembuf op;
  int res;

  op.sem_num = sem_number;
  op.sem_op = 1;
  op.sem_flg = 0;

  res = semop(/* Id    */sem_table,
  /* Sops  */&op,
              /* Nsops */1);

  return res;
}

int sem_remove(int sem_table) {
  int res;
  res = semctl(/* Id */sem_table, /* Num */0, /* Cmd */IPC_RMID);
  return res;
}

int sem_get_public(key_t key) {

  int sem_table;

  /**
   * do not need (IPC_CREAT) | (IPC_EXCL) flags protection,
   * the aim is to use an already defined sem.
   */

  sem_table = semget(/* Key */key,
  /* Num */1,
                     /* Flags */(IPC_CREAT) | (IPC_ALL) |
                     S_IRWXU | S_IRWXG | S_IRWXO);

  return sem_table;
}

int sem_create_private_table() {

  int sem_table;

  /**
   *  (IPC_CREAT) | (IPC_EXCL) flags are usefull to protect from already exist file
   */
  sem_table = semget(/* Key */IPC_PRIVATE,
  /* Num */PRIVATE_SEM_TABLE_SIZE,
                     /* Flags */(IPC_CREAT) | (IPC_EXCL) | (IPC_ALL) |
                     S_IRWXU | S_IRWXG | S_IRWXO);

  return sem_table;
}

int sem_init_value(int sem_table, int sem_number, int val) {
  return semctl(sem_table, sem_number, SETVAL, val);
}

