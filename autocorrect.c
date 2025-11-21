#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "autocorrect.h"

const char *commands[] = {
    "OPEN",
    "SHOW ALL",
    "SHOW ALL SORT BY ID (ASCENDING)",
    "SHOW ALL SORT BY ID (DESCENDING)",
    "SHOW ALL SORT BY MARK (ASCENDING)",
    "SHOW ALL SORT BY MARK (DESCENDING)",
    "SHOW ALL SORT BY NAME (ASCENDING)",
    "SHOW ALL SORT BY NAME (DESCENDING)",
    "SHOW ALL SORT BY PROGRAMME (ASCENDING)",
    "SHOW ALL SORT BY PROGRAMME (DESCENDING)",
    "SHOW SUMMARY",
    "UPDATE",
    "QUERY",
    "INSERT",
    "DELETE",
    "UNDO",
    "HELP",
    "SAVE",
    "EXIT"
};

#define NUM_COMMANDS (sizeof(commands)/sizeof(commands[0]))
#define MAX_SUGGEST_LEN 300

// Case-insensitive Levenshtein-style distance
int string_distance(const char *a, const char *b) {
    int dist = 0;
    int len_a = strlen(a);
    int len_b = strlen(b);
    int min_len = len_a < len_b ? len_a : len_b;

    for (int i = 0; i < min_len; i++)
        if (toupper(a[i]) != toupper(b[i]))
            dist++;

    dist += abs(len_a - len_b);
    return dist;
}

char* autocorrect_command(const char *input) {
    if (!input || strlen(input) == 0) return NULL;

    const char *best = NULL;
    int best_dist = 1000;

    // Loop through all commands
    for (int i = 0; i < NUM_COMMANDS; i++) {
        int len = strlen(commands[i]);
        int compare_len = len < strlen(input) ? len : strlen(input);

        char command_prefix[100];
        strncpy(command_prefix, commands[i], compare_len);
        command_prefix[compare_len] = '\0';

        int d = string_distance(input, command_prefix);
        if (d < best_dist) {
            best_dist = d;
            best = commands[i];
        }
    }

    // Check if input already starts with the best command
    if (best && strncasecmp(input, best, strlen(best)) == 0) {
        // Input already matches the command prefix, do not suggest
        return NULL;
    }

    // Only suggest if distance is small
    if (best && best_dist <= 3) {
        char *corrected = malloc(MAX_SUGGEST_LEN);
        if (!corrected) return NULL;

        // Count words in best command
        int words_in_best = 0;
        for (int i = 0; best[i]; i++)
            if (best[i] == ' ') words_in_best++;
        words_in_best++; // total words

        // Skip same number of words in input to preserve parameters
        const char *ptr = input;
        int skip_words = words_in_best;
        while (*ptr && skip_words > 0) {
            if (*ptr == ' ') skip_words--;
            ptr++;
        }
        while (*ptr == ' ') ptr++; // skip extra spaces

        strcpy(corrected, best);
        if (*ptr) {
            strcat(corrected, " ");
            strcat(corrected, ptr);
        }

        return corrected;
    }

    return NULL;
}
