# define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

// Global constants provided for assignment
#define PREFIX "movies_"
#define EXTENSION "csv"
#define UPPERRANGE 100000

// Custom colors for printing to console
#define YEL "\x1B[33m"
#define RED "\e[0;91m"
#define WHT "\x1B[0m"
#define GRN "\e[0;32m"
#define CYN "\e[0;96m"

/*
* Take the given file name and create a new directory
* Parse the information from the given file into unique year files created in the new directory
*/
int processFile(char* fileName) {

    // Attempt to open the file, if unable to open return warning
    FILE* filePtr = fopen(fileName, "r");
    if (filePtr == NULL) {
        printf("%sThe file %s\"%s\"%s not found, please try again%s\n", RED, CYN, fileName, RED, WHT);
        return 1;
    }

    // Opened successfully, begin processing the file
    printf("%sNow processing the chosen file named %s\"%s\"%s\n", YEL, GRN, fileName, WHT);
    
    // Initialize the random seed w/ system time and random from range 0 to given upper range
    srand(time(0));
    int randNum = rand() % UPPERRANGE;

    // Initialize the variables for parsing    
    char* currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char* token;
    char* title;

    // Initialize variables for new directoy
    char newDir[19];
    sprintf(newDir,"joyke.movies.%d", randNum);

    // Initialize variables for new file
    char* newFile;
    int file_descriptor;
    char* newLine;

    // Create a new directory with rwxr-w--- permission
    mkdir(newDir, 0750);
    printf("%sCreated directory with name %s\"%s\"%s\n", YEL, GRN, newDir, WHT);

    // Read the header line of the file in but don't parse
    nread = getline(&currLine, &len, filePtr);

    // Read the remaining file line by line
    while ((nread = getline(&currLine, &len, filePtr)) != -1) {

        // Parse and save the title from currLine
        char* token = strtok(currLine, ",");
        title = calloc(strlen(token)+1, sizeof(char));
        strcpy(title,token);

        // Parse the year from currLine
        token = strtok(NULL, ",");

        // Print to file path to new file
        newFile = calloc(strlen(newDir)+strlen(token)+strlen("/.txt") + 1, sizeof(char));
        sprintf(newFile,"%s/%s.txt", newDir, token);

        /* Append to the file if it exists, create it with rw-r----- rights if it doesn't
        print warning and return if file can't be opened or created */
        file_descriptor = open(newFile, O_RDWR | O_CREAT | O_APPEND, 0640);
        if (file_descriptor == -1){
            printf("%sThe file %s\"%s\"%s not found, please try again%s\n", RED, CYN, fileName, RED, WHT);
            return 1;
        }

        // Write the movie title to the file
        newLine = calloc(strlen(title)+2, sizeof(char));
        strcpy(newLine, title);
        strcat(newLine, "\n");
        write(file_descriptor, newLine, strlen(title)+1);

        // Free dynamic memory assigned in loop
        free(title);  
        free(newFile);   
        free(newLine);
    }
    // Close file
    fclose(filePtr);

    // Free dynamic memory
    free(currLine);
    return 0;
}

/* 
* Prompt the user to enter a specific file name to be processed
*/
int specificFile(void) {

    // Limit the file name to the max file name length in linux
    char fileName[255];
    printf("\nEnter the complete file name: ");
    scanf("%s", fileName);
    return processFile(fileName); 
}

/*
* Find the smallest file in the directory with the proper prefix and extension and process it
*/
void smallestFileName(void) {

    // Initialize variables
    char* smallestFile = NULL;
    long long int smallestSize = -1;

    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent* dirItem;
    struct stat dirItemStat;

    // Loop through all items in the current directory
    while((dirItem = readdir(currDir)) != NULL) {

        // Find the name and extension of the child directory being examined
        char* itemName = dirItem->d_name;
        char* itemExt = itemName + strlen(itemName) - strlen(EXTENSION);
    
        // Check each item in the directory for the given prefix and extension
        if (strncmp(PREFIX, itemName, strlen(PREFIX)) == 0 && strcmp(EXTENSION, itemExt) == 0) {
            stat(dirItem->d_name, &dirItemStat);

            // Is this the current smallest file? If yes record it.
            if (dirItemStat.st_size < smallestSize || smallestSize == -1) {
                
                // Free dynamic allocation each loop
                free(smallestFile);
    
                // Re-allocate dynamic memory and record new file name
                smallestFile = calloc(strlen(dirItem->d_name) + 1, sizeof(char));
                strcpy(smallestFile, dirItem->d_name);
                smallestSize = dirItemStat.st_size;
            }
        }
    }

    // Was there a file matching the prefix and extension?
    if (smallestFile == NULL) {

        // There was no matching file, print warning
        printf("%sThere are no matches in the current directory for files with prefix %s\"%s\"%s and extension %s\"%s\"%s%s\n", RED, CYN, PREFIX, RED, CYN, EXTENSION, RED, WHT);
    } else {

        // There was a matching file, process it
        processFile(smallestFile);
    }

    // Close directory in use
    closedir(currDir);

    // Free remaining dynamic memory allocated
    free(smallestFile);
}

