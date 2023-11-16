#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define MAX_INODES 1024
#define MAX_NAME_LENGTH 32

typedef struct {
    uint32_t inode;
    uint32_t parentInode;
    char type;
    char name[32];
} Inode;

Inode currentINode;
Inode iNodesList[MAX_INODES];
const char *dir;
int index = 0;
bool initial_load = false;

void *checked_malloc(int len) {
    void *p = malloc(len);
    if (p == NULL)
    {
        printf("Ran out of memory!\n");
        exit(1);
    }
    return p;
}

char *uint32_to_str(uint32_t i) {
    /*pretend to print to a string to get length*/
    int length = snprintf(NULL, 0, "%lu", (unsigned long)i);

    /*allocate space for the actual string*/
    char* str = (char *)checked_malloc(length + 1);

    /*print to string*/
    snprintf(str, length + 1, "%lu", (unsigned long)i);
    return str;
}

void loadInode(uint32_t inode, char type, int total) {
    char *inodeStr = uint32_to_str(inode);
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s", dir, inodeStr);

    FILE *file;
    char character; /*Variable to store the read character*/

    printf("FILE PATH: %s\n", filename);
    
    /*Open the binary file in read mode*/
    file = fopen(filename, "rb"); // "rb" stands for read binary

    /*Check if the file was opened successfully*/
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    /*Read characters from the file and print them as characters*/
    int char_count = 1;
    char prev = '\0';
    char curr = '\0';

    while (fread(&character, sizeof(char), 1, file) == 1) {
        if (char_count == 1) {
            prev = character;
            if (type == 'd') {
                // printf("This is a directory! %c\n", character);
            } else {
                printf("%c", character);
            }
        } else {
            curr = character;
            if (prev == 0 && curr != 0) {
                if (!isprint(curr) && curr != 46) {
                    // printf("INODE #%d:", character);
                } else {
                    printf("%c", character);
                }
            } else if (prev != 0 && curr != 0) {
                if (isprint(curr)) {
                    printf("%c", character);
                }
            } else if (prev != 0 && curr == 0) {
                //printf("Add null terminator. String is done!\n");
                printf("\n");
            }
        }

        char_count = char_count + 1;
        prev = character;
    }

    if (ferror(file)) {
        perror("Error reading file");
        exit(1);
    }
    fclose(file);
}


void loadInodesList(char *filename) {
    FILE *file;
    char character; /*Variable to store the read character*/

    /*Open the binary file in read mode*/
    file = fopen(filename, "rb"); 

    // Check if the file was opened successfully
    if (file == NULL) {
    perror("Error opening file");
    exit(1);
    }

    /*Read characters from the file and print them as characters*/
    int char_count = 1;
    uint32_t inode = 0;
    char type = '\0';

    while (fread(&character, sizeof(char), 1, file) == 1) {
        if(char_count == 1 || char_count % 5 == 1) {
            inode = character;
        } else if(character != 0) {
            type = character;
            printf("NEW FILE/DIR: %d, %c\n", inode, type);
            iNodesList[index].inode = inode;
            iNodesList[index].type = type;
            iNodesList[index].parentInode = getParentInode(inode);
            strcpy(iNodesList[index].name, getNameInode(inode));
            index = index + 1;
        }
        char_count = char_count + 1;
    }

    if (ferror(file)) {
        perror("Error reading file");
        exit(1);
    }
    
    fclose(file);

    for (size_t i = 0; i < index; i++) {
        printf("\n\nNEW INODE: %d\n", iNodesList[i].inode);
        loadInode(iNodesList[i].inode, iNodesList[i].type, index);
    }
}

void ls() {
    loadInode(currentINode.inode, currentINode.type, index);
}

void cd(const char *directory){

    if (strcmp(directory, ".") == 0){

    } else if (strcmp(directory, "..") == 0){
        currentINode.inode = currentINode.parentInode;

    } else {

        // setup
        char *inodeStr = uint32_to_str(currentINode.inode);
        char filename[256];
        snprintf(filename, sizeof(filename), "%s/%s", dir, inodeStr);
        FILE *file;
        file = fopen(filename, "rb");
        
        if (file == NULL) {
            perror("Error opening file");
            exit(1);
        }

        /*Read characters from the file and print them as characters*/
        int char_count = 1;
        char character;
        uint32_t temp_inode = 0;
        char prev = '\0';
        char curr = '\0';

        char foundName[MAX_NAME_LENGTH];

        while (fread(&character, sizeof(char), 1, file) == 1) {
            if (char_count == 1) {
                prev = character;
                if (currentINode.type == 'd') {
                    // printf("This is a directory! %c\n", character);
                } else {
                    strncat(foundName, &character, 1);
                }
            } else {
                curr = character;
                if (prev == 0 && curr != 0) {
                    if (!isprint(curr) && curr != 46) {
                        temp_inode = character;
                    } else {
                        strncat(foundName, &character, 1);
                    }
                } else if (prev != 0 && curr != 0) {
                    if (isprint(curr)) {
                        strncat(foundName, &character, 1);
                    }
                } else if (prev != 0 && curr == 0) {
                    //printf("Add null terminator. String is done!\n");
                    if (strcmp(foundName, directory) == 0) {
                        currentINode.parentInode = currentINode.inode;
                        currentINode.inode = temp_inode;
                        strcpy(currentINode.name, directory);
                    } else {
                        memset(foundName, 0, sizeof(foundName));
                    }
                }
            }

            char_count = char_count + 1;
            prev = character;
        }

        if (ferror(file)) {
            perror("Error reading file");
            exit(1);
        }
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2){
        perror("Error: incorrect number of arguments");
        return 1;
    }

    // Requirement 2: being able to load inodes_list
    dir = argv[1];
    char filepath[256];
    sprintf(filepath, "%s/inodes_list", dir);
    loadInodesList(filepath); 

    // Requirement 3: user starts at inode 0
    currentINode.inode = 0;
    currentINode.parentInode = 0;
    strcpy(currentINode.type, iNodesList[currentINode.inode].type);

    if (currentINode.type == 'f'){
        perror("Error: Inode 0 is a file, not a directory");
        return 1;
    } else {
        // Requirement 4: implementing exit, cd, ls, mkdir, touch
        char command[256];

        while (true) {
            printf("\nEnter command: ");
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = 0;
            printf("\n");

            if (strcmp(command, "exit") == 0) {
                return 0;
            } else if (strcmp(command, "ls") == 0){
                ls();
            } else if (strncmp(command, "cd ", 3) == 0) {
                const char *directory = command + 3;
                cd(directory);
            } else {
                printf("Error: '%s' is an unknown command. Try Again.\n", command);
            }
        }
    }

    return 0;
}