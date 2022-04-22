//
// Created by anna_seli on 26/05/2021.
//

#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"
#include "vector.h"
#include "pair.h"

void *vec_copy_func(const void *elem);
int vec_cmp_func(const void *elem1, const void *elem2);
void vec_free_func(void **elem);
int get_all_pairs (hashmap *hash_map, pair **pair_arr);
int check_key_appearance (hashmap *hash_map, pair **pair_arr, const_keyT key, int *flag);
int update_elem_in_buckets (hashmap *hash_map, pair **pair_arr, int size);
int add_elem (hashmap *hash_map, pair *p);
int delete_old_vectors (hashmap *hash_map);
int create_new_vectors (hashmap *hash_map);
/**
 * Allocates dynamically new hash map element.
 * @param func a function which "hashes" keys.
 * @return pointer to dynamically allocated hashmap.
 * @if_fail return NULL.
 */
void *vec_copy_func(const void *elem)
{
    return pair_copy (elem);
}

/**
 * Allocates dynamically new hash map element.
 * @param func a function which "hashes" keys.
 * @return pointer to dynamically allocated hashmap.
 * @if_fail return NULL.
 */
int vec_cmp_func(const void *elem1, const void *elem2)
{
//    if ((elem1 == NULL) || (elem2 == NULL)) {return 0;}
//    if (pair_copy (elem1) == NULL) {return NULL;}
//    pair *p1 = pair_copy (elem1);
//    if (pair_key_cpy (elem2) == NULL) {return NULL;}
//    keyT key = pair_key_cpy (elem2);
//    return (pair_key_cmp ((const) (p1->key), (const) (key)))
    return pair_cmp (elem1, elem2);
}

/**
 * Allocates dynamically new hash map element.
 * @param func a function which "hashes" keys.
 * @return pointer to dynamically allocated hashmap.
 * @if_fail return NULL.
 */
void vec_free_func(void **elem)
{
//    if (elem == NULL) {return NULL;}
//    pair *p = pair_copy (elem);
//    if (p == NULL) {return NULL;}
//    pair_free (&p);
    pair_free(elem);
}

/**
 * Allocates dynamically new hash map element.
 * @param func a function which "hashes" keys.
 * @return pointer to dynamically allocated hashmap.
 * @if_fail return NULL.
 */
hashmap *hashmap_alloc (hash_func func)
{
    hashmap *h = (hashmap *) malloc (sizeof(hashmap));
    if (h == NULL) {return NULL;}
    if (func == NULL) {return NULL;}
    h->capacity = HASH_MAP_INITIAL_CAP;
    h->size = 0;
    h->buckets = (vector **) malloc (sizeof(vector *) * h->capacity);
    if (h->buckets == NULL)
    {
        free(h);
        h = NULL;
        return NULL;
    }
    for (size_t i = 0; i < h->capacity; ++i)
    {
        (h->buckets)[i] = vector_alloc(vec_copy_func, vec_cmp_func, vec_free_func);
        if (h->buckets[i] == NULL) {return NULL;}
    }
    h->hash_func = func;
    return h;
}

/**
 * Frees a hash map and the elements the hash map itself allocated.
 * @param p_hash_map pointer to dynamically allocated pointer to hash_map.
 */
void hashmap_free (hashmap **p_hash_map)
{
    if ((p_hash_map != NULL) && (*p_hash_map != NULL))
    {
        for (size_t i = 0; i < (*p_hash_map)->capacity; ++i)
        {
            vector_free(&((*p_hash_map)->buckets[i]));
        }
        free((*p_hash_map)->buckets);
        (*p_hash_map)->buckets = NULL;
        free(*p_hash_map);
        *p_hash_map = NULL;
    }
}

/**
 * Inserts a new in_pair to the hash map.
 * The function inserts *new*, *copied*, *dynamically allocated* in_pair,
 * NOT the in_pair it receives as a parameter.
 * @param hash_map the hash map to be inserted with new element.
 * @param in_pair a in_pair the hash map would contain.
 * @return returns 1 for successful insertion, 0 otherwise.
 */
