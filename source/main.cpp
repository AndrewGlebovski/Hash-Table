#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hash_table.hpp"
#include "hash_func.hpp"
#include "text_parser.hpp"


/// Contains hash function pointer and it's name
typedef struct {
    hash_func_t func = nullptr;     ///< Hash function pointer
    const char *name = nullptr;     ///< Hash function name
} HashFuncInfo;


/// Hash table possible hash values
const size_t TABLE_BUFFER_SIZE = 17u;




int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong command line arguments!\n");
        return 1;
    }

    FileInfo info = {};
    mmap_file(argv[1], &info);

    HashFuncInfo func_list[] = {
        {&hash_1, "return 1"},
        {&hash_char0, "first char"},
        {&hash_len, "len"},
        {&hash_charsum, "char sum"},
        {&hash_rol, "cyclic left"},
        {&hash_ror, "cyclic right"},
        {&gnu_hash, "gnu hash"}
    };

    FILE *csv_file = fopen("result.csv", "w");

    fprintf(csv_file, "hash function \\ hash value");

    for (size_t i = 0; i < TABLE_BUFFER_SIZE; i++)
        fprintf(csv_file, ", %lu", i);

    fputc('\n', csv_file);

    HashTable table = {};

    for (size_t func = 0; func < sizeof(func_list) / sizeof(HashFuncInfo); func++) {
        hash_table_constructor(&table, TABLE_BUFFER_SIZE, func_list[func].func);

        get_words(&info, &table);

        fprintf(csv_file, "%s", func_list[func].name);

        for (size_t i = 0; i < table.size; i++)
            fprintf(csv_file, ", %lu", get_list_len(table.buckets[i]));

        fputc('\n', csv_file);

        hash_table_destructor(&table);
    }

    fclose(csv_file);

    unmap_file(&info);

    printf("Hash Table!\n");

    return 0;
}
