#ifndef DATABASE_H // checks the DATABASE_H if it has already been defined
#define DATABASE_H

#define MAX_RECORDS 200
#define NAME_LENGTH 50
#define PROGRAMME_LENGTH 100

// Define the structure for student records
typedef struct {
    int id;
    char name[NAME_LENGTH];
    char programme[PROGRAMME_LENGTH];
    float mark;
}Student;

extern Student students[MAX_RECORDS]; // global array to store the student records
extern int student_count; // global variable to mark the current number of students 

// File operations 
void openDatabase(char *filename);
void showAll(void);
void saveDatabase();
void sort_by_id(void);
void sort_by_mark(void);

// Command functions (Team)
void update_record(const char *command);      // Aiko
//void query_record();       // Andrew  
//void insert_record();      // Xin Huei
void delete_record(const char* command);      // Damien

#endif // end of the DATABASE_H