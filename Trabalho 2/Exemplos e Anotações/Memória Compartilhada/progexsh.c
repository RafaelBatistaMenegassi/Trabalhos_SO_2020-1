#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
main()
{
   int pid, idshm,  estado;
   struct shmid_ds buf;
   int *pshm;
 
   /* cria memoria*/
   if ((idshm = shmget(0x1223, sizeof(int), IPC_CREAT|0x1ff)) < 0)
   {
     printf("erro na criacao da fila\n");
     exit(1);
   }

   /* cria processo filho */
   pid = fork();
   if (pid == 0)
   {
     /* codigo do filho */
     pshm = (int *) shmat(idshm, (char *)0, 0);
     if (pshm == (int *)-1) 
     {
       printf("erro no attach\n");
       exit(1);
     }
     
     printf("vou escrever\n");
     *pshm = 334;
     exit(0);
   }

   /* codigo do pai */
   pshm = (int *) shmat(idshm, (char *)0, 0);
   if (*pshm == -1) 
   {
      printf("erro no attach\n");
      exit(1);
   }
     
   sleep(1);
   printf("pai - numero lido = %d\n", *pshm);

   wait(&estado);
   exit (0);
   
}
     
