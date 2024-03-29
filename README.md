# C_File_System_Emulator

### Description:
The "C_File_System_Emulator" GitHub repository features a comprehensive project that emulates a basic file system using C programming. This project intricately demonstrates the application of file system concepts, inode metadata management, and directory manipulation, showcasing a deep understanding of these areas in C.

## Project Components and Functionalities:

Inode and File Type Management: The program efficiently manages a fixed number of inodes, each signifying either a directory or a file. The metadata for these inodes comprises an index and a type ('d' for directory, 'f' for file).

Directory Structure Implementation: Directories are represented as special files. Each directory entry contains an inode number and a file name, adhering to length constraints for compatibility with the directory format. These entries are stored in binary format and accessed using read/fread functions.

File System Initialization: At launch, the program begins in the root directory (inode 0) and loads the inodes_list file. It verifies inode numbers and file type indicators, reporting and disregarding any invalid entries.

User Command Processing: The project includes functionalities to process user commands such as cd for changing directories, ls for listing contents, mkdir for creating directories, and touch for creating files. Each command is designed to interact with the emulated file system, updating inodes and directory contents as necessary.

State Preservation on Exit: When exiting the program (via the exit command or EOF), it updates the inode list to reflect the session's changes, ensuring persistence of the file system state.

Utility Functions for Memory and Data Management: Included are utility functions like checked_malloc for safe memory allocation and uint32_to_str for converting integers into strings, crucial for handling inode-related data.

Handling File System Directory Argument: The program accepts a single command-line argument specifying the directory storing the emulated file system, with verification to ensure the directory's existence.

### Repository Contents:

- Source code for the file system emulator.
- Utility functions to facilitate memory management and data conversion.
- Comprehensive documentation detailing implementation, usage instructions, and command functionalities.
- Tests and examples to demonstrate the emulator's capabilities.

NOTE: Requirement #5: update inodes_list file is not met when we exit the program. 
Rather the update is done dynamically as commands touch and mkdir are used in the emulated terminal.

To compile:

    gcc -std=c99 -Wall -pedantic -o main main.c -lm


To Run:

./main fs2


EXAMPLE: 
--> Enter command: ls
.
..

357

alex

jake

--> Enter command: cd jake


--> Enter command: ls

.
..

jakesdiary

--> Enter command: cd jakesdiary


--> Enter command: ls

.

..

poopedtoday

ligmaballs

--> Enter command: cd ligmaballs


--> Enter command: ls

.

..

--> Enter command: touch fart

--> Enter command: ls

.

..

fart

--> Enter command: exit
