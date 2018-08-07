//
//  snd_recv_help.h
//  DevEnviro
//
//  Created by Kevin Lewis on 3/8/18.
//  Copyright © 2018 Kevin Lewis. All rights reserved.
//

#ifndef snd_recv_help_h
#define snd_recv_help_h
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define BUFFSIZE 1056

struct InputFileNames
{
    char keyFileName[256];
    char textToEncryptFileName[256];
};


void error(const char *msg);
void VerifyInput(char* plaintextFile, char* keyFile);
void LoadFile(char** buffer, char* fileName);
void LoadPlaintext(char* buffer, char* filename);
int PasswordReceive(int socketFD, char* password);
int SendData(char** buffer, int socketFD);
int ReceiveData(char** buffer, int establishedConnectionFD);
void catchSIGCHLD(int signo);
void catchSIGTERM(int signo);
void ExpandDynArray(char** buffer, int *arraySize);
int GetUserData(char** buffer, char* handle);
char* HandleCommand(char** buffer, int* clientPort, int socketFD);
int CreateServerSocket(int portNumber);
int CreateClientSocket( char* hostName, int portNumber );
char* GetFileDirectory();


#endif /* snd_recv_help_h */
