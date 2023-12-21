#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *pipe;
    char buffer[128];

    // Open a pipe to execute the Python script
    pipe = popen("python FakeMicrophone.py", "r");
    if (!pipe) {
        fprintf(stderr, "Error: Failed to open pipe.\n");
        return EXIT_FAILURE;
    }

    // Read the output of the Python script line by line
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        // Process the output from the Python script (e.g., print or do something else)
        printf("Python Output: %s", buffer);
        // Add your own logic here to process the output

        // Flush the buffer if necessary
        fflush(stdout);
    }

    // Close the pipe
    pclose(pipe);

    return EXIT_SUCCESS;
}