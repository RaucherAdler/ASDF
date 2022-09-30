#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "asdf.h"

/* TO DO:
 * In-line, nested, and associative arrays
 */

int asdfgetval(char* filename, char* valname, struct asdfmulti* retval) { /* Main parser on which all reading functions rely - outputs to retval */
    /* TYPES:
     * int = 0
     * char = 1
     * array = 2
     * */

    int i = 0;
    unsigned int j = 0;
    int o = 0;
    char lnbuf[1024] = {0};
    char* sstr = calloc((strlen(valname) + 4), sizeof(char));
    char* r;
    struct asdfmulti* cur = NULL;
    struct asdfmulti* pre = NULL;

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        free(sstr);
        return(0);
    }

    if (retval == NULL) {
        free(sstr);
        return(0);
    }

    switch (retval->type) {
        case T_TBD:
            strcpy(sstr, valname);
            break;
        case T_INT:
        case T_STR:
            sprintf(sstr, "%s = ", valname);
            break;
        case T_ARR:
            sprintf(sstr, "[%s\n", valname);
            break;
    }

    while (fgets(lnbuf, 1024, fp) != NULL) {
        if (lnbuf[0] == '\n') continue;

        if (i == 0) {
            r = strstr(lnbuf, sstr);
            if (r != NULL) {
                for (j=0;j<strlen(lnbuf);j++) {
                    switch (lnbuf[j]) {
                        case ' ':
                        case '\t':
                            o++;
                            break;
                        default:
                            j = strlen(lnbuf);
                    }
                }
            } else {
                memset(lnbuf, 0, 1024);
                continue;
            }
            if (((r - lnbuf) == o) || ((retval->type = T_TBD) && ((r - lnbuf) == o+1) && (lnbuf[o] != '"'))) {
parsesw:
                switch (retval->type) {
                    case T_TBD: /* Type inference */
                        retval->type = (2 * (lnbuf[o] == '[')) + (lnbuf[o] == '"');
                        goto parsesw;
                    case T_INT:
                        retval->value = calloc(1, sizeof(int)); 
                        sscanf(lnbuf, "%*[^=]= %i[^\n]", (int*) retval->value);
                        break;
                    case T_STR: 
                        retval->value = calloc((strlen(lnbuf) - strlen(sstr) - 1), sizeof(char));
                        sscanf(lnbuf, "%*[^=]= \"%[^\"]", (char*) retval->value);
                        break;
                    case T_ARR:
                        i = 1;
                        break;
                }
            } else {
                o = 0;
                memset(lnbuf, 0, 1024);
                continue;
            }
        } else {
            for (j=0;j<strlen(lnbuf);j++) {
                switch (lnbuf[j]) {
                    case ' ':
                    case '\t':
                        o++;
                        break;
                    default:
                        j = strlen(lnbuf);
                }
            }
            if (lnbuf[o] != ']') {
                if (cur == NULL) {
                    cur = malloc(sizeof(struct asdfmulti));
                    cur->next = NULL;
                    cur->value = NULL;
                }
                if (lnbuf[o] == '"') { /* Will need to account for tabs */
                    cur->type = T_STR;
                    cur->value = calloc((strlen(lnbuf) - 1), sizeof(char));
                    sscanf(lnbuf, "%*[^\"]\"%[^\"]", (char*) cur->value);
                } else {
                    cur->type = T_INT;
                    cur->value = calloc(1, sizeof(int)); 
                    sscanf(lnbuf, "%*[^0123456789]%i[^\n]", (int*) cur->value); /* 0..9 over 0-9 for older compiler compatibility */
                }
                if (retval->next == NULL) {
                    retval->next = cur;
                } else {
                    pre->next = cur;
                }
                pre = cur;
                cur = cur->next;
            } else {
                i = 0;
            }
        }

        if (i == 0) {
            return(1);
        } else {
            o = 0;
            memset(lnbuf, 0, 1024);
            continue;
        }
    }
    free(sstr);
    return(0);
}


int asdffreearr(struct asdfmulti* array) { /* Frees A given ASDF Linked List */
    struct asdfmulti* cur;
    
    while (array != NULL) {
        cur = array;
        array = array->next;
        if (cur->value != NULL) free(cur->value);
        free(cur);
    }
    return(0);
}


struct asdfmulti* asdfgenllist(char* str, int* val) {
    struct asdfmulti* arr = malloc(sizeof(struct asdfmulti));
    arr->type = T_ARR;
    if ((void*) str != (void*) val) {
        arr->next = malloc(sizeof(struct asdfmulti));
        arr->next->next = NULL;
    } else { 
        arr->next = NULL;
    }
    if (str != NULL) {
        arr->next->type = T_STR;
        arr->next->value = calloc(strlen(str)+1, sizeof(char));
        strcpy((char*) arr->next->value, str);
    } else if (val != NULL) {
        arr->next->type = T_INT;
        arr->next->value = calloc(1, sizeof(int)); 
        *(int *)arr->next->value = *val;
    }
    return(arr);
}

