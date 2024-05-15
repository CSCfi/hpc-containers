#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256

void printUsage() {
    printf("Usage: program_name <input_file_path> <output_file_path>\n");
}

int main(int argc, char *argv[]) {
    char inputFile[MAX_FILENAME_LENGTH];
    char outputFile[MAX_FILENAME_LENGTH];
    FILE *input, *output;
    double sum = 0.0;
    double num;
    int count = 0;

    if (argc != 3) {
        printUsage();
        return EXIT_FAILURE;
    }

    strcpy(inputFile, argv[1]);
    strcpy(outputFile, argv[2]);

    input = fopen(inputFile, "r");
    if (input == NULL) {
        printf("Error: Unable to open input file '%s'\n", inputFile);
        return EXIT_FAILURE;
    }

    output = fopen(outputFile, "w");
    if (output == NULL) {
        printf("Error: Unable to open output file '%s'\n", outputFile);
        fclose(input);
        return EXIT_FAILURE;
    }

    // Read numbers from input file and calculate sum
    while (fscanf(input, "%lf", &num) == 1) {
        sum += num;
        count++;
    }

    if (count == 0) {
        printf("Error: No data found in the input file\n");
        fclose(input);
        fclose(output);
        return EXIT_FAILURE;
    }

    // Calculate average
    double average = sum / count;

    // Write average to output file
    fprintf(output, "Average: %.2lf\n", average);

    printf("Average calculated and written to '%s'\n", outputFile);

    fclose(input);
    fclose(output);

    return EXIT_SUCCESS;
}
