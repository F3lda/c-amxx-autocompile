// This program uses _popen and _pclose to receive a stream of text from a system process.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(void)
{
    char psBuffer[512];
    FILE* pPipe;

    if ((pPipe = _popen("echo. | compile.exe", "rt")) == NULL) // "echo. | " -> invokes enter key press
    {
        printf("Error: pipe error!\n");
        
        printf("\nPress any key to finish...");
        scanf("%*c");
        exit(1);
    }

        
    /* Read pipe until end of file, or an error occurs. */
    char fileName[128] = {0};
    while (fgets(psBuffer, 512, pPipe))
    {
        if (strncmp(psBuffer, "//// ", 5) == 0) {
            strncpy(fileName, &psBuffer[5], 127);
        } else if (strncmp(psBuffer, "// Done.", 8) == 0) {
            // move file
            printf("FILE COMPILED: %s---------------------------------\n\n\n", fileName);
            char filePath[255], newFilePath[255];
            *(strrchr(fileName, '.')) = '\0';
            snprintf(filePath, 255, "./compiled/%s.amxx", fileName);
            snprintf(newFilePath, 255, "./../plugins/%s.amxx", fileName);
            remove(newFilePath);
            if (rename(filePath, newFilePath) != 0) {
               perror("Error renaming file");
               printf("Error code: %d\n", errno);
            }
        } else if (strstr(psBuffer, "error") != NULL) {
            printf("%s", psBuffer);
        } else if (strstr(psBuffer, "Error") != NULL) {
            printf("%s---------------------------------\n\n\n", psBuffer);
        }
        //puts(psBuffer);
    }

    int endOfFileVal = feof(pPipe);
    int closeReturnVal = _pclose(pPipe);

    if (endOfFileVal)
    {
        printf("\nProcess returned %d\n", closeReturnVal);
    }
    else
    {
        printf("\nError: Failed to read the pipe to the end.\n");
    }
    
    printf("\nPress any key to finish...");
    scanf("%*c");
    
    return 0;
}
