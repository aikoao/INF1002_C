#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the structure for student records
struct Student{
    int id;
    char name[50];
    char programme[50];
    float mark;
};

// Global variables 
struct Student students[100];
int student_count = 0;

// Opens the database file and loads all student records into memory
void openDatabase() {

    char userinput[20];

    printf("P4_8: ");
    fgets(userinput, sizeof(userinput), stdin);

    FILE *file = fopen("P4_8-CMS.txt", "r");
    char buffer[300]; // temporary storage for each line
    char temp_prog[100]; // temporary storage for programme and mark

    if(file == NULL){
        printf("File not found!\n");
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
                if (student_count >= 100) break; // Prevent the array from overflowing
            }
        }
    }
    
    printf("CMS: The database file \"P4_8-CMS.txt\" is successfully opened.\n");
    fclose(file);
}

void showAll(struct Student students[], int count){

    // Print header
    printf("%-10s %-22s %-26s %-8s\n", "ID", "Name", "Programme", "Mark");
    for(int i = 0; i < student_count; i++){ // Loop through all student record
        printf("%-10d %-22s %-26s %-8.2f\n", 
            students[i].id, 
            students[i].name, 
            students[i].programme, 
            students[i].mark);
    }
}

// Sort students by ID in ascending order
void sort_by_id(struct Student students[], int count){
    for(int i = 0; i < count - 1; i++){
        for(int j = 0; j < count - i - 1; j++){
            if(students[j].id > students[j + 1].id){ // compare IDs
                struct Student temp = students[j]; // swap
                students[j] = students[j + 1]; 
                students[j + 1] = temp; 
            }
        }
    }
}

// Sort students by Mark in ascending order
void sort_by_mark(struct Student students[], int count){
    for(int i = 0; i < count - 1; i++){
        for(int j = 0; j < count - i - 1; j++){
            if(students[j].mark > students[j + 1].mark){ // compare Marks
                struct Student temp = students[j]; // swap
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

}

void saveDatabase(struct Student students[], int count){
    
    // char userinput3[20];

    // printf("P4_8: ");
    // fgets(userinput3, sizeof(userinput3), stdin);

    FILE *file = fopen("P4_8-CMS.txt", "w"); // open file for writing
    if(file == NULL){
        printf("Error to open the file!\n");
        return;
    }

    for(int i = 0; i < count; i++){
        fprintf(file, "%d (%s) %s %.2f\n", 
            students[i].id, 
            students[i].name, 
            students[i].programme, 
            students[i].mark);
    }

    fclose(file);
    printf("CMS: The database file \"P4_8-CMS.txt\" is successfully saved.\n");

}

int main() {

    printf("--Declaration--\n");
    printf("SIT's policy on copying does not allow the students to copy source code as well as assesment solutions from another person AI or other places. ");
    printf("It is the students' responsibility to guarantee that their assessment solutions are their own work. ");
    printf("Meanwhile, the students must also ensure that their work is not accessible by others. ");
    printf("Where such plagiarism is detected, both of the assessments involved will receive ZERO mark.\n");
    printf("We hereby declare that: \n");
    printf(" - We fully understand and agree to the abovementioned plagiarism policy.\n");
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

    openDatabase(); // call function to open database file

    while(1) { 
        char userinput2[30];
        printf("P4_8: ");
        fgets(userinput2, sizeof(userinput2), stdin); // Get user input for SHOW ALL
        userinput2[strlen(userinput2) - 1] = '\0';

        for(int i = 0; userinput2[i] != '\0'; i++){
            userinput2[i] = toupper(userinput2[i]); // convert to uppercase (for case insensitivity)
        }

        if(strstr(userinput2, "SHOW ALL SORT BY ID") != NULL) { // strstr to check if substring exists
            // Call sort by ID function
            printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
            sort_by_id(students, student_count);
            showAll(students, student_count);
        }
        else if(strstr(userinput2, "SHOW ALL SORT BY MARK") != NULL) {
            // Call sort by mark function
            printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
            sort_by_mark(students, student_count);
            showAll(students, student_count);
        }
        else if(strcmp(userinput2, "SHOW ALL") == 0){
            printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
            showAll(students, student_count);
        }
        else if(strcmp(userinput2, "EXIT") == 0){
            break; // to exit the loop
        }
        else if(strcmp(userinput2, "SAVE") == 0){
            saveDatabase(students, student_count); // Save changes to the database file
        }
        else{
            printf("Invalid input. Please try again.\n");
        }
    }

    return 0;
}

