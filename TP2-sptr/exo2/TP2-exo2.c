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
#define CHILD_PROCESS_PATH "./fils"

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

void * shm_area;
int arret=0;
void handler(int signum)
{
	if(signum==SIGUSR1){
		arret=1;
	}
}
int main(int argc, char const *argv[])
{
	int pid, father,child;
	char shm_addr[32];
	int exec_flag;
	int shm_descr,i;
	/*sigalarm*/
    	struct sigaction s;
	s.sa_handler=handler;
	sigemptyset (&s.sa_mask);
	s.sa_flags=0;			
	sigaction(SIGUSR1,&s,NULL);
	
	shm_descr = shm_open(Shm_name, O_CREAT | O_RDWR, 0666 );
	if (ftruncate(shm_descr,PAGESIZE)<0)
	{
		perror("ftruncate error");
	}

	if ((shm_area=mmap(0 , PAGESIZE,PROT_READ| PROT_WRITE | PROT_EXEC , MAP_SHARED, shm_descr, (long)0 ))==NULL)
	{
		perror("mmap error");
	}

	sprintf(( char *)shm_area,"Message de papa" );
		printf("Processus père : ecriture de %s\n",( char *)shm_area );

	char father_pid[10];
	father = getpid();
	pid=fork();
	int exe=0;
	
	switch(pid)
	{
		case -1 : 
			perror("fork");
			exit(0);break;
		case 0 : 
			child=getpid();
			father = getppid();
			sprintf(father_pid,"%d",father);

			printf("Processus fils : lecture de  %s \n", (char *)shm_area);
			sprintf((char *)shm_area,"Message du fils avant exec");
			printf("Processus fils : ecriture de %s \n",(char *)shm_area );
			printf("Processus fils : lecture de  %s \n", (char *)shm_area);

			if(close(shm_descr)==-1)
			{
				perror("error close child");
			}
			i=execlp(CHILD_PROCESS_PATH,CHILD_PROCESS_PATH,shm_area,father_pid,NULL);	
			
			if(i==-1){
				
				perror("error execlp");
			}		
			
			kill(father,SIGUSR1);
			break;
		default : 
			while(!arret);
			printf("Processus père : lecture de  %s\n", (char *)shm_area);
			i=shm_unlink(Shm_name);
			i=close(shm_descr);
			i=munmap(shm_area,PAGESIZE);
	}
	
	
	return 0;
}
