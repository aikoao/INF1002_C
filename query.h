#ifndef QUERY_H
#define QUERY_H

#include "database.h"  

void query_process(const char* input, Student students[], int student_count);
void queryStudentByID(Student students[], int student_count, const char* idInput);
void queryStudentByName(Student students[], int student_count, const char* nameInput);
void queryStudentByProgramme(Student students[], int student_count, const char* progInput);
void queryStudentByGrade(Student students[], int student_count, const char* gradeInput);
void queryStudentByGradeAndProgramme(Student students[], int studentcount, const char* gradeInput, const char* programmeInput);

#endif