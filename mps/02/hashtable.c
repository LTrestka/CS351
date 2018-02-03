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
  bucket_t *last = NULL;
  bucket_t *Next = ht->buckets[idx];
  bucket_t *current = NULL;
  printf("%u\n", idx);
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
  bucket_t *b[ht->size];
  bucket_t *p;
  unsigned long i, k=0;
  int bucks = 0;

  // makes an array of bucket heads
  for (i = 0; i < ht->size; i++){
    p = ht->buckets[i];
    if(p){
      b[k] = p;
      k++;
    }
  }
  
  // makes an array of total buckets, starting from tail to the head
  // followed by the next tail.
  bucket_t *buck_array[1000];
  for (i = 0; i < k; i++){
    while(b[i]){
      buck_array[bucks] = b[i];
      bucks++;
      b[i]= b[i]->next;
    }
  }
  
  // free's all the bucket keys, then vals, then the bucket itself
  for(i = bucks; i > 0; i--){
    free(buck_array[i-1]->key);
    free(buck_array[i-1]->val);
    free(buck_array[i-1]);
  }
  // free's what remains of the hashtable
  
  free(p);
  free(ht);
}

/* TODO */
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
  unsigned long i;
  hashtable_t *new;
  new = make_hashtable(newsize);
  //for(i = 0; i < ht->size; i++){
  bucket_t before_head = {.next = ht->buckets[i]};
  bucket_t *previous = &before_head;
  bucket_t *head = previous->next;
  // while(previous->next){
  //  unsigned long j = hash(previous->next->key)%newsize;
  //  bucket_t bef_new_head = {.next = new->buckets[j]};
  //  bucket_t *new_prev = &bef_new_head;
  //  bucket_t head = {new->buckets[j]};
  //  if(!new_prev->next){
  //	new_prev->next = previous->next;
  //  }
  //  printf("%s\n", new_prev->next->key);
  //  new->buckets[j] = previous->next;
  //  ht->buckets[i] = previous->next->next;
      
      //char *key;
      //char *val;
      //char k;
      //key = previous->next->key;
      //val = previous->next->val;
      //k = *key;
  bucket_t *b;
  for (b = ht->buckets[i]; b !=NULL; b=b->next){
    ht_put(new,b->key, b->val);
    i++;
    if (i>ht->size){
      break;
    }
  }
  if(i<100){
  free_hashtable(ht);
  ht = new;
  }
//ht_put(new, &k, val);
      //printf(" %s\n ", ht_get(new, &k));
      //previous->next = previous->next->next;
  }


