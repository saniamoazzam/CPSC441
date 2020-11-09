/*
Name: Sania Moazzam
Master Server - TCP protocol
*/

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>

#define MASTER_PORT 15005
#define ECHO_PORT 12001
#define REVERSE_PORT 12002
#define UPPER_PORT 12003
#define LOWER_PORT 12004
#define CAESAR_PORT 12005
#define YOUR_PORT 12006

#define MAX_WORD_LENGTH 100
#define UDP_IP "127.0.0.1"

using namespace std;

// Global variable
int child_socket;

// This is a signal handler to do graceful exit if needed
void catcher( int sig )
  {
    close(child_socket);
    exit(0);
  }

// Main program for server
int main() {
    // Initialize server sockaddr structure
    struct sockaddr_in master_server;

    static struct sigaction act;
    char send_msg[MAX_WORD_LENGTH], incom_msg[MAX_WORD_LENGTH];

    memset(&master_server, 0, sizeof(master_server));
    master_server.sin_family = AF_INET;
    master_server.sin_port = htons(MASTER_PORT);
    master_server.sin_addr.s_addr = htonl(INADDR_ANY);

    // Set up a signal handler to catch some weird termination conditions
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);

    // Set up the transport-level end point to use TCP
    int master_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (master_socket < 0) {
	     perror("TCP Server: socket() call failed!\n");
	     exit(1);
    }

    // Bind a specific address and port to the end point
    int bind_status = bind(master_socket, (struct sockaddr *)&master_server,
                      sizeof(master_server));
    if (bind_status < 0) {
  	   perror("TCP Server: bind() call failed!\n");
  	   exit(1);
    }

    // Start listening for incoming connections from clients
    if (listen(master_socket, 5) == -1 ) {
	     perror("TCP Server: listen() call failed!\n");
	     exit(1);
    }

    // initialize message strings just to be safe (null-terminated)
    bzero(send_msg, MAX_WORD_LENGTH);
    bzero(incom_msg, MAX_WORD_LENGTH);

    char service[10];
    int bytes, i, pid;

    // Infinite while loop for listening and accepting connections
    while(1) {
      child_socket = accept(master_socket, NULL, NULL);
      if(child_socket == -1) {
        perror("TCP Server: accept() call failed!\n");
        exit(1);
       }

    	 // Create a child process to deal with this new client
    	 pid = fork();

    	 // Use process id (pid) returned by fork to decide what to do next
    	 if( pid < 0 ) {
    	    perror("TCP Server: fork() call failed!\n");
    	    exit(1);
    	 }
    	 else if( pid == 0 ) {
  	     // the child process is the one doing the "then" part
  	     close(master_socket);

  	    while( recv(child_socket, incom_msg, MAX_WORD_LENGTH, 0) > 0 ) {
          bool flag = true;

          if (incom_msg[0] != '\0') {
            cout << "\nChild process received word: " << incom_msg << "\n";
            recv(child_socket, service, 10, 0);

            for (i=0; i<strlen(service); i=i+1) {
                bzero(send_msg, MAX_WORD_LENGTH);

                struct sockaddr_in echo_client;
                struct sockaddr *echo_server;
                int len = sizeof(echo_client);
                //address initialization
              	memset(&echo_client, 0, sizeof(echo_client));
              	echo_client.sin_family = AF_INET; //specifies the address family
                if (service[i] == '1') {
                  echo_client.sin_port = htons(ECHO_PORT); //specificies the port number
                }
                else if (service[i] == '2') {
                  echo_client.sin_port = htons(REVERSE_PORT); //specificies the port number
                }
                else if (service[i] == '3') {
                  echo_client.sin_port = htons(UPPER_PORT); //specificies the port number
                }
                else if (service[i] == '4') {
                  echo_client.sin_port = htons(LOWER_PORT); //specificies the port number
                }
                else if (service[i] == '5') {
                  echo_client.sin_port = htons(CAESAR_PORT); //specificies the port number
                }
                else if (service[i] == '6') {
                  echo_client.sin_port = htons(YOUR_PORT); //specificies the port number
                }

                echo_server = (struct sockaddr *) &echo_client;
              	inet_pton(AF_INET, UDP_IP, &echo_client.sin_addr); //specifies the IP address

                // Socket creation
              	int echo_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
              	// Check if socket creation was successful
              	if (echo_socket == -1){
              		cout << "UDP socket() call failed\n";
              		exit(-1);
              	}

                bytes = sendto(echo_socket, incom_msg, MAX_WORD_LENGTH, 0, echo_server, len);
            		if (bytes == -1){
            			cout << "UDP send() call failed\n";
            			exit(-1);
            		}
                else {
                  cout << "\nSending "<< incom_msg << " to server";
                }

                bytes = recvfrom(echo_socket, send_msg, MAX_WORD_LENGTH, 0, echo_server, (socklen_t*)&len);
            		if (bytes == -1){
            			cout << "UDP recv() call failed\n";
            			exit(-1);
            		}
                sprintf(incom_msg, "%s", send_msg);
              } // Iterate over transformation numbers
              // create the outgoing message (as an ASCII string)
  		        sprintf(send_msg, "%s\n", send_msg);
              send(child_socket, send_msg, strlen(send_msg), 0);

              bzero(service, 10);
      		    bzero(incom_msg, MAX_WORD_LENGTH);
          } // If incom_msg is not NULL
       } // Recv while loop
      } // Else for pid
	    else
	       cout << "\nCreated child process " << pid << " to handle that client";

    } // Infinite while loop
    close(child_socket);
    exit(0);
}// Main func
