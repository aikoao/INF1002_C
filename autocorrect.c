#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "autocorrect.h"

// List of valid commands
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

int string_distance(const char *a, const char *b) {
    // Simple Levenshtein-like distance for small strings
    int dist = 0;
    int len_a = strlen(a);
    int len_b = strlen(b);
    int min_len = len_a < len_b ? len_a : len_b;

    for (int i = 0; i < min_len; i++) {
        if (toupper(a[i]) != toupper(b[i]))
            dist++;
    }
    dist += abs(len_a - len_b);
    return dist;
}

char* autocorrect_command_prompt(const char *input) {
    if (input == NULL || strlen(input) == 0) return NULL;

    const char *best_match = NULL;
    int best_distance = 1000;

    for (int i = 0; i < NUM_COMMANDS; i++) {
        int dist = string_distance(input, commands[i]);
        if (dist < best_distance) {
            best_distance = dist;
            best_match = commands[i];
        }
    }

    // Only suggest if it's actually different
    if (best_match != NULL && strcasecmp(best_match, input) != 0 && best_distance <= 3) {
        return (char*)best_match;
    }

    return NULL;
}
