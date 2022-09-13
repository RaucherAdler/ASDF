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
                        sscanf(lnbuf, "%*[^=]= %i[^\n]", &retval->ival);
                        break;
                    case T_STR: 
                        retval->strval = calloc((strlen(lnbuf) - strlen(sstr) - 1), sizeof(char));
                        sscanf(lnbuf, "%*[^=]= \"%[^\"]", retval->strval);
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
                    cur->arrval = NULL;
                    cur->strval = NULL;
                }
                if (lnbuf[o] == '"') { /* Will need to account for tabs */
                    cur->type = T_STR;
                    cur->strval = calloc((strlen(lnbuf) - 1), sizeof(char));
                    sscanf(lnbuf, "%*[^\"]\"%[^\"]", cur->strval);
                } else {
                    cur->type = T_INT;
                    sscanf(lnbuf, "%*[^0123456789]%i[^\n]", &cur->ival); /* 0..9 over 0-9 for older compiler compatibility*/
                }
                if (retval->arrval == NULL) {
                    retval->arrval = cur;
                } else {
                    pre->arrval = cur;
                }
                pre = cur;
                cur = cur->arrval;
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
        array = array->arrval;
        if (cur->strval != NULL) free(cur->strval);
        free(cur);
    }
    return(0);
}


struct asdfmulti* asdfgenllist(char* str, int* val) {
    struct asdfmulti* arr = malloc(sizeof(struct asdfmulti));
    arr->type = T_ARR;
    if ((void*) str != (void*) val) {
        arr->arrval = malloc(sizeof(struct asdfmulti));
        arr->arrval->arrval = NULL;
    } else { 
        arr->arrval = NULL;
    }
    if (str != NULL) {
        arr->arrval->type = T_STR;
        arr->arrval->strval = calloc(strlen(str)+1, sizeof(char));
        strcpy(arr->arrval->strval, str);
    } else if (val != NULL) {
        arr->arrval->type = T_INT;
        arr->arrval->ival = *val;
    }
    return(arr);
}

void asdfappendllist(struct asdfmulti* head, char* str, int* val) {
    struct asdfmulti* cur = NULL;

    if (head == NULL) {
        head = malloc(sizeof(struct asdfmulti));
        head->type = T_ARR;
        head->arrval = NULL;
    }
    cur = head;
    while (1) {
        if (cur->arrval != NULL) {
            cur = cur->arrval;
        } else {
            break;
        }
    }
    cur->arrval = malloc(sizeof(struct asdfmulti));
    cur = cur->arrval;
    if (str != NULL) {
        cur->type = T_STR;
        cur->strval = calloc(strlen(str)+1, sizeof(char));
        strcpy(cur->strval, str);
    } else if (val != NULL) {
        cur->type = T_INT;
        cur->ival = *val;
    }
    cur->arrval = NULL;
}


/* Functions for outputting ASDF Linked Lists in a formatted manner */
int asdfprintarr_ol(struct asdfmulti* array) { /* Print ASDF Linked List in ordered (numbered) list */
    int i = 1;
    struct asdfmulti* cur = NULL;
    if (array == NULL) {
        return(1);
    } else {
        if (array->arrval != NULL) {
            cur = array;
        }
    }

    while (cur != NULL) {
        switch (cur->type) {
            case T_INT:
                printf("%i. %i\n", i, cur->ival);
                i++;
                break;
            case T_STR:
                if (cur->strval != NULL) {
                    printf("%i. %s\n", i, cur->strval);
                }
                i++;
                break;
            default:
                break;
        }
        cur = cur->arrval;
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
                printf("%s %i\n", delim, cur->ival);
                break;
            case T_STR:
                if (cur->strval != NULL) {
                    printf("%s %s\n",delim, cur->strval);
                }
                break;
            default:
                break;
        }
        cur = cur->arrval;
    }
    return(0);
}


struct asdfmulti* asdfgetindex(struct asdfmulti* array, int index) { /* Get given index of ASDF Linked List - returns NULL if index is out of range */
    struct asdfmulti* cur = array;
    int i;
    for (i=0;i<=index;i++) {
        if (cur->arrval == NULL) {
            if (i != index) cur = NULL;
            break;
        } else {
            cur = cur->arrval;
        }
    }
    return(cur);
}


int asdfarrlen(struct asdfmulti* array) {
    int i = 0;
    struct asdfmulti* cur = array;
    while (cur != NULL) {
        i++;
        cur = cur->arrval;
    }
    return(i);
}


