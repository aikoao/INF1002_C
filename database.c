#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"

// Global variables 
Student students[MAX_RECORDS];
int student_count = 0;

void openDatabase(void) {
    student_count = 0;
    
    FILE *file = fopen("P4_8-CMS.TXT", "r");
    char buffer[300]; // temporary storage for each line
    char temp_prog[PROGRAMME_LENGTH]; // temporary storage for programme and mark

    if(file == NULL){
        printf("CMS: Error, the database file \"P4_8-CMS.TXT\" cannot be found!\n");
        return;
    }

    // Read and store each student record
    while(fgets(buffer, sizeof(buffer), file)) {
        if(sscanf(buffer, "%d (%49[^)]) %99[^\n]",
                &students[student_count].id,
                students[student_count].name,
                temp_prog) == 3) {

            // Extract mark from the end of temp_prog
            char *mark_str = strrchr(temp_prog, ' ');
            if(mark_str != NULL){ // Found a space before the mark
                students[student_count].mark = atof(mark_str + 1); 

                // Remove the mark from the programme string
                *mark_str = '\0';

                // Copy the programme string to the student record
                strcpy(students[student_count].programme, temp_prog);

                student_count++;
                if (student_count >= MAX_RECORDS) break; // Prevent the array from overflowing
            }
        }
    }
    
    fclose(file);
    printf("CMS: The database file \"P4_8-CMS.TXT\" is successfully opened.\n");
    
}

void showAll(void) {
    printf("%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
    for(int i = 0; i < student_count; i++){ // Loop through all student record
        printf("%-10d %-22s %-26s %-8.2f\n", 
            students[i].id, 
            students[i].name, 
            students[i].programme, 
            students[i].mark);
    }
}

void saveDatabase(void) {
    FILE *file = fopen("P4_8-CMS.txt", "w"); // open file for writing
    if(file == NULL){
        printf("CMS: Error, cannot save to \"P4_8-CMS.txt\" file!\n");
        return;
    }

    for(int i = 0; i < student_count; i++){
        fprintf(file, "%d (%s) %s %.2f\n", 
            students[i].id, 
            students[i].name, 
            students[i].programme, 
            students[i].mark);
    }

    fclose(file);
    printf("CMS: The database file \"P4_8-CMS.txt\" is successfully saved.\n");
}

void sort_by_id_asc(void) { // sort by id ascending order 
    for(int i = 0; i < student_count - 1; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(students[j].id > students[j + 1].id){ // compare IDs
                Student temp = students[j]; // swap
                students[j] = students[j + 1]; 
                students[j + 1] = temp; 
            }
        }
    }
}

void sort_by_mark_asc(void) { // sort by mark ascending order 
    for(int i = 0; i < student_count - 1; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(students[j].mark > students[j + 1].mark){ // compare Marks
                Student temp = students[j]; // swap
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void sort_by_id_desc(void) { // sort by id descending order 
    for(int i = 0; i < student_count - 1; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(students[j].id < students[j + 1].id){ // compare IDs
                Student temp = students[j]; // swap
                students[j] = students[j + 1]; 
                students[j + 1] = temp; 
            }
        }
    }
}

void sort_by_mark_desc(void) { // sort by mark descending order 
    for(int i = 0; i < student_count - 1; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(students[j].mark < students[j + 1].mark){ // compare Marks
                Student temp = students[j]; // swap
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void sort_by_name_asc (void) { // sort by name ascending order
    for (int i = 0; i < student_count; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(strcmp(students[j].name, students[j + 1].name) > 0){ // compare Name
                Student temp = students[j]; // swap 
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void sort_by_name_desc (void) { // sort by name descending order
    for (int i = 0; i < student_count; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(strcmp(students[j].name, students[j + 1].name) < 0){ // compare Name
                Student temp = students[j]; // swap 
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void sort_by_programme_asc (void) { // sort by programme ascending order
    for (int i = 0; i < student_count; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(strcmp(students[j].programme, students[j + 1].programme) > 0){ // compare Programme
                Student temp = students[j]; // swap 
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void sort_by_programme_desc (void) { // sort by programme ascending order
    for (int i = 0; i < student_count; i++){
        for(int j = 0; j < student_count - i - 1; j++){
            if(strcmp(students[j].programme, students[j + 1].programme) < 0){ // compare Programme
                Student temp = students[j]; // swap 
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}
