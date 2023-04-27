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


int get_words(FileInfo *info, HashTable *table) {
    ASSERT(info, INVALID_ARG, "File info is null ptr!\n");
    ASSERT(info -> ptr, INVALID_ARG, "Can't scan for words in null ptr!\n");
    ASSERT(info -> size, INVALID_ARG, "Can't scan for words in file with zero size!\n");
    
    char *str_begin = info -> ptr;
    size_t str_len = 0;

    for (size_t i = 0; i < info -> size; i++) {
        if (!isalnum((info -> ptr)[i])) {
            (info -> ptr)[i] = '\0';

            if (table && str_len) 
                hashtable_insert(table, str_begin, rand());

            str_begin = info -> ptr + i + 1;
            str_len = 0;
        }
        else 
            str_len++;
    }

    return OK;
}
