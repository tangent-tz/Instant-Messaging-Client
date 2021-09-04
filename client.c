#include "Lets-talk.h"
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;


//creates thread for keyboard input
//Param: Struct client type see lets-talk.c for implementation 
void* keyboard_thread(void* keyboard_sender)
{
	client_args *temp=keyboard_sender;
	while(1)
    {
    	pthread_mutex_lock(&lock1);
        char userinput[MAX];
        fgets(userinput, 1024, stdin);
      	strcpy(userinput,encryption(userinput));
      	userinput[strlen(userinput)-1]='\0';
        List_add(temp->client_info, userinput);
        //printf("%s\n",(char*) List_remove(temp->client_info));



        pthread_cond_signal(&cond2);
        pthread_mutex_unlock(&lock1);
    }
}


//Encrypts message before sending over UDP
//Param: char* type
char* encryption(char* temp)
{
    int key=10;
    char* str=(char*) malloc(MAX*sizeof(char));
    strcpy(str,temp);
    for(int i=0;i<strlen(str); i++)
    {	
    	if(((str[i]+key)%256)==0 || ((str[i]+key)%256)==10)
    		continue;
    	else if((str[i])=='\n'){
    		str[i]=254;
    		continue;
    	}
    	else if((str[i])=='\0'){
    		str[i]=253;
    		continue;
    	}
    	else
        str[i]=(str[i]+key)%256;
    }
    return str;
}

//creates client thread that sends over messages to remote server
//Param: Struct client type see lets-talk.c for implementation 
void* client_thread(void* keyboard_sender)
{
	client_args *inputs=keyboard_sender;
	int fd;
	char *temp;
	char *server;
	struct sockaddr_in remote_address;
	struct sockaddr_in my_address;

   	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   	{
		perror("cannot create socket");
		return 0;
	}

	/* bind it to all local addresses and pick any port number */
	memset((char *)&my_address, 0, sizeof(my_address));
	my_address.sin_family = AF_INET;
	my_address.sin_addr.s_addr = htonl(INADDR_ANY);
	my_address.sin_port = htons(0);

	if(!strcmp(inputs->IP, "localhost"))
		server = "127.0.0.1";
	else
		server=inputs->IP;

	if (bind(fd, (struct sockaddr *)&my_address, sizeof(my_address)) < 0) 
	{
		perror("bind failed");
		return 0;
	}
	
	/* fill in the server's address and data */
	memset((char*)&remote_address, 0, sizeof(remote_address));
	remote_address.sin_family = AF_INET;
	remote_address.sin_port = htons(atoi(inputs->remote_port));
	
	if (inet_aton(server, &remote_address.sin_addr)==0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	//Fix STRLEN
	socklen_t slen = sizeof(remote_address); 
	while(1) 
	{
		pthread_mutex_lock(&lock2);
		pthread_cond_wait(&cond2, &lock2);
		temp=List_remove(inputs->client_info);
		if(temp!=NULL)
		{
			sleep(0.5);
			if (sendto(fd, temp, 4000, MSG_WAITALL, (struct sockaddr *)&remote_address, slen)==-1){
				perror("sendto\n");
			}
			if(temp[0]==43 && temp[1]==111 && temp[2]==-126 && temp[3]==115 && temp[4]==126 && temp[5]==0)
			{
					close(fd);
					exit(1);
			}
			//Fix STATUS
			if(temp[0]==43 && temp[1]==125 && temp[2]==126 && temp[3]==107 && temp[4]==126 && temp[5]==127 && temp[6]==125 && temp[7]==0)
			{
				int recvlen;
				char buf[MAX];
				sleep(1);
				recvlen = recvfrom(fd, buf, 10, MSG_DONTWAIT, (struct sockaddr *)&remote_address, &slen);
			  if (recvlen >= 0) {
                        buf[recvlen] = 0;	/* expect a printable string - terminate it */
                        printf("Remote user is %s\n", buf);
                }
                else
                	printf("Remote user is Offline\n");
                
			}
			
		}
		pthread_mutex_unlock(&lock2);
	}
}