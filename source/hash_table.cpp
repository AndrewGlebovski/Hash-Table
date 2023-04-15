#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "assert.hpp"
#include "hash_table.hpp"




/**
 * \brief Allocates new node
 * \param [in] key  New node key
 * \param [in] data New node data
 * \return Pointer to new node or null ptr if allocate failed
*/
Node *create_node(okey_t key, data_t data);


/**
 * \brief Returns pointer to list first node corresponding key hash
 * \param [in] key Key to get hash from
 * \return Pointer to list first node
*/
Node *get_list(HashTable *table, okey_t key);


/**
 * \brief Free list
 * \param [out] node Free node and it's next one recursively
*/
void free_list(Node *node);




int hash_table_constructor(HashTable *table, size_t buffer_size,
    hash_func_t hash_func,
    cmp_keys_t cmp_keys,
    print_key_t print_key, 
    print_data_t print_data
) {
    ASSERT(table, INVALID_ARG, "Can't construct nullptr!\n");
    ASSERT(buffer_size, INVALID_ARG, "Buffer size can't be zero!\n");
    ASSERT(hash_func, INVALID_ARG, "Hash function can't be nullptr!\n");

    table -> hash_func = hash_func;
    table -> cmp_keys = cmp_keys;
    table -> print_key = print_key;
    table -> print_data = print_data;

    table -> buffer = (Node *) calloc(buffer_size, sizeof(Node));
    ASSERT(table -> buffer, ALLOC_FAIL, "Can't allocate buffer for table!\n");
    
    table -> buffer_size = buffer_size;

    return OK;
}


int hash_table_insert(HashTable *table, okey_t new_key, data_t new_data) {
    ASSERT(table, INVALID_ARG, "Can't insert in nullptr!\n");
    ASSERT(table -> buffer, BUF_ERROR, "Can't insert in table with null buffer!\n");

    Node *node = get_list(table, new_key);

    if (node -> key == POISON_KEY && node -> data == POISON_DATA) {
        node -> key = new_key;
        node -> data = new_data;
        node -> next = nullptr;
    }
    else {
        while (node -> next) {
            if (!(table -> cmp_keys)(new_key, node -> key)) return OK; // Key already exists

            node = node -> next;
        }

        if (!(table -> cmp_keys)(new_key, node -> key)) return OK; // Key already exists

        node -> next = create_node(new_key, new_data);
        ASSERT(node -> next, ALLOC_FAIL, "Failed to alloc new node!\n");
    }

    return OK;
}


int hash_table_find(HashTable *table, okey_t key, data_t *data) {
    ASSERT(table, INVALID_ARG, "Can't insert in nullptr!\n");
    ASSERT(data, INVALID_ARG, "Can't store result in nullptr!\n");
    ASSERT(table -> buffer, BUF_ERROR, "Can't insert in table with null buffer!\n");

    Node *node = get_list(table, key);

    while (node) {
        if (!table -> cmp_keys(key, node -> key)) {
            *data = node -> data;
            return OK;
        }

        node = node -> next;
    }

    *data = POISON_DATA;
    return OK;
}


int hash_table_remove(HashTable *table, okey_t key, data_t *data) {
    ASSERT(table, INVALID_ARG, "Can't insert in nullptr!\n");
    ASSERT(table -> buffer, BUF_ERROR, "Can't insert in table with null buffer!\n");

    Node *node = get_list(table, key), *prev = nullptr;

    while (node) {
        if (!table -> cmp_keys(key, node -> key)) {
            if (data) *data = node -> data;

            if (prev) {
                prev -> next = node -> next;
                free(node);
            }
            else {
                if (node -> next) {
                    Node *next = node -> next;
                    memcpy(node, node -> next, sizeof(Node));
                    free(next);
                }
                else {
                    node -> key = POISON_KEY;
                    node -> data = POISON_DATA;
                    node -> next = nullptr;
                }
            }

            return OK;
        }

        prev = node;
        node = node -> next;
    }

    if (data) *data = POISON_DATA;

    return OK;
}


int hash_table_verifier(HashTable *table) {
    ASSERT(table, INVALID_ARG, "[Verifier] Table is nullptr!\n");
    ASSERT(table -> buffer, BUF_ERROR, "[Verifier] Table buffer is nullptr!\n");
    ASSERT(table -> buffer_size, BUF_SIZE, "[Verifier] Table buffer size is zero!\n");

    for (size_t i = 0; i < table -> buffer_size; i++) {
        for (Node *node = table -> buffer + i; node; node = node -> next) {
            if (!(table -> cmp_keys)(POISON_KEY, node -> key) && node -> next) return POISON_ERR;
        }
    }

    return OK;
}


int hash_table_dump(HashTable *table, FILE *stream) {
    ASSERT(!hash_table_verifier(table), INVALID_ARG, "Can't dump due to verification error!\n");

    fprintf(stream, "Hash Table [%p]\n", table);
    fprintf(stream, "Buffer size: %lu\n", table -> buffer_size);
    fprintf(stream, "Buffer:\n");

    for (size_t i = 0; i < table -> buffer_size; i++) {
        fprintf(stream, "  %5lu: ", i);

        for (Node *node = table -> buffer + i; node; node = node -> next) {
            fprintf(stream, "(");
            (table -> print_key)(stream, node -> key);
            fprintf(stream, ", ");
            (table -> print_data)(stream, node -> data);
            fprintf(stream, ")");

            if (node -> next) fprintf(stream, " -> ");
        }

        fputc('\n', stream);
    }

    fputc('\n', stream);
    return OK;
}


int hash_table_destructor(HashTable *table) {
    ASSERT(!hash_table_verifier(table), INVALID_ARG, "Can't destroy due to verification error!\n");

    for (size_t i = 0; i < table -> buffer_size; i++) free_list((table -> buffer + i) -> next);

    free(table -> buffer);
    table -> buffer = nullptr;

    table -> buffer_size = 0;

    table -> hash_func = nullptr;
    table -> cmp_keys = nullptr;
    table -> print_key = nullptr;
    table -> print_data = nullptr;

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


Node *get_list(HashTable *table, okey_t key) {
    hash_t hash = table -> hash_func(&key) % (hash_t) table -> buffer_size;
    return table -> buffer + hash;
}


void free_list(Node *node) {
    if (node) {
        free_list(node -> next);
        free(node);
    }
}


size_t get_list_len(Node *node) {
    size_t count = 0;
    for (; node; node = node -> next, count++);
    return count;
}
