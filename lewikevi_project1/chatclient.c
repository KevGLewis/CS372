//---------------------------------------------------------------/
// Author: Kevin Lewis
// ONID: LewiKevi
// Date: 07/29/2018
// Description: Basic chat client. Takes the server host location
// and portnumber from the command line:
// USAGE: chatclient host port
// Sends the first message to the client, then alternates receiving
// and sending. Client can end the chat by typing \quit
//---------------------------------------------------------------/

#include "snd_recv_help.h"


//Note: The boiler plate code was provided by the CS344 class

int main(int argc, char *argv[])
{
    
	int socketFD, portNumber, shouldContinue;
    char* handle = NULL;
    size_t handleBufSize = 0;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
    char* buffer = NULL;
    
    char* password = "PORTNUM"; // Allows us to know we are working with a friendly server
    
	if (argc < 3) { fprintf(stderr,"USAGE: %s host port\n", argv[0]); exit(0); } // Check usage & args

    /*************************************** FROM CS 344 *********************************************/
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address argv1 is the host name
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
    
	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0)
    {
        error("CLIENT: ERROR opening socket");
        exit(2);
    }
     /*************************************** End FROM CS 344 *********************************************/
    
    // Get the handle from the user
    printf("Please provide your handle (up to 10 characters): ");
    getline(&handle, &handleBufSize, stdin); //read in the line.
    handle[(int)(strstr(handle, "\n") - handle)] = '\0'; // End the string early to wipe out the terminal
    
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
        error("CLIENT: ERROR connecting");
    
    // Handshake with the server
    if(!ConnectionSetup(socketFD, password))
    {
        // Close it if it was not successfull
        close(socketFD);
        exit(2);
    }
    
    // Loop the send/receive connection until the user types quit
    shouldContinue = 1;
    while(shouldContinue == 1)
    {
        // Grab the user data from the
        shouldContinue = GetUserData(&buffer, handle);
        if(shouldContinue)
        {
            SendData(&buffer, socketFD);
            free(buffer);
            buffer = NULL;
            ReceiveData(&buffer, socketFD);
            if(strcmp(buffer, "close") == 0)
            {
                printf("Server has closed the connection\n");
                shouldContinue = 0;
            }
            else
            {
                printf("%s\n", buffer);
            }

            free(buffer);
        }
        else
        {
            printf("Trying to quit\n");
        }
    }
    
    // Cleanup
    close(socketFD); // Close the socket
    
    exit(0);
}
