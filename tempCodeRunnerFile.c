void openDatabase() {
    FILE *file = fopen("TeamName-CMS.txt", "r");
    char buffer[100]; // temporary storage for each line

    if(file == NULL){
        printf("File not found!\n");
        return;
    }

    printf("File opened successfully.\n");

    // Skip the header line
    fgets(buffer, sizeof(buffer), file);

    // Read and store each student record
    student_count = 0;
    while(fgets(buffer, sizeof(buffer), file)) {
        if(sscanf(buffer, "%d\t%49s[^\t]\t%49s[^\t]\t%f",
                &students[student_count].id,
                students[student_count].name,
                students[student_count].programme,
                &students[student_count].mark) == 4) {
            student_count++;
            if (student_count >= 100) break; // Prevent the array from overflowing
        
        }
    }

    printf("%d student records loaded.\n", student_count);
    fclose(file);
}