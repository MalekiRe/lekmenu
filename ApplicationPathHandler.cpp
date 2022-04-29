//
// Created by malek on 4/28/22.
//

#include <cstring>
#include <dirent.h>
#include "ApplicationPathHandler.h"
using namespace std;
vector<string> getAllFiles() {
    char* input, shell_prompt[100];
    snprintf(shell_prompt, sizeof(shell_prompt), "%s", getenv("PATH"));
    char *token;
    /* get the first token */
    token = strtok(shell_prompt, ":");
    vector<string> files;
    /* walk through other tokens */
    while( token != nullptr ) {
        printf( " %s\n", token );
        token = strtok(nullptr, ":");
        DIR *d;
        if(token == nullptr) {
            continue;
        }
        struct dirent *dir;
        d = opendir(token);
        if (d) {
            while ((dir = readdir(d)) != nullptr) {
                files.emplace_back(dir->d_name);
            }
            closedir(d);
        }
    }
    return files;
}
static std::vector<std::string> paths;
std::vector<std::string> &ApplicationPathHandler::getAppsOnPath() {
    if(paths.empty()) {
        paths = getAllFiles();
    }
    return paths;
}

