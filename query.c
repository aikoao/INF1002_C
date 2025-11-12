#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "query.h"

void queryStudentByID(Student students[], int student_count) {
    char idInput[100];
    int queryId, found = 0, valid = 0;
    while (!valid) {
        printf("Enter the student ID to search (7 digits): ");
        fgets(idInput, sizeof(idInput), stdin);
        idInput[strcspn(idInput, "\n")] = 0; // Remove newline

        int len = strlen(idInput);
        valid = 1;
        if (len != 7) valid = 0;
        for (int i = 0; i < len && valid; i++) {
            if (!isdigit((unsigned char)idInput[i])) valid = 0;
        }
        if (!valid)
            printf("Please input a proper ID (2xxxxxx)\n");
    }
    sscanf(idInput, "%d", &queryId);

    for (int i = 0; i < student_count; i++) {
        if (students[i].id == queryId) {
            printf("Record Found:\n");
            printf("%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n",
                students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Warning: No record found with student ID %d.\n", queryId);
    }
}

void queryStudentByName(Student students[], int student_count) {
    char queryName[NAME_LENGTH];
    int found = 0;
    printf("Enter the student name to search: ");
    fgets(queryName, sizeof(queryName), stdin);
    queryName[strcspn(queryName, "\n")] = 0;

    for (int i = 0; i < student_count; i++) {
        if (strcasecmp(students[i].name, queryName) == 0) {
            if (!found) {
                printf("Record(s) Found:\n");
                printf("%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            }
            printf("%-10d %-22s %-26s %-8.2f\n",
                   students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    if (!found) {
        printf("Warning: No record found with student name \"%s\".\n", queryName);
    }
}

void queryStudentByProgramme(Student students[], int student_count) {
    char queryProg[PROGRAMME_LENGTH];
    int found = 0;
    printf("Enter the programme to search: ");
    fgets(queryProg, sizeof(queryProg), stdin);
    queryProg[strcspn(queryProg, "\n")] = 0;

    for (int i = 0; i < student_count; i++) {
        if (strcasecmp(students[i].programme, queryProg) == 0) {
            if (!found) {
                printf("Record(s) Found:\n");
                printf("%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            }
            printf("%-10d %-22s %-26s %-8.2f\n",
                students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    if (!found) {
        printf("Warning: No record found with programme \"%s\".\n", queryProg);
    }
}