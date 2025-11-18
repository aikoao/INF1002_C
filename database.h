#ifndef DATABASE_H // checks the DATABASE_H if it has already been defined
#define DATABASE_H

#define MAX_RECORDS 200
#define NAME_LENGTH 50
#define PROGRAMME_LENGTH 100
#define UNDO_STACK_SIZE 3 // number of undo user has

// Define the structure for student records
typedef struct {
    int id;
    char name[NAME_LENGTH];
    char programme[PROGRAMME_LENGTH];
    float mark;
}Student;

typedef struct {
    Student students[MAX_RECORDS];
    int student_count;
    char description[100];
} UndoState;

extern Student students[MAX_RECORDS]; // global array to store the student records
extern int student_count; // global variable to mark the current number of students 

extern UndoState undo_stack[UNDO_STACK_SIZE];
extern int undo_top;

// File operations 
void openDatabase(void);
void showAll(void);
void saveDatabase();
void sort_by_id_asc(void);
void sort_by_mark_asc(void);
void sort_by_id_desc(void);
void sort_by_mark_desc(void);
void sort_by_name_asc(void);
void sort_by_name_desc(void);
void sort_by_programme_asc(void);
void sort_by_programme_desc(void);

// Command functions (Team)
void update_record(const char *command);      // Aiko
//void query_record();       // Andrew  
void insert_record(const char *command);      // Xin Huei
void show_summary(void);
void delete_record(const char* command);      // Damien
void show_help(void);                     // Help menu

// Undo functions
void save_undo_state(const char *description);
void undo_last_command(void);
void clear_undo_history(void);

#endif // end of the DATABASE_H