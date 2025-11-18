#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"

void delete_record(const char* command) {
    int id = -1;

    char upper[50];
    strcpy(upper, command);
    for (int i = 0; upper[i]; i++)
        upper[i] = toupper(upper[i]);

    if (strncmp(upper, "DELETE ID=", 10) != 0) {
        printf("CMS: Wrong delete format. Use: DELETE ID=xxxxxxx\n");
        return;
    }

    id = atoi(command + 10);

    int index = -1;
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }

    // Save undo state BEFORE deletion
    char description[50];
    snprintf(description, sizeof(description), "DELETE ID=%d", id);
    save_undo_state(description);

    // Confirmation
    printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to confirm or type \"N\" to cancel.\n", id);
    printf("P4_8: ");
    char confirm[10];
    fgets(confirm, sizeof(confirm), stdin);
    confirm[strcspn(confirm, "\n")] = '\0';

    for (int i = 0; confirm[i]; i++)
        confirm[i] = toupper(confirm[i]);

    if (strcmp(confirm, "Y") != 0) {
        printf("CMS: The deletion is cancelled.\n");
        return;
    }

    // Delete record
    for (int i = index; i < student_count - 1; i++) {
        students[i] = students[i + 1];
    }
    student_count--;

    printf("CMS: The record with ID=%d is successfully deleted.\n", id);
}

