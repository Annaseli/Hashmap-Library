//
// Created by anna_seli on 26/05/2021.
//
#include "test_suite.h"
#include "test_pairs.h"
#include "hash_funcs.h"
#include "hashmap.h"
#include <stdio.h>
#include <assert.h>

/**
 * This function checks the hashmap_insert function of the hashmap library.
 * If hashmap_insert fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_insert(void)
{
    hashmap *map = hashmap_alloc (hash_char);

    pair *p0 = NULL;
    assert (hashmap_insert(map, NULL) == 0);
    assert (hashmap_insert(map, p0) == 0);
    assert (hashmap_insert(NULL , NULL) == 0);

    for (size_t i = 0; i < 25; ++i)
    {
        char key = (char) ('A' + i);
        pair *p1 = pair_alloc(&key, &i, char_key_cpy, int_value_cpy,
                             char_key_cmp, int_value_cmp,
                             char_key_free, int_value_free);
        assert (hashmap_insert(map, p1) == 1);
        // check that same pair cant be inserted twice.
        assert (hashmap_insert(map, p1) == 0);

        //check that the pair us inside the map.
        assert (hashmap_at(map, &key) != NULL);

        //check that a copy of the element was inserted.
        assert (hashmap_at(map, &key) != p1->value);

        // check invalid map with valid pair.
        assert (hashmap_insert(NULL, p1) == 0);

        pair_free((void **) &p1);

    }
    size_t size = map->size;
    size_t capacity = map->capacity;
    assert (capacity == 64);
    assert (size == 25);

    for (size_t i = 0; i < 24; ++i)
    {
        char key = (char) ('A' + i);
        size_t value = i + 1;
        pair *p2 = pair_alloc(&key, &value, char_key_cpy, int_value_cpy,
                              char_key_cmp, int_value_cmp,
                              char_key_free, int_value_free);
        // check that same key cant be inserted twice.
        assert (hashmap_insert(map, p2) == 0);
        if (i == 2)
        {
            //check that the old pair ('C', 2) is inside the map.
            assert (* (int *) hashmap_at(map, &key) == 2);
        }
        //check that size and capacity didn't change.
        assert (map->size == size);
        assert (map->capacity == capacity);
        pair_free((void **) &p2);

    }
    hashmap_free(&map);
    assert (map == NULL);
}
/**
 * This function checks the hashmap_at function of the hashmap library.
 * If hashmap_at fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_at(void)
{
    hashmap *map = hashmap_alloc (hash_char);
    for (size_t i = 0; i < 26; ++i)
    {
        char key = (char) ('A' + i);
        size_t val = i + 1;
        pair *p = pair_alloc (&key, &val, char_key_cpy, int_value_cpy,
                              char_key_cmp, int_value_cmp,
                              char_key_free, int_value_free);
        hashmap_insert(map, p);

        if (i == 2)
        {
            // check invalid
            assert(hashmap_at(NULL, &key) == 0);
            assert(hashmap_at(NULL, p) == 0);
            // check that ('C', 3) us inside.
            assert(*(int *) hashmap_at(map, &key) == 3);
        }
        if (i == 25)
        {
            // check that ('Z', 26) us inside.
            assert(*(int *) hashmap_at(map, &key) == 26);
            //check key that doesn't exists
            char k1 = 'a';
            assert(hashmap_at(map, &k1) == 0);
        }
        pair_free((void **) &p);
    }
    assert(hashmap_at(map, NULL) == 0);
    assert(hashmap_at(NULL, NULL) == 0);

    hashmap_free(&map);
    assert (map == NULL);
}

/**
 * This function checks the hashmap_erase function of the hashmap library.
 * If hashmap_erase fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_erase(void)
{
    hashmap *map = hashmap_alloc (hash_char);

    char key0 = 'R';
    assert(hashmap_erase(map, NULL) == 0);
    assert(hashmap_erase(NULL, &key0) == 0);
    assert(hashmap_erase(NULL, NULL) == 0);

    for (size_t i = 0; i < 4; ++i)
    {
        char key1 = (char) ('A' + i);
        size_t val = i + 1;
        pair *p = pair_alloc (&key1, &val, char_key_cpy, int_value_cpy,
                              char_key_cmp, int_value_cmp,
                              char_key_free, int_value_free);
        hashmap_insert(map, p);

        if (i == 3)
        {
            // ('D', 4)
            assert(hashmap_erase(map, &key1) == 1);
            //check that 'D doesn't exists anymore.
            assert(hashmap_at(map, &key1) == 0);
            //check that can't erase key that doesn't exists.
            assert(hashmap_erase(map, &key1) == 0);

            // check sizes
            assert (map->capacity == 8);
            assert (map->size == 3);
        }

        pair_free((void **) &p);
    }
//
//    for (size_t i = 0; i < 9; ++i)
//    {
//        char key2 = (char) ('E' + i);
//        assert(hashmap_erase(map, &key2) == 1);
//    }
//    assert (map->capacity == 32);
//    assert (map->size == 15);
    hashmap_free(&map);
    assert (map == NULL);
}

/**
 * This function checks the hashmap_get_load_factor function of the hashmap library.
 * If hashmap_get_load_factor fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_get_load_factor(void)
{
    hashmap *map = hashmap_alloc (hash_char);

    for (size_t i = 0; i < 25; ++i)
    {
        char key = (char) ('A' + i);
        size_t val = i + 1;
        pair *p = pair_alloc (&key, &val, char_key_cpy, int_value_cpy,
                              char_key_cmp, int_value_cmp,
                              char_key_free, int_value_free);
        hashmap_insert(map, p);
        if (i == 3)
        {
            assert (hashmap_get_load_factor(map) == 0.25);
        }
        if (i == 23)
        {
            assert (hashmap_get_load_factor(map) == 0.75);
        }
        pair_free((void **) &p);
    }
    hashmap_free(&map);
    assert (map == NULL);

    hashmap *map2 = hashmap_alloc (hash_char);
    pair *pairs[4];

    for (size_t i = 0; i < 4; ++i)
    {
        char key = (char) ('A' + i);
        size_t val = i + 1;
        pair *p = pair_alloc (&key, &val, char_key_cpy, int_value_cpy,
                              char_key_cmp, int_value_cmp,
                              char_key_free, int_value_free);
        hashmap_insert(map2, p);
        pairs[i] = p;
    }
    //char c1 = *(char *) (pairs[3]->key);
    //char c2 = *(char *) (pairs[2]->key);
    hashmap_erase(map2, (pairs[3]->key));
    hashmap_erase(map2, (pairs[2]->key));
    assert (hashmap_get_load_factor(map2) == 0.25);

    for (size_t i = 0; i < 4; ++i)
    {
        pair_free((void **) &(pairs[i]));
    }

    hashmap_free(&map2);
    assert (map == NULL);
}

/**
 * This function checks the HashMapGetApplyIf function of the hashmap library.
 * If HashMapGetApplyIf fails at some points, the functions exits with exit code 1.
 */
