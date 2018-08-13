//---------------------------------------------------------------/
// Author: Kevin Lewis
// ONID: LewiKevi
// Date: 08/10/2018
// Description: Basic ftp server. Client connects to server, client
// creates data connection, server sends file to client over connectin
// USAGE: ftserver server port
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
	socklen_t sizeOfClientInfo;
    int clientPort, serverPort = 0;
    char* buffer = NULL;
    CommandParse* commandResult;
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
        printf("Waiting on connection with client\n");
        
        /*************************************** FROM CS 344 *********************************************/

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0) error("ERROR on accept");

        /*************************************** END CS 344 *********************************************/
        
        // Get the name of the client
        // From https://beej.us/guide/bgnet/html/multi/getnameinfoman.html
        getnameinfo( (struct sockaddr *)&clientAddress, sizeof clientAddress, host, sizeof host, NULL, 0, 0);
    
    
        // Handshake with the client
        if(PasswordReceive(establishedConnectionFD, password))
        {
            printf("Connection from %s\n", host);
            
            // Receive the data from the Client
            if(ReceiveData(&buffer, establishedConnectionFD))
            {
                perror("Error Receiving Data\n");
            }
            else
            {
                // buffer now stores the command
                commandResult = HandleCommand(&buffer, &clientPort, host, establishedConnectionFD);
                free(buffer);
                
                // If we have an error, just send the error message back over the first connection
                if(commandResult->error)
                {
                    SendData(&(commandResult->message), establishedConnectionFD);
                }
                else
                {
                    // Tell the client the command got through and it was ok
                    buffer = calloc(10, sizeof(char));
                    sprintf(buffer, "%s", "OK");
                    SendData(&buffer, establishedConnectionFD);
                    free(buffer);
                    
                    // Receive message from client, informing us that the connection is ready
                    ReceiveData(&buffer, establishedConnectionFD);
                    free(buffer);
                    
                    // Send the data to the client
                    SendRequestedDataToClient(host, clientPort, commandResult->message, serverPort, establishedConnectionFD);
                }
                
            }
        }
        
        else
        {
            printf("Bad connection setup\n");
        }
        
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
        printf("\n");
    }
    
	return 0;
}
                   
                   

                   

