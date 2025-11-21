#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"
#include "query.h"
#include "autocorrect.h"

void trim(char *str){
    int start = 0;
    while(str[start] == ' ' || str[start] == '\t'){
        start++;
    }

    if(start > 0){
        int i = 0;
        while(str[start] != '\0'){
            str[i] = str[start];
            i++;
            start++;
        } 
        str[i] = '\0';
    }

    int end = strlen(str) - 1;
    while(end >= 0 && (str[end] == ' ' || str[end] == '\t')){
        str[end] = '\0';
        end--;
    }
}

int main() {
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
    printf(" 5. Happy Calista\n\n");
    printf("Date: 23 November 2025\n\n");

    while(1) {
        char userinput[256];
        char original_input[256];

        printf("P4_8: ");
        fgets(userinput, sizeof(userinput), stdin);
        userinput[strcspn(userinput, "\n")] = '\0';

        trim(userinput);
        strcpy(original_input, userinput);

        char *suggestion = autocorrect_command_prompt(userinput);
        if (suggestion != NULL) {
            printf("CMS: Did you mean '%s'? (Y/N): ", suggestion);
            char answer[10];
            fgets(answer, sizeof(answer), stdin);
            if (toupper(answer[0]) == 'Y') {
                strcpy(userinput, suggestion);
                strcpy(original_input, suggestion);
            }
            free(suggestion);
        }


        for(int i = 0; userinput[i]; i++)
            userinput[i] = toupper(userinput[i]);

        if(strcmp(userinput, "OPEN") == 0) {
            openDatabase();
        }
        else if(strstr(userinput, "SHOW ALL") != 0) {
            if(student_count == 0){
                printf("CMS: Error, no records found! Please open the database file first.\n");
            } else {

                if(strcmp(userinput, "SHOW ALL") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY ID (ASCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_id_asc();
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY ID (DESCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_id_desc();
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY MARK (ASCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_mark_asc();
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY MARK (DESCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_mark_desc();
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY NAME (ASCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_name_asc();
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY NAME (DESCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_name_desc();
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY PROGRAMME (ASCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_programme_asc();
                    showAll();
                }
                else if(strcmp(userinput, "SHOW ALL SORT BY PROGRAMME (DESCENDING)") == 0){
                    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                    sort_by_programme_desc();
                    showAll();
                }
                else{
                    printf("CMS: Error, invalid feature. Please enter 'HELP' for available commands!\n");
                }
            }
        }
        else if (strcmp(userinput, "SHOW SUMMARY") == 0) {
            if (student_count == 0) {
                printf("CMS: Error, no records found! Please open the database file first.\n");
            } else {
                show_summary();
            }
        }
        else if(strstr(userinput, "UPDATE") != NULL) {
            update_record(original_input);
        }
        else if (strstr(userinput, "QUERY") != NULL) {
            query_process(original_input, students, student_count);
        }
        else if (strstr(userinput, "INSERT") != NULL) {
            insert_record(original_input);  
        }
        else if(strstr(userinput, "DELETE") != NULL) {
            delete_record(original_input);
        }
        else if(strcmp(userinput, "HELP") == 0) {
            show_help();
        }
        else if(strcmp(userinput, "SAVE") == 0) {
            if(student_count == 0){
                printf("CMS: Error, no records to save! Please open the database file first!\n");
            } else {
                saveDatabase();
            }
        }
        else if(strcmp(userinput, "UNDO") == 0) {
            if (student_count == 0) {
                printf("CMS: No records loaded. Please open the database file first.\n");
            } else {
                undo_last_command();
            }
        }
        else if(strcmp(userinput, "EXIT") == 0) {
            break;
        }
        else {
            printf("CMS: Error, invalid feature. Please enter 'HELP' for available commands!\n");
        }
    }
    return 0;
}
