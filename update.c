#include <stdio.h>
#include <string.h>
#include "database.h"

void update_record(const char *command) {
    
    int search_id;
    char input[100];
    
    // extract id from user input format
    if (sscanf(command, "UPDATE ID=%d", &search_id) != 1) {
        printf("CMS: Invalid UPDATE format. Use: UPDATE ID=number\n");
        return;
    }
    
    // search for record in database
    int record = -1;
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == search_id) {
            record = i;
            break;
        }
    }
    
    if (record == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", search_id);
        return;
    }
    
    // record is found
    printf("CMS: Please enter the new data for each column:\n");
    
    printf("Name: ");
    fgets(students[record].name, sizeof(students[record].name), stdin);
    students[record].name[strcspn(students[record].name, "\n")] = 0;
    
    printf("Programme: ");
    fgets(students[record].programme, sizeof(students[record].programme), stdin);
    students[record].programme[strcspn(students[record].programme, "\n")] = 0;
    
    printf("Mark: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%f", &students[record].mark);
    
    printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
}