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

    //validamos que la direccion del argumento sea el correcto
    check_valid_addr((const void *) f->esp);

    //incializamos arreglo de las llamadas al sistema
    int args[3];

    //puntero para manejar la direccion fisica de memoria
    void * memFisica_ptr;

		//realizamos cases para diferentes tipos de llamadas al sistema
		switch(*(int *) f->esp)
		{
      //llamada al sistema para apagar y detener el so
			case SYS_HALT:
				halt();
				break;

      //llamada del sistema para finalizar programa actual del usuario
			case SYS_EXIT:
      //obtencion de argumentos
        get_stack_arguments(f, &args[0], 1);
        //le pasamos el estado_actual del proceso al exit
				exit(args[0]);
				break;

        //llamada del sistema para ejecucion de un nuevo programa en un nuevo thread
        //hacemos uso de un solo argumento, que es la linea entera de comandos para ejecutar el programa
			case SYS_EXEC:
        //obtencion de argumentos
				get_stack_arguments(f, &args[0], 1);

        //convertimos la direccion de memoria virtual a fisica
        memFisica_ptr = (void *) pagedir_get_page(thread_current()->pagedir, (const void *) args[0]);

        //comprobamos la validez de la direccion
        if (memFisica_ptr == NULL)
        {
          exit(-1);
        }
        args[0] = (int) memFisica_ptr;

        /* Return the result of the exec() function in the eax register. */
				f->eax = exec((const char *) args[0]);
				break;

			case SYS_WAIT:
        /* The first argument is the PID of the child process
           that the current process must wait on. */
				get_stack_arguments(f, &args[0], 1);

        /* Return the result of the wait() function in the eax register. */
				f->eax = wait((pid_t) args[0]);
				break;

			case SYS_CREATE:
        /* The first argument is the name of the file being created,
           and the second argument is the size of the file. */
				get_stack_arguments(f, &args[0], 2);
        check_buffer((void *)args[0], args[1]);

        /* Ensures that converted address is valid. */
        memFisica_ptr = pagedir_get_page(thread_current()->pagedir, (const void *) args[0]);
        if (memFisica_ptr == NULL)
        {
          exit(-1);
        }
        args[0] = (int) memFisica_ptr;

        /* Return the result of the create() function in the eax register. */
        f->eax = create((const char *) args[0], (unsigned) args[1]);
				break;

			case SYS_REMOVE:
        /* The first argument of remove is the file name to be removed. */
        get_stack_arguments(f, &args[0], 1);

        /* Ensures that converted address is valid. */
        memFisica_ptr = pagedir_get_page(thread_current()->pagedir, (const void *) args[0]);
        if (memFisica_ptr == NULL)
        {
          exit(-1);
        }
        args[0] = (int) memFisica_ptr;

        /* Return the result of the remove() function in the eax register. */
        f->eax = remove((const char *) args[0]);
				break;

			case SYS_OPEN:
        /* The first argument is the name of the file to be opened. */
        get_stack_arguments(f, &args[0], 1);

        /* Ensures that converted address is valid. */
        memFisica_ptr = pagedir_get_page(thread_current()->pagedir, (const void *) args[0]);
        if (memFisica_ptr == NULL)
        {
          exit(-1);
        }
        args[0] = (int) memFisica_ptr;

        /* Return the result of the remove() function in the eax register. */
        f->eax = open((const char *) args[0]);

				break;

			case SYS_FILESIZE:
        /* filesize has exactly one stack argument, representing the fd of the file. */
        get_stack_arguments(f, &args[0], 1);

        /* We return file size of the fd to the process. */
        f->eax = filesize(args[0]);
				break;

			case SYS_READ:
        /* Get three arguments off of the stack. The first represents the fd, the second
           represents the buffer, and the third represents the buffer length. */
        get_stack_arguments(f, &args[0], 3);

        /* Make sure the whole buffer is valid. */
        check_buffer((void *)args[1], args[2]);

        /* Ensures that converted address is valid. */
        memFisica_ptr = pagedir_get_page(thread_current()->pagedir, (const void *) args[1]);
        if (memFisica_ptr == NULL)
        {
          exit(-1);
        }
        args[1] = (int) memFisica_ptr;

        /* Return the result of the read() function in the eax register. */
        f->eax = read(args[0], (void *) args[1], (unsigned) args[2]);
				break;

			case SYS_WRITE:
        /* Get three arguments off of the stack. The first represents the fd, the second
           represents the buffer, and the third represents the buffer length. */
        get_stack_arguments(f, &args[0], 3);

        /* Make sure the whole buffer is valid. */
        check_buffer((void *)args[1], args[2]);

        /* Ensures that converted address is valid. */
        memFisica_ptr = pagedir_get_page(thread_current()->pagedir, (const void *) args[1]);
        if (memFisica_ptr == NULL)
        {
          exit(-1);
        }
        args[1] = (int) memFisica_ptr;

        /* Return the result of the write() function in the eax register. */
        f->eax = write(args[0], (const void *) args[1], (unsigned) args[2]);
        break;

			case SYS_SEEK:
        /* Get two arguments off of the stack. The first represents the fd, the second
           represents the position. */
        get_stack_arguments(f, &args[0], 2);

        /* Return the result of the seek() function in the eax register. */
        seek(args[0], (unsigned) args[1]);
        break;

			case SYS_TELL:
        /* tell has exactly one stack argument, representing the fd of the file. */
        get_stack_arguments(f, &args[0], 1);

        /* We return the position of the next byte to read or write in the fd. */
        f->eax = tell(args[0]);
        break;

			case SYS_CLOSE:
        /* close has exactly one stack argument, representing the fd of the file. */
        get_stack_arguments(f, &args[0], 1);

        /* We close the file referenced by the fd. */
        close(args[0]);
				break;

			default:
        /* If an invalid system call was sent, terminate the program. */
				exit(-1);
				break;
		}
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


//funcion encargado de obtener los argumentos que se encuentran en el stack pasados al momento de
//la llamada al sistema
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


//-->METODOS DE LLAMADA DEL SISTEMA<--

//finaliza por completo el SO
void halt (void)
{
	shutdown_power_off();
}


//metodo encargado de finalizar el programa actual del usuario, termina el
//hilo y luego regresa al kernel
void exit (int estado_actual)
{
	thread_current()->exit_estado_actual = estado_actual;
	printf("%s: exit(%d)\n", thread_current()->name, estado_actual);
  thread_exit ();
}


//ejecuta el programa que se le especifica, con el nombre dado
pid_t exec (const char * file)
{
  //si el char es nulo entonces retonra -1 y no hace nada
	if(!file)
	{
		return -1;
	}
  //se da lock al archivo para que no interfieran otros procesos sobre este mismo
  lock_acquire(&lock_filesys);
  //se ejecuta el proceso, se obtiene el id del child para ser colcoado en la cola de threads para ser ejecutado
	pid_t child_tid = process_execute(file);
  lock_release(&lock_filesys);
	return child_tid;
}
//-->METODOS DE LLAMADA DEL SISTEMA<--

