/**
 * \file
 * \brief Header file for hash table
*/


/// Return type of all hash functions
typedef unsigned long long hash_t;

/// Type of data stored in hash table
typedef int data_t;

/// Type of key in hash table
typedef const char *okey_t;

/// Hash function type for table
typedef hash_t (*hash_func_t)(okey_t *ptr);     ///< Hash function type for table


/// Node structure for linked list
typedef struct Node {
    okey_t key  = nullptr;              ///< Node key field
    data_t data = 0;                    ///< Node data field
    Node *next  = nullptr;              ///< Pointer to the next node
} Node;


/// Structure for holding hash table info
typedef struct {
    hash_func_t hash_func = nullptr;    ///< Pointer to hash function used by this table
    size_t size = 0;                    ///< Amount of buckets in hash table
    Node **buckets = nullptr;           ///< Buffer holding lists first nodes
} HashTable;


/**
 * \brief Constructs hash table from buffer size and hash function
 * \param [out] table           Table to be constructed
 * \param [in]  size            Amount of buckets in hash table
 * \param [in]  hash_func       Hash function used by table
 * \param [in]  cmp_keys        Function to compare keys
 * \param [in]  print_key       Function to print key
 * \param [in]  print_data      Function to print data
 * \return Non zero value means error
*/
int hashtable_constructor(HashTable *table, size_t size,  hash_func_t hash_func);


/**
 * \brief Inserts new data in table
 * \param [out] table       Hash table pointer
 * \param [in]  new_key     Key to insert
 * \param [in]  new_data    Data to insert
 * \return Non zero value means error
*/
int hashtable_insert(HashTable *table, okey_t new_key, data_t new_data);


/**
 * \brief Finds data in table by key
 * \param [in]  table       Hash table pointer
 * \param [in]  key         Key to find
 * \param [out] data        Data will be copied to this address
 * \return Non zero value means error
*/
int hashtable_find(HashTable *table, okey_t key, data_t *data);


/**
 * \brief Removes data from table by key
 * \param [out] table       Hash table pointer
 * \param [in]  key         Key to remove
 * \return Non zero value means error
*/
int hashtable_remove(HashTable *table, okey_t key);


/**
 * \brief Checks hash table for errors
 * \param [in]  table       Hash table pointer
 * \return Non zero value means error
*/
int hashtable_verifier(HashTable *table);


/**
 * \brief Dumps hash table state and data to stream
 * \param [in]  table       Hash table pointer
 * \param [out] stream      Stream for output
 * \return Non zero value means error
*/
int hashtable_dump(HashTable *table, FILE *stream);


/**
 * \brief Destructs table, free all linked lists and buckets
 * \param [out] table       Hash table pointer
 * \return Non zero value means error
*/
int hashtable_destructor(HashTable *table);


/**
 * \brief Counts list length starting from node
 * \param [in] node Node to start count from
 * \return List size (0 if node is null)
*/
size_t get_list_len(Node *node);
