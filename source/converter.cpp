#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hash_table.hpp"
#include "text_parser.hpp"




int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong command line arguments!\n");
        return 1;
    }

    int error = convert(argv[1], argv[2]);

    printf("Converter!\n");

    return error;
}
