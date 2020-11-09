/*
Name: Sania Moazzam
Your Server - UDP protocol
Description: This is a multiple shift cipher. Letters in the alphabet are shifted with a shift number.
             The number depends on if the letter falls in one of the four cases: vowels, common 
             consonants, rare consonants or the rest of the letters.
*/

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <algorithm>

/* Manifest constants */
#define YOUR_PORT 12006
#define MAX_WORD_LENGTH 100
#define UDP_IP "127.0.0.1"

using namespace std;

int main() {
    // Initialize server sockaddr structure
    struct sockaddr_in echo_server, echo_client;
    struct sockaddr *server, *client;

    char org[MAX_WORD_LENGTH];
    char your[MAX_WORD_LENGTH];
    int bytes, len = sizeof(echo_client);;

    memset(&echo_server, 0, sizeof(echo_server));
    memset(&echo_client, 0, sizeof(echo_client));

	  echo_server.sin_family = AF_INET; //specifies the address family
	  echo_server.sin_port = htons(YOUR_PORT); //specificies the port number
    inet_pton(AF_INET, UDP_IP, &echo_server.sin_addr); //specifies the IP address

    server = (struct sockaddr *) &echo_server;
    client = (struct sockaddr *) &echo_client;

    // Socket creation
  	int echo_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  	if (echo_socket == -1){
  		perror("socket() call failed");
  		exit(-1);
  	}

    // Binding
  	int status = bind(echo_socket, (struct sockaddr *)&echo_server, sizeof(echo_server));
  	if (status == -1){
  		perror("bind() call failed\n");
  		exit(-1);
  	}

    while (1) {
	     bzero(org, MAX_WORD_LENGTH);
       bzero(your, MAX_WORD_LENGTH);

    	if ((bytes=recvfrom(echo_socket, org, MAX_WORD_LENGTH, 0, client, (socklen_t*)&len) < 0)) {
    	    perror("Read error!\n");
    	    exit(-1);
    	}

      cout << "Received " << org << " from client\n";

      int i, org_len = strlen(org);
      char vowel[] = "AEIOU";
      char common[] = "RST";
      char rare[] = "JXZ";
      char rest[] = "BCDFGHKLMNPQVWY";

      //char* pch = strchr(vowel, 'a');
      //cout << pch-vowel+1;

      for (i=0; i<org_len; i++) {
        org[i] = toupper(org[i]);
        char* vptr = strchr(vowel, org[i]);
        char* cptr = strchr(common, org[i]);
        char* raptr = strchr(rare, org[i]);
        char* reptr = strchr(rest, org[i]);

        if (vptr != NULL) your[i] = char(int(org[i]-55)%26 + 65);

        else if (cptr != NULL) your[i] = char(int(org[i]-58)%26 + 65);

        else if (raptr != NULL) your[i] = char(int(org[i]-49)%26 + 65);

        else if (reptr != NULL) your[i] = char(int(org[i]-63)%26 + 65);

        else your[i] = org[i];
      }

	    // Create the outgoing message (as an ASCII string)
	    sprintf(your, "%s", your);

	    // Send the result message back to the client
	    bytes = sendto(echo_socket, your, strlen(your), 0, client, len);
      if (bytes == -1){
			    perror("send() call failed");
			    exit(-1);
      }
    } // Infinite while loop
    close(echo_socket);
    return 0;
} // Main func
