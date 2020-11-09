/*
Name: Sania Moazzam
Tutorial: 01
UCID: 30018572

Reverse Server - UDP protocol
*/

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#define REVERSE_PORT 12002
#define MAX_WORD_LENGTH 100
#define UDP_IP "127.0.0.1"

using namespace std;

int main() {
    // Initialize server sockaddr structure
    struct sockaddr_in echo_server, echo_client;
    struct sockaddr *server, *client;

    char org[MAX_WORD_LENGTH];
    char rev[MAX_WORD_LENGTH];
    int bytes, len = sizeof(echo_client);;

    memset(&echo_server, 0, sizeof(echo_server));
    memset(&echo_client, 0, sizeof(echo_client));

	  echo_server.sin_family = AF_INET; //specifies the address family
	  echo_server.sin_port = htons(REVERSE_PORT); //specificies the port number
    inet_pton(AF_INET, UDP_IP, &echo_server.sin_addr); //specifies the IP address

    server = (struct sockaddr *) &echo_server;
    client = (struct sockaddr *) &echo_client;

    // Socket creation
  	int echo_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  	if (echo_socket == -1) {
  		perror("socket() call failed");
  		exit(-1);
  	}

    // Binding
  	int status = bind(echo_socket, (struct sockaddr *)&echo_server, sizeof(echo_server));
  	if (status == -1) {
  		perror("bind() call failed\n");
  		exit(-1);
  	}

    while (1) {
	     bzero(org, MAX_WORD_LENGTH);
       bzero(rev, MAX_WORD_LENGTH);

    	if ((bytes=recvfrom(echo_socket, org, MAX_WORD_LENGTH, 0, client, (socklen_t*)&len) < 0)) {
    	    perror("Read error!\n");
    	    exit(-1);
    	}

      cout << "Received " << org << " from client\n";

      int i;
      int org_len = strlen(org);
      for (i=0; i<org_len; i++) {
        rev[i] = org[org_len-i-1];
      }

	    // Create the outgoing message (as an ASCII string)
	    sprintf(rev, "%s", rev);

	    // Send the result message back to the client
	    bytes = sendto(echo_socket, rev, strlen(rev), 0, client, len);
      if (bytes == -1){
			    perror("send() call failed");
			    exit(-1);
      }
    } // Infinite while loop
    close(echo_socket);
    return 0;
} // Main func
