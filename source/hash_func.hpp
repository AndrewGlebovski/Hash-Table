/**
 * \file
 * \brief This header contains some hash functions for perfomance tests
*/

/// Always return 1
hash_t hash_1(okey_t *key);


/// Returns first letter of the string
hash_t hash_char0(okey_t *key);


/// Returns length of the string
hash_t hash_len(okey_t *key);


/// Returns sum of the string characters
hash_t hash_charsum(okey_t *key);


/// Returns sum of the string character with left cyclic bit shift
hash_t hash_rol(okey_t *key);


/// Returns sum of the string character with right cyclic bit shift
hash_t hash_ror(okey_t *key);


/// Classic gnu hash
hash_t gnu_hash(okey_t *key);




hash_t hash_1(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");
    return 1;
}


hash_t hash_char0(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");
    return (*key)[0];
}


hash_t hash_len(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");
    return strlen(*key);
}


hash_t hash_charsum(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");

    hash_t sum = 0;
    for (const char *str = *key; *str; str++)
        sum += (unsigned char) *str;

    return sum;
}


hash_t hash_rol(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");
    
    hash_t sum = 0;
    for (const char *str = *key; *str; str++)
        sum = ((sum << 1) | (sum >> (sizeof(hash_t) * 8 - 1))) + (unsigned char) *str;

    return (sum) ? sum : 1;
}


hash_t hash_ror(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");
    
    hash_t sum = 0;
    for (const char *str = *key; *str; str++)
        sum = ((sum >> 1) | (sum << (sizeof(hash_t) * 8 - 1))) + (unsigned char) *str;

    return (sum) ? sum : 1;
}


hash_t gnu_hash(okey_t *key) {
    assert(key && *key && "Null pointer or string!\n");
    
    hash_t sum = 5381;
    for (const char *str = *key; *str; str++)
        sum = sum * 33 + *str;

    return sum;
}