void test_hash_map_apply_if()
{
    hashmap *map = hashmap_alloc (hash_char);

    assert(hashmap_apply_if (NULL, NULL, NULL) == -1);
    assert(hashmap_apply_if (NULL, is_digit, NULL) == -1);
    assert(hashmap_apply_if (NULL, NULL, double_value) == -1);
    assert(hashmap_apply_if (map, NULL, NULL) == -1);
    assert(hashmap_apply_if (NULL, is_digit, double_value) == -1);
    assert(hashmap_apply_if (map, NULL, double_value) == -1);
    assert(hashmap_apply_if (map, is_digit, NULL) == -1);

    pair *pairs[8];
    for (size_t i = 0; i < 8; ++i)
    {
        char key = (char) ('1' + i);
        // if odd the key is a capital letter.
        if (key % 2) {
            key += 16;
        }
        size_t val = i + 1;
        pair *p = pair_alloc(&key, &val, char_key_cpy, int_value_cpy,
                             char_key_cmp, int_value_cmp,
                             char_key_free, int_value_free);
        pairs[i] = p;
        hashmap_insert(map, p);
    }
        char k1 = '2';
        char k2 = 'C';
        assert (*((int *)hashmap_at(map, &k1)) == 2);
        assert (*((int *)hashmap_at(map, &k2)) == 3);
        assert(hashmap_apply_if(map, is_digit, double_value) == 4);
        assert (*((int *)hashmap_at(map, &k1)) == 4);
        assert (*((int *)hashmap_at(map, &k2)) == 3);


    for (size_t i = 0; i < 8; ++i)
    {
        pair_free((void **) &(pairs[i]));
    }

    hashmap_free(&map);
    assert (map == NULL);
}

//int main ()
//{
//    test_hash_map_insert ();
//    test_hash_map_at ();
//    test_hash_map_erase ();
//    test_hash_map_get_load_factor ();
//    test_hash_map_apply_if ();
//
//    printf("DONE\n");
//    return 0;
//}

