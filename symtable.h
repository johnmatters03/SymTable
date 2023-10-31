/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: John Matters                                               */
/*--------------------------------------------------------------------*/

#ifndef STACK_INCLUDED
#define STACK_INCLUDED

#include <stddef.h>

/* Define type SymTable_T to be a pointer towards a SymTable struct */
typedef struct SymTable *SymTable_T;

/* Creates an empty SymTable and returns the pointer to it */
SymTable_T SymTable_new(void);

/* Free the SymTable associated with pointer oSymTable and all memory
that it uses for its bindings (does not free memory allocated for 
values) */
void SymTable_free(SymTable_T oSymTable);

/* Returns the number of bindings in oSymTable */
size_t SymTable_getLength(SymTable_T oSymTable);

/* If oSymTable does not already have a binding with key pcKey, creates
a new binding with key pcKey and value pvValue and inserts it into 
oSymtable and returns 1, otherwise does nothing and return 0 */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue);

/* If oSymTable has binding with key pcKey, replace the value of the
binding with pvValue and return the pointer to the old value, otherwise
does nothing and returns NULL */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue);

/* Returns 1 if oSymTable contains a binding with key pcKey,
returns 0 otherwise */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Returns the pointer to the value of the binding with key pcKey
returns NULL if there is no such binding in oSymTable */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Removes binding with key pcKey and returns the pointer to the 
value of the removed binding, returns NULL if there is no such
binding in oSymTable */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Apply function pfApply to every binding in oSymTable that applies
some constant pvExtra to each key-value pair */
void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);

#endif