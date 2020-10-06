/*
Integrantes: 

Comando:

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// ...

int main(int argc, char** argv) {
	int pid;
	//printf("arg: %d\n", atoi(argv[1]));

	if(atoi(argv[1]) > 10) {
		printf("Argumento invalido!\nForneca um argumento menor ou igual a 10.\n");
		exit(1);
	}

	for(int n = 1; n < atoi(argv[1]); n++) {
		pid = fork();
		if(pid == 0) {
			//printf("Processo-filho numero %d\nppid %d -> pid %d\n", n, get_ppid(), get_pid());
			sleep(30);
			exit(0);
		}
	}

	//printf("Processo-pai\npid %d\n", get_pid());
	sleep(30);

	return 0;
}
