#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "teeny_sha1.h"
#include "hmac.h"

#define BLOCKSIZE 64
#define DIGESTSIZE 20

int hmac_sha1(uint8_t* digest, uint8_t* key, uint8_t* message, size_t keylen, size_t msglen)
{
    int i;
    uint8_t newkey[BLOCKSIZE] = {0};
    uint8_t o_key_pad[BLOCKSIZE];
    uint8_t i_key_pad[BLOCKSIZE];
    uint8_t tmpmsg[BLOCKSIZE + DIGESTSIZE];
    uint8_t tmpdigeset[DIGESTSIZE]; 
    uint8_t* tmp;

    // If key is longer than blocksize (64), hash the key and make that the new key
    if (keylen > BLOCKSIZE) {
        sha1digest(newkey, key, keylen);
    }
    else {
        // Transfer to newkey array since that has 0s as padding already
        memcpy(newkey, key, keylen);
    }

    // Create o key pad 
    for (i = 0; i < BLOCKSIZE; i++)
    {
        o_key_pad[i] = 0x5c ^ newkey[i];
    }

    // Create i key pad 
    for (i = 0; i < BLOCKSIZE; i++)
    {
        i_key_pad[i] = 0x36 ^ newkey[i];
    }

    tmp = malloc(msglen + BLOCKSIZE);
    
    memcpy(tmp, i_key_pad, BLOCKSIZE);
    memcpy(tmp + BLOCKSIZE, message, msglen); 
    sha1digest(tmpdigeset, tmp, msglen + BLOCKSIZE);

    memcpy(tmpmsg, o_key_pad, BLOCKSIZE);
    memcpy(tmpmsg + BLOCKSIZE, tmpdigeset, DIGESTSIZE);

    sha1digest(digest, tmpmsg, BLOCKSIZE + DIGESTSIZE);
    
    free(tmp);
    
    return 0;
}

