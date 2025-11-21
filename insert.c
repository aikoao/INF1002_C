#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"

static void to_title_case(char *s){
    int cap = 1;
    for(int i = 0; s[i]; i++){
        if (isspace((unsigned char)s[i]) || s[i]=='-' || s[i]=='\'' || s[i]=='/'){
            cap = 1;
        } else if (cap){
            s[i] = toupper((unsigned char)s[i]);
            cap = 0;
        } else {
            s[i] = tolower((unsigned char)s[i]);
        }
    }
}

// ---------- helper functions ----------

// trim spaces
static void rtrim(char *s){ int n=(int)strlen(s); while(n>0 && isspace((unsigned char)s[n-1])) s[--n]='\0'; }
static void ltrim(char *s){ int i=0,n=(int)strlen(s); while(i<n && isspace((unsigned char)s[i])) i++; if(i>0) memmove(s,s+i,n-i+1); }
static void trim(char *s){ rtrim(s); ltrim(s); }

// parse integer and float safely
static int parse_int(const char *s, int *out){ char *e; long v=strtol(s,&e,10); if(s==e) return 0; *out=(int)v; return 1; }
static int parse_float(const char *s, float *out){ char *e; float v=strtof(s,&e); if(s==e) return 0; *out=v; return 1; }

// check name/programme validity (letters + some punctuation)
static int is_letters_spaces(const char *s){
    int has_alpha=0;
    for(const unsigned char *p=(const unsigned char*)s; *p; ++p){
        if(isalpha(*p)){ has_alpha=1; continue; }
        if(*p==' '||*p=='-'||*p=='\''||*p=='.'||*p=='/') continue;
        return 0;
    }
    return has_alpha;
}

// case-insensitive prefix check
static int ci_starts_with(const char *s, const char *pat){
    while (*s && *pat){
        if (tolower((unsigned char)*s) != tolower((unsigned char)*pat)) return 0;
        ++s; ++pat;
    }
    return *pat == '\0';
}

// case-insensitive strstr
static const char* ci_strstr(const char *hay, const char *needle){
    if (!*needle) return hay;
    for (const char *p = hay; *p; ++p){
        const char *h = p, *n = needle;
        while (*h && *n &&
               tolower((unsigned char)*h) == tolower((unsigned char)*n)){
            ++h; ++n;
        }
        if (!*n) return p;  // found full needle
    }
    return NULL;
}

/* compact keyword-like check:
   returns 1 if s essentially equals "id"/"name"/"programme"/"program"/"prog"/"mark"/"marks"
   (ignores spaces and case) */
static int is_keyword_like(const char *s){
    char buf[64];
    size_t w = 0;
    for (const unsigned char *p=(const unsigned char*)s; *p && w < sizeof(buf)-1; ++p){
        if (!isspace(*p))
            buf[w++] = (char)tolower(*p);
    }
    buf[w] = '\0';
    return  strcmp(buf,"id") == 0       ||
            strcmp(buf,"name") == 0     ||
            strcmp(buf,"programme") == 0||
            strcmp(buf,"program") == 0  ||
            strcmp(buf,"prog") == 0     ||
            strcmp(buf,"mark") == 0     ||
            strcmp(buf,"marks") == 0;
}

/* return 1 if value contains another field pattern like "ID=" inside (wrong format) */
static int contains_reserved_pattern(const char *s){
    return  ci_strstr(s,"ID=")        ||
            ci_strstr(s,"NAME=")      ||
            ci_strstr(s,"PROGRAMME=") ||
            ci_strstr(s,"PROGRAM=")   ||
            ci_strstr(s,"MARK=")      ||
            ci_strstr(s,"MARKS=");
}


/* Extract value for a given key (e.g., "Name") in a very forgiving way:
   - matches key in any case
   - allows optional spaces around '='
   - value ends at the next key (any case) or end of line */
