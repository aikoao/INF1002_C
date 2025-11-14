#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"

void update_record(const char *command) {
    // Convert entire command to uppercase for case-insensitive checking
    char upper_cmd[256];
    strcpy(upper_cmd, command);
    for(int i = 0; upper_cmd[i]; i++) {
        upper_cmd[i] = toupper(upper_cmd[i]);
    }
    
    // Check if it starts with "UPDATE ID=" (case insensitive)
    if (strncmp(upper_cmd, "UPDATE ID=", 10) != 0) {
        printf("CMS: Invalid UPDATE format. Use: UPDATE ID=number Field=value\n");
        return;
    }
    
    // Extract ID - skip "UPDATE ID=" (10 characters)
    int search_id = atoi(command + 10);
    
    // Find where the field name starts (after ID=number)
    const char *field_start = command + 10;
    while (*field_start && *field_start != ' ') field_start++;
    if (*field_start == ' ') field_start++; // skip space
    
    if (*field_start == '\0') {
        printf("CMS: Please specify which column to update (Name/Programme/Mark)\n");
        return;
    }
    
    // Find the '=' after field name
    const char *equal_sign = strchr(field_start, '=');
    if (equal_sign == NULL) {
        printf("CMS: Invalid UPDATE format. Use: UPDATE ID=number Field=value\n");
        return;
    }
    
    // Extract field name (convert to uppercase for comparison)
    char field[20];
    int field_len = equal_sign - field_start;
    if (field_len >= 20) field_len = 19;
    strncpy(field, field_start, field_len);
    field[field_len] = '\0';
    
    // Convert field to uppercase for comparison
    for(int i = 0; field[i]; i++) {
        field[i] = toupper(field[i]);
    }
    
    // Extract new_value (everything after '=')
    char new_value[100];
    const char *value_start = equal_sign + 1;
    
    // Check if value is empty
    if (*value_start == '\0') {
        printf("CMS: %s cannot be empty.\n", field);
        return;
    }
    
    strncpy(new_value, value_start, 99);
    new_value[99] = '\0';
    
    // Find the record
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
    
    // Update the appropriate field
    if (strcmp(field, "MARK") == 0) {
        students[record].mark = atof(new_value);
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else if (strcmp(field, "PROGRAMME") == 0) {
        strcpy(students[record].programme, new_value);
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else if (strcmp(field, "NAME") == 0) {
        strcpy(students[record].name, new_value);
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else {
        printf("CMS: Invalid field '%s'. Use: Name, Programme, or Mark\n", field);
    }
}