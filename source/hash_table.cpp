#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.hpp"
#include "hash_table.hpp"


#define VERIFICATE(table_ptr)                                                                   \
do {                                                                                            \
    int error = hash_table_verifier(table_ptr);                                                 \
    if (error) {                                                                                \
        printf("[Verification failed] %s (%i) %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);  \
        return error;                                                                           \
    }                                                                                           \
} while(0)


/**
 * \brief Allocates new node
 * \param [in] key  New node key
 * \param [in] data New node data
 * \return Pointer to new node or null ptr if allocate failed
*/
Node *create_node(okey_t key, data_t data);


/**
 * \brief Free node
 * \param [out] node This node will be free
 * \note Node's data will not be free
*/
void free_node(Node *node);


/**
 * \brief Returns pointer to list first node corresponding key hash
 * \param [in] key Key to get hash from
 * \return Pointer to list first node
*/
inline Node *get_list(HashTable *table, okey_t key);


/**
 * \brief Free list
 * \param [out] node Free node and it's next one recursively
*/
void free_list(Node *node);


/**
 * \brief Calculates bucket index of the key
 * \param [in] table    Hash table to take hash function from
 * \param [in] key      Key value to calculate hash
 * \return Bucket index correspoding to the key
*/
inline hash_t get_hash(HashTable *table, okey_t key);


/**
 * \brief Finds node in list by key and optionally sets previous one
 * \param [in]  begin   List begin 
 * \param [in]  key     Key to search for
 * \param [out] prev    If not NULL will be filled with address of the previous node
 * \return NULL if nothing found and pointer to the node otherwise
*/
Node *find_node(Node *begin, okey_t key, Node **prev);




int hash_table_constructor(HashTable *table, size_t size, hash_func_t hash_func) {
    ASSERT(table, INVALID_ARG, "Can't construct nullptr!\n");
    ASSERT(size, INVALID_ARG, "Buckets size can't be zero!\n");
    ASSERT(hash_func, INVALID_ARG, "Hash function can't be nullptr!\n");

    table -> hash_func = hash_func;

    table -> buckets = (Node **) calloc(size, sizeof(Node *));
    ASSERT(table -> buckets, ALLOC_FAIL, "Can't allocate buckets for table!\n");
    
    table -> size = size;

    return OK;
}


int hash_table_insert(HashTable *table, okey_t new_key, data_t new_data) {
    VERIFICATE(table);

    hash_t hash = get_hash(table, new_key);

    if (!table -> buckets[hash]) {
        table -> buckets[hash] = create_node(new_key, new_data);
        ASSERT(table -> buckets[hash], ALLOC_FAIL, "Failed to alloc new node!\n");
    }
    else {
        Node *node = find_node(table -> buckets[hash], new_key, nullptr);

        if (node) {
            node -> data = new_data;
            return OK;
        }
        else {
            node = table -> buckets[hash];

            table -> buckets[hash] = create_node(new_key, new_data);
            ASSERT(table -> buckets[hash], ALLOC_FAIL, "Failed to alloc new node!\n");

            table -> buckets[hash] -> next = node;
        }
    }

    return hash_table_verifier(table);
}


int hash_table_find(HashTable *table, okey_t key, data_t *data) {
    VERIFICATE(table);

    Node *node = find_node(get_list(table, key), key, nullptr);

    if (node) {
        *data = node -> data;
        return OK;
    }
    
    *data = 0;
    return KEY_NOT_FOUND;
}


int hash_table_remove(HashTable *table, okey_t key) {
    VERIFICATE(table);

    Node *node = nullptr, *prev = nullptr;
    node = find_node(get_list(table, key), key, &prev);

    if (node) {
        if (prev) prev -> next = node -> next;
        else table -> buckets[get_hash(table, key)] = node -> next;

        free_node(node);
    }

    return hash_table_verifier(table);
}


int hash_table_verifier(HashTable *table) {
    ASSERT(table, INVALID_ARG, "[Verifier] Table is nullptr!\n");
    ASSERT(table -> buckets, BUF_ERROR, "[Verifier] Table buckets is nullptr!\n");
    ASSERT(table -> size, BUF_SIZE, "[Verifier] Table buckets size is zero!\n");

    for (size_t i = 0; i < table -> size; i++) {
        for (Node *node = table -> buckets[i]; node; node = node -> next) {
            if (node -> key == nullptr) return POISON_ERR;
        }
    }

    return OK;
}


int hash_table_dump(HashTable *table, FILE *stream) {
    VERIFICATE(table);

    fprintf(stream, "Hash Table [%p]\n", table);
    fprintf(stream, "Buffer size: %lu\n", table -> size);
    fprintf(stream, "Buffer:\n");

    for (size_t i = 0; i < table -> size; i++) {
        fprintf(stream, "  %5lu: ", i);

        // PRINT IS NOT SAFE FOR NULLPTR!!!
        for (Node *node = table -> buckets[i]; node; node = node -> next) {
            fprintf(stream, "(%s, %i)", node -> key, node -> data);

            if (node -> next) fprintf(stream, " -> ");
        }

        fputc('\n', stream);
    }

    fputc('\n', stream);
    return OK;
}


int hash_table_destructor(HashTable *table) {
    VERIFICATE(table);

    for (size_t i = 0; i < table -> size; i++) free_list(table -> buckets[i]);

    free(table -> buckets);
    table -> buckets = nullptr;

    table -> size = 0;

    table -> hash_func = nullptr;

    return OK;
}


Node *create_node(okey_t key, data_t data) {
    Node *new_node = (Node *) calloc(1, sizeof(Node));

    if (new_node) {
        new_node -> key = key;
        new_node -> data = data;
        new_node -> next = nullptr;
    }

    return new_node;
}


void free_node(Node *node) {
    if (!node) return;

    free(node);
}


inline hash_t get_hash(HashTable *table, okey_t key) {
    return table -> hash_func(&key) % (hash_t) table -> size;
}


inline Node *get_list(HashTable *table, okey_t key) {
    return table -> buckets[get_hash(table, key)];
}


void free_list(Node *node) {
    if (node) {
        free_list(node -> next);
        free_node(node);
    }
}


size_t get_list_len(Node *node) {
    size_t count = 0;
    for (; node; node = node -> next, count++);
    return count;
}


Node *find_node(Node *begin, okey_t key, Node **prev) {
    if (prev) *prev = nullptr;

    if (!begin) return nullptr;
    
    // STRCMP IS NOT SAFE FOR NULLPTR!!!
    for (; begin && strcmp(begin -> key, key); begin = begin -> next) {
        if (prev) *prev = begin;
    }

    return begin;
}
