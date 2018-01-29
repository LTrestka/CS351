#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"

/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = malloc(sizeof(hashtable_t));
  ht->size = size;
  ht->buckets = calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  /* FIXME: the current implementation doesn't update existing entries */
  unsigned int idx = hash(key) % ht->size;
  int x = 0;
  bucket_t *last = NULL;
  bucket_t *Next = ht->buckets[idx];
  bucket_t *current = NULL;
  while(Next && Next->key &&(strcmp(key, Next->key)!= 0)){
      last = Next;
      Next = Next->next;
    }
  if(Next && Next->key && strcmp(key, Next->key)==0){
    free(Next->key);
    free(Next->val);
    Next->key = key;
    Next->val = val;
  }
  else{
    current = malloc(sizeof(bucket_t));
    current->key = key;
    current->val = val;
    if(Next == ht->buckets[idx]){
      current->next = Next;
      ht->buckets[idx]=current;
    }
    else if(!Next){
      last->next = current;
    }
    else{
      current->next = Next;
      last->next = current;
    }
  }
}


void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return ; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  free(ht); // FIXME: must free all substructures!
}

/* TODO */
void  ht_del(hashtable_t *ht, char *key) {
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
}
