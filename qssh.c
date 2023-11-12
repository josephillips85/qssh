//QuickSSH
//Copyright (C) 2023 Jose Phillips (jose@hddlive.net)

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser.h" 
#include "autocomplete.h"
#include <stdarg.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 1024
const char *getConfigpath(void);

struct Host {
    char alias[100];
    char host[100];
    char username[100];
    char identity[100];
    int port;
};
char qssh_version [3] = "1.1";

void initializeConfigFile() {
    FILE *configFile = fopen(getConfigpath(), "w");
    if (configFile == NULL) {
        printf("Error creating config file.\n");
        return;
    }
    fclose(configFile);
    printf("Initialized new config file.\n");
}

int isAliasDuplicate(const char *alias) {
    FILE *configFile = fopen(getConfigpath(), "r");
    if (configFile == NULL) {
        return 0; 
    }

    char line[256];
    while (fgets(line, sizeof(line), configFile) != NULL) {
        if (strncmp(line, "Host ", 5) == 0) {
            char *foundAlias = line + 5; // Skip "Host "
            foundAlias[strcspn(foundAlias, "\n")] = '\0';
            if (strcmp(foundAlias, alias) == 0) {
                fclose(configFile);
                return 1; // Duplicated
            }
        }
    }

    fclose(configFile);
    return 0; // Not a duplicate

}
//Case insensitive check
int strcmpi(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        int diff = tolower((unsigned char)*str1) - tolower((unsigned char)*str2);
        if (diff != 0)
            return diff;
        str1++;
        str2++;
    }
    return tolower((unsigned char)*str1) - tolower((unsigned char)*str2);
}


void addHost(const char *alias, const char *host, const char *username, const char *identity, int port) {
    int isDuplicate = isAliasDuplicate(alias);
    FILE *configFile = fopen(getConfigpath(), "a");
    if (configFile == NULL) {
        printf("Warning: Configuration file not found creating a new file...\n");
        return;
    }
    
    //Check duplicate host
    if (isDuplicate){
        printf("Hostname or alias %s already have a configuration.\n",alias);
    } else if (strcmpi(alias, "add") == 0 || strcmpi(alias, "remove") == 0 || strcmpi(alias, "list") == 0 || strcmpi(alias, "version") == 0){
        printf("The selected hostname or alias is invalid. Please refrain from using 'ADD,' 'REMOVE,','LIST' or 'VERSION' as alias or hostname values.\n");
        
    } else {

    fprintf(configFile, "Host %s\n", alias);
    fprintf(configFile, "    HostName %s\n", host);
    if (strlen(username) > 0) {
        fprintf(configFile, "    User %s\n", username);
    }
    if (strlen(identity) > 0) {
        fprintf(configFile, "    IdentityFile %s\n", identity);
    }
    if (port > 0 && port <= 65535) {
        fprintf(configFile, "    Port %d\n", port);
    }

// Force the use of keychain on MacOS if the user don;t have keychain is ignored.
    #ifdef __APPLE__
        fprintf(configFile,"    UseKeychain %s\n", "yes");
    #endif

    fclose(configFile);
    makeAutocomplete();
    printf("Added host: %s\n", alias);
}
}

