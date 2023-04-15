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
    OK          = 0,                    ///< OK
    INVALID_ARG = 1,                    ///< Invalid argument passed to function
    ALLOC_FAIL  = 2,                    ///< Calloc failed to allocate memory
    BUF_ERROR   = 3,                    ///< Table has null buffer
    BUF_SIZE    = 4,                    ///< Table has invalid buffer size
    POISON_ERR  = 5,                    ///< Unexpected poison value
} ExitCodes;
