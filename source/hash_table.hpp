/**
 * \file
 * \brief Header file for hash table
*/


#define POISON_KEY nullptr
#define POISON_DATA nullptr


/// Return type of all hash functions
typedef unsigned long long hash_t;

/// Type of data stored in hash table
typedef const char *data_t;

/// Type of key in hash table
typedef const char *okey_t;

/// Hash function type for table
typedef hash_t (*hash_func_t)(okey_t *ptr);                 ///< Hash function type for table
typedef int (*cmp_keys_t)(okey_t a, okey_t b);              ///< Compare keys functions
typedef void (*print_key_t)(FILE *stream, okey_t key);      ///< Prints key
typedef void (*print_data_t)(FILE *stream, data_t data);    ///< Prints data


/// Node structure for linked list
typedef struct Node {
    okey_t key  = POISON_KEY;           ///< Node key field
    data_t data = POISON_DATA;          ///< Node data field
    Node *next  = nullptr;              ///< Pointer to the next node
} Node;


/// Structure for holding hash table info
typedef struct {
    hash_func_t hash_func = nullptr;    ///< Pointer to hash function used by this table
    cmp_keys_t cmp_keys = nullptr;      ///< Pointer to compare function for keys
    print_key_t print_key = nullptr;    ///< Pointer to function printing key
    print_data_t print_data = nullptr;  ///< Pointer to function printing data
    size_t buffer_size = 0;             ///< Size of the buffer holding lists first nodes
    Node *buffer = nullptr;             ///< Buffer holding lists first nodes
} HashTable;


/**
 * \brief Constructs hash table from buffer size and hash function
 * \param [out] table           Table to be constructed
 * \param [in]  buffer_size     Size of hash table buffer
 * \param [in]  hash_func       Hash function used by table
 * \param [in]  cmp_keys        Function to compare keys
 * \param [in]  print_key       Function to print key
 * \param [in]  print_data      Function to print data
 * \return Non zero value means error
*/
int hash_table_constructor(HashTable *table, size_t buffer_size, 
    hash_func_t hash_func,
    cmp_keys_t cmp_keys,
    print_key_t print_key, 
    print_data_t print_data
);


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
 * \param [out] data        Data will be copied to this address or set to POISON_DATA if not found
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


/**
 * \brief Counts list length starting from node
 * \param [in] node Node to start count from
 * \return List size (0 if node is null)
*/
size_t get_list_len(Node *node);
