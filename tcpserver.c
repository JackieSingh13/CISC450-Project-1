/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* Sept. 13, 2018 */    

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024   

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 65006

int main(void) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char sentence[STRING_SIZE];  /* receive message */
   char modifiedSentence[STRING_SIZE]; /* send message */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */

   int checking = 0;
   int savings = 0;

   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);
   }

   /* initialize server address information */

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("I am here to listen ... on port %hu\n\n", server_port);

   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n");
         close(sock_server);
         exit(1);
      }

      printf("Connection successful.\n");

  //    for (;;) {
      /* receive the message */
      send(sock_connection, "Specify a command. (chck, depo, with, tran, disc):\n", 51, 0);

      for (;;) {
      bytes_recd = recv(sock_connection, sentence, STRING_SIZE, 0);
      printf("Received message is: %s\n", sentence);

      if (bytes_recd > 0){
	 if (!strncmp("disc", sentence, 4)) {
	   printf("Closing connection.\n");
	   break;
	 }
	 char value[20];
	 if (!strncmp("chck", sentence, 4)) {
		send(sock_connection, "Specify (checking or savings):\n", 32, 0);
		recv(sock_connection, sentence, STRING_SIZE, 0);
		if (!strncmp("savings", sentence, 7)) {
			sprintf(value, "Balance of savings is: %d \n\nEnter new command:\n", savings);
			send(sock_connection, value, strlen(value), 0);
		}
		else if (!strncmp("checking", sentence, 8)) {
			sprintf(value, "Balance of checking is: %d \n\nEnter new command:\n", checking);
			send(sock_connection, value, strlen(value), 0);
		}
		else {
			send(sock_connection, "Invalid", 7, 0);
		}
		continue;
	 }
	 if (!strncmp("depo", sentence, 4)) {
		send(sock_connection, "Specify (checking or savings):\n", 32, 0);
		recv(sock_connection, sentence, STRING_SIZE, 0);
		if (!strncmp("savings", sentence, 7)) {
			sprintf(value, "Balance before transaction: %d \nEnter amount:\n", savings);
                        send(sock_connection, value, strlen(value), 0);
			recv(sock_connection, sentence, 8, 0);
			savings += atoi(sentence);
			sprintf(value, "Balance after: %d \nEnter new command:\n", savings);
			send(sock_connection, value, strlen(value), 0);checking += atoi(sentence);
		}
		else if (!strncmp("checking", sentence, 8)) {
			sprintf(value, "Balance before transaction: %d \nEnter amount:\n", checking);
                        send(sock_connection, value, strlen(value), 0);
                        recv(sock_connection, sentence, 8, 0);
                        checking += atoi(sentence);
                        sprintf(value, "Balance after: %d \nEnter new command:\n", checking);
                        send(sock_connection, value, strlen(value), 0);
		}
		else {
			send(sock_connection, "Invalid", 7, 0);
		}
	 }
	 if (!strncmp("tran", sentence, 4)) {
		send(sock_connection, "Specify destination (checking or saving):\n", 50, 0);
		recv(sock_connection, sentence, STRING_SIZE, 0);
		 if (!strncmp("savings", sentence, 7)) {
                        sprintf(value, "Balance of checking before: %d Balance of savings before: %d \nEnter amount:\n", checking, savings);
                        send(sock_connection, value, strlen(value), 0);
                        recv(sock_connection, sentence, 8, 0);
			if ((checking - atoi(sentence)) >= 0) {
                        	savings += atoi(sentence);
				checking -= atoi(sentence);
                       		sprintf(value, "Balance of checking: %d Balance of savings: %d\nEnter new command:\n", checking, savings);
                        	send(sock_connection, value, strlen(value), 0);
			} else {
				send(sock_connection, "Error: overdraft.\nEnter new command:\n", 50, 0);
			}
                }
                else if (!strncmp("checking", sentence, 8)) {
                        sprintf(value, "Balance of checking before: %d Balance of savings before: %d \nEnter amount:\n", checking, savings);
                        send(sock_connection, value, strlen(value), 0);
                        recv(sock_connection, sentence, 8, 0);
                        if ((savings - atoi(sentence)) >= 0) {
                                savings -= atoi(sentence);
                                checking += atoi(sentence);
                                sprintf(value, "Balance of checking: %d Balance of savings: %d\nEnter new command:\n", checking, savings);
                                send(sock_connection, value, strlen(value), 0);
                        } else {
                                send(sock_connection, "Error: overdraft.\nEnter new command:\n", 50, 0);
                        }

                }
                else {
                        send(sock_connection, "Invalid", 7, 0);
                }

	 }
	 if (!strncmp("with", sentence, 4)) {

	 }
	}
      }
      /* close the socket */
      close(sock_connection);
   }
}
