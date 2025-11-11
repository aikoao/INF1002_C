#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#include "query.h"
#include "insert.h"

/* Uppercase a string in-place (used only for command matching). */
static void to_upper_inplace(char *s){
    for (; *s; ++s) *s = (char)toupper((unsigned char)*s);
}

int main(void) {
    /* ----- declaration banner ----- */
    printf("--Declaration--\n");
    printf("SIT's policy on copying does not allow the students to copy source code as well as assesment solutions from another person AI or other places. ");
    printf("It is the students' responsibility to guarantee that their assessment solutions are their own work. ");
    printf("Meanwhile, the students must also ensure that their work is not accessible by others. ");
    printf("Where such plagiarism is detected, both of the assessments involved will receive ZERO mark.\n");
    printf("We hereby declare that: \n");
    printf(" - We fully understand and agree to the above mentioned plagiarism policy.\n");
    printf(" - We did not copy any code from others or from other places.\n");
    printf(" - We did not share our codes with others or upload to any other places for public access and will not do that in the future.\n");
    printf(" - We agree that our project will receive Zero mark if there is any plagiarism detected.\n");
    printf(" - We agree that we will not disclose any information or material of the group project to others or upload to any other places for public access.\n");
    printf(" - We agree that we did not copy any code directly from AI generated sources.\n");
    printf("Declared by: P4-8\n");
    printf("Team members: \n");
    printf(" 1. Andrew Gan\n");
    printf(" 2. Aiko Mendoza Tan\n");
    printf(" 3. Chong Xinhuei\n");
    printf(" 4. Damien Teh\n");
    printf(" 5. Happy Calista\n");
    printf("Date: \n\n");

    while (1) {
        char raw[256];        /* ORIGINAL line (preserve value casing) */
        char cmd[32] = {0};   /* first token only, uppercased for dispatch */
        char upperline[256];  /* uppercase copy for SHOW comparisons */
        char filename[100] = "";

        printf("P4_8: ");
        if (!fgets(raw, sizeof raw, stdin)) break;
        raw[strcspn(raw, "\r\n")] = '\0';     /* strip newline */

        /* Extract first token as command, uppercase ONLY that token */
        sscanf(raw, " %31s", cmd);
        to_upper_inplace(cmd);

        /* ---- Dispatch ---- */
        if (strcmp(cmd, "OPEN") == 0) {
            /* parse filename from ORIGINAL line so filename case is preserved */
            if (sscanf(raw, " %*s %99s", filename) != 1) {
                printf("CMS: Please specify the database file name!\n");
            } else {
                /* Your database.h should declare: void openDatabase(const char *filename); */
                openDatabase(filename);
            }
        }
        else if (strcmp(cmd, "SHOW") == 0) {
            /* uppercase COPY of whole line for phrase matching */
            strncpy(upperline, raw, sizeof upperline - 1);
            upperline[sizeof upperline - 1] = '\0';
            to_upper_inplace(upperline);

            if (strcmp(upperline, "SHOW ALL SORT BY ID") == 0) {
                if (student_count == 0) {
                    printf("CMS: Error, no records found! Please open the database file first.\n");
                } else {
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_id(); showAll();
                }
            } else if (strcmp(upperline, "SHOW ALL SORT BY MARK") == 0) {
                if (student_count == 0) {
                    printf("CMS: Error, no records found! Please open the database file first.\n");
                } else {
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_mark(); showAll();
                }
            } else if (strcmp(upperline, "SHOW ALL") == 0) {
                if (student_count == 0) {
                    printf("CMS: Error, no records found! Please open the database file first.\n");
                } else {
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    showAll();
                }
            } else {
                printf("CMS: Error, invalid feature!\n");
            }
        }
        else if (strcmp(cmd, "INSERT") == 0) {
            /* pass ORIGINAL raw line so values' casing is preserved */
            insert_record(raw);
        }
        else if (strcmp(cmd, "UPDATE") == 0) {
            update_record(raw);
        }
        else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(raw);
        }
        else if (strcmp(cmd, "QUERY") == 0) {
            queryStudent(students, student_count);
        }
        else if (strcmp(cmd, "SAVE") == 0) {
            if (student_count == 0) {
                printf("CMS: Error, no records to save! Please open the database file first!\n");
            } else {
                saveDatabase();
            }
        }
        else if (strcmp(cmd, "EXIT") == 0) {
            break;
        }
        else {
            printf("CMS: Error, invalid feature!\n");
        }
    }

    return 0;
}
