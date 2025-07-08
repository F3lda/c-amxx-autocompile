// main.c
/*
 * A C utility that wraps the AMX Mod X compiler (compile.exe) to automate plugin compilation and deployment.
 * Uses popen to execute compiler and parse its output for file copying.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512
#define FILENAME_SIZE 128
#define FILEPATH_SIZE 255
#define COMPILER_COMMAND "echo. | compile.exe" // "echo. | " -> invokes enter key press
#define COMPILED_DIR "./compiled"
#define PLUGINS_DIR "./../plugins"

// Function prototypes
static int copy_compiled_file(const char* filename);

int main(void)
{
    char buffer[BUFFER_SIZE];
    char current_filename[FILENAME_SIZE] = {0};
    
    FILE* pipe = _popen(COMPILER_COMMAND, "rt");
    if (pipe == NULL) {
        fprintf(stderr, "Error: Failed to open pipe to compiler\n");
        printf("\nPress any key to finish...");
        fflush(stdout);
        scanf("%*c");
        return 1;
    }
    
    // Process compiler output
    while (fgets(buffer, sizeof(buffer), pipe)) {
        if (strncmp(buffer, "//// ", 5) == 0) {
            // Extract filename from compiler output
            strncpy(current_filename, &buffer[5], sizeof(current_filename) - 1);
            current_filename[sizeof(current_filename) - 1] = '\0';
            
            // Remove trailing whitespace and newlines
            size_t len = strlen(current_filename);
            while (len > 0 && (current_filename[len-1] == '\n' || current_filename[len-1] == '\r' || 
                               current_filename[len-1] == ' ' || current_filename[len-1] == '\t')) {
                current_filename[--len] = '\0';
            }
            
        } else if (strncmp(buffer, "// Done.", 8) == 0) {
            // Compilation successful
            if (strlen(current_filename) == 0) {
                fprintf(stderr, "Warning: No filename available for compiled file\n");
            } else {
                printf("FILE COMPILED: %s\n", current_filename);
                printf("---------------------------------\n\n\n");
                
                if (copy_compiled_file(current_filename) != 0) {
                    fprintf(stderr, "Warning: Failed to copy compiled file\n");
                }
            }
            memset(current_filename, 0, sizeof(current_filename));
            
        } else if (strstr(buffer, "error") != NULL) {
            // Print lowercase error messages
            printf("%s\n", buffer);
            
        } else if (strstr(buffer, "Error") != NULL) {
            // Handle capitalized error messages with separator
            printf("%s\n", buffer);
            printf("---------------------------------\n\n\n");
        }
    }
    
    int end_of_file = feof(pipe);
    int close_result = _pclose(pipe);
    
    if (end_of_file) {
        printf("\nCompiler process returned %d\n", close_result);
    } else {
        fprintf(stderr, "\nError: Failed to read pipe to completion\n");
    }
    
    printf("\nPress any key to finish...");
    fflush(stdout);
    scanf("%*c");
    
    return (end_of_file && close_result == 0) ? 0 : 1;
}

static int copy_compiled_file(const char* filename)
{
    char source_path[FILEPATH_SIZE];
    char dest_path[FILEPATH_SIZE];
    char base_filename[FILENAME_SIZE];
    
    // Create base filename without extension
    strncpy(base_filename, filename, sizeof(base_filename) - 1);
    base_filename[sizeof(base_filename) - 1] = '\0';
    
    char* dot_pos = strrchr(base_filename, '.');
    if (dot_pos != NULL) {
        *dot_pos = '\0';
    }
    
    // Construct file paths
    int source_result = snprintf(source_path, sizeof(source_path), 
                                "%s/%s.amxx", COMPILED_DIR, base_filename);
    int dest_result = snprintf(dest_path, sizeof(dest_path), 
                              "%s/%s.amxx", PLUGINS_DIR, base_filename);
    
    if (source_result >= sizeof(source_path) || dest_result >= sizeof(dest_path)) {
        fprintf(stderr, "Error: File path too long\n");
        return -1;
    }
    
    // Open source file for reading
    FILE* source = fopen(source_path, "rb");
    if (source == NULL) {
        perror("Error opening source file");
        fprintf(stderr, "Source: %s\n", source_path);
        return -1;
    }
    
    // Open destination file for writing
    FILE* dest = fopen(dest_path, "wb");
    if (dest == NULL) {
        perror("Error opening destination file");
        fprintf(stderr, "Destination: %s\n", dest_path);
        fclose(source);
        return -1;
    }
    
    // Copy file contents
    char buffer[4096];
    size_t bytes_read;
    int copy_success = 1;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dest) != bytes_read) {
            perror("Error writing to destination file");
            copy_success = 0;
            break;
        }
    }
    
    // Check for read errors
    if (ferror(source)) {
        perror("Error reading from source file");
        copy_success = 0;
    }
    
    // Close files
    fclose(source);
    fclose(dest);
    
    if (!copy_success) {
        // Remove partially written destination file on error
        remove(dest_path);
        return -1;
    }
    
    return 0;
}
