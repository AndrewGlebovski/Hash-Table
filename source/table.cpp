#include <stdio.h>
#include <stdlib.h>
#include "hash_table.hpp"
#include "text_parser.hpp"


/// Hash table possible hash values
const size_t TABLE_BUFFER_SIZE = 1009u;

/// Amount of hashtable_find() to perform
const size_t FIND_NUMBER = 10000000;




int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong command line arguments!\n");
        return 1;
    }

    FileInfo info = {};
    mmap_file(argv[1], &info);

    HashTable table = {};

    hashtable_constructor(&table, TABLE_BUFFER_SIZE);

    insert_words(&info, &table);

    data_t data = 0;
    for (size_t i = 0; i < FIND_NUMBER; i++)
        hashtable_find(&table, info.ptr + (i % (table.count - 1)) * 32, &data);

    hashtable_destructor(&table);

    unmap_file(&info);

    printf("Hash Table!\n");

    return 0;
}
