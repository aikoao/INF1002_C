#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#include "insert.h"

/* ---------- helpers ---------- */
static void rtrim(char *s){ int n=(int)strlen(s); while(n>0 && isspace((unsigned char)s[n-1])) s[--n]='\0'; }
static void ltrim(char *s){ int i=0,n=(int)strlen(s); while(i<n && isspace((unsigned char)s[i])) i++; if(i>0) memmove(s,s+i,n-i+1); }
static void trim(char *s){ rtrim(s); ltrim(s); }

static int parse_int(const char *s, int *out){ char *e; long v=strtol(s,&e,10); if(s==e) return 0; *out=(int)v; return 1; }
static int parse_float(const char *s, float *out){ char *e; float v=strtof(s,&e); if(s==e) return 0; *out=v; return 1; }

/* letters/spaces/punct allowed in names/programmes 82342934 */
static int is_letters_spaces(const char *s){
    int has_alpha=0;
    for(const unsigned char *p=(const unsigned char*)s; *p; ++p){
        if(isalpha(*p)){ has_alpha=1; continue; }
        if(*p==' '||*p=='-'||*p=='\''||*p=='.'||*p=='/') continue;
        return 0;
    }
    return has_alpha;
}

/* case-insensitive prefix & strstr */
static int ci_starts_with(const char *s, const char *pat){
    while (*s && *pat){
        if (tolower((unsigned char)*s) != tolower((unsigned char)*pat)) return 0;
        ++s; ++pat;
    }
    return *pat == '\0';
}
static const char* ci_strstr(const char *hay, const char *needle){
    if (!*needle) return hay;
    for (const char *p = hay; *p; ++p){
        const char *h = p, *n = needle;
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)){ ++h; ++n; }
        if (!*n) return p;
    }
    return NULL;
}

/* super-forgiving "KEY[ spaces ]=[ spaces ]VALUE" extractor (case-insensitive),
   value ends at next key token or end of line */
static int extract_value_core(const char *line, const char *key_with_eq, char *out, size_t outsz){
    char bare[32]; size_t klen=0;
    while (key_with_eq[klen] && key_with_eq[klen] != '=' && klen < sizeof(bare)-1){ bare[klen]=key_with_eq[klen]; klen++; }
    bare[klen]='\0';

    const char *p=line, *start=NULL;
    while (*p){
        while (*p && isspace((unsigned char)*p)) p++;
        if (ci_starts_with(p, bare)){
            const char *q=p+strlen(bare);
            while (*q && isspace((unsigned char)*q)) q++;
            if (*q=='='){
                q++;
                while (*q && isspace((unsigned char)*q)) q++;
                start=q; break;
            }
        }
        p++;
    }
    if(!start) return 0;

    const char *end=line+strlen(line);
    const char *cands[] = {" ID", " NAME", " PROGRAMME", " PROGRAM", " PROG", " MARK", " MARKS",
                           "ID",  "NAME",  "PROGRAMME",  "PROGRAM",  "PROG",  "MARK",  "MARKS", NULL};
    for (int i=0; cands[i]; ++i){
        const char *scan=start;
        for(;;){
            const char *hit=ci_strstr(scan, cands[i]);
            if(!hit) break;
            const char *t=hit+strlen(cands[i]);
            const char *t2=t; while(*t2 && isspace((unsigned char)*t2)) t2++;
            if (*t2=='=' || ci_starts_with(t2,"ID") || ci_starts_with(t2,"NAME") ||
                ci_starts_with(t2,"PROGRAMME") || ci_starts_with(t2,"PROGRAM") ||
                ci_starts_with(t2,"PROG") || ci_starts_with(t2,"MARK") || ci_starts_with(t2,"MARKS")){
                if(hit<end) end=hit;
                break;
            }
            scan=hit+1;
        }
    }

    size_t len=(size_t)(end-start);
    if(len>=outsz) len=outsz-1;
    memcpy(out,start,len); out[len]='\0';
    trim(out);
    return out[0]!='\0';
}

