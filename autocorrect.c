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

// Autocorrect: finds the closest command prefix and preserves parameters
char* autocorrect_command(const char *input) {
    if (!input || strlen(input) == 0) return NULL;

    const char *best = NULL;
    int best_dist = 1000;
    int best_len = 0;

    // Compare input to all commands and pick the closest match
    for (int i = 0; i < NUM_COMMANDS; i++) {
        int cmd_len = strlen(commands[i]);
        int len_to_compare = strlen(input) < cmd_len ? strlen(input) : cmd_len;

        int dist = string_distance(input, commands[i]);
        if (dist < best_dist) {
            best_dist = dist;
            best = commands[i];
            best_len = cmd_len;
        }
    }

    // Suggest only if distance is small and input is not already correct
    if (best && string_distance(input, best) > 0 && best_dist <= 4) {
        char *corrected = malloc(MAX_SUGGEST_LEN);
        if (!corrected) return NULL;

        // Copy the best matching command
        strcpy(corrected, best);

        // Attach remaining input as parameters
        const char *params = input + strlen(best);
        while (*params == ' ') params++;
        if (*params) {
            strcat(corrected, " ");
            strcat(corrected, params);
        }

        return corrected;
    }

    return NULL;
}
