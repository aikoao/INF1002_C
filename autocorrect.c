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
    "UPDATE",
    "QUERY",
    "INSERT",
    "DELETE",
    "HELP",
    "SAVE",
    "EXIT"
};

#define NUM_COMMANDS (sizeof(commands)/sizeof(commands[0]))
#define MAX_SUGGEST_LEN 300

// Simple case-insensitive Levenshtein-like distance
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

// Return a corrected full command with parameters preserved
char* autocorrect_command_prompt(const char *input) {
    if (!input || strlen(input) == 0) return NULL;

    const char *best = NULL;
    int best_dist = 1000;

    // Compare input to all command prefixes
    for (int i = 0; i < NUM_COMMANDS; i++) {
        int cmd_len = strlen(commands[i]);
        int prefix_len = cmd_len < strlen(input) ? cmd_len : strlen(input);

        // Compare only the input length against the command prefix
        char input_prefix[100];
        strncpy(input_prefix, input, prefix_len);
        input_prefix[prefix_len] = '\0';

        int d = string_distance(input_prefix, commands[i]);
        if (d < best_dist) {
            best_dist = d;
            best = commands[i];
        }
    }

    // Only suggest if distance small enough and input is not already a match
    if (best && strncasecmp(best, input, strlen(best)) != 0 && best_dist <= 3) {
        char *corrected = malloc(MAX_SUGGEST_LEN);
        if (!corrected) return NULL;

        // Preserve everything after first space (parameters)
        const char *space = strchr(input, ' ');
        if (space) {
            strcpy(corrected, best);
            strcat(corrected, space);
        } else {
            strcpy(corrected, best);
        }
        return corrected;
    }

    return NULL;
}
