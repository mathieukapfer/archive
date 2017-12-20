#define PRIVATE_SEM_TABLE_SIZE 250

int sem_create_private_table();
int sem_get_public(key_t key);

int sem_remove(int sem_table);

int sem_unlock(int sem_table, int sem_number);
int sem_lock(int sem_table, int sem_number);

int sem_init_value(int sem_table, int sem_number, int val);
