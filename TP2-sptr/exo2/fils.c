#include <unistd.h>
#include <mqueue.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#define Shm_name "/Shared_memoire"

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif
void * shm_area;
char shm_addr[32];
int shm_descr;


int main(int argc, char **argv)
{

	void *shm_area;
	int pid_father = atoi(argv[2]);
	shm_area = (void *)atol(argv[1]);
	shm_descr = shm_open(Shm_name, O_RDWR, 0666);
	if (shm_descr == -1)
		perror ("error open ");
	shm_area=mmap(shm_area, PAGESIZE,PROT_READ| PROT_WRITE | PROT_EXEC , MAP_SHARED, shm_descr, (long)0 );
	printf("Processus fils : écriture Message %s  \n",(char *)shm_area);
	
	if(sprintf((char *)shm_area,"Message du fiston après exec")==-1) {
		perror("Error sprintf imqge process");
	}

	if(close(shm_descr)==-1){
		perror("error close imqge process");
	}
	kill(pid_father,SIGUSR1);

}
