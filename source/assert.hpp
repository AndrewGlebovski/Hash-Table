/**
 * \file
 * \brief Contains my own assert macros and exit codes common for all functions
*/


/// Checks for condition; if failed prints __VA_ARGS__ and returns with the given exit code
#define ASSERT(condition, exitcode, ...)            \
do {                                                \
    if (!(condition)) {                             \
        printf(__VA_ARGS__);                        \
        return exitcode;                            \
    }                                               \
} while (0)


/// Possible exit codes
typedef enum {
    OK,                 ///< OK
    INVALID_ARG,        ///< Invalid argument passed to function
    ALLOC_FAIL,         ///< Calloc failed to allocate memory
    BUF_ERROR,          ///< Table has null buffer
    BUF_SIZE,           ///< Table has invalid buffer size
    POISON_ERR,         ///< Unexpected poison value
    KEY_NOT_FOUND,      ///< Key wasn't found in hash table
    INFO_NULLPTR,       ///< File info has null ptr
    INFO_SIZE,          ///< File info has invalid file size
    FILE_NOT_FOUND,     ///< File doesn't exist
    FILE_MMAP_ERR,      ///< Failed to mmap file
    FILE_UNMAP_ERR,     ///< Failed to unmap file
    FILE_STAT_ERR,      ///< Failed to get file stat
    WRITE_FAIL,         ///< Failed to write into file
    ALIGN_FAIL,         ///< Pointer was not aligned properly
} ExitCodes;
