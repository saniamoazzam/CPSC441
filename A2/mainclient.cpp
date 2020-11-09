/*
Name: Sania Moazzam
Client for TCP protocol
*/

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>


// Some generic error handling stuff
extern int errno;
void perror(const char *s);

// Manifest constants used by client progra
#define MASTER_PORT 15005
#define MAX_WORD_LENGTH 100
#define SERVER_IP "127.0.0.1"

using namespace std;

void printMenu() {
  cout << "\nPlease choose from the following selections:\n";
  cout << " 1 - Enter a new sentence\n";
  cout << " 2 - Use previous sentence\n";
  cout << " 0 - Exit program\n";
  cout << "Your desired menu selection? ";
}

void printService() {
  cout << "Enter transformation numbers to perform (Example: 2153): \n";
  cout << "  1 - Identity\n";
  cout << "  2 - Reverse\n";
  cout << "  3 - Upper\n";
  cout << "  4 - Lower\n";
  cout << "  5 - Caesar\n";
  cout << "  6 - Multiple shift cipher\n";
  cout << "Transformation: ";
}


int main()
  {
    // Initializing variables
    struct sockaddr_in server;
    char send_msg[MAX_WORD_LENGTH], incom_msg[MAX_WORD_LENGTH];
    int choice, len, i;
    char ch, service[10];
    char error[MAX_WORD_LENGTH] = "Please enter valid service numbers\n";
    char service_nums[] = "123456";

    // Set up the TCP server socket
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(MASTER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Create the client socket for its transport-level end point
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
	     perror("TCP Client: socket() call failed!\n");
	     exit(1);
    }

    // Connect the socket to the server's address using TCP
    int client_connect = connect(client_socket,
                         (struct sockaddr *)&server, sizeof(server));
    if (client_connect < 0) {
	     perror("TCP Client: connect() call failed!\n");
	     exit(1);
    }

    // Print welcome banner
    cout << "\nWelcome!!!";
    cout << "\nPlease choose from the following selections:\n";
    cout << " 1 - Enter a new sentence\n";
    cout << " 0 - Exit program\n";
    cout << "Your desired menu selection? ";
    cin >> choice;

    while (choice != 0) {
      if (choice == 1 or choice==2) {
        if (choice == 1) {
          bzero(send_msg, MAX_WORD_LENGTH);

          ch = getchar();
          // Prompt user for the input
          cout << "\nEnter a sentence: ";
          len = 0;
          while((ch = getchar()) != '\n') {
                send_msg[len] = ch;
    	    len++;
          }

    	    // Make sure the message is null-terminated
          send_msg[len] = '\0';
        }  // choice=1

        // Send it to the TCP server via the socket
        send(client_socket, send_msg, len, 0);

        // Get service commands to perform on the message
        bool flag = true;
        printService();
        cin >> service;

        while (flag) {
          for (i=0; i<strlen(service); i=i+1) {
            char* ptr = strchr(service_nums, service[i]);
            if (ptr == NULL) flag = false;
          }
          if (flag) break;

          cout << "\nINVALID - Enter a valid transformation\n\n";
          flag = true;
          printService();
          cin >> service;
       }

        // Send it to the TCP server via the socket
        send(client_socket, service, 10, 0);

        // See what the server sends back
        int bytes = recv(client_socket, incom_msg, MAX_WORD_LENGTH, 0);
        if(bytes > 0) {
          incom_msg[bytes] = '\0';
          cout << "Transformations performed, the answer is: " << incom_msg << "\n";
        }
        else {
          cout << "TCP Server failed to respond";
          close(client_socket);
          exit(1);
        }

        bzero(incom_msg, MAX_WORD_LENGTH);
        bzero(service, 10);
      } // Valid choice
      else
        cout << "\nINVALID - Please enter a valid number\n";
      // Print welcome banner
      printMenu();
      cin >> choice;
    } // While

    close(client_socket);
    exit(0);
} // Main func
