#include <stdio.h>
#include <stdlib.h>


typedef struct {
    uint32_t inode;
    uint32_t parentInode;
    char type;
    char name[32];
} Inode;

void loadInodeList(const char *path) {
    // read the inodeList from the file path to your inodeList array.
    // update the inodeCount.
    // remember that your directory will have only one
    // inodeList with a given number of inodes in the list.
}

void saveInodeList(const char *path) {
    // write the content of the inodeList to the file path.
}

void changeDirectory(const char *name) {
    // iterate through the inodeList to search
    // whether a directory with the name already exists.
    // if yes, then set the currentInode value
    // to the index of the list with that name.
    // otherwise print an error message.
}
void listContents() {
    // iterate through inodeList
    // and print the members of those elements in the list
    // for which the parentInode is the currentInode.
}

void createDirectory(const char *name) {
    // iterate through the inodeList to see if the name of the directory exists; if yes, exit.
    // check to see if the inodeCount is 1024; if yes, exit.
    // create a new inode with new inodeCount, set its parentInode and type, and also its name.
    // create a file with its name as the inode number.
    // write into this file its . and .. inode values.
}

void createFile(const char *name) {
    // iterate through inodeList to see if filename exists; if yes, exit.
    // check if the inodeCount is 1024; if yes, exit.
    // create a new entry for an inode in inodeList.
    // create a file with its name as the inode number.
    // write into this file its filename.
}

int main (void){
    // initialize variables.
    Inode inodeList[1024]; // list of 1024 inodes
    size_t inodeCount = 0; // current size of list
    uint32_t currentInode = 0; // pointer to track which inode we are on
}