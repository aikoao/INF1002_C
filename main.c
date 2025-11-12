#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#include "query.h"  
#include "insert.h"

int main() {
    // display declaration 
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
    printf("Date: \n");
    printf("\n");
    
    
    while(1) {
        char userinput[256];
        char command[50];
        char filename[100] = "";
        printf("P4_8: ");
        fgets(userinput, sizeof(userinput), stdin);
        userinput[strlen(userinput)-1] = '\0';
        
        // Convert to uppercase
        for(int i=0; userinput[i]; i++) 
            userinput[i] = toupper(userinput[i]);

        sscanf(userinput, "%s %s", command, filename);
        
        // Handle features
        if(strcmp(command, "OPEN") == 0) {
            if(strlen(filename) == 0 || filename[0] == '\0'){
                printf("CMS: Please specify the database file name!\n");
            } else {
                openDatabase(filename);
            }
        }
        else if(strcmp(userinput, "SHOW ALL SORT BY ID (ASCENDING)") == 0) {
            if(student_count == 0){
                printf("CMS: Error, no records found! Please open the database file first.\n");
            } else {
                printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                sort_by_id_asc();
                showAll();  
            }
        }
        else if(strcmp(userinput, "SHOW ALL SORT BY MARK (ASCENDING)") == 0) {
            if(student_count == 0){
                printf("CMS: Error, no records found! Please open the database file first.\n");
            } else {
                printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                sort_by_mark_asc();
                showAll();
            }
        }
        else if(strcmp(userinput, "SHOW ALL SORT BY ID (DESCENDING)") == 0) {
            if(student_count == 0){
                printf("CMS: Error, no records found! Please open the database file first.\n");
            } else {
                printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                sort_by_id_desc();
                showAll();  
            }
        }
        else if(strcmp(userinput, "SHOW ALL SORT BY MARK (DESCENDING)") == 0) {
            if(student_count == 0){
                printf("CMS: Error, no records found! Please open the database file first.\n");
            } else {
                printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                sort_by_mark_desc();
                showAll();
            }
        }
        else if(strcmp(userinput, "SHOW ALL") == 0) {
            if(student_count == 0){
                printf("CMS: Error, no records found! Please open the database file first.\n");
            } else {
                printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
                showAll();
            }
        }
        else if(strstr(userinput, "UPDATE") != NULL) {
            update_record(userinput);
        }
        else if (strcmp(userinput, "QUERY ID") == 0) {
            queryStudentByID(students, student_count);
        }
        else if (strcmp(userinput, "QUERY NAME") == 0) {
            queryStudentByName(students, student_count);
        }
        else if (strcmp(userinput, "QUERY PROGRAMME") == 0) {
            queryStudentByProgramme(students, student_count);
        }
        else if (strcmp(userinput, "QUERY GRADE") == 0) {
            queryStudentByGrade(students, student_count);
        }
        else if(strstr(userinput, "INSERT") != NULL) {
            insert_record(userinput);
        }
        else if(strstr(userinput, "DELETE") != NULL) {
            delete_record(userinput); 
        }
        else if (strcmp(userinput, "HELP") == 0) {
            show_help();
        }
        else if(strcmp(userinput, "SAVE") == 0) {
            if(student_count == 0){
                printf("CMS: Error, no records to save! Please open the database file first!\n");
            } else {
                saveDatabase();
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