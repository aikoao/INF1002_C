#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"

// function to auto cap first letter of each word
void auto_capitalise(char *str) {
    int capitalise_next = 1;  // caps first character
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (capitalise_next && isalpha(str[i])) {
            str[i] = toupper(str[i]);
            capitalise_next = 0;
        } else if (isspace(str[i])) {
            capitalise_next = 1;  // Next character should be caps
        } else {
            str[i] = tolower(str[i]);  // Make the rest lowercase
        }
    }
}

void update_record(const char *command) {
    // Convert entire command to uppercase for case-insensitive checking
    char upper_cmd[256];
    strcpy(upper_cmd, command);
    for(int i = 0; upper_cmd[i]; i++) {
        upper_cmd[i] = toupper(upper_cmd[i]);
    }
    
    // Strict format checking - must start with exactly "UPDATE ID="
    if (strncmp(upper_cmd, "UPDATE ID=", 10) != 0) {
        printf("CMS: Invalid UPDATE format. Use: UPDATE ID=number Field=value\n");
        return;
    }
    
    // Extract ID - skip "UPDATE ID=" (10 characters)
    const char *id_start = command + 10;
    
    // Find the end of ID (whitespace or end of string)
    const char *id_end = id_start;
    while (*id_end && *id_end != ' ') id_end++;
    
    // Extract ID string
    char id_str[20];
    int id_len = id_end - id_start;
    if (id_len >= 20) id_len = 19;
    strncpy(id_str, id_start, id_len);
    id_str[id_len] = '\0';
    
    // Proper ID validation
    char *endptr;
    long id_long = strtol(id_str, &endptr, 10);

    // Check if conversion failed (no digits found)
    if (endptr == id_str) {
        printf("CMS: Invalid ID '%s'. ID must be a number.\n", id_str);
        return;
    }

    // Check if there are extra characters after the number
    if (*endptr != '\0') {
        printf("CMS: Invalid ID '%s'. ID must contain only numbers.\n", id_str);
        return;
    }

    int search_id = (int)id_long;
    
    // Find where the field name starts (skip any single space after ID)
    const char *field_start = id_end;
    if (*field_start == ' ') field_start++;
    
    if (*field_start == '\0') {
        printf("CMS: Please specify which column to update (Name/Programme/Mark)\n");
        return;
    }
    
    // Check for multiple '=' signs to detect multiple field updates
    int equal_count = 0;
    for (const char *p = field_start; *p; p++) {
        if (*p == '=') equal_count++;
    }
    
    if (equal_count > 1) {
        printf("CMS: Please update only one field at a time. Use: UPDATE ID=number Field=value\n");
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
    const char *value_start = equal_sign + 1;
    
    // Check if value is empty
    if (*value_start == '\0') {
        printf("CMS: %s cannot be empty.\n", field);
        return;
    }
    
    // CHECK LENGTH FIRST before any copying/truncation
    int input_length = strlen(value_start);
    // Validate field-specific length requirements
    if (strcmp(field, "PROGRAMME") == 0) {
        if (input_length >= PROGRAMME_LENGTH) {
            printf("CMS: Programme name is too long.\n");
            return;
        }
    }
    else if (strcmp(field, "NAME") == 0) {
        if (input_length >= NAME_LENGTH) {
            printf("CMS: Name is too long.\n");
            return;
        }
    }
    
    // Only now copy to buffer after validation passes
    char new_value[PROGRAMME_LENGTH];
    strncpy(new_value, value_start, PROGRAMME_LENGTH - 1);
    new_value[PROGRAMME_LENGTH - 1] = '\0';
    
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
        // Validate that mark is a valid number
        char *endptr;
        float mark_value = strtof(new_value, &endptr);
        
        // Check if conversion failed or there are extra characters
        if (endptr == new_value || *endptr != '\0') {
            printf("CMS: Invalid mark value '%s'. Mark must be a number.\n", new_value);
            return;
        }
        
        // Check if mark is within valid range (0-100)
        if (mark_value < 0.0 || mark_value > 100.0) {
            printf("CMS: Mark must be between 0.0 and 100.0.\n");
            return;
        }
        
        students[record].mark = mark_value;
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else if (strcmp(field, "PROGRAMME") == 0) {
        auto_capitalise(new_value);  // Auto caps before saving
        strcpy(students[record].programme, new_value);
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else if (strcmp(field, "NAME") == 0) {
        auto_capitalise(new_value);  // Auto caps before saving
        strcpy(students[record].name, new_value);
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else {
        printf("CMS: Invalid field '%s'. Use: Name, Programme, or Mark\n", field);
    }
}