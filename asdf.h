#ifndef _H_ASDF_
#define _H_ASDF_

struct asdfmulti {
    enum {
        T_TBD,
        T_INT,
        T_STR,
        T_ARR
    } type;
    void* value;
    struct asdfmulti* next;
};


/* ASDF Linked List Utilities */
int asdffreearr(struct asdfmulti* array); /* Frees a given ASDF Linked List - use where *.strval is statically allocated results in undefined behaviour */
struct asdfmulti* asdfgenllist(char* str, int* val); /* Pass NULL to unused argument - Returns new linked list head */
void asdfappendllist(struct asdfmulti* head, char* str, int* val); /* Pass NULL to unused argument */
struct asdfmulti* asdfgetindex(struct asdfmulti* array, unsigned int index); /* Get given index of ASDF Linked List - returns NULL if index is out of range */
unsigned int asdfarrlen(struct asdfmulti* array);


/* Important Read Function */
int asdfgetval(char* filename, char* valname, struct asdfmulti* retval); /* Main parser on which all reading functions rely - outputs to retval */


/* Read Wrappers*/
int* asdfgetint(char* filename, char* valname); /* Returns NULL pointer if not found, else returns pointer to result and must be freed */
char* asdfgetstr(char* filename, char* valname); /* Returns NULL pointer if not found, else returns pointer to result and must be freed */
struct asdfmulti* asdfgetarr(char* filename, char* valname); /* Needs to free returned value if value is found - Returns NULL if value isn't found */

/* Formatted printing of ASDF Linked Lists */
int asdfprintarr_ol(struct asdfmulti* array); /* Print ASDF Linked List in ordered (numbered) list */
int asdfprintarr_ul(struct asdfmulti* array, char* delim); /* Print ASDF Linked List in unordered list */


/* Important Write Functions */
int asdfassignval(char* filename, char* valname, struct asdfmulti* inval); /* Assigns a given value or array */
int asdfappendarr(char* filename, char* valname, struct asdfmulti* inval); /* Append a value to an ASDF array - creates array if it doesn't exist */

/* Write Wrappers */
int asdfassignstr(char* filename, char* valname, char* instr);
int asdfassignint(char* filename, char* valname, int inint);
int asdfappendstr(char* filename, char* valname, char* instr);
int asdfappendint(char* filename, char* valname, int inint);

#endif