static int extract_value(const char *line, const char *key_with_eq, char *out, size_t outsz){
    // derive bare key (e.g., "Name" from "Name=")
    char bare[16]; size_t klen = 0;
    while (key_with_eq[klen] && key_with_eq[klen] != '=' && klen < sizeof(bare)-1){
        bare[klen] = key_with_eq[klen]; klen++;
    }
    bare[klen] = '\0';

    // 1) find key position (any case)
    const char *p = line;
    const char *start = NULL;
    while (*p){
        // skip spaces
        while (*p && isspace((unsigned char)*p)) p++;
        if (ci_starts_with(p, bare)){
            const char *q = p + strlen(bare);
            // allow spaces before '='
            while (*q && isspace((unsigned char)*q)) q++;
            if (*q == '='){
                q++; // skip '='
                // allow spaces after '='
                while (*q && isspace((unsigned char)*q)) q++;
                start = q;
                break;
            }
        }
        // move forward
        p++;
    }
    if (!start) return 0;

    // 2) find end by searching next key token
    const char *end = line + strlen(line);
    const char *cands[] = {
        " ID", " NAME", " PROGRAMME", " PROGRAM", " PROG", " MARK", " MARKS",
        "ID",  "NAME",  "PROGRAMME",  "PROGRAM",  "PROG",  "MARK",  "MARKS",
        NULL
    };
    for (int i=0; cands[i]; ++i){
        const char *scan = start;
        for (;;){
            const char *hit = ci_strstr(scan, cands[i]);
            if (!hit) break;
            // ensure it's a key start: followed by optional spaces and maybe '='
            const char *t = hit + strlen(cands[i]);
            const char *t2 = t;
            while (*t2 && isspace((unsigned char)*t2)) t2++;
            if (*t2 == '=' || ci_starts_with(t2, "ID") || ci_starts_with(t2, "NAME")
                || ci_starts_with(t2, "PROGRAMME") || ci_starts_with(t2, "PROGRAM")
                || ci_starts_with(t2, "PROG") || ci_starts_with(t2, "MARK")
                || ci_starts_with(t2, "MARKS")){
                if (hit < end) end = hit;   // choose earliest next key
                break;
            }
            scan = hit + 1; // keep searching
        }
    }

    // 3) copy trimmed slice
    size_t len = (size_t)(end - start);
    if (len >= outsz) len = outsz - 1;
    memcpy(out, start, len);
    out[len] = '\0';
    // trim both sides
    trim(out);
    return out[0] != '\0';
}


// find student by ID
static int find_index_by_id(int id){
    for(int i=0;i<student_count;i++)
        if(students[i].id==id) return i;
    return -1;
}

// ---------- main insert logic ----------
void insert_record(const char *command){
    // Save state BEFORE making changes (for your UNDO feature)
    save_undo_state("INSERT operation");

    if(!command){
        printf("CMS: Invalid command. Use: INSERT ID=... Name=... Programme=... Mark=...\n");
        return;
    }

    // fields
    char idbuf[32]="", name[NAME_LENGTH]="", programme[PROGRAMME_LENGTH]="", markbuf[32]="";
    int id; float mark;

    /* ---------- ID ---------- */
    if(!extract_value(command,"ID=",idbuf,sizeof idbuf)){
        printf("CMS: Missing ID for INSERT.\n"); return;
    }
    int len = (int)strlen(idbuf);
    if (len != 7){
        printf("CMS: Invalid ID format. ID must be a 7-digit number from 15XXXXX to 25XXXXX.\n");
        return;
    }
    for (int i=0; i<len; ++i){
        if (!isdigit((unsigned char)idbuf[i])){
            printf("CMS: Invalid ID format. ID must be numeric.\n");
            return;
        }
    }
    if(!parse_int(idbuf,&id)){
        printf("CMS: Invalid ID value.\n"); return;
    }
    int prefix = (id / 100000);      // first two digits
    if (prefix < 15 || prefix > 25){
        printf("CMS: Invalid ID range. Only IDs from 15XXXXX to 25XXXXX are allowed.\n");
        return;
    }
    if(find_index_by_id(id)!=-1){
        printf("CMS: The record with ID=%d already exists.\n", id);
        return;
    }

    /* ---------- Name ---------- */
    if(!extract_value(command,"Name=",name,sizeof name) || name[0]=='\0'){
        printf("CMS: Missing Name for INSERT.\n"); return;
    }
    if(!is_letters_spaces(name)){
        printf("CMS: Invalid Name. Use only letters/spaces (- ' . / allowed).\n");
        return;
    }
    if(is_keyword_like(name) || contains_reserved_pattern(name)){
        printf("CMS: Invalid Name format for INSERT.\n");
        return;
    }

    /* ---------- Programme ---------- */
    if(!extract_value(command,"Programme=",programme,sizeof programme) || programme[0]=='\0'){
        printf("CMS: Missing Programme for INSERT.\n"); return;
    }
    if(!is_letters_spaces(programme)){
        printf("CMS: Invalid Programme. Use only letters/spaces (- ' . / allowed).\n");
        return;
    }
    if(is_keyword_like(programme) || contains_reserved_pattern(programme)){
        printf("CMS: Invalid Programme format for INSERT.\n");
        return;
    }

    /* ---------- Mark ---------- */
    if(!extract_value(command,"Mark=",markbuf,sizeof markbuf)){
        printf("CMS: Missing Mark for INSERT.\n"); return;
    }
    if(is_keyword_like(markbuf) || contains_reserved_pattern(markbuf)){
        printf("CMS: Invalid Mark format for INSERT.\n");
        return;
    }
    if(!parse_float(markbuf,&mark)){
        printf("CMS: Invalid Mark value.\n"); return;
    }
    if(mark < 0.0f || mark > 100.0f){
        printf("CMS: Invalid Mark. Must be between 0.00 and 100.00.\n");
        return;
    }

    // Database full
    if(student_count >= MAX_RECORDS){
        printf("CMS: Database full. Cannot insert more records.\n");
        return;
    }

    // 👉 NORMALISE CASING *before* saving
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
