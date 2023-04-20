//librerias necesarias para funcionamiento
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "threads/init.h"
//importa lo necesario para realizar el shutdown que se usara en halt
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "devices/input.h"
#include "threads/malloc.h"

#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

//->SETEO DE VARIABLES GLOBALES<-
static void syscall_handler (struct intr_frame *);
void get_stack_arguments (struct intr_frame *f, int * args, int num_of_args);
//esta estructura nos permite llevar control de los archivos abiertos por el thread actual
struct thread_file
{
    struct list_elem file_elem;
    struct file *file_addr;
    int file_descriptor;
};
//este lock se encargara de que un proceso entre a la vez a las llamadas del sistema
struct lock lock_filesys;

//-->FIN DE SETEO DE VARIABLES GLOBALES<--


void
syscall_init (void)
{
  //realizamos lock mientras se realizan las llamadas al sistema, para evitar que se interrumpa el proceso
  lock_init (&lock_filesys);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


//funcion que manejara las llamadas del sistema
static void
syscall_handler (struct intr_frame *f UNUSED)
{
  printf ("system call!\n");
  thread_exit ();
}


//-->INICIO DE DEFINCION DE METODOS PROPIOS DE ARCHIVO<--

//metodo encargado de verifcacion si el pointer es valido y no nulo.
//si el puntero no se encuentra en el espacio del usuario realiza un exit y libera los recursos
//tambien verifica si se encuentra dentro de los limites del espacio de la memoria virtual
void check_valid_addr (const void *puntero_check)
{
  if(!is_user_vaddr(puntero_check) || puntero_check == NULL || puntero_check < (void *) 0x08048000)
	{
    exit(-1);
	}
}



/*
   Get up to three arguments from a programs stack (they directly follow the system
   call argument). */

//
void get_stack_arguments (struct intr_frame *f, int *args, int num_of_args)
{
  int i;
  int *ptr;
  for (i = 0; i < num_of_args; i++)
    {
      ptr = (int *) f->esp + i + 1;
      check_valid_addr((const void *) ptr);
      args[i] = *ptr;
    }
}
//-->FINAL DE DEFINCION DE METODOS PROPIOS DE ARCHIVO<--

