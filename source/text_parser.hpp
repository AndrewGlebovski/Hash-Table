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
 * \brief Split text in memory into words by replacing each not alnum() symbol with '\0'
 * \param [out] info    Pointer to text and it's size
 * \param [out] table   Table to insert word into (only if table ptr is not null)
 * \return Non zero value means error
*/
int get_words(FileInfo *info, HashTable *table);
