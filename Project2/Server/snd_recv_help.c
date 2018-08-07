//
//  snd_recv_help.c
//  DevEnviro
//
//  Created by Kevin Lewis on 3/8/18.
//  Copyright © 2018 Kevin Lewis. All rights reserved.
//

#include "snd_recv_help.h"
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues


// Load a file into the buffer
void LoadFile(char** buffer, char* fileName)
{
    char* terminalLocation;
    int totalSize = 0;
    int arraySize = BUFFSIZE;
    *buffer = calloc(arraySize, sizeof(char));
    FILE *fp = fopen(fileName, "r");
    if(fp == NULL)
    {
        perror("Unable to open file");
        exit(1);
    }
    
    size_t bufferSize = 0; // Holds how large the allocated buffer is
    char* lineEntered = NULL; // Points to a buffer allocated by getline() that holds our entered string
    
    // Read the file line by line and remote the new line escape
    while(getline(&lineEntered, &bufferSize, fp) != -1) //read in the line.
    {
        if(strstr(lineEntered, "\n") != NULL)
        {
            terminalLocation = strstr(lineEntered, "\n"); // Where is the terminal
            *terminalLocation = '\0'; // End the string early to wipe out the terminal
        }
        totalSize += strlen(lineEntered);
        while(arraySize < totalSize)
        {
            ExpandDynArray(buffer, &arraySize);
        }
        
        strcat(*buffer, lineEntered);
    }
    fclose(fp);
    free(lineEntered);
}

void ExpandDynArray(char** buffer, int *arraySize)
{
    char* newBuffer = calloc(*arraySize, sizeof(char));
    memcpy(newBuffer, *buffer, *arraySize);
    free(*buffer);
    
    *arraySize = *arraySize * 2;
    *buffer = calloc(*arraySize, sizeof(char));
    memcpy(*buffer, newBuffer, *arraySize / 2);
}

// Sets up the connection with the server
// Verfies that it is a trusted server by checking its response
// Input: int Socket number, char* the password you want to receive
// Returns 1 if the reception was successful, and 0 if it was not
int PasswordReceive(int socketFD, char* password)
{
    char* buffer = NULL;
    // Wait to receive the handshake password from a client
    ReceiveData(&buffer, socketFD);
    if(strcmp(buffer, password) != 0)
    {
        perror("Handshake Failed - Server\n");
        free(buffer);
        
        buffer = calloc(50, sizeof(char));
        sprintf(buffer, "%s", "INCORRECT PASSWORD"); // Load the buffer with our password
        SendData(&buffer, socketFD);
        free(buffer);
        return 0;
    }
    
    // if not we need to send back our buffer
    free(buffer);
    buffer = calloc(50, sizeof(char));
    sprintf(buffer, "%s", "OK"); // Load the buffer with our password
    SendData(&buffer, socketFD);
    printf("Connection Successfully Established\n");
    
    return 1;
}

// Send a small spurt to the server, because it is small (< 500 chars), we don't need to iterate through
// the string, breaking it into chunks
// Input: char** buffer full of what we want to send, int socket ID
// All messages will not end in @@, so we shall add it
int SendData(char** buffer, int socketFD)
{
    // Add our ending text
    strcat(*buffer, "@@");
    
    // Loop the sending action until we are sure we
    int charsWritten = 0;
    int totalSent = 0;
    while(totalSent < strlen(*buffer))
    {
        charsWritten = (int) send(socketFD, &(*buffer)[totalSent], strlen(&(*buffer)[totalSent]), 0); // Write to the server
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        totalSent += charsWritten;
    }
    
    return 0;
}

// Receives the file from the server, because the max file size is small (< 500 chars), we do not
// need to interate through the data, breaking it into smaller chunks
// Input: char** null buffer where we will put response, int socket ID
// When we receive the file, we want to ensure that all messages end with @@
int ReceiveData(char** buffer, int establishedConnectionFD)
{
    char buffSpurt[BUFFSIZE];
    int totalSize = 0;
    int arraySize = BUFFSIZE;
    *buffer = calloc(arraySize, sizeof(char));
    
    int charsRead = 0;
    
    while(strstr(*buffer, "@@") == NULL)
    {
        // Get the message from the client / server
        memset(buffSpurt, '\0', sizeof(buffSpurt));
        charsRead = (int) recv(establishedConnectionFD, buffSpurt, BUFFSIZE - 1, 0);
        if (charsRead < 0) error("ERROR reading from socket");
        totalSize += charsRead;
        // Expand the buffer as needed
        while(arraySize < totalSize)
        {
            ExpandDynArray(buffer, &arraySize);
        }
        strcat(*buffer, buffSpurt);
    }
    
    int terminalLocation = (int) (strstr(*buffer, "@@") - *buffer); // Where is the terminal
    (*buffer)[terminalLocation] = '\0'; // End the string early to wipe out the terminal
    
    printf("We received %s\n", *buffer);
    
    return 0;
}

// Basic function to get input from the command prompt and put
// it into a buffer. Note, the buffer is limit to 1056 characters
// Input: char** buffer full of what we want to send, char* User handle
int GetUserData(char** buffer, char* handle)
{
    // First, allocate the buffer
    *buffer = calloc(BUFFSIZE, sizeof(char));
    
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

// Takes the command and will respond with the correct information
char* HandleCommand(char** buffer, int* clientPort, int socketFD)
{
    // first, separate out the words, we only want to check the first word
    char command[BUFFSIZE];
    char fileName[BUFFSIZE];
    
    char *token = strtok(*buffer, " ");
    char *replyBuffer = NULL;
    
    strcpy(command, token);
    
    if(strcmp(command, "-l") == 0)
    {
        // The second input is the text to encrpt file name
        token = strtok(NULL, " ");
        *clientPort = atoi(token);
        
        return GetFileDirectory();
    }
    else if(strcmp(command, "-g") == 0)
    {
        // second input is the filename
        token = strtok(NULL, " ");
        strcpy(fileName, token);
        
        // Third input is the client port
        token = strtok(NULL, " ");
        *clientPort = atoi(token);
        
        replyBuffer = calloc(50, sizeof(char));
        sprintf(replyBuffer, "%s", "TESTSTSTS"); // Load the buffer with our password
        return replyBuffer;
    }
    else
    {
        replyBuffer = calloc(50, sizeof(char));
        sprintf(replyBuffer, "%s", "INCORRECT COMMAND"); // Load the buffer with our password
        return replyBuffer;
    }
}

char* GetFileDirectory()
{
    char* replyBuffer = NULL;
    replyBuffer = calloc(BUFFSIZE, sizeof(char));
    sprintf(replyBuffer, "%s", "TEST123"); // Load the buffer with our password
    return replyBuffer;
}

int CreateServerSocket(int portNumber)
{
    struct sockaddr_in serverAddress;
    int listenSocketFD;
    /*************************************** FROM CS 344 *********************************************/
    
    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
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
    
    return listenSocketFD;
    
    /*************************************** END CS 344 *********************************************/
}

int CreateClientSocket( char* hostName, int portNumber )
{
    printf("Creating Connection To Client %s %d \n", hostName, portNumber);
    
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    int socketFD;
    
    /*************************************** FROM CS 344 *********************************************/
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname(hostName); // Convert the machine name into a special form of address argv1 is the host name
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
    
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
        error("CLIENT: ERROR connecting");
    
    return socketFD;
}


