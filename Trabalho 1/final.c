/*
2020/1
Trabalho 1 de Sistemas Operacionais

Integrantes: 
- Jessica Oliveira - 13/0028983
- Rafael Menegassi - 14/0159355
- Victor Martorelli

Especificacoes de sistema utilizadas para testes:
- Ubuntu 18.04 (Máquina Virtual)
- GCC versao 7.5.0 

Compilação:
gcc final.c

Execução:
./final [NUMERO_DE_PROCESSOS_DESEJADO] &

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
	int pid;

	if(atoi(argv[1]) > 10) {
		printf("\nArgumento invalido!\nForneca um argumento menor ou igual a 10.\n");
		exit(1);
	}

	for(int n = 1; n < atoi(argv[1]); n++) {
		pid = fork();
		if(pid == 0) {
			printf("\nProcesso-filho numero %d\nppid %d -> pid %d\n", n, (int) getppid(), (int) getpid());
			sleep(30);
			exit(0);
		}
	}

	printf("\nProcesso-pai\npid %d\n", (int) getpid());
	sleep(30);

	return 0;
}
