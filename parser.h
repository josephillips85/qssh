#ifndef PARSER_H
#define PARSER_H

#define MAX_LINE_LENGTH 100
#define MAX_HOSTS 700 // Maximum number of hosts to store

struct HostInfo {
    char alias[MAX_LINE_LENGTH];
    char hostname[MAX_LINE_LENGTH];
    char username[MAX_LINE_LENGTH];  
    char tempUsername[MAX_LINE_LENGTH];
    int port;
};

void generateGrid(const char* filename);

#endif 
