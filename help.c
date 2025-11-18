#include <stdio.h>
#include "database.h"

void show_help(void) {
    printf("=============================================\n");
    printf("         CMS HELP MENU (P4_8 SYSTEM)\n");
    printf("=============================================\n");
    printf("Available Commands:\n");
    printf("  OPEN                                      - Open the student database file.\n");
    printf("  SHOW ALL                                  - Display all student records.\n");
    printf("  SHOW ALL SORT BY ID (ASCENDING)           - Display all records sorted by ID in ascending order.\n");
    printf("  SHOW ALL SORT BY MARK (ASCENDING)         - Display all records sorted by mark in ascending order.\n");
    printf("  SHOW ALL SORT BY NAME (ASCENDING)         - Display all records sorted by name in ascending order.\n");
    printf("  SHOW ALL SORT BY PROGRAMME (ASCENDING)    - Display all records sorted by programme in ascending order.\n");
    printf("  SHOW ALL SORT BY ID (DESCENDING)          - Display all records sorted by ID in descending order.\n");
    printf("  SHOW ALL SORT BY MARK (DESCENDING)        - Display all records sorted by mark in descending order.\n");
    printf("  SHOW ALL SORT BY NAME (DESCENDING)        - Display all records sorted by name in descending order.\n");
    printf("  SHOW ALL SORT BY PROGRAMME (DESCENDING)   - Display all records sorted by programme in descending order.\n");
    printf("  INSERT                                    - Add a new student record.\n");
    printf("  UPDATE ID=<number>                        - Update an existing student's info.\n");
    printf("  DELETE ID=<number>                        - Delete a student record by ID.\n");
    printf("  QUERY                                     - Search for records (by ID/name/programme/grade.).\n");
    printf("  UNDO                                      - Undo last operation (up to 3 steps\n");
    printf("  SAVE                                      - Save all changes to the database file.\n");
    printf("  HELP                                      - Display this help menu.\n");
    printf("  EXIT                                      - Exit the program.\n");
    printf("---------------------------------------------\n");
    printf("Usage Tips:\n");
    printf(" - Commands are not case-sensitive (e.g., 'show all' works too).\n");
    printf(" - Use 'SAVE' after making changes to ensure data is stored.\n");
    printf(" - Use 'EXIT' to quit safely.\n");
    printf("=============================================\n\n");
}
