#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include "assert.hpp"
#include "hash_table.hpp"


/// Alignment required for char string
const size_t STR_ALIGN = 32;


// #define VERIFY
#define ALIGN_PROTECT
#define ASM_CRC32


/// Wrapper for find_node call
#define FIND_NODE(...) asm_find_node(__VA_ARGS__)


#ifdef VERIFY

    #define VERIFICATE(table_ptr)                                                                   \
    do {                                                                                            \
        int error = hashtable_verifier(table_ptr);                                                  \
        if (error) {                                                                                \
            printf("[Verification failed] %s (%i) %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);  \
            return error;                                                                           \
        }                                                                                           \
    } while(0)

#else

    #define VERIFICATE(table_ptr)

#endif


#ifdef ALIGN_PROTECT

    #define CHECK_ALIGN(ptr)                                                                            \
    do {                                                                                                \
        if (!ptr || ((unsigned long long) ptr & (STR_ALIGN - 1))) {                                     \
            printf("[Alignment failed %p] %s (%i) %s\n", ptr, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
            return ALIGN_FAIL;                                                                          \
        }                                                                                               \
    } while(0)

#else

    #define CHECK_ALIGN(ptr)

#endif


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
 * \warning Doesn't free node's data
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
 * \brief Finds node in list by key and optionally sets previous one (MY ASSEMBLER IMPLEMENTATION)
 * \param [in]  begin   List begin 
 * \param [in]  key     Key to search for
 * \param [out] prev    If not NULL will be filled with address of the previous node
 * \return NULL if nothing found and pointer to the node otherwise
*/
extern "C" Node *asm_find_node(Node *begin, okey_t key, Node **prev);


/**
 * \brief Finds node in list by key and optionally sets previous one (COMPILER IMPLEMENTATION)
 * \param [in]  begin   List begin 
 * \param [in]  key     Key to search for
 * \param [out] prev    If not NULL will be filled with address of the previous node
 * \return NULL if nothing found and pointer to the node otherwise
*/
Node *c_find_node(Node *begin, okey_t key, Node **prev);


/**
 * \brief Compares string aligned on 32-bytes using SIMD if ALIGN_PROTECT defined otherwise uses strcmp
 * \param [in] str1 First string to compare
 * \param [in] str2 Second string to compare
 * \warning No protection from nullptr strings
 * \return 1 - strings are equal, 0 - otherwise
*/
extern "C" int is_equal(const char *str1, const char *str2);


#ifdef ALIGN_PROTECT

/**
 * \brief CRC32 hash function implementation using intrinsics
 * \param [in] key String to calculate hash for
 * \return Hash sum of the string
*/
hash_t crc32_hash(okey_t key);

#endif


/**
 * \brief Classical GNU hash function implementation
 * \param [in] key String to calculate hash for
 * \return Hash sum of the string
*/
hash_t gnu_hash(okey_t key);




int hashtable_constructor(HashTable *table, size_t size) {
    ASSERT(table, INVALID_ARG, "Can't construct nullptr!\n");
    ASSERT(size, INVALID_ARG, "Buckets size can't be zero!\n");

    table -> buckets = (Node **) calloc(size, sizeof(Node *));
    ASSERT(table -> buckets, ALLOC_FAIL, "Can't allocate buckets for table!\n");
    
    table -> size = size;
    table -> count = 0;

    return OK;
}


int hashtable_insert(HashTable *table, okey_t new_key, data_t new_data) {
    VERIFICATE(table);
    ASSERT(new_key, INVALID_ARG, "Key is nullptr!\n");
    CHECK_ALIGN(new_key);

    hash_t hash = get_hash(table, new_key);

    if (!table -> buckets[hash]) {
        table -> buckets[hash] = create_node(new_key, new_data);
        ASSERT(table -> buckets[hash], ALLOC_FAIL, "Failed to alloc new node!\n");

        table -> count++;
    }
    else {
        Node *node = FIND_NODE(table -> buckets[hash], new_key, nullptr);

        if (node) {
            node -> data = new_data;
            printf("New key:%s, Node key: %s, Equal: %d!\n", new_key, node -> key, is_equal(new_key, node -> key));
            return OK;
        }
        else {
            node = table -> buckets[hash];

            table -> buckets[hash] = create_node(new_key, new_data);
            ASSERT(table -> buckets[hash], ALLOC_FAIL, "Failed to alloc new node!\n");

            table -> buckets[hash] -> next = node;

            table -> count++;
        }
    }

    VERIFICATE(table);
    return OK;
}


int hashtable_find(HashTable *table, okey_t key, data_t *data) {
    VERIFICATE(table);
    ASSERT(key, INVALID_ARG, "Key is nullptr!\n");
    CHECK_ALIGN(key);
    ASSERT(data, INVALID_ARG, "Can't store value in null ptr!\n");

    Node *node = FIND_NODE(get_list(table, key), key, nullptr);

    if (node) {
        *data = node -> data;
        return OK;
    }
    
    *data = 0;
    return KEY_NOT_FOUND;
}


int hashtable_remove(HashTable *table, okey_t key) {
    VERIFICATE(table);
    ASSERT(key, INVALID_ARG, "Key is nullptr!\n");
    CHECK_ALIGN(key);

    Node *node = nullptr, *prev = nullptr;
    node = FIND_NODE(get_list(table, key), key, &prev);

    if (node) {
        if (prev) prev -> next = node -> next;
        else table -> buckets[get_hash(table, key)] = node -> next;

        free_node(node);
        table -> count--;
    }

    VERIFICATE(table);
    return OK;
}


int hashtable_verifier(HashTable *table) {
    ASSERT(table, INVALID_ARG, "[Verifier] Table is nullptr!\n");
    ASSERT(table -> buckets, BUF_ERROR, "[Verifier] Table buckets is nullptr!\n");
    ASSERT(table -> size, BUF_SIZE, "[Verifier] Table buckets size is zero!\n");

    for (size_t i = 0; i < table -> size; i++) {
        for (Node *node = table -> buckets[i]; node; node = node -> next) {
            if (node -> key == nullptr) return POISON_ERR;
            CHECK_ALIGN(node -> key);
        }
    }

    return OK;
}


int hashtable_dump(HashTable *table, FILE *stream) {
    VERIFICATE(table);

    fprintf(stream, "Hash Table [%p]\n", table);
    fprintf(stream, "Buffer size: %lu\n", table -> size);
    fprintf(stream, "Buffer:\n");

    for (size_t i = 0; i < table -> size; i++) {
        fprintf(stream, "  %5lu: ", i);

        for (Node *node = table -> buckets[i]; node; node = node -> next) {
            fprintf(stream, "(%s, %i)", node -> key, node -> data);

            if (node -> next) fprintf(stream, " -> ");
        }

        fputc('\n', stream);
    }

    fputc('\n', stream);
    return OK;
}


int hashtable_destructor(HashTable *table) {
    VERIFICATE(table);

    for (size_t i = 0; i < table -> size; i++) free_list(table -> buckets[i]);

    free(table -> buckets);
    table -> buckets = nullptr;

    table -> size = 0;
    table -> count = 0;

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
    return crc32_hash(key) % (hash_t) table -> size;
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


__attribute__ ((noinline)) Node *c_find_node(Node *begin, okey_t key, Node **prev) {
    if (!begin) return nullptr;

    Node *prev_ = nullptr;

    for (; begin && !is_equal(begin -> key, key); begin = begin -> next) prev_ = begin;

    if (prev) *prev = prev_;

    return begin;
}


#ifdef ALIGN_PROTECT

__attribute__ ((noinline)) int is_equal(const char *str1, const char *str2) {
    while (1) {
        __m256i a = _mm256_load_si256((const __m256i *) str1);
        __m256i b = _mm256_load_si256((const __m256i *) str2);
        
        __m256i res = _mm256_sub_epi8(a, b);

        if (!_mm256_testz_si256(res, _mm256_set1_epi32(0xFFFFFFFF))) return 0;

        if (str1[STR_ALIGN - 1] == 0 && str2[STR_ALIGN - 1] == 0) break;
        
        str1 += STR_ALIGN, str2 += STR_ALIGN;
    };

    return 1;
}


__attribute__ ((noinline)) hash_t crc32_hash(okey_t key) {
    hash_t sum = 0;

#ifdef ASM_CRC32
                
    asm(".intel_syntax noprefix     \n\t"
        "xor %0, %0                 \n\t"
        ".loop:                     \n\t"
        "crc32 %0, QWORD PTR [%1]   \n\t"
        "cmp BYTE PTR [%1+7], 0     \n\t"
        "je .finish                 \n\t"
        "add %1, 8                  \n\t"
        "jmp .loop                  \n\t"
        ".finish:                   \n\t"
        :"=r"(sum)
        :"r"(key)
        :"cc"
    );

#else

    while (1) {
        sum = _mm_crc32_u64(sum, *((const hash_t *) key));
        if (!key[7]) break;
        key += 8;
    }

#endif

    return sum;
}

#else

__attribute__ ((noinline)) int is_equal(const char *str1, const char *str2) {
    return !strcmp(str1, str2);
}

#endif


__attribute__ ((noinline)) hash_t gnu_hash(okey_t key) {
    hash_t sum = 5381;

    for (; *key; key++)
        sum = sum * 33 + *key;

    return sum;
}
