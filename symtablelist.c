/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: John Matters                                               */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/* STBinding is the structure for a node in SymTable that contains a
key-value pair and the next binding that follows it to form a linked
list */
struct STBinding 
{
    /* pointer towards the key */
    char *pcKey;
    /* pointer towards the value */
    void *pvValue;
    /* next node in the linked list */
    struct STBinding *psNextNode;
};

/* SymTable is the structure for a SymTable that contains its size and
a pointer to the first binding in the linked list */
struct SymTable
{
    /* number of bindings in the SymTable */
    size_t size;
    /* first node in the linked list */
    struct STBinding *first;
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    }
    oSymTable->first = NULL;
    oSymTable->size = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    struct STBinding *psCurrentNode, *psNextNode;

    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->first; 
    psCurrentNode != NULL; 
    psCurrentNode = psNextNode)
    {
      psNextNode = psCurrentNode->psNextNode;
      free(psCurrentNode->pcKey);
      free(psCurrentNode);
    }

   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->size;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct STBinding *psNewNode, *psCurrentNode;
    char* keyCopy;

    assert(pcKey != NULL);
    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->first; 
    psCurrentNode != NULL; 
    psCurrentNode = psCurrentNode->psNextNode) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            return 0;
        }
    }


    keyCopy = malloc(sizeof(char) * (strlen(pcKey) + 1));
    if (keyCopy == NULL) return 0;

    keyCopy = strcpy(keyCopy, pcKey);

    psNewNode = (struct STBinding*)malloc(sizeof(struct STBinding));
    if (psNewNode == NULL) return 0;

    psNewNode->pcKey = keyCopy;
    psNewNode->pvValue = (void*)pvValue;
    psNewNode->psNextNode = oSymTable->first;

    oSymTable->first = psNewNode;
    oSymTable->size++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct STBinding *psCurrentNode;
    void *tempValue;

    assert(pcKey != NULL);
    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->first; 
    psCurrentNode != NULL; 
    psCurrentNode = psCurrentNode->psNextNode) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            tempValue = psCurrentNode->pvValue;
            psCurrentNode->pvValue = (void*)pvValue;
            return tempValue;
        }
    }

    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct STBinding *psCurrentNode;

    assert(pcKey != NULL);
    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->first; 
    psCurrentNode != NULL; 
    psCurrentNode = psCurrentNode->psNextNode) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            return 1;
        }
    }

    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct STBinding *psCurrentNode;

    assert(pcKey != NULL);
    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->first; 
    psCurrentNode != NULL; 
    psCurrentNode = psCurrentNode->psNextNode) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            return psCurrentNode->pvValue;
        }
    }

    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct STBinding *psCurrentNode, *psPrevious;
    void *pvValue;

    assert(pcKey != NULL);
    assert(oSymTable != NULL);

    psCurrentNode = oSymTable->first;
    if (psCurrentNode == NULL) return NULL;

    if (!strcmp(psCurrentNode->pcKey, pcKey)) {
        pvValue = psCurrentNode->pvValue;
        oSymTable->first = oSymTable->first->psNextNode;
        free(psCurrentNode->pcKey);
        free(psCurrentNode);
        oSymTable->size--;
        return pvValue;
    }

    psPrevious = psCurrentNode;

    for (psCurrentNode = psCurrentNode->psNextNode; 
    psCurrentNode != NULL; 
    psCurrentNode = psCurrentNode->psNextNode) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            pvValue = psCurrentNode->pvValue;
            psPrevious->psNextNode = psCurrentNode->psNextNode;
            free(psCurrentNode->pcKey);
            free(psCurrentNode);
            oSymTable->size--;
            return pvValue;
        }
        
        psPrevious = psCurrentNode;
    }

    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
        struct STBinding *psCurrentNode;

        assert(pfApply != NULL);

        for (psCurrentNode = oSymTable->first; 
        psCurrentNode != NULL; 
        psCurrentNode = psCurrentNode->psNextNode) {
            (*pfApply)(psCurrentNode->pcKey, 
            (void*)psCurrentNode->pvValue,
            (void*)pvExtra);
        }
    }