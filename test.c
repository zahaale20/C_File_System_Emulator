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
bool loadbool = false;

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

    // printf("FILE PATH: %s\n", filename);
    
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

    char foundName[MAX_NAME_LENGTH] = {'\0'};

    while (fread(&character, sizeof(char), 1, file) == 1) {
        printf("%d", character);
        if (char_count == 1) {
            prev = character;
            if (type == 'd') {
                // printf("This is a directory! %d\n", character);
            } else {
                //printf("%c", character);
            }
        } else {
            curr = character;
            if (prev == 0 && curr != 0) {
                if (!isprint(curr) && curr != 46) {
                    //printf("INODE #%d:", character);
                } else {
                    //printf("%c", character);
                }
            } else if (prev != 0 && curr != 0) {
                if (isprint(curr)) {
                    //printf("%c", character);
                }
            } else if (prev != 0 && curr == 0) {
                //printf("Add null terminator. String is done!\n");
                if (foundName[0] != '\0') {
                    //printf("%s\n", foundName);
                }
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
            //printf("NEW FILE/DIR: %d, %c\n", inode, type);
            iNodesList[index].inode = inode;
            iNodesList[index].type = type;
            index = index + 1;
        }
        char_count = char_count + 1;
    }

    if (ferror(file)) {
        perror("Error reading file");
        exit(1);
    }
    
    fclose(file);
}

void ls() {
    loadInode(currentINode.inode, currentINode.type, index);
}

void cd(const char *directory){

    if (strcmp(directory, ".") == 0){

    } else if (strcmp(directory, "..") == 0){
        currentINode.inode = currentINode.parentInode;
        currentINode.parentInode = iNodesList[(int)currentINode.inode].parentInode;
        currentINode.type = iNodesList[(int)currentINode.inode].type;
        strcpy(currentINode.name, iNodesList[(int)currentINode.inode].name);

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
        bool errorMessage = true;

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
                        errorMessage = false;
                        if (iNodesList[(int) (temp_inode)].type == 'f'){
                            printf("\nError: You cannot cd into a file. Ex: cd <directory>\n");
                            break;
                        }
                        currentINode.parentInode = currentINode.inode;
                        currentINode.inode = temp_inode;
                        currentINode.type = iNodesList[(int)(currentINode.inode)].type;
                        strcpy(currentINode.name, directory);

                        iNodesList[(int)(currentINode.inode)].parentInode = currentINode.parentInode;
                        strcpy(iNodesList[(int)(currentINode.inode)].name, directory);
                        
                        break; 
                    } else {
                        memset(foundName, 0, sizeof(foundName));
                    }
                }
            }
            char_count = char_count + 1;
            prev = character;
        }
        if(errorMessage == true){
            printf("\nError: directory doesn't exist. Use command 'ls' to see all available directories.");
        }

        if (ferror(file)) {
            perror("Error reading file");
            exit(1);
        }
        fclose(file);
    }
    printf("\n");
}

void mkdir(const char* directory) {
    // Check if directory already exists in the current directory
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%u", dir, currentINode.inode);
    FILE *dirFile = fopen(filepath, "rb");
    if (dirFile == NULL) {
        perror("Error opening directory file");
        return;
    }

    uint32_t temp_inode;
    char nameBuffer[MAX_NAME_LENGTH];
    while (fread(&temp_inode, sizeof(uint32_t), 1, dirFile) == 1) {
        fread(nameBuffer, sizeof(char), MAX_NAME_LENGTH, dirFile);
        if (strcmp(nameBuffer, directory) == 0) {
            printf("\nError: Directory '%s' already exists.\n", directory);
            fclose(dirFile);
            return;
        }
    }
    fclose(dirFile);

    strcpy(iNodesList[index].name, directory);
    iNodesList[index].parentInode = currentINode.inode;
    iNodesList[index].inode = (uint32_t) index;
    iNodesList[index].type = 'd';

    // Create a new file for the directory
    char *inodeStr = uint32_to_str((uint32_t) index);
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s", dir, inodeStr);

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(1);
    }

    // Write metadata for new directory
    char entry1[MAX_NAME_LENGTH] = ".";
    char entry2[MAX_NAME_LENGTH] = "..";
    fwrite(&index, sizeof(uint32_t), 1, file); // Write inode number
    fwrite(entry1, sizeof(char), MAX_NAME_LENGTH, file); // Write "."
    fwrite(&currentINode.inode, sizeof(uint32_t), 1, file); // Write parent inode number
    fwrite(entry2, sizeof(char), MAX_NAME_LENGTH, file); // Write ".."

    // Close the file
    fclose(file);

    // Update parent directory
    char *parentInodeStr = uint32_to_str(currentINode.inode);
    snprintf(filename, sizeof(filename), "%s/%s", dir, parentInodeStr);

    file = fopen(filename, "ab"); // Open in append mode
    if (file == NULL) {
        perror("Error opening parent directory file for writing");
        exit(1);
    }

    char entry3[MAX_NAME_LENGTH] = {'0'};
    strcpy(entry3, directory);

    // Append new directory entry to parent directory
    fwrite(&index, sizeof(uint32_t), 1, file); // Write new directory's inode number
    fwrite(entry3, sizeof(char), MAX_NAME_LENGTH, file); // Write new directory's name

    // Close the file
    fclose(file);
    
    // Update inodes_list file
    snprintf(filename, sizeof(filename), "%s/inodes_list", dir);
    file = fopen(filename, "ab");
    if (file == NULL) {
        perror("Error opening inodes_list file for writing");
        exit(1);
    }

    // Write new inode information to inodes_list
    fwrite(&index, sizeof(uint32_t), 1, file); // Write inode number
    char dirType = 'd';
    fwrite(&dirType, sizeof(char), 1, file); // Write directory type

    fclose(file);

    free(inodeStr);
    free(parentInodeStr);
    index++;
}

