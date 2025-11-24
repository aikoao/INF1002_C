#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"

// function to auto cap first letter of each word
void auto_capitalise(char *str) {
    int capitalise_char = 1;  // caps first character
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (capitalise_char && isalpha(str[i])) {
            str[i] = toupper(str[i]);
            capitalise_char = 0;
        } else if (isspace(str[i]) || str[i] == '-' || str[i] == '\'' || str[i] == '.') {
            capitalise_char = 1;
        } else {
            str[i] = tolower(str[i]);  // lowercase
        }
    }
}

void update_record(const char *command) {

    // Save state for undo function
    save_undo_state("UPDATE operation");
    
    //  for case-insensitive checking
    char upper_cmd[256];
    strcpy(upper_cmd, command);
    for(int i = 0; upper_cmd[i]; i++) {
        upper_cmd[i] = toupper(upper_cmd[i]);
    }
    // check if start with update id=
    if (strncmp(upper_cmd, "UPDATE ID=", 10) != 0) {
        printf("CMS: Invalid UPDATE format. Use: UPDATE ID=number Field=value\n");
        return;
    }
    
    const char *id_start = command + 10;
    const char *id_end = id_start;
    while (*id_end && *id_end != ' ') id_end++;
    
    char id_str[20];
    int id_len = id_end - id_start;
    if (id_len >= 20) id_len = 19;
    strncpy(id_str, id_start, id_len);
    id_str[id_len] = '\0';
    
    // id validation
    char *endptr;
    long id_long = strtol(id_str, &endptr, 10);

    if (endptr == id_str) {
        printf("CMS: Invalid ID '%s'. ID must be a number.\n", id_str);
        return;
    }

    if (*endptr != '\0') {
        printf("CMS: Invalid ID '%s'. ID must contain only numbers.\n", id_str);
        return;
    }

    int search_id = (int)id_long;
    
    const char *field_start = id_end;
    if (*field_start == ' ') field_start++;
    
    if (*field_start == '\0') {
        printf("CMS: Please specify which column to update (Name/Programme/Mark)\n");
        return;
    }
    
    
    //  to detect multiple field updates
    int equal_count = 0;
    for (const char *p = field_start; *p; p++) {
        if (*p == '=') equal_count++;
    }
    
    if (equal_count > 1) {
        printf("CMS: Please update only one field at a time. Use: UPDATE ID=number Field=value\n");
        return;
    }
    
    const char *equal_sign = strchr(field_start, '=');
    if (equal_sign == NULL) {
        printf("CMS: Invalid UPDATE format. Use: UPDATE ID=number Field=value\n");
        return;
    }
    
    // extract field name,convert to uppercase for comparison
    char field[20];
    int field_len = equal_sign - field_start;
    if (field_len >= 20) field_len = 19;
    strncpy(field, field_start, field_len);
    field[field_len] = '\0';
    
    for(int i = 0; field[i]; i++) {
        field[i] = toupper(field[i]);
    }
    const char *value_start = equal_sign + 1;
    
    // Check if value is empty
    if (*value_start == '\0') {
        printf("CMS: %s cannot be empty.\n", field);
        return;
    }
    
    // for checking the length
    int input_length = strlen(value_start);
    // validation for respective fieldss
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
    //  copy to buffer after validation passes
    char new_value[PROGRAMME_LENGTH];
    strncpy(new_value, value_start, PROGRAMME_LENGTH - 1);
    new_value[PROGRAMME_LENGTH - 1] = '\0';
    
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
    
    //  update the appropriate field
    if (strcmp(field, "MARK") == 0) {
        char *endptr;
        float mark_value = strtof(new_value, &endptr);
        
        if (endptr == new_value || *endptr != '\0') {
            printf("CMS: Invalid mark value '%s'. Mark must be a number.\n", new_value);
            return;
        }
        // check if mark is within range
        if (mark_value < 0.0 || mark_value > 100.0) {
            printf("CMS: Mark must be between 0.0 and 100.0.\n");
            return;
        }
        students[record].mark = mark_value;
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else if (strcmp(field, "PROGRAMME") == 0) {
        auto_capitalise(new_value); 
        strcpy(students[record].programme, new_value);
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else if (strcmp(field, "NAME") == 0) {
        // name validation
        int alpha_char = 0;
        int numbers_char = 0;
        int special_char = 0;
        const char *valid_char = " -.'";

        for (int i = 0; new_value[i] != '\0'; i++) {
            if (isalpha(new_value[i])) {
                alpha_char = 1;
            }
            if (isdigit(new_value[i])) {
                numbers_char = 1;  // detect any numbers
            }
            
            // check for invalid special characters
            if (!isalpha(new_value[i]) && !isspace(new_value[i])) {
                int is_allowed = 0;
                for (int j = 0; valid_char[j] != '\0'; j++) {
                    if (new_value[i] == valid_char[j]) {
                        is_allowed = 1;
                        break;
                    }
                }
                if (!is_allowed) {
                    special_char = 1;
                }
            }
        }
        // validation checks
        if (!alpha_char) {
            printf("CMS: Name must contain alphabetic characters.\n");
            return;
        }
        if (numbers_char) {
            printf("CMS: Name cannot contain numbers.\n");
            return;
        }
        if (special_char) {
            printf("CMS: Name can only contain letters, spaces, hyphens (-), periods (.), and apostrophes (').\n");
            return;
        }
        // name must start with a letter
        if (!isalpha(new_value[0])) {
            printf("CMS: Name must start with a letter.\n");
            return;
        }
        auto_capitalise(new_value);
        strcpy(students[record].name, new_value);
        printf("CMS: The record with ID=%d is successfully updated.\n", search_id);
    }
    else {
        printf("CMS: Invalid field '%s'. Use: Name, Programme, or Mark\n", field);
    }
}