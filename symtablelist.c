#include <assert.h>
#include <stdlib.h>
#include "symtable.h"

struct STBinding 
{
    char *pcKey;
    void *pvValue;
    struct STBinding *psNextNode;
};

struct SymTable
{
    struct STBinding *first;
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    }
    oSymTable->first = NULL;
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
    size_t i;
    struct STBinding *psCurrentNode;

    assert(oSymTable != NULL);

    i = 0;
    psCurrentNode = oSymTable->first;

    while (psCurrentNode != NULL) {
        i++;
        psCurrentNode = psCurrentNode->psNextNode;
    }

    return i;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct STBinding *psNewNode;
    struct STBinding *psCurrentNode;

    assert(oSymTable != NULL);

    psCurrentNode = oSymTable->first;
    while(psCurrentNode != NULL) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            return 0;
        }
    }

    psNewNode = (struct STBinding*)malloc(sizeof(struct STBinding));
    if (psNewNode == NULL) return 0;

    psNewNode->pcKey = pcKey;
    psNewNode->pvValue = pvValue;
    psNewNode->psNextNode = oSymTable->first;

    oSymTable->first = psNewNode;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
    struct STBinding *psCurrentNode;
    void *tempValue;

    assert(oSymTable != NULL);

    psCurrentNode = oSymTable->first;

    while (psCurrentNode != NULL) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            tempValue = psCurrentNode->pvValue;
            psCurrentNode->pvValue = pvValue;
            return tempValue;
        }
    }

    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct STBinding *psCurrentNode;

    assert(oSymTable != NULL);

    psCurrentNode = oSymTable->first;

    while (psCurrentNode != NULL) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            return 1;
        }
    }

    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct STBinding *psCurrentNode;

    assert(oSymTable != NULL);

    psCurrentNode = oSymTable->first;

    while (psCurrentNode != NULL) {
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

    if (!strcmp(psCurrentNode->pcKey, pcKey)) {
        pvValue = psCurrentNode->pvValue;
        oSymTable->first = psCurrentNode->psNextNode;
        free(psCurrentNode);
        return pvValue;
    }

    psPrevious = psCurrentNode;
    psCurrentNode = psCurrentNode->psNextNode;

    while (psCurrentNode != NULL) {
        if (!strcmp(psCurrentNode->pcKey, pcKey)) {
            pvValue = psCurrentNode->pvValue;
            psPrevious->psNextNode = psCurrentNode->psNextNode;
            free(psCurrentNode);
            return pvValue;
        }
    }

    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
        struct STBinding psCurrentNode;
    }