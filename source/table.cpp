#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "hash_table.hpp"
#include "text_parser.hpp"


/// Hash table possible hash values
const size_t TABLE_BUFFER_SIZE = 109u;


/// Classic gnu hash
hash_t gnu_hash(okey_t *key);




int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong command line arguments!\n");
        return 1;
    }

    srand((unsigned int) time(NULL));

    FileInfo info = {};
    mmap_file(argv[1], &info);

    HashTable table = {};

    hashtable_constructor(&table, TABLE_BUFFER_SIZE, gnu_hash);

    insert_words(&info, &table);
    
    data_t data = 0;
    for (size_t i = 0; i < 1000000; i++)
        hashtable_find(&table, info.ptr + ((size_t) rand() % 58000) * 32, &data);

    hashtable_destructor(&table);

    unmap_file(&info);

    printf("Hash Table!\n");

    return 0;
}




hash_t gnu_hash(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");
    
    hash_t sum = 5381;
    for (const char *str = *key; *str; str++)
        sum = sum * 33 + *str;

    return sum;
}