void touch(const char* filename) {
    // Check if file already exists in the current directory
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%u", dir, currentINode.inode);
    FILE *dirFile = fopen(filepath, "rb");
    if (dirFile == NULL) {
        perror("Error opening directory file");
        return;
    }

    uint32_t temp_inode;
    char nameBuffer[MAX_NAME_LENGTH];
    while (fread(&temp_inode, sizeof(uint32_t), 1, dirFile) == 1) {
        fread(nameBuffer, sizeof(char), MAX_NAME_LENGTH, dirFile);
        if (strcmp(nameBuffer, filename) == 0) {
            printf("\nError: File '%s' already exists.\n", filename);
            fclose(dirFile);
            return;
        }
    }
    fclose(dirFile);

    // Add to inodes list
    strcpy(iNodesList[index].name, filename);
    iNodesList[index].parentInode = currentINode.inode;
    iNodesList[index].inode = (uint32_t) index;
    iNodesList[index].type = 'f'; // 'f' for file

    // Create a new file for the inode
    char entry4[MAX_NAME_LENGTH] = {0};
    strcpy(entry4, filepath);
    snprintf(filepath, MAX_NAME_LENGTH, "%s/%d", dir, index);

    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(1);
    }

    // Write any initial content or metadata for the file
    // For example, writing the filename as initial content
    fwrite(filename, sizeof(char), strlen(filename), file);

    fclose(file);

    // Update parent directory
    char *parentInodeStr = uint32_to_str(currentINode.inode);
    snprintf(filepath, sizeof(filepath), "%s/%s", dir, parentInodeStr);

    file = fopen(filepath, "ab");
    if (file == NULL) {
        perror("Error opening parent directory file for writing");
        exit(1);
    }

    // Append new file entry to parent directory
    fwrite(&index, sizeof(uint32_t), 1, file); // Write new file's inode number
    fwrite(filename, sizeof(char), strlen(filename) + 1, file); // Write new file's name

    fclose(file);

    // Update inodes_list file
    snprintf(filepath, sizeof(filepath), "%s/inodes_list", dir);
    file = fopen(filepath, "ab");
    if (file == NULL) {
        perror("Error opening inodes_list file for writing");
        exit(1);
    }

    // Write new inode information to inodes_list
    fwrite(&index, sizeof(uint32_t), 1, file); // Write inode number
    char fileType = 'f';
    fwrite(&fileType, sizeof(char), 1, file); // Write file type

    fclose(file);
    free(parentInodeStr);
    index++;
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
    loadInodesList(filepath); //sets inode and type

    // Requirement 3: user starts at inode 0
    currentINode.inode = 0;
    currentINode.parentInode = 0;
    currentINode.type = iNodesList[(int)(currentINode.inode)].type;
    strcpy(currentINode.name, dir);

    iNodesList[(int)(currentINode.inode)].parentInode = 0;
    strcpy(iNodesList[(int)(currentINode.inode)].name, dir); // sets name to parent folder (empty or fs)

    if (currentINode.type == 'f'){
        perror("Error: Inode is a file, not a directory");
        return 1;
    } else {
        // Requirement 4: implementing exit, cd, ls, mkdir, touch
        char command[256];
        bool start = false;
        while (true) {
            if (start == false){
                printf("--> Enter command: ");
                start = true;
            } else {
                printf("\n--> Enter command: ");
            }
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = 0;

            if (strcmp(command, "exit") == 0) {
                return 0;
            } else if (strcmp(command, "ls") == 0){
                ls();
            } else if (strncmp(command, "cd ", 3) == 0) {
                const char *directory = command + 3;
                cd(directory);
            } else if (strncmp(command, "mkdir ", 6) == 0) {
                const char *directory = command + 6;
                mkdir(directory);
            } else if (strncmp(command, "touch ", 6) == 0) {
                const char *filename = command + 6;
                touch(filename);
            } else {
                printf("\nError: '%s' is an unknown command. Try Again.\n", command);
            }
        }
    }

    return 0;
}