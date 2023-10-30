#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

static const int BUCKETSIZE[8] = {509, 1021, 2039, 4093, 8191,
16381, 32749, 65521};

struct STBinding 
{
   char *pcKey;
   void *pvValue;
   struct STBinding *psNextNode;
};

struct SymTable
{
   size_t size;
   int iBuckets;
   struct STBinding *buckets[1];
};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

static SymTable_T SymTable_resize(SymTable_T oSymTable)
{
   struct STBinding *psCurrentNode;
   struct STBinding *psTempNode;
   struct STBinding **buckets;
   void *pvValue;
   int i;
   int out;
   int newSize;

   for (i = 1; i < 8; i++) {
      if (oSymTable->iBuckets < BUCKETSIZE[i]) {
         newSize = BUCKETSIZE[i];
         break;
      }
   }

   assert(oSymTable->iBuckets != BUCKETSIZE[7]);

   buckets = malloc(sizeof(char*) * oSymTable->iBuckets);

   for (i = 0; i < oSymTable->iBuckets; i++) {
      buckets[i] = oSymTable->buckets[i];
   }

   oSymTable = realloc(oSymTable, sizeof(struct SymTable) + 
   newSize * sizeof(char*));

   for (i = 0; i < oSymTable->iBuckets; i++) {
      psCurrentNode = buckets[i];
      while (psCurrentNode != NULL) {
         psTempNode = psCurrentNode->psNextNode;
         pvValue = SymTable_remove(oSymTable, psCurrentNode->pcKey);
         out = SymTable_put(oSymTable, psTempNode->pcKey, pvValue);
         assert(out == 1);

         psCurrentNode = psTempNode;
      }
   }

   oSymTable->iBuckets = newSize;

   free(buckets);

   return oSymTable;
}

SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;
   int i;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable) + 
   BUCKETSIZE[0] * sizeof(char*));
   if (oSymTable == NULL) return NULL;

   oSymTable->iBuckets = BUCKETSIZE[0];
   oSymTable->size = 0;
   for (i = 0; i < oSymTable->iBuckets; i++) {
      oSymTable->buckets[i] = NULL;
   }

   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
   struct STBinding *psCurrentNode;
   struct STBinding *psNextNode;
   int i;

   for (i = 0; i < oSymTable->iBuckets; i++) {
      psCurrentNode = oSymTable->buckets[i];
      for (; psCurrentNode != NULL; 
      psCurrentNode = psNextNode) {
         psNextNode = psCurrentNode->psNextNode;
         free(psCurrentNode);
      }
   }

   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
   return oSymTable->size;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
   int index;
   struct STBinding *psNewNode;
   struct STBinding *psCurrentNode;
   char* keyCopy;
   
   if (oSymTable->size == oSymTable->iBuckets) 
   oSymTable = SymTable_resize(oSymTable);

   index = SymTable_hash(pcKey, oSymTable->iBuckets);

   for (psCurrentNode = oSymTable->buckets[index];
   psCurrentNode != NULL;
   psCurrentNode = psCurrentNode->psNextNode) {
      if (!strcmp(pcKey, psCurrentNode->pcKey)) {
         return 0;
      }
   }

   keyCopy = malloc(sizeof(char) * strlen(pcKey));
   keyCopy = strcpy(keyCopy, pcKey);

   psNewNode = (struct STBinding*)malloc(sizeof(struct STBinding));
   if (psNewNode == NULL) return 0;

   psNewNode->pcKey = keyCopy;
   psNewNode->pvValue = (void*)pvValue;
   psNewNode->psNextNode = oSymTable->buckets[index];

   oSymTable->buckets[index] = psNewNode;
   oSymTable->size++;

   return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
const void *pvValue) {
   struct STBinding *psCurrentNode;
   void *tempValue;
   int index;

   assert(oSymTable != NULL);

   index = SymTable_hash(pcKey, oSymTable->iBuckets);

   for (psCurrentNode = oSymTable->buckets[index]; 
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
   int index;

   assert(oSymTable != NULL);

   index = SymTable_hash(pcKey, oSymTable->iBuckets);

   for (psCurrentNode = oSymTable->buckets[index]; 
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
   int index;

   assert(oSymTable != NULL);

   index = SymTable_hash(pcKey, oSymTable->iBuckets);

   for (psCurrentNode = oSymTable->buckets[index]; 
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
    int index;

    assert(oSymTable != NULL);

    index = SymTable_hash(pcKey, oSymTable->iBuckets);

    psCurrentNode = oSymTable->buckets[index];
    if (psCurrentNode == NULL) return NULL;

    if (!strcmp(psCurrentNode->pcKey, pcKey)) {
        pvValue = psCurrentNode->pvValue;
        oSymTable->buckets[index] = psCurrentNode->psNextNode;
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
      int i;
      struct STBinding *psCurrentNode;

      for (i = 0; i < oSymTable->iBuckets; i++) {
         for (psCurrentNode = oSymTable->buckets[i]; 
         psCurrentNode != NULL; 
         psCurrentNode = psCurrentNode->psNextNode) {
            pfApply(psCurrentNode->pcKey, 
            (void*)psCurrentNode->pvValue,
            (void*)pvExtra);
        }
      }
    }