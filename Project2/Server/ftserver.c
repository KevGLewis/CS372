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
    int establishedConnectionFD;
    int clientServerSocket;
	socklen_t sizeOfClientInfo;
    int clientPort, serverPort = 0;
    char* buffer = NULL;
    char* bufferTwo = NULL;
    char host[BUFFSIZE];
	struct sockaddr_in clientAddress;
    
    char* password = "&&&&&"; // Let's us acknowledge we are talking with a friendly client


	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

    serverPort = atoi(argv[1]);
    listenSocketFD = CreateServerSocket(serverPort);

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
        
        // Get the name of the client
        // From https://beej.us/guide/bgnet/html/multi/getnameinfoman.html
        getnameinfo( (struct sockaddr *)&clientAddress, sizeof clientAddress, host, sizeof host, NULL, NULL, 0);
    
    
        // Handshake with the client
        if(PasswordReceive(establishedConnectionFD, password))
        {
            printf("Connection from %s\n", host);
            
            // Receive the data from the Client
            if(ReceiveData(&buffer, establishedConnectionFD))
            {
                perror("Error Receiving Data");
            }
            else
            {
                // buffer now stores the command
                bufferTwo = HandleCommand(&buffer, &clientPort, host, establishedConnectionFD);
                
                clientServerSocket = CreateClientSocket(host, clientPort);
                
                if(clientServerSocket == -1)
                {
                    printf("Error connecting to %s:%d, sending error message to %s:%d\n", host, clientPort, host, serverPort);
                    free(bufferTwo);
                    
                    bufferTwo = calloc(50, sizeof(char));
                    sprintf(bufferTwo, "%s", "Connection Error");
                    SendData(&bufferTwo, establishedConnectionFD);
                    
                }
                else
                {
                    SendData(&bufferTwo, clientServerSocket);
                    free(bufferTwo);
                }
            }
            free(buffer);
        }
        
        else
        {
            printf("Bad connection setup\n");
        }
        
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
    }
    
	return 0;
}
                   
                   

                   

