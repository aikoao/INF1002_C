#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"
UndoState undo_stack[UNDO_STACK_SIZE];
int undo_top = -1;

// save the current state so that user can undo 
void save_undo_state(const char *description) {
    if (undo_top >= UNDO_STACK_SIZE - 1) {
        // Stack is full, shift everything down
        for (int i = 0; i < UNDO_STACK_SIZE - 1; i++) {
            undo_stack[i] = undo_stack[i + 1];
        }
        undo_top = UNDO_STACK_SIZE - 1;
    } else {
        undo_top++;
    }
    // save current state
    undo_stack[undo_top].student_count = student_count;
    strncpy(undo_stack[undo_top].description, description, sizeof(undo_stack[undo_top].description) - 1);
    undo_stack[undo_top].description[sizeof(undo_stack[undo_top].description) - 1] = '\0';
    
    // copy all student record
    for (int i = 0; i < student_count; i++) {
        undo_stack[undo_top].students[i] = students[i];
    }
    
    // clear any extra records in the undo state
    for (int i = student_count; i < MAX_RECORDS; i++) {
        undo_stack[undo_top].students[i].id = 0;
        undo_stack[undo_top].students[i].name[0] = '\0';
        undo_stack[undo_top].students[i].programme[0] = '\0';
        undo_stack[undo_top].students[i].mark = 0.0f;
    }
}

// restore previous state from undo
void undo_last_command(void) {
    if (undo_top < 0) {
        printf("CMS: Nothing to undo.\n");
        return;
    }
    
    // restore the previous state
    student_count = undo_stack[undo_top].student_count;
    // copy all student records back
    for (int i = 0; i < student_count; i++) {
        students[i] = undo_stack[undo_top].students[i];
    }
    //clear any extra records
    for (int i = student_count; i < MAX_RECORDS; i++) {
        students[i].id = 0;
        students[i].name[0] = '\0';
        students[i].programme[0] = '\0';
        students[i].mark = 0.0f;
    }
    printf("CMS: Undo successful. Reverted: %s\n", undo_stack[undo_top].description);
    undo_top--;
}
void clear_undo_history(void) {
    undo_top = -1;
}