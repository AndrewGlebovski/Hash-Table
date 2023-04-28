/**
 * \file
 * \brief Header for text parser module
*/


/// Contains info about mmapped file reuqired to unmap() it later
typedef struct {
    char *ptr = nullptr;    ///< Pointer to file begin
    size_t size = 0;        ///< File size
} FileInfo;


/**
 * \brief Store file in memory using mmap() and save file info in the corresponding structure
 * \param [in]  filename    Path to file
 * \param [out] info        File info required for later unmap()
 * \return Non zero value means error
*/
int mmap_file(const char *filename, FileInfo *info);


/**
 * \brief Unmap previously mmapped file using FileInfo
 * \param [out] info Pointer in this structure will be unmapped and set to null
 * \return Non zero value means error 
*/
int unmap_file(FileInfo *info);


/**
 * \brief Converts input file with words into specific binary with each word aligned
 * \param [in] input    Input file to take words from
 * \param [in] output   Output file to write words into
 * \return Non zero value means error
*/
int convert(const char *input, const char *output);


/**
 * \brief Fill hash table with aligned words from specific binary
 * \param [out] info    Pointer to specific binary and it's size
 * \param [out] table   Table to insert word into
 * \return Non zero value means error
*/
int insert_words(FileInfo *info, HashTable *table);