void asdfappendllist(struct asdfmulti* head, char* str, int* val) {
    struct asdfmulti* cur = NULL;

    if (head == NULL) {
        head = malloc(sizeof(struct asdfmulti));
        head->type = T_ARR;
        head->next = NULL;
    }
    cur = head;
    while (1) {
        if (cur->next != NULL) {
            cur = cur->next;
        } else {
            break;
        }
    }
    cur->next = malloc(sizeof(struct asdfmulti));
    cur = cur->next;
    if (str != NULL) {
        cur->type = T_STR;
        cur->value = calloc(strlen(str)+1, sizeof(char));
        strcpy((char*) cur->value, str);
    } else if (val != NULL) {
        cur->type = T_INT;
        cur->value = calloc(1, sizeof(int));
        *(int*)cur->value = *val;
    }
    cur->next = NULL;
}


/* Functions for outputting ASDF Linked Lists in a formatted manner */
int asdfprintarr_ol(struct asdfmulti* array) { /* Print ASDF Linked List in ordered (numbered) list */
    int i = 1;
    struct asdfmulti* cur = NULL;
    if (array == NULL) {
        return(1);
    } else {
        if (array->next != NULL) {
            cur = array;
        }
    }

    while (cur != NULL) {
        switch (cur->type) {
            case T_INT:
                if (cur->value != NULL) {
                    printf("%i. %i\n", i, *(int*)cur->value);
                }
                i++;
                break;
            case T_STR:
                if (cur->value != NULL) {
                    printf("%i. %s\n", i, (char*) cur->value);
                }
                i++;
                break;
            default:
                break;
        }
        cur = cur->next;
    }
    return(0);
}


int asdfprintarr_ul(struct asdfmulti* array, char* delim) { /* Print ASDF Linked List in unordered list */
    struct asdfmulti* cur;

    if (array == NULL) {
        return(1);
    } else {
        cur = array;
    }
    
    if (delim == NULL) {
        delim = "-";
    }

    while (cur != NULL) {
        switch (cur->type) {
            case T_INT:
                if (cur->value != NULL) {
                    printf("%s %i\n", delim, *(int*)cur->value);
                }
                break;
            case T_STR:
                if (cur->value != NULL) {
                    printf("%s %s\n", delim, (char*) cur->value);
                }
                break;
            default:
                break;
        }
        cur = cur->next;
    }
    return(0);
}


struct asdfmulti* asdfgetindex(struct asdfmulti* array, unsigned int index) { /* Get given index of ASDF Linked List - returns NULL if index is out of range */
    struct asdfmulti* cur = array;
    unsigned int i;
    for (i=0;i<=index;i++) {
        if (cur->next == NULL) {
            if (i != index) cur = NULL;
            break;
        } else {
            cur = cur->next;
        }
    }
    return(cur);
}


unsigned int asdfarrlen(struct asdfmulti* array) {
    unsigned int i = 0;
    struct asdfmulti* cur = NULL;
    for (cur = array; cur; cur = cur->next) {
        i++;
    }
    i--;
    return(i);
}


/* Wrappers for reading */
int* asdfgetint(char* filename, char* valname) { /* Returns NULL pointer if not found, else returns pointer to result and must be freed */
    struct asdfmulti ams = {T_INT, NULL, NULL};
    int* p = NULL;
    if (asdfgetval(filename, valname, &ams) != 0) {
        p = malloc(sizeof(int));
        *p = *(int*)ams.value;
    }
    return(p);
}


char* asdfgetstr(char* filename, char* valname) { /* Returns NULL pointer if not found, else returns pointer to result and must be freed */
    char* retstr = NULL;
    struct asdfmulti ams = {T_STR, NULL, NULL};
    if (asdfgetval(filename, valname, &ams) != 0) {
        retstr = malloc(strlen((char*) ams.value)+1);
        strcpy(retstr, (char*) ams.value);
    }
    if (retstr != NULL) {
        free(ams.value);
    }
    return(retstr);
}


struct asdfmulti* asdfgetarr(char* filename, char* valname) { /* Needs to free returned value if value is found - Returns NULL if value isn't found */
    struct asdfmulti* array = malloc(sizeof(struct asdfmulti));
    array->type = T_ARR;
    array->next = NULL;
    array->value = NULL;
    array->value = NULL;
    if (asdfgetval(filename, valname, array) == 0) {
        asdffreearr(array);
        array = NULL;
    }
    return(array);
}


