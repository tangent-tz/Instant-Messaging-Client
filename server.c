#include "Lets-talk.h"
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond4 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock4 = PTHREAD_MUTEX_INITIALIZER;


void* screen_output_thread(void* receiver_printer)
{
    client_args *param=receiver_printer;
    char* temp=(char *) malloc(MAX*sizeof(char));
    while(1)
    {
        pthread_mutex_lock(&lock4);
        pthread_cond_wait(&cond4, &lock4);
        temp=(char*)List_remove(param->server_info);
        if(temp!=NULL)
        {
            printf("%s\n", temp);
            fflush(stdout);
            pthread_mutex_unlock(&lock4);
            continue;
        }
        pthread_mutex_unlock(&lock4);
    }
    free(temp);
}

char* decryption(char* temp)
{
    int key=10;
    char* str=(char*) malloc(MAX);
    strcpy(str,temp);
    for(int i=0;i<strlen(str); i++)
    {
        if(((str[i]-key)%256)==0 || ((str[i]-key)%256)==10)
            continue;
        else if((str[i])==254){
            str[i]='\n';
            continue;
        }
        else if((str[i])==253){
            str[i]='\0';
            continue;
        }
        else
        str[i]=(str[i]-key)%256;   
    }
    return str;
}

void* server_thread(void* receiver_printer)
{
    client_args *param=receiver_printer;
    struct sockaddr_in my_address;  
    struct sockaddr_in remote_address; 
    socklen_t addrlen = sizeof(remote_address);        /* length of addresses */
    int recvlen;            /* # bytes received */
    int fd;             /* our socket */
    char buf[MAX*sizeof(char)]; /* receive buffer */


    /* create a UDP socket */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("cannot create socket\n");
        return 0;
    }


    /* Set up server address and port */
    memset((char *)&my_address, 0, sizeof(my_address));
    my_address.sin_family = AF_INET;
    /* Server accept connection from any address */
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Generate random port number */
    my_address.sin_port = htons(atoi(param->my_port));

    /* Bind address and socket */
    if (bind(fd, (struct sockaddr *)&my_address, sizeof(my_address)) < 0) 
    {
        perror("bind failed");
        return 0;
    }

    /* now loop, receiving data and printing what we received */
    while(1) 
    {
        pthread_mutex_lock(&lock3);
        recvlen = recvfrom(fd, (char*)buf, 4000, MSG_WAITALL, (struct sockaddr *)&remote_address, &addrlen);
        buf[recvlen] = 0;
       // printf("%d\n",  recvlen);
        strcpy(buf, decryption(buf));

        //Check if !exit command is send
        if(strcmp(buf, "!exit")==0)
        {
            List_add(param->server_info, buf);
            pthread_cond_signal(&cond4);
            close(fd);
            pthread_mutex_unlock(&lock3);
            exit(0);
        }

        //Check if !status command is send
        if(strcmp(buf, "!status")==0)
        {
            sendto(fd, "Online", 6, 0, (struct sockaddr *)&remote_address, addrlen);
            pthread_cond_signal(&cond4);
            pthread_mutex_unlock(&lock3);
            continue;

        }

        List_add(param->server_info, buf);
        fflush(stdout);
        pthread_cond_signal(&cond4);
        pthread_mutex_unlock(&lock3);
    }
}
