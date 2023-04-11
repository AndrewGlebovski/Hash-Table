/**
 * \file
 * \brief Header file for hash table
*/


/// Return type of all hash functions
typedef unsigned long long hash_t;

/// Type of data stored in hash table
typedef char *data_t;

/// Type of key in hash table
typedef char *okey_t;

/// Hash function type for table
typedef hash_t (*hash_func_t)(void *ptr, size_t size);


/// Node structure for linked list
typedef struct Node {
    okey_t key  = nullptr;              ///< Node key field
    data_t data = nullptr;              ///< Node data field
    Node *next  = nullptr;              ///< Pointer to the next node
} Node;


/// Structure for holding hash table info
typedef struct {
    hash_func_t hash_func = nullptr;    ///< Pointer to hash function used by this table
    size_t buffer_size = 0;             ///< Size of the buffer holding lists first nodes
    Node *buffer = nullptr;             ///< Buffer holding lists first nodes
} HashTable;


/// Possible exit codes for hash table functions
typedef enum {
    OK          = 0,                    ///< OK
    INVALID_ARG = 1,                    ///< Invalid argument passed to function
    ALLOC_FAIL  = 2,                    ///< Calloc failed to allocate memory
} ExitCodes;


/**
 * \brief Constructs hash table from buffer size and hash function
 * \param [out] table           Table to be constructed
 * \param [in]  buffer_size     Size of hash table buffer
 * \param [in]  hash_func       Hash function used by table
 * \return Non zero value means error
*/
int hash_table_constructor(HashTable *table, size_t buffer_size, hash_func_t hash_func);


/**
 * \brief Inserts new data in table
 * \param [out] table       Hash table pointer
 * \param [in]  new_key     Key to insert
 * \param [in]  new_data    Data to insert
 * \return Non zero value means error
*/
int hash_table_insert(HashTable *table, okey_t new_key, data_t new_data);


/**
 * \brief Finds data in table by key
 * \param [in]  table       Hash table pointer
 * \param [in]  key         Key to find
 * \param [out] data        Data will be copied to this address
 * \return Non zero value means error
*/
int hash_table_find(HashTable *table, okey_t key, data_t *data);


/**
 * \brief Removes data from table by key
 * \param [out] table       Hash table pointer
 * \param [in]  key         Key to remove
 * \param [out] data        (Optional) If not null data will be copied to this address before erase 
 * \return Non zero value means error
*/
int hash_table_remove(HashTable *table, okey_t key, data_t *data);


/**
 * \brief Checks hash table for errors
 * \param [in]  table       Hash table pointer
 * \return Non zero value means error
*/
int hash_table_verifier(HashTable *table);


/**
 * \brief Dumps hash table state and data to stream
 * \param [in]  table       Hash table pointer
 * \param [out] stream      Stream for output
 * \return Non zero value means error
*/
int hash_table_dump(HashTable *table, FILE *stream);


/**
 * \brief Destructs table, free all linked lists and buffer
 * \param [out] table       Hash table pointer
 * \return Non zero value means error
*/
int hash_table_destructor(HashTable *table);
