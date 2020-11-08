/*
2020/1
Trabalho 2 de Sistemas Operacionais

Integrantes: 
- Jessica Oliveira - 13/0028983
- Rafael Menegassi - 14/0159355
- Victor Martorelli - 16/0085012

Especificacoes de sistema utilizadas para testes:
- Ubuntu 18.04.4 (Máquina Virtual)
- GCC versao 7.5.0 

Compilação:
gcc trabalho2.c -o trabalho2

Execução:
./trabalho2 &

*/

#define KEY 0x9355 // key <- quatro ultimos numeros da matricula do Rafael
#define MAX_INTERACAO 5 // Interacoes desejadas de escrita e leitura de shm

#include<unistd.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>

#include<stdio.h>
#include<stdlib.h>

/* implementacao de semaforos de maneira invertida (P incrementa e V decrementa, justamente pelo valor do semaforo iniciar em 0) */
/* uma maneira alternativa e fazer um "semctl(SET_VAL)" e implementar tal qual o Djikstra */

struct sembuf operacao[2];
int idsem;

int p_sem()
{
     operacao[0].sem_num = 0;
     operacao[0].sem_op = 0;
     operacao[0].sem_flg = 0;
     operacao[1].sem_num = 0;
     operacao[1].sem_op = 1;
     operacao[1].sem_flg = 0;
     if ( semop(idsem, operacao, 2) < 0)
       printf("erro no p=%d\n", errno);
}

int v_sem()
{
     operacao[0].sem_num = 0;
     operacao[0].sem_op = -1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}

/* rotina principal */

int main()
{
    int pid, idshm, estado;
    struct shmid_ds buf;
    int *pshm;

    /* criacao de area de memoria compartilhada */
    if ((idshm = shmget(KEY, sizeof(int), IPC_CREAT|0x1ff)) < 0) 
    {
        printf("\nErro #%d\t", errno);
        perror("Erro ao tentar criar area de memoria compartilhada - ");
        exit(1);
    }

    /* criacao de semaforo */
    if ((idsem = semget(KEY, 1, IPC_CREAT|0x1ff)) < 0)
    {
        printf("\nErro #%d\t", errno);
        perror("\nErro ao tentar criar semaforo - ");
        exit(1);
    }

    /* criacao de processo filho */
    pid = fork();

    for (int i = 1; i < MAX_INTERACAO + 1; i++) // Testar inicialmente com 3 e depois mudar para vinte
    {

        printf("\nentrei for -> pid %d", (int) getpid());

        if (pid == 0)
        {
            /* filho */

            printf("\nentrei if filho");

            int* shmaddr_filho;

            /* attach do filho em area compartilhada */
            pshm = (int *) shmat(idshm, shmaddr_filho, 0); // Ultimo argumento equivale a "O_RDWR"

            if (pshm == (int *)-1) 
            {
                printf("\nErro #%d\t", errno);
                perror("\nErro ao tentar attach - ");
                exit(1);
            }

            //printf("vou escrever\n");

            //p_sem();

            *pshm = i;

            printf("\nprocesso<%d> escreveu %d\n", (int) getpid(), *pshm /* ou i */);

            // TODO: inserir P(sem) (??) para garanir que a leitura sera feita apos a escrita
            
            //v_sem();

            /* detach */

            if(shmdt(shmaddr_filho) < 0)
            {
                printf("\nErro ao tentar o detach!");
                exit(1);
            }

            p_sem();

	        if(i == MAX_INTERACAO)
            {
		        exit(0);
            }
            
        }

        /* pai */

        int* shmaddr_pai;

        /* attach do pai em area compartilhada */
        pshm = (int *) shmat(idshm, shmaddr_pai, 0); // Ultimo argumento equivale a "O_RDWR"

        if (*pshm == -1) 
        {
            printf("\nErro #%d\t", errno);
            perror("\nErro ao tentar attach - ");
            exit(1);
        }

        // TODO: inserir V(sem) (??) para garantir que a leitura sera feita apos a escrita

        v_sem();

        //p_sem();

        printf("\nprocesso<%d> leu %d\n", (int) getpid(), *pshm);

        printf("\n\naaaaaaaaaaaaaaa");

        //v_sem();

        /* detach */

        if(shmdt(shmaddr_pai) < 0)
        {
            printf("\nErro ao tentar o detach!");
            exit(1);
        }

    }

    /* finalizacao */

    // TODO: verificar se e preciso remover area de memoria compartilhada ao finalizar o programa
    //sleep(10);

    if (shmctl(idshm, IPC_RMID, 0) < 0)
    {
        printf("\nErro #%d\t", errno);
        perror("\nErro ao tentar remover area de memoria compartilhada - ");
        exit(1);
    }   

    //wait(&estado); 

    return 0;
   
}
    