int hashmap_insert (hashmap *hash_map, const pair *in_pair)
{
    if ((hash_map == NULL) || (in_pair == NULL)) {return 0;}
    pair **pair_arr = malloc(sizeof(pair *) * hash_map->size);
    if (pair_arr == NULL) {return 0;}
    int flag = 0;
    int result = check_key_appearance (hash_map, pair_arr, (in_pair->key), &flag);
    if (result == -1)
    {
        free(pair_arr);
        return 0;
    }
    else if (flag == 1)
    {
        // free allocated pairs in array
        for (size_t i = 0; i <= ((size_t) result); ++i)
        {
            pair_free((void **) &(pair_arr[i]));
        }
        free(pair_arr);
        return 0;
    }

    if (hashmap_get_load_factor(hash_map) >= HASH_MAP_MAX_LOAD_FACTOR)
    {
        size_t resize = hash_map->capacity * HASH_MAP_GROWTH_FACTOR * sizeof(vector *);
        vector **v_temp = malloc (resize);
        if (v_temp == NULL)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }

        // free old buckets
        if (delete_old_vectors(hash_map) == 0)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
        hash_map->buckets = v_temp;
        hash_map->capacity *= HASH_MAP_GROWTH_FACTOR;
        if (create_new_vectors(hash_map) == 0)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
        if (update_elem_in_buckets(hash_map, pair_arr, hash_map->size) == 0)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
    }
    pair *new_pair = pair_copy (in_pair);
    if (new_pair == NULL) {return 0;}
    add_elem (hash_map, new_pair);
    free (new_pair->key);
    free (new_pair->value);
    free (new_pair);
    // free allocated pairs in array
    for (size_t i = 0; i < hash_map->size; ++i)
    {
        //pair *p = pair_arr[i];
        pair_free((void **) &(pair_arr[i]));
    }
    free(pair_arr);
    ++hash_map->size;
    return 1;
}

/**
 * The function returns the value associated with the given key.
 * @param hash_map a hash map.
 * @param key the key to be checked.
 * @return the value associated with key if exists, NULL otherwise (the value itself,
 * not a copy of it).
 */
valueT hashmap_at (const hashmap *hash_map, const_keyT key)
{
    if ((hash_map == NULL) || (key == NULL)) {return NULL;}
    size_t hashed_key = (hash_map->hash_func (key));
    size_t hash_value = hashed_key & (hash_map->capacity - 1);
    //if ((hash_value < 0) || (hash_value >= (hash_map->capacity))) {return NULL;}
    vector *temp_v = (hash_map->buckets)[hash_value];
//    int idx = vector_find(temp_v, key);
//    if (idx == -1) {return NULL;}
//    pair *p = vector_at(temp_v, (size_t) idx);
    for (size_t i = 0; i < temp_v->size; ++i)
    {
        pair *p = temp_v->data[i];
        //printf("%p", p->key);
        //printf("%p", key);
        if (p->key_cmp(p->key, key) == 1)
        {
            return p->value;
        }
    }
    return NULL;
}

/**
 * The function erases the pair associated with key.
 * @param hash_map a hash map.
 * @param key a key of the pair to be erased.
 * @return 1 if the erasing was done successfully, 0 otherwise. (if key not in map,
 * considered fail).
 */