/* Functions for writing */
int asdfassignval(char* filename, char* valname, struct asdfmulti* inval) { /* Assigns a given value or list of values to an ASDF Array */
    FILE* tfp = NULL;
    FILE* fp = NULL;
    char* r;
    int i = 0;
    unsigned int j = 0;
    unsigned int o = 0;
    char* sstr = calloc((strlen(valname) + 4), sizeof(char));
    char lnbuf[1024] = {0};
    struct asdfmulti* cur;
    char tname[L_tmpnam] = {0};

    for (j=0;j<=L_tmpnam-5;j++) { /* Substitute for tmpnam */
        tname[j] = 'a' + (rand() % 26);
    }
    strcat(tname, ".tmp");

    tfp = fopen(tname, "w+b");
    if (tfp == NULL) {
        free(sstr);
        return(0);
    }
    fp = fopen(filename, "r+b");
    if (fp == NULL) {
        /* Create new file if it doesn't already exist */
        fp = fopen(filename, "w+b");
        if (fp == NULL) {
            fclose(tfp);
            remove(tname);
            free(sstr);
            return(0);
        }
    }
    switch (inval->type) {
        case T_ARR:
            sprintf(sstr, "[%s\n", valname);
            break;
        default:
            sprintf(sstr, "%s = ", valname);
            break;
    }
    fseek(fp, 0, SEEK_SET);
    while (fgets(lnbuf, 1024, fp) != NULL) {
        r = strstr(lnbuf, sstr);
        if (r != NULL || i == 2) { /* If any part of array */
            for (j=0;j<strlen(lnbuf);j++) {
                switch (lnbuf[j]) {
                    case ' ':
                    case '\t':
                        o++;
                        break;
                    default:
                        j = strlen(lnbuf);
                        break;
                }
            }
        }
        if ((r - lnbuf) == (int) o) { /* Cast to prevent (int) and (unsigned int) comparison */
swst:
            switch (inval->type) {
                case T_TBD:
                    if (lnbuf[strlen(lnbuf-2)] == '"') {
                        inval->type = T_STR;
                        goto swst;
                    } else {
                        inval->type = T_INT;
                    }
                    break;
                case T_INT:
                    if (inval->value != NULL) {
                        for (j=0;j<o;j++) { /* Match indentation */
                            fputc(' ', tfp);
                        }
                        fprintf(tfp, "%s%i\n", sstr, *(int*)inval->value);
                        i = 1;
                    }
                    break;
                case T_STR:
                    if (inval->value != NULL) {
                        for (j=0;j<o;j++) {
                            fputc(' ', tfp);
                        }
                        fprintf(tfp, "%s\"%s\"\n", sstr, (char*) inval->value);
                        i = 1;
                    }
                    break;
                case T_ARR:
                    i = 2;
                    o = 0;
                    continue;

            }
        }
        switch (i) {
            case 0:
                fputs(lnbuf, tfp);
                o = 0;
                memset(lnbuf, 0, 1024);
                continue;
            case 1:
            case 2:
                if (lnbuf[o] == ']') {
                    goto crarr;
                }
                o = 0;
                memset(lnbuf, 0, 1024);
                continue;
        }
crarr:
            cur = inval;
            for (cur = inval; cur; cur = cur->next) {
                switch (cur->type) {
                    case T_TBD:
                        break;
                    case T_INT:
                        if (cur == inval) {
                            fprintf(tfp, "%s%i\n", sstr, *(int*)cur->value);
                        } else {
                            for (j=0;j<o+4;j++) {
                                fputc(' ', tfp);
                            }
                            fprintf(tfp, "%i\n", *(int*)cur->value);
                        }
                        break;
                    case T_STR:
                        if (cur->value != NULL) {
                            if (cur == inval) {
                                fprintf(tfp, "%s\"%s\"\n", sstr, (char*) cur->value);
                            } else {
                                for (j=0;j<o+4;j++) {
                                    fputc(' ', tfp);
                                }
                                fprintf(tfp, "\"%s\"\n", (char*) cur->value);
                            }
                        }
                        break;
                    case T_ARR:
                        fprintf(tfp, "%s", sstr);
                        break;
                    }
            }
        if (inval->type == T_ARR) {
            fputs("]\n", tfp);
        }
        i = 1;
        o = 0;
        memset(lnbuf, 0, 1024);
    }
    if (i == 0) {
        goto crarr;
    }
    free(sstr);

    freopen(filename, "wb", fp);
    fseek(tfp, 0, SEEK_SET); /* Copy temp file to real file */
    while (fgets(lnbuf, 1024, tfp) != NULL) {
        fputs(lnbuf, fp);
        memset(lnbuf, 0, 1024);
    }
    fclose(tfp);
    fclose(fp);
    remove(tname);
    return(i);
}

