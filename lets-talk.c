#include "Lets-talk.h"



int main(int argc, char *argv[])
{
    //Checking if the number of arguments are correct
    if(argc!=4){
        printf("Wrong number of arguments\nMust have three arguments in the form shown below:::\n[my port number] [remote machine name] [remote port number]\n");
        exit(1);
    }

    //Declaring all the variables to be used in main
    pthread_t keyboard, client, server, screen_output;
    List* keyboard_sender=List_create();
    List* receiver_printer=List_create();
    char my_port[100];
    char IP[100];
    char remote_port[100];
    strcpy(my_port, argv[1]);
    strcpy(IP, argv[2]);
    strcpy(remote_port, argv[3]);
    printf("[%s]  [%s]  [%s]\n", my_port,IP,remote_port);

    //Puting the necessary variables into the struct
    client_args C_S_vals;
    C_S_vals.remote_port=remote_port;
    C_S_vals.IP=IP;
    C_S_vals.client_info=keyboard_sender;
    C_S_vals.my_port=my_port;
    C_S_vals.server_info=receiver_printer;

    //Creating threads	
	pthread_create(&keyboard, NULL, keyboard_thread, &C_S_vals);
    sleep(0.1);
	pthread_create(&client, NULL, client_thread, &C_S_vals);
    sleep(0.1);
    pthread_create(&server, NULL, server_thread, &C_S_vals);
    sleep(0.1);
	pthread_create(&screen_output, NULL, screen_output_thread, &C_S_vals);
	
    //Join all the threads
    pthread_join(keyboard, NULL);
	pthread_join(client, NULL);	
	pthread_join(server, NULL);
	pthread_join(screen_output, NULL);
    
	return 0;
}