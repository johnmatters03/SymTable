#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

struct STBinding 
{
    char *pcKey;
    void *pvValue;
    struct STBinding *psNextNode;
};

struct SymTable
{
    size_t size;
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
    struct STBinding *psCurrentNode;
    struct STBinding *psNextNode;

    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->first; 
    psCurrentNode != NULL; 
    psCurrentNode = psNextNode)
    {
      psNextNode = psCurrentNode->psNextNode;
      free(psCurrentNode);
    }

   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->size;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct STBinding *psNewNode;
    struct STBinding *psCurrentNode;
    char* keyCopy;

    assert(oSymTable != NULL);

    keyCopy = malloc(sizeof(char) * strlen(pcKey));
    keyCopy = strcpy(keyCopy, pcKey);

    for (psCurrentNode = oSymTable->first; 
    psCurrentNode != NULL; 
    psCurrentNode = psCurrentNode->psNextNode) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            return 0;
        }
    }

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
    struct STBinding *psCurrentNode;
    struct STBinding *psPrevious;
    void *pvValue;

    assert(oSymTable != NULL);

    psCurrentNode = oSymTable->first;
    if (psCurrentNode == NULL) return NULL;

    if (!strcmp(psCurrentNode->pcKey, pcKey)) {
        pvValue = psCurrentNode->pvValue;
        oSymTable->first = oSymTable->first->psNextNode;
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

        for (psCurrentNode = oSymTable->first; 
        psCurrentNode != NULL; 
        psCurrentNode = psCurrentNode->psNextNode) {
            pfApply(psCurrentNode->pcKey, 
            (void*)psCurrentNode->pvValue,
            (void*)pvExtra);
        }
    }