int asdfappendarr(char* filename, char* valname, struct asdfmulti* inval) { /* Append a value to an ASDF array - creates array if it doesn't exist */
    FILE* tfp = NULL;
    FILE* fp = NULL;
    char* r;
    int n = 0;
    int i = 0;
    unsigned int j = 0;
    unsigned int o = 0;
    char* sstr = calloc((strlen(valname) + 3), sizeof(char));
    char lnbuf[1024] = {0};
    struct asdfmulti* cur = NULL;
    char tname[L_tmpnam] = {0};

    for (j=0;j<=L_tmpnam-5;j++) { /* Substitute for tmpnam */
        tname[j] = 'a' + (rand() % 26);
    }
    strcat(tname, ".tmp");

    tfp = fopen(tname, "w+b");
    if (tfp == NULL) {
        free(sstr);
        return(0);
    }
    fp = fopen(filename, "r+b");
    if (fp == NULL) {
            free(sstr);
            fclose(tfp);
            remove(tname);
            return(asdfassignval(filename, valname, inval));
    }

    sprintf(sstr, "[%s\n", valname);
    fseek(fp, 0, SEEK_SET);
    while (fgets(lnbuf, 1024, fp) != NULL) {
        if (n == 0) {
            r = strstr(lnbuf, sstr);
            if (r != NULL) {
                for (j=0;j<strlen(lnbuf);j++) {
                    switch (lnbuf[j]) {
                        case ' ':
                        case '\t':
                            o++;
                            break;
                        default:
                            j = strlen(lnbuf);
                    }
                }
            }
            if ((unsigned)(r - lnbuf) == o) {
                n = 1;
            }
            o = 0;
        } else if (n == 1) {
            for (j=0;j<strlen(lnbuf);j++) {
                switch (lnbuf[j]) {
                    case ' ':
                    case '\t':
                        o++;
                        break;
                    default:
                        j = strlen(lnbuf);
                }
            }
            if (lnbuf[o] == ']') { 
                for (cur = inval; cur; cur = cur->next) {
                    switch (inval->type) {
                        case T_INT:
                            for (j=0;j<o;j++) {
                                fputc(' ', tfp);
                            }
                            fprintf(tfp, "%i\n", *(int*)cur->value);
                            break;
                        case T_STR:
                            if (inval->value != NULL) {
                                for (j=0;j<o;j++) {
                                    fputc(' ', tfp);
                                }
                                fprintf(tfp, "\"%s\"\n", (char*) cur->value);
                            }
                            break;
                        default:
                            break;
                    }
                }
                i = 1;
                n = 2;
            }
        }
        fputs(lnbuf, tfp);
        memset(lnbuf, 0, 1024);
        o = 0;
    }
    free(sstr);

    fseek(fp, 0, SEEK_SET);
    fseek(tfp, 0, SEEK_SET); /* Copy temp file to real file */
    while (fgets(lnbuf, 1024, tfp) != NULL) {
        fputs(lnbuf, fp);
        memset(lnbuf, 0, 1024);
    }
    if (n == 0) { /* Create array if one does not yet exist */
        fprintf(fp, "[%s\n", valname);
        for (cur = inval; cur; cur = cur->next) {
            switch (inval->type) {
                case T_INT:
                    for (j=0;j<o;j++) {
                        fputc(' ', fp);
                    }
                    fprintf(fp, "%i\n", *(int*)cur->value);
                    break;
                case T_STR:
                    if (inval->value != NULL) {
                        for (j=0;j<o;j++) {
                            fputc(' ', fp);
                        }
                        fprintf(fp, "\"%s\"\n", (char*) cur->value);
                    }
                    break;
                default:
                    break;
            }
        }
        fputs("]\n", fp);
    }
    fclose(tfp);
    fclose(fp);
    remove(tname);
    return(i);
}


/* Wrappers for writing */
int asdfassignstr(char* filename, char* valname, char* instr) {
    struct asdfmulti tmp = {T_STR, NULL, NULL};
    tmp.value = (void*) instr;
    return(asdfassignval(filename, valname, &tmp));
}


int asdfassignint(char* filename, char* valname, int inint) {
    struct asdfmulti tmp = {T_INT, NULL, NULL};
    *(int*)tmp.value = inint;
    return(asdfassignval(filename, valname, &tmp));
}


int asdfappendstr(char* filename, char* valname, char* instr) {
    struct asdfmulti tmp = {T_STR, NULL, NULL};
    tmp.value = (void*) instr;
    return(asdfappendarr(filename, valname, &tmp));
}


int asdfappendint(char* filename, char* valname, int inint) {
    struct asdfmulti tmp = {T_INT, NULL, NULL};
    *(int*)tmp.value = inint;
    return(asdfappendarr(filename, valname, &tmp));
}

