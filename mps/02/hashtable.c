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
  unsigned long int idx = hash(key) % ht->size;
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
    else if(!Next&&last){
      last->next = current;
    }
    else{
      current->next = Next;
      last->next = current;
    }
  }
}


void *ht_get(hashtable_t *ht, char *key) {
  unsigned long int idx = hash(key) % ht->size;
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
  bucket_t *current;
  unsigned long i;
  for (i = 0; i < ht->size; i++){
    current = ht->buckets[i];
    while(current){
      ht_del(ht, current->key);
      current = current->next;
    }
  }
  free(ht->buckets);
  free(ht);
}

void  ht_del(hashtable_t *ht, char *key) {
  unsigned long i;
  i = hash(key)%ht->size; 
  bucket_t before_head = { .next = ht->buckets[i]};
  bucket_t *previous = &before_head;
  while(previous->next && strcmp(previous->next->key,key)!=0) {
    previous = previous->next;
  }
  if(previous->next ) {
    bucket_t *next = previous->next->next;
    bucket_t *b = previous->next;
    free(previous->next->key);
    free(previous->next->val);
    previous->next = next;
    free(b);
    if(previous->next){
    ht->buckets[i] = previous->next;
    }
    else{
      ht->buckets[i]=NULL;;
    }
  }
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  unsigned long i=0;
  hashtable_t *new = make_hashtable(newsize);
  bucket_t *b;
  hashtable_t oldT, newT;
  while(i<ht->size){
    for (b = ht->buckets[i]; b !=NULL; b=b->next){
      ht_put(new,b->key , b->val);
    }
    free(ht->buckets[i]);
    i++;
  }
  free(ht->buckets);
  oldT = *ht;
  newT = *new;
  *new = oldT;
  *ht = newT;
}

      