/* Try multiple alias keys (case-insensitive). Example: {"Programme=","Program=","Prog=",NULL} */
static int extract_value_alias(const char *line, const char *const keys[], char *out, size_t outsz){
    for (int i=0; keys[i]; ++i){
        if (extract_value_core(line, keys[i], out, outsz)) return 1;
    }
    return 0;
}

/* Title-case: "applied computing" -> "Applied Computing", "o'neill" -> "O'Neill" */
static void to_title_case(char *s){
    int new_word=1;
    for (unsigned char *p=(unsigned char*)s; *p; ++p){
        if (isalpha(*p)){
            if (new_word) *p=(unsigned char)toupper(*p);
            else          *p=(unsigned char)tolower(*p);
            new_word=0;
        } else {
            /* start new word after common separators */
            new_word = (*p==' '||*p=='-'||*p=='\''||*p=='/'||*p=='.');
        }
    }
}

/* find student by ID */
static int find_index_by_id(int id){
    for(int i=0;i<student_count;i++)
        if(students[i].id==id) return i;
    return -1;
}

/* ---------- main insert logic ---------- */
void insert_record(const char *command){
    if(!command){
        printf("CMS: Invalid command. Use: INSERT ID=... Name=... Programme=... Mark=...\n");
        return;
    }

    char idbuf[32]="", name[NAME_LENGTH]="", programme[PROGRAMME_LENGTH]="", markbuf[32]="";
    int id; float mark;

    /* ID (case-insensitive) */
    if(!extract_value_alias(command, (const char*[]){"ID=", NULL}, idbuf, sizeof idbuf)){
        printf("CMS: Missing ID for INSERT.\n"); return;
    }
    if(!parse_int(idbuf,&id) || id<=0){ printf("CMS: Invalid ID value.\n"); return; }
    if(find_index_by_id(id)!=-1){ printf("CMS: The record with ID=%d already exists.\n", id); return; }

    /* Name (case-insensitive) */
    if(!extract_value_alias(command, (const char*[]){"Name=", NULL}, name, sizeof name) || name[0]=='\0'){
        printf("CMS: Missing Name for INSERT.\n"); return;
    }
    if(!is_letters_spaces(name)){ printf("CMS: Invalid Name. Use only letters/spaces (- ' . / allowed).\n"); return; }

    /* Programme (accept Programme/Program/Prog; case-insensitive) */
    if(!extract_value_alias(command, (const char*[]){"Programme=","Program=","Prog=", NULL}, programme, sizeof programme) || programme[0]=='\0'){
        printf("CMS: Missing Programme for INSERT.\n"); return;
    }
    if(!is_letters_spaces(programme)){ printf("CMS: Invalid Programme. Use only letters/spaces (- ' . / allowed).\n"); return; }

    /* Mark (accept Mark/Marks; case-insensitive) */
    if(!extract_value_alias(command, (const char*[]){"Mark=","Marks=", NULL}, markbuf, sizeof markbuf)){
        printf("CMS: Missing Mark for INSERT.\n"); return;
    }
    if(!parse_float(markbuf,&mark)){ printf("CMS: Invalid Mark value.\n"); return; }
    if(mark<0.0f || mark>100.0f){ printf("CMS: Invalid Mark. Must be between 0.00 and 100.00.\n"); return; }

    if(student_count >= MAX_RECORDS){ printf("CMS: Database full. Cannot insert more records.\n"); return; }

    /* normalize casing for Name & Programme */
    to_title_case(name);
    to_title_case(programme);

    /* insert */
    students[student_count].id = id;
    strncpy(students[student_count].name, name, NAME_LENGTH - 1);
    students[student_count].name[NAME_LENGTH - 1] = '\0';
    strncpy(students[student_count].programme, programme, PROGRAMME_LENGTH - 1);
    students[student_count].programme[PROGRAMME_LENGTH - 1] = '\0';
    students[student_count].mark = mark;
    student_count++;

    printf("CMS: A new record with ID=%d is successfully inserted.\n", id);
}
