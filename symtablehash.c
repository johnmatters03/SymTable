#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

enum BUCKETS {A = 509, B = 1021, C = 2039, D = 4093, 
E = 8191, F = 16381, G = 32749,  H = 65521};

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
   struct STBinding **buckets;
};

SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable) + 509 * sizeof(char*));
   if (oSymTable == NULL) return NULL;

   oSymTable->iBuckets = 509;
   oSymTable->size = 0;
   for (int i = 0; i < oSymTable->iBuckets; i++) {
      oSymTable->buckets[i] = NULL;
   }

   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
   struct STBinding *psCurrentNode;
   struct STBinding *psNextNode;

   for (int i = 0; i < oSymTable->iBuckets; i++) {
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

