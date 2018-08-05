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

//Note: The boiler plate code was provided by the CS344 class

#include "snd_recv_help.h"

int listenSocketFD;

// Set up the InputFileNames struct and Return Message
void SetupSignalHandlers()
{
    struct sigaction SIGTERM_action = {{0}};
    //struct sigaction ignore_action = {{0}};
    
    SIGTERM_action.sa_handler = catchSIGTERM;
    sigfillset(&SIGTERM_action.sa_mask);
    SIGTERM_action.sa_flags = 0;
    
    //ignore_action.sa_handler = SIG_IGN;
    sigaction(SIGTERM, &SIGTERM_action, NULL);
}

// Catch the term signal
void catchSIGTERM(int signo)
{
    char* message = "Terminated by signal\n";
    write(STDOUT_FILENO, message, 24);
    close(listenSocketFD); // Close the listening socket
    exit(0);
}

int main(int argc, char *argv[])
{
    int establishedConnectionFD, portNumber;
	socklen_t sizeOfClientInfo;
    char* buffer = NULL;
	struct sockaddr_in serverAddress, clientAddress;
    
    char* password = "&&&&&"; // Let's us acknowledge we are talking with a friendly client


	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

    /*************************************** FROM CS 344 *********************************************/

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 1); // Flip the socket on - it can now receive up to 5 connections

    /*************************************** END CS 344 *********************************************/

    // Handle the Signal Hanlders
    SetupSignalHandlers();
    
    
    while (1)
    {
        printf("Waiting on connection with client");
        
        /*************************************** FROM CS 344 *********************************************/

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0) error("ERROR on accept");

        /*************************************** END CS 344 *********************************************/
    
        // Handshake with the client
        if(PasswordReceive(establishedConnectionFD, password))
        {
            // Receive the data from the Client
            if(ReceiveData(&buffer, establishedConnectionFD))
            {
                perror("Error Receiving Data");
            }
            else
            {
                // buffer now stores the command
                
                
                
                SendData(&buffer, establishedConnectionFD);
                free(buffer);
            }
        }
        
        else
        {
            printf("Bad connection setup\n");
        }
        
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
    }
    
    
    
	return 0;
}
                   
                   

                   

