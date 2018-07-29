
#include "snd_recv_help.h"
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

// Sets up the connection with the server
// Verfies that it is a trusted server by checking its response
// Input: int Socket number, char* the password you want to send
int ConnectionSetup(int socketFD, char* password)
{
    // Clear the arrays
    char* buffer = calloc(strlen(password), sizeof(char));
    sprintf(buffer, "%s", password); // Load the buffer with our password
    SendData(&buffer, socketFD);
    free(buffer);
    
    buffer = NULL;
    ReceiveData(&buffer, socketFD);
    
    if(strcmp(buffer, password) != 0)
    {
        perror("Handshake Failed - Client\n");
        return 0;
    }
    else
    {
        printf("Successfully connected to server, send \\quit to exit\n");
    }
    free(buffer);
    return 1;
}

// Send a small spurt to the server, because it is small (< 500 chars), we don't need to iterate through
// the string, breaking it into chunks
// Input: char** buffer full of what we want to send, int socket ID
int SendData(char** buffer, int socketFD)
{
    int charsWritten = (int) send(socketFD, *buffer, strlen(*buffer), 0); // Write to the server
    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
    
    return 0;
}

// Receives the file from the server, because the max file size is small (< 500 chars), we do not
// need to interate through the data, breaking it into smaller chunks
// Input: char** null buffer where we will put response, int socket ID
int ReceiveData(char** buffer, int establishedConnectionFD)
{
    int arraySize = 1056;
    *buffer = calloc(arraySize, sizeof(char));
    
    int charsRead = (int) recv(establishedConnectionFD, *buffer, 1055, 0);
    if (charsRead < 0) error("ERROR reading from socket");
    
    return 0;
}

// Basic function to get input from the command prompt and put
// it into a buffer. Note, the buffer is limit to 1056 characters
// Input: char** buffer full of what we want to send, char* User handle
int GetUserData(char** buffer, char* handle)
{
    // First, allocate the buffer
    *buffer = calloc(1056, sizeof(char));
    
    int isOK = 1;
    size_t tempBufferSize = 0;
    char* tempBuffer = NULL;
    // Get the handle from the user
    printf("%s> ", handle);
    fflush (stdout);
    
    getline (&tempBuffer, &tempBufferSize, stdin);
    tempBuffer[strlen(tempBuffer)-1] = '\0'; // End the string early to wipe out the terminal
    
    // Fill the buffer
    sprintf(*buffer, "%s> %s", handle, tempBuffer); // Load the buffer with our arguments
    
    // Test if we want to quit
    if(strcmp(tempBuffer, "\\quit") == 0) {isOK = 0;}
    
    free(tempBuffer);
    
    return isOK;
}

