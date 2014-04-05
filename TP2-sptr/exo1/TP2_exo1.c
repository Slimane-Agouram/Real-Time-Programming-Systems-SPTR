
    #include        <unistd.h>
    #include        <mqueue.h>
    #include        <limits.h>
    #include        <stdlib.h>
    #include        <stdio.h>
    #include        <signal.h>
	#include   		<string.h>
	#include 		<errno.h>
	

   #define MQMESG "Hello"
	#define max_message 100

    int arret = 0;
    int father, child;
    
    void signal_handler(int sig)
    {
        if(sig==SIGALRM)
        {
        	arret =1;
            printf("alarme déclenchée \n ");
            
            
        }
    }



int main(int argc, char const *argv[])
{	
    father= getpid();
    int i,j,taille,pid,count_send=0,count_recieve=0 ; 
    int send1, send2, recieve1, recieve2;
    struct mq_attr mq_attr;
    mqd_t my_mq,my_mq2;
	mqd_t child_mq;
	mq_attr.mq_maxmsg = 1;
	mq_attr.mq_msgsize = 4;
	mq_attr.mq_flags = 0;
	mq_attr.mq_curmsgs = 0;  
	struct sigaction s;
	s.sa_handler=signal_handler;
	sigemptyset (&s.sa_mask);
	s.sa_flags = 0;
	i = mq_unlink("/terminal.0");
	j = mq_unlink("/terminal2.0");
	
	char buffer[2048];
	sigaction(SIGALRM, &s, NULL);
	 printf("donner la taille du message \n");
	 scanf("%d", &taille);
	
	//taille = atoi(argv[1]);
	if(taille>strlen(MQMESG))
	{
	    	mq_attr.mq_msgsize = taille;
	}
	else
	{
		mq_attr.mq_msgsize=strlen(MQMESG);
	}
	
	my_mq = mq_open("/terminal.0", O_CREAT | O_RDWR, 0666, &mq_attr);

	if (my_mq ==(mqd_t) -1 )
	{	
	    perror("mq_open error ");
	}

		my_mq2 = mq_open("/terminal2.0", O_CREAT | O_RDWR, 0666, &mq_attr);
		
	if (my_mq2 ==(mqd_t) -1 )
	{
		
	    perror("mq_open error, second queue ");
	}
	
	
	

    pid=fork();
	alarm(10);
	while(arret != 1)
	{
	if(pid==0)
	{
	    child = getpid();
	    
	    	send1 = mq_send(my_mq2, MQMESG, sizeof(MQMESG), 0);
	    	 if(send1==-1)
	        {
	        	printf("erreur ecriture fils :%d\n",errno );
	        }
	    	count_send++;

	        recieve1 = mq_receive(my_mq, buffer, sizeof(buffer), 0);
	        if(recieve1==-1)
	        {
	        	printf("erreur lecture fils: %d\n",errno );
	        }
	       //printf("le fils a lu : %s\n",buffer );
	        count_recieve++;
	}
	else
	{
	    
	    	send2 = mq_send(my_mq, MQMESG, sizeof(MQMESG), 0);
	    	 if(send2==-1)
	        {
	        	printf("erreur ecriture pere: %d\n",errno );
	        }
	        count_send++;

	    	 recieve2 = mq_receive(my_mq2, buffer, sizeof(buffer), 0);
	    	 if(recieve2==-1)
	        {
	        	printf(" erreur lecture pere :%d\n",errno );
	        }
	   	//printf("le pere a lu : %s\n", buffer );
	        count_recieve++;
	}
	}
	
			if(getpid()==father)
			{
				printf("le pére a envoyé : %d , et reçu : %d  de message de %d \n",count_send, count_recieve,taille );
				printf("la bande passanre pour le pére pour l'envoi est :%f , pour la reception : %f\n",(float)count_send/60, (float)count_recieve/60 );
			}		
			if (getpid()==child)
			{
				printf("le fils a envoyé : %d , et reçu : %d de message de %d \n",count_send, count_recieve,taille );
				printf("la bande passanre pour le fils pour l'envoi est :%f , pour la reception : %f\n",(float)count_send/60, (float)count_recieve/60 );
			}
			sleep(2);
		i = mq_unlink("/terminal.0");
		j = mq_unlink("/terminal2.0");
	
	return 0;
}
