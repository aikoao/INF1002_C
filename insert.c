#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"

// ---------- Title-case helper ----------
static void to_title_case(char *s){
    int cap = 1;
    for (int i = 0; s[i]; i++){
        unsigned char c = (unsigned char)s[i];
        if (isspace(c) || c=='-' || c=='\'' || c=='/'){
            cap = 1;
        } else if (cap){
            s[i] = (char)toupper(c);
            cap = 0;
        } else {
            s[i] = (char)tolower(c);
        }
    }
}

// ---------- trim helpers ----------
static void rtrim(char *s){
    int n = (int)strlen(s);
    while (n > 0 && isspace((unsigned char)s[n-1])){
        s[--n] = '\0';
    }
}
static void ltrim(char *s){
    int i = 0;
    int n = (int)strlen(s);
    while (i < n && isspace((unsigned char)s[i])){
        i++;
    }
    if (i > 0){
        memmove(s, s + i, n - i + 1);
    }
}
static void trim(char *s){
    rtrim(s);
    ltrim(s);
}

// ---------- parse helpers ----------
static int parse_int(const char *s, int *out){
    char *e;
    long v = strtol(s, &e, 10);
    if (s == e) return 0; // no digits
    while (*e){
        if (!isspace((unsigned char)*e)) return 0;
        e++;
    }
    *out = (int)v;
    return 1;
}

static int parse_float(const char *s, float *out){
    char *e;
    float v = strtof(s, &e);
    if (s == e) return 0; // no digits
    while (*e){
        if (!isspace((unsigned char)*e)) return 0;
        e++;
    }
    *out = v;
    return 1;
}

// ---------- validity checks ----------
static int is_letters_spaces(const char *s){
    int has_alpha = 0;
    const unsigned char *p = (const unsigned char*)s;
    while (*p){
        if (isalpha(*p)){
            has_alpha = 1;
        } else if (*p==' ' || *p=='-' || *p=='\'' || *p=='.' || *p=='/'){
            // allowed
        } else {
            return 0;
        }
        p++;
    }
    return has_alpha;
}

// case-insensitive prefix
static int ci_starts_with(const char *s, const char *pat){
    while (*s && *pat){
        if (tolower((unsigned char)*s) != tolower((unsigned char)*pat)) return 0;
        ++s;
        ++pat;
    }
    return *pat == '\0';
}

// case-insensitive strstr
static const char* ci_strstr(const char *hay, const char *needle){
    if (!*needle) return hay;
    for (const char *p = hay; *p; ++p){
        const char *h = p;
        const char *n = needle;
        while (*h && *n &&
               tolower((unsigned char)*h) == tolower((unsigned char)*n)){
            ++h;
            ++n;
        }
        if (!*n) return p;  // found
    }
    return NULL;
}

/* return 1 if value contains another field pattern like "ID=" inside */
static int contains_reserved_pattern(const char *s){
    return  ci_strstr(s, "ID=")        ||
            ci_strstr(s, "NAME=")      ||
            ci_strstr(s, "PROGRAMME=") ||
            ci_strstr(s, "PROGRAM=")   ||
            ci_strstr(s, "PROG=")      ||
            ci_strstr(s, "MARK=")      ||
            ci_strstr(s, "MARKS=");
}

/* generic duplicate-key checker: returns 1 if key appears more than once */
static int has_duplicate_key(const char *command, const char *key_with_eq){
    size_t klen = strlen(key_with_eq);
    const char *first = ci_strstr(command, key_with_eq);
    if (!first) return 0;
    const char *second = ci_strstr(first + klen, key_with_eq);
    return second != NULL;
}

/*
 * Extract value for a given key (e.g., "Name="):
 *  - matches key in any case
 *  - allows spaces around '='
 *  - value ends at next key (ID/NAME/PROGRAMME/PROGRAM/PROG/MARK/MARKS) or end of line
 */
static int extract_value(const char *line, const char *key_with_eq, char *out, size_t outsz){
    // bare key: "Name" from "Name="
    char bare[16];
    size_t klen = 0;
    while (key_with_eq[klen] && key_with_eq[klen] != '=' && klen < sizeof(bare)-1){
        bare[klen] = key_with_eq[klen];
        klen++;
    }
    bare[klen] = '\0';

    const char *p = line;
    const char *start = NULL;

    // find key position
    while (*p){
        while (*p && isspace((unsigned char)*p))
            p++;
        if (ci_starts_with(p, bare)){
            const char *q = p + strlen(bare);
            while (*q && isspace((unsigned char)*q))
                q++;
            if (*q == '='){
                q++; // skip '='
                while (*q && isspace((unsigned char)*q))
                    q++;
                start = q;
                break;
            }
        }
        p++;
    }
    if (!start) return 0;

    const char *end = line + strlen(line);
    const char *scan = start;

    while (*scan){
        if (isspace((unsigned char)*scan)){
            const char *key_start = scan;
            const char *q = scan;
            while (*q && isspace((unsigned char)*q))
                q++;

            const char *candidates[] = {
                "ID","NAME","PROGRAMME","PROGRAM","PROG","MARK","MARKS",NULL
            };
            for (int i = 0; candidates[i]; ++i){
                if (ci_starts_with(q, candidates[i])){
                    const char *r = q;
                    while (*r && isalpha((unsigned char)*r))
                        r++;
                    const char *s2 = r;
                    while (*s2 && isspace((unsigned char)*s2))
                        s2++;
                    if (*s2 == '='){
                        end = key_start;
                        goto value_end_found;
                    }
                }
            }
        }
        scan++;
    }
value_end_found:;

    size_t len = (size_t)(end - start);
    if (len >= outsz)
        len = outsz - 1;
    memcpy(out, start, len);
    out[len] = '\0';
    trim(out);
    return out[0] != '\0';
}

