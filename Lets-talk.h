#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include "list.h"
#define MAX 400000



typedef struct client client_args;
struct client 
{
   char* remote_port;
   char* IP;
   List* client_info;
   List* server_info;
   char* my_port;
};

void* keyboard_thread(void* keyboard_sender);
void* screen_output_thread(void* receiver_printer);
void* client_thread(void* keyboard_sender);
void* server_thread(void* receiver_printer);
char* encryption(char* temp);
char* decryption(char* temp);