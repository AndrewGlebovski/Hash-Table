#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hash_table.hpp"
#include "hash_func.hpp"




int cmp_keys(okey_t a, okey_t b);

void print_key(FILE *stream, okey_t key);

void print_data(FILE *stream, data_t data);




int main() {
    HashTable table = {};

    hash_table_constructor(&table, 16, &gnu_hash, &cmp_keys, &print_key, &print_data);

    hash_table_dump(&table, stdout);

    hash_table_destructor(&table);

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
