#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "query.h"

char *strcasestr(const char *haystack, const char *needle) {
    if (!*needle) {
        return (char *)haystack;  // empty needle matches at start
    }

    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && (tolower((unsigned char)*h) == tolower((unsigned char)*n))) {
            h++;
            n++;
        }
        if (!*n) {
            return (char *)haystack;  // match found
        }
    }
    return NULL;  // no match
}

void trimSpaces(char *str) {
    // Remove starting spaces
    int start = 0;
    while (isspace((unsigned char)str[start])) start++;
    if (start > 0) memmove(str, str + start, strlen(str + start) + 1);
    // Remove trailing spaces
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}
void reduceInnerSpaces(char *str) {
    int i = 0, j = 0, in_space = 0;
    while (str[i]) {
        if (isspace((unsigned char)str[i])) {
            if (!in_space) { str[j++] = ' '; in_space = 1; }
        } else {
            str[j++] = str[i];
            in_space = 0;
        }
        i++;
    }
    str[j] = '\0';
}
void removeAllSpaces(char *str) {
    int i, j=0;
    for (i = 0; str[i]; i++) {
        if (!isspace((unsigned char)str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void gradeRange(const char *grade, float *min, float *max) {
    if (strcmp(grade, "A+") == 0) {*min = 85; *max = 100;}
    else if (strcmp(grade, "A") == 0) {*min = 80; *max = 84;}
    else if (strcmp(grade, "A-") == 0) {*min = 75; *max = 79;}
    else if (strcmp(grade, "B+") == 0) {*min = 70; *max = 74;}
    else if (strcmp(grade, "B") == 0) {*min = 65; *max = 69;}
    else if (strcmp(grade, "B-") == 0) {*min = 60; *max = 64;}
    else if (strcmp(grade, "C+") == 0) {*min = 55; *max = 59;}
    else if (strcmp(grade, "C") == 0) {*min = 50; *max = 54;}
    else if (strcmp(grade, "D+") == 0) {*min = 45; *max = 49;}  // D+, D, C all have same range in your table
    else if (strcmp(grade, "D") == 0) {*min = 40; *max = 44;}
    else if (strcmp(grade, "F") == 0) {*min = 0; *max = 39;}
    else {*min = -1; *max = -1;}
}

void query_process(const char* input, Student students[], int student_count) {
    if (strstr(input, "GRADE") && strstr(input, "PROGRAMME")) {
        const char* gradePos = strstr(input, "GRADE=");
        const char* progPos = strstr(input, "PROGRAMME=");
        if (gradePos && progPos) {
            char gradeVal[8], progVal[PROGRAMME_LENGTH];
        
            sscanf(gradePos, "GRADE=%7s", gradeVal);
        
            const char* progStart = progPos + strlen("PROGRAMME=");
            strncpy(progVal, progStart, PROGRAMME_LENGTH - 1);
            progVal[PROGRAMME_LENGTH - 1] = '\0';
            trimSpaces(progVal);
            reduceInnerSpaces(progVal);
        
            queryStudentByGradeAndProgramme(students, student_count, gradeVal, progVal);
            return; // Exit after execution
        }
    }
    else if (strstr(input, "ID=") != NULL) {
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
    char id[16];
    strncpy(id, idInput, 15);
    id[15] = '\0';
    trimSpaces(id);          // Trim spaces on id copy

    int len = strlen(id), valid = 1, queryId, found = 0;
    if (len != 7) valid = 0;   // Validate length of trimmed string
    for (int i = 0; i < len && valid; i++) {
        if (!isdigit((unsigned char)id[i])) valid = 0;  // Validate digits in trimmed string
    }
    if (!valid) {
        printf("Please input a proper ID (xxxxxxx)\n");
        return;
    }

    sscanf(id, "%d", &queryId);  // Parse from trimmed string

    for (int i = 0; i < student_count; i++) {
        if (students[i].id == queryId) {
            printf("Record Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Warning: No record found with student ID %d.\n", queryId);
    }
}


void queryStudentByName(Student students[], int student_count, const char* queryNameInput) {
    char queryName[NAME_LENGTH];
    strncpy(queryName, queryNameInput, NAME_LENGTH-1);
    queryName[NAME_LENGTH-1] = '\0';
    trimSpaces(queryName);
    reduceInnerSpaces(queryName);
    removeAllSpaces(queryName);
    int found = 0;
    for (int i = 0; i < student_count; i++) {
        char nameNoSpace[NAME_LENGTH];
        strncpy(nameNoSpace, students[i].name, NAME_LENGTH-1);
        nameNoSpace[NAME_LENGTH-1] = '\0';
        removeAllSpaces(nameNoSpace);
        if (strcasestr(nameNoSpace, queryName) != NULL) {
            if (!found) printf("Record(s) Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    if (!found) { printf("Warning: No record found with student name \"%s\".\n", queryName); }
}

void queryStudentByProgramme(Student students[], int student_count, const char* queryProgInput) {
    char queryProg[PROGRAMME_LENGTH];
    strncpy(queryProg, queryProgInput, PROGRAMME_LENGTH-1);
    queryProg[PROGRAMME_LENGTH-1] = '\0';
    trimSpaces(queryProg);
    reduceInnerSpaces(queryProg);
    removeAllSpaces(queryProg);
    int found = 0;
    for (int i = 0; i < student_count; i++) {
        char progNoSpace[PROGRAMME_LENGTH];
        strncpy(progNoSpace, students[i].programme, PROGRAMME_LENGTH-1);
        progNoSpace[PROGRAMME_LENGTH-1] = '\0';
        removeAllSpaces(progNoSpace);
        if (strcasestr(progNoSpace, queryProg) != NULL) {
            if (!found) printf("Record(s) Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    if (!found) { printf("Warning: No record found with programme \"%s\".\n", queryProg); }
}

void queryStudentByGrade(Student students[], int student_count, const char* gradeInputHold) {
    char gradeInput[4];
    strncpy(gradeInput, gradeInputHold, 3);
    gradeInput[3] = '\0';
    trimSpaces(gradeInput);

    for (int i = 0; gradeInput[i]; i++) gradeInput[i] = toupper((unsigned char)gradeInput[i]);
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
void queryStudentByGradeAndProgramme(Student students[], int studentcount, const char* gradeInput, const char* programmeInput) {
    char grade[4];
    float minScore, maxScore;
    int found = 0;

    // Copy and uppercase the grade input
    strncpy(grade, gradeInput, 3);
    grade[3] = '\0';
    for (int i = 0; grade[i]; i++) {
        grade[i] = toupper((unsigned char)grade[i]);
    }
    
    // Get the score range for the grade
    gradeRange(grade, &minScore, &maxScore);
    if (minScore < 0) {
        printf("Invalid grade entered.\n");
        return;
    }
    
    // Normalize programme input
    char queryProg[PROGRAMME_LENGTH];
    strncpy(queryProg, programmeInput, PROGRAMME_LENGTH - 1);
    queryProg[PROGRAMME_LENGTH - 1] = '\0';
    trimSpaces(queryProg);
    reduceInnerSpaces(queryProg);
    removeAllSpaces(queryProg);  // Remove spaces for loose matching
    
    // Search students matching both grade range and programme substring
    for (int i = 0; i < studentcount; i++) {
        char progNoSpace[PROGRAMME_LENGTH];
        strncpy(progNoSpace, students[i].programme, PROGRAMME_LENGTH - 1);
        progNoSpace[PROGRAMME_LENGTH - 1] = '\0';
        removeAllSpaces(progNoSpace);
        
        if (students[i].mark >= minScore &&
            students[i].mark <= maxScore &&
            strcasestr(progNoSpace, queryProg) != NULL) {
            if (!found) {
                printf("Record(s) Found:\n%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            }
            printf("%-10d %-22s %-26s %-8.2f\n", students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
        }
    }
    
    if (!found) {
        printf("Warning: No records found for grade '%s' and programme '%s'.\n", gradeInput, programmeInput);
    }
}
