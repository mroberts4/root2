#ifndef CRYPTO_H
#define CRYPTO_H

#include <cstddef>
#include <cstdint>

#define AES_BLOCKLEN 16 //block size for AES

void xorEnDecrypt(char* input, const char* key, size_t keysize);
void pad_buffer(uint8_t* buf, size_t* length);
void remove_padding(uint8_t* buf, size_t* length);

#endif
