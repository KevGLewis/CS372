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

void error(const char *msg);
int ConnectionSetup(int socketFD, char* password);
int PasswordReceive(int socketFD, char* password);
int SendData(char** buffer, int socketFD);
int ReceiveData(char** buffer, int establishedConnectionFD);
int GetUserData(char** buffer, char* handle);


#endif /* snd_recv_help_h */
