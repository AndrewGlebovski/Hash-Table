#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include "assert.hpp"
#include "hash_table.hpp"
#include "text_parser.hpp"


/// Words alignment in output file
const size_t WORD_ALIGN = 32u;


/**
 * \brief Store words from mmapped file with 32-bytes alignment in output file filling gaps with zeros
 * \param [in]  info    Mmapped file info
 * \param [out] output  File descriptor to write into
 * \return Non zero value means error
*/
int store_words(const FileInfo *info, int output);



int mmap_file(const char *filename, FileInfo *info) {
    ASSERT(filename, INVALID_ARG, "Filename is null ptr!\n");
    ASSERT(info, INVALID_ARG, "File info is null ptr!\n");

    int binary = open(filename, O_RDWR, 0x0777);
    ASSERT(binary >= 0, FILE_NOT_FOUND, "Can't open %s!\n", filename);

    struct stat file_stat = {};
    ASSERT(fstat(binary, &file_stat) >= 0, FILE_STAT_ERR, "Can't get %s stat!\n", filename);
    info -> size = file_stat.st_size;

    info -> ptr = (char *) mmap(0, info -> size, PROT_READ | PROT_WRITE, MAP_PRIVATE, binary, 0);
    ASSERT(info -> ptr, FILE_MMAP_ERR, "Can't get %s mapping!\n", filename);

    close(binary);

    return OK;
}


int unmap_file(FileInfo *info) {
    ASSERT(info, INVALID_ARG, "File info is null ptr!\n");
    ASSERT(info -> ptr, INFO_NULLPTR, "Can't unmap null ptr!\n");
    ASSERT(info -> size, INFO_SIZE, "Can't unmap file with zero size!\n");

    ASSERT(!munmap(info -> ptr, info -> size), FILE_UNMAP_ERR, "File failed to unmap!\n");

    info -> ptr = nullptr, info -> size = 0;

    return OK;
}


int convert(const char *input, const char *output) {
    ASSERT(input, INVALID_ARG, "Converter can't work with null input!\n");
    ASSERT(output, INVALID_ARG, "Converter can't work with null output!\n");

    FileInfo info = {};
    int error = mmap_file(input, &info);
    if (error) return error;

    int out_file = open(output, O_WRONLY | O_CREAT, 00770);
    ASSERT(out_file > -1, FILE_NOT_FOUND, "Failed to open %s!\n", output);

    error = store_words(&info, out_file);
    if (error) return error;

    close(out_file);

    return unmap_file(&info);
}


int store_words(const FileInfo *info, int output) {
    ASSERT(info, INVALID_ARG, "Store_words can't work with null info!\n");
    ASSERT(info -> ptr, INVALID_ARG, "Can't scan for words in null ptr!\n");
    ASSERT(info -> size, INVALID_ARG, "Can't scan for words in file with zero size!\n");
    ASSERT(output > -1, INVALID_ARG, "Invalid desciptor passed to the store_words!\n");

    char *str_begin = info -> ptr;
    size_t str_len = 0;

    char gap[WORD_ALIGN] = "";

    for (size_t i = 0; i < info -> size; i++) {
        if (!isalnum((info -> ptr)[i])) {
            if (str_len) {
                ASSERT(write(output, str_begin, str_len) != -1, WRITE_FAIL, "Failed to write to file!\n");

                str_len %= WORD_ALIGN;
                ASSERT(write(output, gap + str_len, WORD_ALIGN - str_len) != -1, WRITE_FAIL, "Failed to write to file!\n");
            }

            str_begin = info -> ptr + i + 1;
            str_len = 0;
        }
        else str_len++;
    }

    return OK;
}


int insert_words(FileInfo *info, HashTable *table) {
    ASSERT(info, INVALID_ARG, "File info is null ptr!\n");
    ASSERT(info -> ptr, INVALID_ARG, "Can't scan for words in null ptr!\n");
    ASSERT(info -> size, INVALID_ARG, "Can't scan for words in file with zero size!\n");
    ASSERT(table, INVALID_ARG, "Can't insert words in null table!\n");
    
    char *str_begin = info -> ptr;

    for (size_t i = 0; i < info -> size; i += WORD_ALIGN) {
        while ((info -> ptr)[i + WORD_ALIGN - 1]) i += WORD_ALIGN;

        if (*str_begin) hashtable_insert(table, str_begin, 0);
        
        str_begin = info -> ptr + i + WORD_ALIGN;
    }

    return OK;
}