/* Wrappers for reading */
int* asdfgetint(char* filename, char* valname) { /* Returns NULL pointer if not found, else returns pointer to result and must be freed */
    struct asdfmulti ams;
    int* p = NULL;
    ams.arrval = NULL;
    ams.strval = NULL;
    ams.type = T_INT;
    if (asdfgetval(filename, valname, &ams) != 0) {
        p = malloc(sizeof(int));
        *p = ams.ival;
    }
    return(p);
}


char* asdfgetstr(char* filename, char* valname) { /* Returns NULL pointer if not found, else returns pointer to result and must be freed */
    char* retstr = NULL;
    struct asdfmulti ams;
    ams.arrval = NULL;
    ams.strval = NULL;
    ams.type = T_STR;
    if (asdfgetval(filename, valname, &ams) != 0) {
        retstr = malloc(strlen(ams.strval)+1);
        strcpy(retstr, ams.strval);
    }
    if (retstr != NULL) {
        free(ams.strval);
    }
    return(retstr);
}


struct asdfmulti* asdfgetarr(char* filename, char* valname) { /* Needs to free returned value if value is found - Returns NULL if value isn't found */
    struct asdfmulti* array = malloc(sizeof(struct asdfmulti));
    array->type = T_ARR;
    array->arrval = NULL;
    array->strval = NULL;
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
                    for (j=0;j<o;j++) { /* Match indentation */
                        fputc(' ', tfp);
                    }
                    fprintf(tfp, "%s%i\n", sstr, inval->ival);
                    i = 1;
                    break;
                case T_STR:
                    if (inval->strval != NULL) {
                        for (j=0;j<o;j++) {
                            fputc(' ', tfp);
                        }
                        fprintf(tfp, "%s\"%s\"\n", sstr, inval->strval);
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
            while (cur != NULL) {
                switch (cur->type) {
                    case T_TBD:
                        break;
                    case T_INT:
                        if (cur == inval) {
                            fprintf(tfp, "%s%i\n", sstr, cur->ival);
                        } else {
                            for (j=0;j<o+4;j++) {
                                fputc(' ', tfp);
                            }
                            fprintf(tfp, "%i\n", cur->ival);
                        }
                        break;
                    case T_STR:
                        if (cur->strval != NULL) {
                            if (cur == inval) {
                                fprintf(tfp, "%s\"%s\"\n", sstr, cur->strval);
                            } else {
                                for (j=0;j<o+4;j++) {
                                    fputc(' ', tfp);
                                }
                                fprintf(tfp, "\"%s\"\n", cur->strval);
                            }
                        }
                        break;
                    case T_ARR:
                        fprintf(tfp, "%s", sstr);
                        break;
                    }
                cur = cur->arrval;
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
    int o = 0;
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
            if ((r - lnbuf) == o) {
                n = 1;
            }
            o = 0;
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
            if (lnbuf[o] == ']') { 
                cur = inval;
                while (cur != NULL) {
                    switch (inval->type) {
                        case T_INT:
                            fprintf(tfp, "%i\n", cur->ival);
                            break;
                        case T_STR:
                            if (inval->strval != NULL) {
                                fprintf(tfp, "\"%s\"\n", cur->strval);
                            }
                            break;
                        default:
                            break;
                    }
                    cur = cur->arrval;
                }
                i = 1;
                n = 0;
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
    fclose(tfp);
    fclose(fp);
    remove(tname);
    return(i);
}


/* Wrappers for writing */
int asdfassignstr(char* filename, char* valname, char* instr) {
    struct asdfmulti tmp = {T_STR, 0, NULL, NULL};
    tmp.strval = instr;
    return(asdfassignval(filename, valname, &tmp));
}


int asdfassignint(char* filename, char* valname, int inint) {
    struct asdfmulti tmp = {T_INT, 0, NULL, NULL};
    tmp.ival = inint;
    return(asdfassignval(filename, valname, &tmp));
}


int asdfappendstr(char* filename, char* valname, char* instr) {
    struct asdfmulti tmp = {T_STR, 0, NULL, NULL};
    tmp.strval = instr;
    return(asdfappendarr(filename, valname, &tmp));
}


int asdfappendint(char* filename, char* valname, int inint) {
    struct asdfmulti tmp = {T_INT, 0, NULL, NULL};
    tmp.ival = inint;
    return(asdfappendarr(filename, valname, &tmp));
}

