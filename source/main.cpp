#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hash_table.hpp"
#include "hash_func.hpp"
#include "text_parser.hpp"


/// Hash table possible hash values
const size_t TABLE_BUFFER_SIZE = 17u;


int cmp_keys(okey_t a, okey_t b);

void print_key(FILE *stream, okey_t key);

void print_data(FILE *stream, data_t data);




int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong command line arguments!\n");
        return 1;
    }

    FileInfo info = {};
    mmap_file(argv[1], &info);

    HashTable table = {};

    hash_table_constructor(&table, TABLE_BUFFER_SIZE, gnu_hash, &cmp_keys, &print_key, &print_data);

    get_words(&info, &table);

    fprintf(stdout, "gnu hash: ");

    for (size_t i = 0; i < table.buffer_size; i++)
        fprintf(stdout, "%lu ", get_list_len(table.buffer + i));

    fputc('\n', stdout);

    hash_table_destructor(&table);

    unmap_file(&info);

    printf("Hash Table!\n");

    return 0;
}




int cmp_keys(okey_t a, okey_t b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    return strcmp(a, b);
}


void print_key(FILE *stream, okey_t key) {
    fprintf(stream, "%s", (key) ? key : "[POISON]");
}


void print_data(FILE *stream, data_t data) {
    fprintf(stream, "%s", (data) ? data : "[POISON]");
}