/*
* Find the largest file in the directory with the proper prefix and extension and process it
*/
void largestFileName(void) {

    // Initialize variables
    char* largestFile = NULL;
    long long int largestSize = 0;

    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent* dirItem;
    struct stat dirItemStat;

    // Loop through all items in the current directory
    while((dirItem = readdir(currDir)) != NULL) {
                
        // Find the name and extension of the child directory being examined
        char* itemName = dirItem->d_name;
        char* itemExt = itemName + strlen(itemName) - strlen(EXTENSION);
    
        // Check each item in the directory for the given prefix and extension
        if (strncmp(PREFIX, itemName, strlen(PREFIX)) == 0 && strcmp(EXTENSION, itemExt) == 0) {
            stat(dirItem->d_name, &dirItemStat);

            // Is this the current largest file? If yes record it
            if (dirItemStat.st_size > largestSize) {

                // Free dynamic allocation each loop
                free(largestFile);
    
                // Re-allocate dynamic memory and record new file name
                largestFile = calloc(strlen(dirItem->d_name) + 1, sizeof(char));
                strcpy(largestFile, dirItem->d_name);
                largestSize = dirItemStat.st_size;
            }
        }
    }

    // Was there a file matching the prefix and extension?
    if (largestFile == NULL) {

        // There was no matching file, print warning
        printf("%sThere are no matches in the current directory for files with prefix %s\"%s\"%s and extension %s\"%s\"%s%s\n", RED, CYN, PREFIX, RED, CYN, EXTENSION, RED, WHT);
    } else {

        // There was a matching file, process it
        processFile(largestFile);
    }

    // Close directory in use
    closedir(currDir);

    // Free remaining dynamic memory allocated
    free(largestFile);
}

/*
* Prompt the user with choices on which file to process
*/
void fileChoice(void) {
    
    int choice = 0;
    int tryAgain = 0;

    // Loop the choices until the user chooses a valid response
    while (choice == 0 || tryAgain == 1) {
        
        printf("\nWhich file do you want to process?\n"
        "1. Process the largest file\n"
        "2. Process the smallest file\n"
        "3. Specify the name of a file to process\n"
        "\nEnter a choice from 1 to 3: ");

        scanf("%d", &choice);

        // Is the input valid?
        if (choice < 1 || choice > 3) {

            // No, give a warning
            printf("%sInvalid choice, please choose again\n%s", RED, WHT);
            choice = 0;
        } else {

            // Yes, call the appropriate case for each valid response
            switch (choice) {
                case 1:
                    largestFileName();
                    tryAgain = 0;
                    break;
                case 2:
                    smallestFileName();
                    tryAgain = 0;
                    break;
                case 3:
                    // If specific file name not provided, try again
                    tryAgain = specificFile();
            }
        }
    }
}

/*
* Prompt the user with a main menu until the user exits the program
*/
void menu(void) {

    int choice = 0, end = 0;

    /* Contiue prompting the user with the main menu until a valid input is given,
    continue bringing the main menu back until the user exits */
    while (choice == 0 || end == 0) {

        printf("\n1. Select file to process\n"
                "2. Exit the program\n"
                "\nEnter a choice 1 or 2: ");

        scanf("%d", &choice);
        
        // Is the user input valid?
        if (choice < 1 || choice > 2) {

            // No, provide a warning
            printf("%sInvalid choice, please choose again%s\n", RED, WHT);
            choice = 0;
        } else {

            // Yes, call the appropriate case for the response.
            switch(choice) {
                case 1: 
                    fileChoice();
                    break;
                default:
                    end++; 
            }
        }
    }
    printf("\n");
}

/*
* Process file selected
* Create new directories with each file processed
* Within the directory create new year.txt files containing movies from that year
*   Compile the program as follows:
*       gcc --std=c99 -o files files.c
*/
int main(void) {
    menu();
    
    return EXIT_SUCCESS;
}