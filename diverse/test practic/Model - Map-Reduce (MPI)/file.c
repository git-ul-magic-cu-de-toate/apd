#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 100
#define NUM_MAPPERS 3
#define REDUCER_1 4
#define REDUCER_2 5

// Function to count vowels and consonants
void count_vowels_and_consonants(const char *line, int *vowel_count, int *consonant_count) {
    *vowel_count = 0;
    *consonant_count = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        char c = tolower(line[i]);
        if (isalpha(c)) {
            if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
                (*vowel_count)++;
            } else {
                (*consonant_count)++;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 6) {
        if (rank == 0) {
            fprintf(stderr, "This program requires exactly 6 MPI tasks.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (rank == 0) {
        // Task 0: Coordinator
        if (argc < 2) {
            fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
            MPI_Finalize();
            return EXIT_FAILURE;
        }

        // Read the input file
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            MPI_Finalize();
            return EXIT_FAILURE;
        }

        int num_lines;
        fscanf(file, "%d\n", &num_lines);

        // Read all lines
        char lines[num_lines][MAX_LINE_LENGTH];
        for (int i = 0; i < num_lines; i++) {
            fgets(lines[i], MAX_LINE_LENGTH, file);
            lines[i][strcspn(lines[i], "\n")] = '\0';  // Remove newline
        }
        fclose(file);

        // Distribute lines to mappers
        for (int i = 0; i < num_lines; i++) {
            int target_mapper = 1 + (i % NUM_MAPPERS);  // Round-robin
            MPI_Send(lines[i], MAX_LINE_LENGTH, MPI_CHAR, target_mapper, 0, MPI_COMM_WORLD);
        }

        // Signal end of work to mappers
        for (int i = 1; i <= NUM_MAPPERS; i++) {
            MPI_Send("", 1, MPI_CHAR, i, 1, MPI_COMM_WORLD);  // Empty message with tag 1
        }
    } else if (rank >= 1 && rank <= NUM_MAPPERS) {
        // Mappers
        int vowel_count_total = 0;
        int consonant_count_total = 0;

        while (1) {
            char line[MAX_LINE_LENGTH];
            MPI_Status status;

            // Receive a line from Task 0
            MPI_Recv(line, MAX_LINE_LENGTH, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == 1) {
                break;  // End of work signal
            }

            // Count vowels and consonants in the line
            int vowels, consonants;
            count_vowels_and_consonants(line, &vowels, &consonants);
            vowel_count_total += vowels;
            consonant_count_total += consonants;
        }

        // Send results to reducers
        MPI_Send(&vowel_count_total, 1, MPI_INT, REDUCER_1, 0, MPI_COMM_WORLD);
        MPI_Send(&consonant_count_total, 1, MPI_INT, REDUCER_2, 0, MPI_COMM_WORLD);
    } else if (rank == REDUCER_1 || rank == REDUCER_2) {
        // Reducers
        int total = 0;
        for (int i = 1; i <= NUM_MAPPERS; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += count;
        }

        // Print results
        if (rank == REDUCER_1) {
            printf("Total vowels: %d\n", total);
        } else if (rank == REDUCER_2) {
            printf("Total consonants: %d\n", total);
        }
    }

    MPI_Finalize();
    return 0;
}
