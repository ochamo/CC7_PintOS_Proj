//librerias necesarias para funcionamiento
#include "devices/shutdown.h"
#include "devices/input.h"
#include "userprog/process.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "lib/kernel/list.h"

#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

//->SETEO DE VARIABLES GLOBALES<-
static void syscall_handler (struct intr_frame *);
struct lock filesys_lock;
static int memread_user (void *src, void *des, size_t bytes);
static int get_user (const uint8_t *uaddr);

void
syscall_init (void) 
{
  //realizamos lock mientras se realizan las llamadas al sistema, para evitar que se interrumpa el proceso
  lock_init (&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

//cuando un thread que tiene el lock hace un acceso de memoria fallido
//comprobamos si posse el lock y luego le damos release si se cumple la condicion
//llamamos al sys_exit y colocamos etiqueta not_reached para facilitar deteccion de errores
static void fail_invalid_access(void) {
  if (lock_held_by_current_thread(&filesys_lock) == true){
    lock_release (&filesys_lock);
  }

  sys_exit (-1);
  NOT_REACHED();
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //instanciamos variable del numero de syscall y luego se le asigna valor con memread_user
  int syscall_number;
  memread_user(f->esp, &syscall_number, sizeof(syscall_number));

  _DEBUG_PRINTF ("[DEBUG] system call, number = %d!\n", syscall_number);


  //thread_current()->current_esp = f->esp;

  switch(syscall_number){
    case SYS_HALT: // 0
    {
      //llamada de metodo especifico
    }

    case SYS_EXIT: // 1
    {
      //llamada de metodo especifico
    }

    case SYS_EXEC: // 2
    {
      //llamada de metodo especifico
    }

    case SYS_WAIT: // 3
    {
      //llamada de metodo especifico
    }

    case SYS_CREATE: // 4
    {
      //llamada de metodo especifico
    }

    case SYS_REMOVE: // 5
    {
      //llamada de metodo especifico
    }

    case SYS_OPEN: // 6
    {
      //llamada de metodo especifico
    }

    case SYS_FILESIZE: // 7
    {
      //llamada de metodo especifico
    }

    case SYS_READ: // 8
    {
      //llamada de metodo especifico
    }

    case SYS_WRITE: // 9
    {
      //llamada de metodo especifico
    }

    case SYS_SEEK: // 10
    {
      //llamada de metodo especifico
    }

    case SYS_TELL: // 11
    {
      //llamada de metodo especifico
    }

    case SYS_CLOSE: // 12
    {
      //llamada de metodo especifico
    }

    default:
      printf("[ERROR] system call %d is unimplemented!\n", syscall_number);


          sys_exit(-1);
          break;

  }





  printf ("system call!\n");
  thread_exit ();
}


//-->INICIO DE DEFINCION DE METODOS PROPIOS DE ARCHIVO<--

//lee byte por byte la direccion de src(direccion del stacl pointer en el momento) para luego concatenarla en dst(syscall_number)
static int
memread_user (void *src, void *dst, size_t bytes)
{
  int value;
  size_t i = 0;
  while(i<bytes){
    value = get_user(src + i);
    if(value == -1){
      fail_invalid_access();
    } else{
      *(char*)(dst + i) = value & 0xff;
    }
    i=i+1;
  }
  return (int)bytes;
}


static int
get_user (const uint8_t *uaddr) {

  if (! ((void*)uaddr < PHYS_BASE)) {
    return -1; //problema con int32
  }


  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
  : "=&a" (result) : "m" (*uaddr));
  return result;
}


void sys_halt(void) {
   
}

void sys_exit(int status) {
  
}

pid_t sys_exec(const char *cmdline) {
  
}

int sys_wait(pid_t pid) {
  
}

bool sys_create(const char* filename, unsigned initial_size) {
 
}

