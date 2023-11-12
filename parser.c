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
#include "parser.h"
#include <stdbool.h>

#define MAX_ALIAS_HOSTNAME_LENGTH 50

void generateGrid(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("No entries found.\n");
        return;
    }

    struct HostInfo hosts[MAX_HOSTS];
    int hostCount = -1;

   
    for (int i = 0; i < MAX_HOSTS; i++) {
        hosts[i].alias[0] = '\0';
        hosts[i].hostname[0] = '\0';
        hosts[i].username[0] = '\0';
        hosts[i].port = 0;
    }

    char line[MAX_LINE_LENGTH];
    char currentAlias[MAX_ALIAS_HOSTNAME_LENGTH] = "";
    char LastcurrentAlias[MAX_ALIAS_HOSTNAME_LENGTH] = "";
    int maxAliasLength = 0;
    int maxHostnameLength = 0;
    int maxUsernameLength = 0;
    char tempUsername[MAX_ALIAS_HOSTNAME_LENGTH] = "" ;

    while (fgets(line, sizeof(line), file) != NULL && hostCount +1   < MAX_HOSTS) {
         
        if (strstr(line, "Host ") != NULL) {
            sscanf(line, "Host %s", currentAlias);
            int aliasLength = strlen(currentAlias);
            if (aliasLength > MAX_ALIAS_HOSTNAME_LENGTH) {
                aliasLength = MAX_ALIAS_HOSTNAME_LENGTH;
            }
            if (aliasLength > maxAliasLength) {
                maxAliasLength = aliasLength;
            }
            if ( LastcurrentAlias != currentAlias){
                hostCount++;
            }
            
       
        } else if (strstr(line, "HostName") != NULL) {
            sscanf(line, "    HostName %s", hosts[hostCount].hostname);
            int hostnameLength = strlen(hosts[hostCount].hostname);
            if (hostnameLength > MAX_ALIAS_HOSTNAME_LENGTH) {
                hostnameLength = MAX_ALIAS_HOSTNAME_LENGTH;
            }
            if (hostnameLength > maxHostnameLength) {
                maxHostnameLength = hostnameLength;
            }
            
         
        } else if (strstr(line, "User") !=NULL) {
            sscanf(line, "    User %s", tempUsername);
            strncpy(hosts[hostCount].username, tempUsername, MAX_ALIAS_HOSTNAME_LENGTH - 1);
            hosts[hostCount].username[MAX_ALIAS_HOSTNAME_LENGTH - 1] = '\0';
            char tempUsername[MAX_ALIAS_HOSTNAME_LENGTH] = "\0" ;
            
                
            
        } else if (strstr(line, "Port") != NULL) {
            sscanf(line, "    Port %d", &hosts[hostCount].port);
            strncpy(hosts[hostCount].alias, currentAlias, MAX_LINE_LENGTH - 1);
            hosts[hostCount].alias[MAX_LINE_LENGTH - 1] = '\0';
               
        }
        
            
            int usernameLength = strlen(hosts[hostCount].username);
            if (usernameLength > MAX_ALIAS_HOSTNAME_LENGTH) {
                usernameLength = MAX_ALIAS_HOSTNAME_LENGTH;
            }
            if (usernameLength > maxUsernameLength) {
                maxUsernameLength = usernameLength;
            }
        if (strlen (hosts[hostCount].username) == NULL ){
                            const char* defaultUsername = getenv("USER");
               
                    strncpy(hosts[hostCount].username, defaultUsername, MAX_ALIAS_HOSTNAME_LENGTH - 1);
                    hosts[hostCount].username[MAX_ALIAS_HOSTNAME_LENGTH - 1] = '\0';
        }
       
    }

    fclose(file);

    // Calculate the total width of the table
    int totalWidth = maxAliasLength + maxHostnameLength + maxUsernameLength + 34;
     char replacementChar = '-'; 
    char replacementStringAlias[maxAliasLength + 3]; 
    char replacementStringUsername[maxUsernameLength + 3];
    memset(replacementStringAlias, replacementChar, maxAliasLength +2);
    replacementStringAlias[maxAliasLength +2 ] = '\0';
    memset(replacementStringUsername,replacementChar,maxUsernameLength + 2);
    replacementStringUsername[maxUsernameLength +2 ] = '\0';

    if (hostCount >= 0 ){
        
    // Printing Head of Title
    printf("--%-*s%-19.19s%-*s%-9.9s\n", maxAliasLength, replacementStringAlias, "-----------------------", maxUsernameLength, replacementStringUsername, "----------------");
    // Printing the parsed data in tabular form
    printf("| %-*s | %-15s | %--*s | %-6s |\n", maxAliasLength, "Alias", "Hostname", maxUsernameLength, "Username", "Port");
     printf("|%-*s|%-17.17s|%-*s|%-8.8s|\n", maxAliasLength, replacementStringAlias, "---------------------", maxUsernameLength, replacementStringUsername, "----------");
    for (int i = 0; i < hostCount +1 ; i++) {
        printf("| %-*.*s | %-15.14s | %-*.*s | %-6d |\n", maxAliasLength, maxAliasLength, hosts[i].alias, hosts[i].hostname, maxUsernameLength, maxUsernameLength, hosts[i].username, hosts[i].port);
    }
    //print bottom of table
    printf("--%-*s%-19.19s%-*s%-9.9s\n", maxAliasLength, replacementStringAlias, "-----------------------", maxUsernameLength, replacementStringUsername, "----------------");
    }else {
    printf("No entries found. \n");
}
}
