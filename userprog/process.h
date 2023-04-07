#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H
#include <unistd.h>

#include "threads/thread.h"
#include "threads/synch.h"

//definicion de pid_t
typedef int pid_t;

#define pid_init  ((pid_t) -2);

pid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);


struct process_control_block{
  pid_t pid;

  const char* file_name;

  struct list_elem child_elem;
  struct thread* parent_thread;

  bool file_name_waiting;
  bool file_name_exited;
  bool file_name_orpahn;

  int32_t exit_code;

  struct semaphore sema_initialization;

  struct semaphore sema_wait;

};

struct file_descriptor{
  int id;
  struct list_elem elem;
  struct file* file;
  struct dir* dir;

}

#endif /* userprog/process.h */
