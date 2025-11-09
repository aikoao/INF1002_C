#include <stdio.h>
#include "query.h"   
#include "database.h"

void queryStudent(Student students[], int student_count) {
    int queryId;
    printf("Enter the student ID to search: ");
    scanf("%d", &queryId);
    getchar();

    int found = 0;
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == queryId) {
            printf("Record Found:\n");
            printf("%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
            printf("%-10d %-22s %-26s %-8.2f\n",
                   students[i].id, students[i].name, students[i].programme, students[i].mark);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Warning: No record found with student ID %d.\n", queryId);
    }
}