void removeHost(const char *alias) {
    FILE *tempFile = fopen("temp_config", "w");
    if (tempFile == NULL) {
        printf("Error creating temporary file.\n");
        return;
    }

    FILE *configFile = fopen(getConfigpath(), "r");
    if (configFile == NULL) {
        printf("Error opening config file.\n");
        fclose(tempFile);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int isHostToRemove = 0;

    while (fgets(line, MAX_LINE_LENGTH, configFile) != NULL) {
        if (strncmp(line, "Host ", 5) == 0) {
            isHostToRemove = (strstr(line, alias) != NULL);
        }

        if (!isHostToRemove) {
            fputs(line, tempFile);
        }
    }

    fclose(configFile);
    fclose(tempFile);

    if (remove(getConfigpath()) != 0) {
        printf("Error deleting original config file.\n");
        return;
    }

    if (rename("temp_config", getConfigpath()) != 0) {
        printf("Error renaming temporary file.\n");
        return;
    }
    if ( isHostToRemove == 0 ) {
        printf("Missing host or alias: %s\n",alias);
        return ; 
    }
    makeAutocomplete();
    printf("Removed host: %s\n", alias);
}

void listHosts() {
    FILE *configFile = fopen(getConfigpath(), "r");
    if (configFile == NULL) {
        printf("Error opening config file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int isHostLine = 0;
    
    while (fgets(line, MAX_LINE_LENGTH, configFile) != NULL) {
        if (strncmp(line, "Host ", 5) == 0) {
            isHostLine = 1;
            printf("%s", line);
        } else if (isHostLine) {
            if (line[0] == ' ') {
                printf("%s", line);
            } else {
                isHostLine = 0;
            }
        }
    }

    fclose(configFile);
}

void showOptions() {
    printf("Usage: qssh <command> [options]\n");
    printf("\nCommands:\n");
    printf("  add\n");
    printf("      Add a new host to the configuration.\n");
    printf("      Options:\n");
    printf("          [user]@host Specify the user and host to connect.\n");
    printf("          -i Specify the path of the identity file.\n");
    printf("          -a Specify a alias to connect to remote server.\n");
    printf("          -p Specify the port of remote server.\n");
    printf("  remove\n");
    printf("      Remove a alias from the configuration.\n");
    printf("          Specify the alias of the host to remove.\n");
    printf("  list\n");
    printf("      List all hosts in the configuration.\n");
    printf("  version\n");
    printf("      Show version and quit.\n");
   
}

int fileExists(const char *filePath) {
    return access(filePath, F_OK) != -1;
}

const char *getConfigpath() {
    char *ssh_config_path = "/.ssh/qsshconfig";

    const char *homepath = getenv("HOME");
    if (homepath == NULL) {
        printf("Unable to determine homepath\n");
    }
  char *full_path = (char *)malloc(strlen(homepath) + strlen(ssh_config_path) + 1);
    
    if (full_path == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    strcpy(full_path, homepath);
    strcat(full_path, ssh_config_path);


    return full_path;

    }


int spawnSSH(const char *fmt, ...) {

  
    char sshCommand[512]; 
    va_list args;
    va_start(args, fmt);
    snprintf(sshCommand, sizeof(sshCommand), "ssh -F \"%s\" ", getConfigpath());
    vsnprintf(sshCommand + strlen(sshCommand), sizeof(sshCommand) - strlen(sshCommand), fmt, args);
    va_end(args);

    int status = system(sshCommand);

    return status;
}




int main(int argc, char *argv[]) {

    if (argc < 2) {
        showOptions();
        return 1;
    }

    if (strcmpi(argv[1], "add") == 0) {
        const char *user = NULL;
        const char *host = NULL;
        const char *identity = "";
        int port = 22;
        const char *alias = NULL;

        int i = 2;
        while (i < argc) {
            if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
                alias = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
                identity = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
                port = atoi(argv[i + 1]);
                if (port <= 0 || port > 65535) {
                    printf("Invalid port number.\n");
                    return 1;
                }
                i++;
            } else if (host == NULL && user == NULL && strchr(argv[i], '@')) {
                char *atChar = strchr(argv[i], '@');
                *atChar = '\0';  
                user = argv[i];
                host = atChar + 1;
            } else if (host == NULL && user == NULL) {
                host = argv[i];
                user = "";
            } else {
                printf("Invalid argument: %s\n", argv[i]);
                return 1;
            }
            i++;
        }

        if (host == NULL) {
            printf("Hostname is required.\n");
            return 1;
        }

        if (!alias) {
            alias = host;
        }
        if (strlen(identity) > 0 && !fileExists(identity)) {
            printf("Identity file does not exist: %s\n", identity);
            return 1;
        }
        addHost(alias, host, user, identity, port);
    
    

     } else if (strcmpi(argv[1], "remove") == 0) {
        if (argc < 3 ) {
            printf("Usage: qssh remove [Alias]\n");
            return 1;
        }
        const char *alias = argv[2];
        removeHost(alias);
    } else if (strcmpi(argv[1], "list") == 0) {
        generateGrid(getConfigpath());
        
    } else if (strcmpi(argv[1], "version") == 0) {
        printf ("QuickSSH Version %s \n",qssh_version);
        printf ("---------------------\n");
        printf("https://www.github.com/josephillips85/qssh\n");
        return 0;

    } else {
        int isDuplicate = isAliasDuplicate(argv[1]);

        if (!isDuplicate){
        printf("Unable to find %s.\n",argv[1]);
        return 1;
        } else {
        int status = spawnSSH("%s", argv[1]);
        
        
            if (status == -1) {
        perror("Unable to spawn SSH Client");
        return 1;
            }
    } 
    return 0;

        printf("Invalid command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
