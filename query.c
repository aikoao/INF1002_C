#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "query.h"


void gradeRange(const char *grade, float *min, float *max) {
    if (strcmp(grade, "A+") == 0) {*min = 85; *max = 100;}
    else if (strcmp(grade, "A") == 0) {*min = 80; *max = 84;}
    else if (strcmp(grade, "A-") == 0) {*min = 75; *max = 79;}
    else if (strcmp(grade, "B+") == 0) {*min = 70; *max = 74;}
    else if (strcmp(grade, "B") == 0) {*min = 65; *max = 69;}
    else if (strcmp(grade, "B-") == 0) {*min = 60; *max = 64;}
    else if (strcmp(grade, "C+") == 0) {*min = 55; *max = 59;}
    else if (strcmp(grade, "C") == 0) {*min = 50; *max = 54;}
    else if (strcmp(grade, "D+") == 0) {*min = 50; *max = 54;}  // D+, D, C all have same range in your table
    else if (strcmp(grade, "D") == 0) {*min = 50; *max = 54;}
    else if (strcmp(grade, "F") == 0) {*min = 0; *max = 49;}
    else {*min = -1; *max = -1;}
}

void query_process(const char* input, Student students[], int student_count) {
    if (strstr(input, "ID=") != NULL) {
        const char* idpos = strstr(input, "ID=") + 3;
        while (*idpos == ' ') idpos++;
        queryStudentByID(students, student_count, idpos);
    }
    else if (strstr(input, "NAME=") != NULL) {
        const char* namepos = strstr(input, "NAME=") + 5;
        while (*namepos == ' ') namepos++;
        queryStudentByName(students, student_count, namepos);
    }
    else if (strstr(input, "PROGRAMME=") != NULL) {
        const char* progpos = strstr(input, "PROGRAMME=") + 10;
        while (*progpos == ' ') progpos++;
        queryStudentByProgramme(students, student_count, progpos);
    }
    else if (strstr(input, "GRADE=") != NULL) {
        const char* gradepos = strstr(input, "GRADE=") + 6;
        while (*gradepos == ' ') gradepos++;
        queryStudentByGrade(students, student_count, gradepos);
    }
    else {
        printf("Invalid QUERY format. Use QUERY ID=xxxxxxx, QUERY NAME=..., QUERY PROG=..., QUERY GRADE=...\n");
    }
}

void queryStudentByID(Student students[], int student_count, const char* idInput) {
    int len = strlen(idInput), valid = 1, queryId, found = 0;
    if (len != 7) valid = 0;
    for (int i = 0; i < len && valid; i++)
        if (!isdigit((unsigned char)idInput[i])) valid = 0;
    if (!valid) { printf("Please input a proper ID (2xxxxxx)\n"); return; }
    sscanf(idInput, "%d", &queryId);
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == queryId) {
            printf("Record Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
            break;
        }
    }
    if (!found) { printf("Warning: No record found with student ID %d.\n", queryId); }
}

void queryStudentByName(Student students[], int student_count, const char* queryName) {
    int found = 0;
    for (int i = 0; i < student_count; i++) {
        if (strcasecmp(students[i].name, queryName) == 0) {
            if (!found) printf("Record(s) Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    if (!found) { printf("Warning: No record found with student name \"%s\".\n", queryName); }
}

void queryStudentByProgramme(Student students[], int student_count, const char* queryProg) {
    int found = 0;
    for (int i = 0; i < student_count; i++) {
        if (strcasecmp(students[i].programme, queryProg) == 0) {
            if (!found) printf("Record(s) Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    if (!found) { printf("Warning: No record found with programme \"%s\".\n", queryProg); }
}

void queryStudentByGrade(Student students[], int student_count, const char* gradeInput) {
    float minScore, maxScore; int found = 0;
    char grade[4]; strncpy(grade, gradeInput, 3); grade[3] = '\0';
    for (int i = 0; grade[i]; i++) grade[i] = toupper((unsigned char)grade[i]);
    gradeRange(grade, &minScore, &maxScore);
    if (minScore < 0) { printf("Invalid grade entered.\n"); return; }
    for (int i = 0; i < student_count; i++) {
        if (students[i].mark >= minScore && students[i].mark <= maxScore) {
            if (!found) printf("Record(s) Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    if (!found) { printf("Warning: No record found for grade %s.\n", gradeInput); }
}