// find existing ID
static int find_index_by_id(int id){
    for (int i = 0; i < student_count; i++){
        if (students[i].id == id)
            return i;
    }
    return -1;
}

// ---------- main INSERT ----------
void insert_record(const char *command){
    // Save state BEFORE making changes (for UNDO)
    save_undo_state("INSERT operation");

    if (!command){
        printf("CMS: Invalid command. Use: INSERT ID=... Name=... Programme=... Mark=...\n");
        return;
    }

    // ---------- duplicate keyword checks ----------
    if (has_duplicate_key(command, "id=")){
        printf("CMS: Invalid ID format for INSERT.\n");
        return;
    }
    if (has_duplicate_key(command, "name=")){
        printf("CMS: Invalid Name format for INSERT.\n");
        return;
    }
    // treat programme/program/prog as same field
    if (has_duplicate_key(command, "programme=") ||
        has_duplicate_key(command, "program=")   ||
        has_duplicate_key(command, "prog=")){
        printf("CMS: Invalid Programme format for INSERT.\n");
        return;
    }
    if (has_duplicate_key(command, "mark=") ||
        has_duplicate_key(command, "marks=")){
        printf("CMS: Invalid Mark format for INSERT.\n");
        return;
    }

    // fields
    char idbuf[32] = "";
    char name[NAME_LENGTH] = "";
    char programme[PROGRAMME_LENGTH] = "";
    char markbuf[32] = "";
    int id;
    float mark;

    /* ---------- ID ---------- */
    if (!extract_value(command, "ID=", idbuf, sizeof idbuf)){
        printf("CMS: Missing ID for INSERT.\n");
        return;
    }

    int len = (int)strlen(idbuf);
    if (len != 7){
        printf("CMS: Invalid ID format. ID must be a 7-digit number from 1500000 to 2599999.\n");
        return;
    }

    for (int i = 0; i < len; ++i){
        if (!isdigit((unsigned char)idbuf[i])){
            printf("CMS: Invalid ID format. ID must be numeric.\n");
            return;
        }
    }

    if (!parse_int(idbuf, &id)){
        printf("CMS: Invalid ID value.\n");
        return;
    }

    if (id < 1500000 || id > 2599999){
        printf("CMS: Invalid ID range. Only IDs from 1500000 to 2599999 are allowed.\n");
        return;
    }

    if (find_index_by_id(id) != -1){
        printf("CMS: The record with ID=%d already exists.\n", id);
        return;
    }

    /* ---------- Name ---------- */
    if (!extract_value(command, "Name=", name, sizeof name) || name[0] == '\0'){
        printf("CMS: Missing Name for INSERT.\n");
        return;
    }

    if (!is_letters_spaces(name)){
        printf("CMS: Invalid Name. Use only letters/spaces (- ' . / allowed).\n");
        return;
    }

    if (contains_reserved_pattern(name)){
        printf("CMS: Invalid Name format for INSERT.\n");
        return;
    }

    /* ---------- Programme ---------- */
    if (!extract_value(command, "Programme=", programme, sizeof programme) || programme[0] == '\0'){
        printf("CMS: Missing Programme for INSERT.\n");
        return;
    }

    if (!is_letters_spaces(programme)){
        printf("CMS: Invalid Programme. Use only letters/spaces (- ' . / allowed).\n");
        return;
    }

    if (contains_reserved_pattern(programme)){
        printf("CMS: Invalid Programme format for INSERT.\n");
        return;
    }

    /* ---------- Mark ---------- */
    if (!extract_value(command, "Mark=", markbuf, sizeof markbuf)){
        printf("CMS: Missing Mark for INSERT.\n");
        return;
    }

    if (contains_reserved_pattern(markbuf)){
        printf("CMS: Invalid Mark format for INSERT.\n");
        return;
    }

    if (!parse_float(markbuf, &mark)){
        printf("CMS: Invalid Mark value. Must be numeric.\n");
        return;
    }

    if (mark < 0.0f || mark > 100.0f){
        printf("CMS: Invalid Mark. Must be between 0.00 and 100.00.\n");
        return;
    }

    // round mark to 2 decimal places
    {
        int tmp = (int)(mark * 100.0f + (mark >= 0 ? 0.5f : -0.5f));
        mark = tmp / 100.0f;
    }

    // database full?
    if (student_count >= MAX_RECORDS){
        printf("CMS: Database full. Cannot insert more records.\n");
        return;
    }

    // normalise casing
    to_title_case(name);
    to_title_case(programme);

    // insert new record
    students[student_count].id = id;
    strncpy(students[student_count].name, name, NAME_LENGTH - 1);
    students[student_count].name[NAME_LENGTH - 1] = '\0';

    strncpy(students[student_count].programme, programme, PROGRAMME_LENGTH - 1);
    students[student_count].programme[PROGRAMME_LENGTH - 1] = '\0';

    students[student_count].mark = mark;
    student_count++;

    printf("CMS: A new record with ID=%d is successfully inserted.\n", id);
}