int hashmap_erase (hashmap *hash_map, const_keyT key)
{
    if ((hash_map == NULL) || (key == NULL)) {return 0;}
    if (hashmap_get_load_factor (hash_map) <= HASH_MAP_MIN_LOAD_FACTOR)
    {
        pair **pair_arr = malloc(sizeof(pair *) * hash_map->size);
        if (pair_arr == NULL) {return 0;}
        int result = get_all_pairs(hash_map, pair_arr);
        if (result == -1)
        {
            free(pair_arr);
            return 0;
        }
        else if (result != -2)
        {
            // free allocated pairs in array
            for (size_t i = 0; i <= ((size_t) result); ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
        size_t resize = hash_map->capacity / HASH_MAP_GROWTH_FACTOR * sizeof(vector *);
        vector **v_temp = malloc(resize);
        if (v_temp == NULL)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
        // free old buckets
        if (delete_old_vectors(hash_map) == 0)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
        hash_map->buckets = v_temp;
        hash_map->capacity /= HASH_MAP_GROWTH_FACTOR;
        if (create_new_vectors(hash_map) == 0)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
        if (update_elem_in_buckets(hash_map, pair_arr, hash_map->size) == 0)
        {
            // free allocated pairs in array
            for (size_t i = 0; i < hash_map->size; ++i)
            {
                pair_free((void **) &(pair_arr[i]));
            }
            free(pair_arr);
            return 0;
        }
        // free allocated pairs in array
        for (size_t i = 0; i < hash_map->size; ++i)
        {
            pair_free((void **) &(pair_arr[i]));
        }
        free(pair_arr);
    }
    size_t hash_value = (hash_map->hash_func (key)) & (hash_map->capacity - 1);
    //if ((hash_value < 0) || (hash_value >= (hash_map->capacity))) {return 0;}
    vector *temp_v = (hash_map->buckets)[hash_value];
    int idx = -1;
    for (size_t i = 0; i < temp_v->size; ++i)
    {
        pair *p = temp_v->data[i];
        //printf("%p", p->key);
        //printf("%p", key);
        if (p->key_cmp(p->key, key) == 1)
        {
            idx = i;
            break;
        }
    }

    //int idx = vector_find(temp_v, key);
    if (idx == -1) {return 0;}
    if (vector_erase(temp_v, (size_t) idx) == 0) {return 0;}
    --hash_map->size;
    return 1;
}

/**
 * The function erases the pair associated with key.
 * @param hash_map a hash map.
 * @param key a key of the pair to be erased.
 * @return 1 if the erasing was done successfully, 0 otherwise. (if key not in map,
 * considered fail).
 */
int get_all_pairs (hashmap *hash_map, pair **pair_arr)
{
    if ((pair_arr == NULL) || (hash_map == NULL)) {return -1;}
    int size = 0;
    for (size_t i = 0; i < hash_map->capacity; ++i)
    {
        vector *v = (hash_map->buckets)[i];
        for (size_t j = 0; j < v->size; ++j)
        {
            pair *temp_p = pair_copy(v->data[j]);
            if (temp_p == NULL)
            {
                return size - 1;
            }
            pair_arr[size] = temp_p;
            ++size;
        }
    }
    return -2;
}

int check_key_appearance (hashmap *hash_map, pair **pair_arr, const_keyT key, int *flag)
{
    if ((pair_arr == NULL) || (hash_map == NULL) || (key == NULL)) {return -1;}
    int size = 0;
    for (size_t i = 0; i < hash_map->capacity; ++i)
    {
        vector *v = (hash_map->buckets)[i];
        for (size_t j = 0; j < v->size; ++j)
        {
            pair *temp_p = pair_copy(v->data[j]);
            if (temp_p == NULL)
            {
                *flag = 1;
                return size - 1;
            }
            pair_arr[size] = temp_p;
            if ((temp_p->key_cmp) (key, (temp_p->key)) == 1)
            {
                *flag = 1;
                return size;
            }
            ++size;
        }
    }
    return 1;
}

/**
 * The function erases the pair associated with key.
 * @param hash_map a hash map.
 * @param key a key of the pair to be erased.
 * @return 1 if the erasing was done successfully, 0 otherwise. (if key not in map,
 * considered fail).
 */
int update_elem_in_buckets (hashmap *hash_map, pair **pair_arr, int size)
{
    if ((hash_map == NULL) || (pair_arr == NULL) || (size < 0)) {return 0;}
    for (int i = 0; i < size; ++i)
    {
        if (add_elem (hash_map, pair_arr[i]) == 0) {return 0;}
    }
    return 1;
}

/**
 * The function erases the pair associated with key.
 * @param hash_map a hash map.
 * @param key a key of the pair to be erased.
 * @return 1 if the erasing was done successfully, 0 otherwise. (if key not in map,
 * considered fail).
 */
int add_elem (hashmap *hash_map, pair *p)
{
    //char c = *(char *) (p->key);
    size_t hash_value = (hash_map->hash_func (p->key)) & (hash_map->capacity - 1);
    //if ((hash_value < 0) || (hash_value >= (hash_map->capacity))) {return 0;}
    vector *vector_in_bucket = (hash_map->buckets)[hash_value];
    if ((vector_push_back(vector_in_bucket, p)) == 0) {return 0;}
    return 1;
}

/**
 * The function erases the pair associated with key.
 * @param hash_map a hash map.
 * @param key a key of the pair to be erased.
 * @return 1 if the erasing was done successfully, 0 otherwise. (if key not in map,
 * considered fail).
 */
int delete_old_vectors (hashmap *hash_map)
{
    if (hash_map == NULL) {return 0;}
    for (size_t i = 0; i < hash_map->capacity; ++i)
    {
        vector_free(&((hash_map->buckets)[i]));
    }
    free(hash_map->buckets);
    hash_map->buckets = NULL;
    return 1;
}

/**
 * The function erases the pair associated with key.
 * @param hash_map a hash map.
 * @param key a key of the pair to be erased.
 * @return 1 if the erasing was done successfully, 0 otherwise. (if key not in map,
 * considered fail).
 */
int create_new_vectors (hashmap *hash_map)
{
    if (hash_map == NULL) {return 0;}
    for (size_t i = 0; i < hash_map->capacity; ++i)
    {
        hash_map->buckets[i] = vector_alloc
                (vec_copy_func, vec_cmp_func, vec_free_func);
        if (hash_map->buckets[i] == NULL) {return 0;}
    }
    return 1;
}

/**
 * This function returns the load factor of the hash map.
 * @param hash_map a hash map.
 * @return the hash map's load factor, -1 if the function failed.
 */
double hashmap_get_load_factor (const hashmap *hash_map)
{
    if ((hash_map == NULL) || (hash_map->capacity == 0)) {return -1;}
    return ((double) hash_map->size / (double) hash_map->capacity);
}

/**
 * This function receives a hashmap and 2 functions, the first checks a condition on the keys,
 * and the seconds apply some modification on the values. The function should apply the modification
 * only on the values that are associated with keys that meet the condition.
 *
 * Example: if the hashmap maps char->int, keyT_func checks if the char is a capital letter (A-Z),
 * and val_t_func multiples the number by 2, hashmap_apply_if will change the map:
 * {('C',2),('#',3),('X',5)}, to: {('C',4),('#',3),('X',10)}, and the return value will be 2.
 * @param hash_map a hashmap
 * @param keyT_func a function that checks a condition on keyT and return 1 if true, 0 else
 * @param valT_func a function that modifies valueT, in-place
 * @return number of changed values
 */
int hashmap_apply_if (const hashmap *hash_map, keyT_func keyT_func, valueT_func valT_func)
{
    if ((hash_map == NULL) || (keyT_func == NULL) || (valT_func == NULL)) {return -1;}
    int changes_counter = 0;
    for (size_t i = 0; i < hash_map->capacity; ++i)
    {
        vector *v = (hash_map->buckets)[i];
        for (size_t j = 0; j < v->size; ++j)
        {
            pair *p = v->data[j];
            if (keyT_func(p->key) == 1)
            {
                valT_func(p->value);
                ++changes_counter;
            }
        }
    }
    return changes_counter;
}
