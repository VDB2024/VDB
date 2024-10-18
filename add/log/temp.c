#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blst.h"

void hex_to_bytes(const char* hex, unsigned char* bytes, size_t bytes_len) {
    for (size_t i = 0; i < bytes_len; i++) {
        sscanf(hex + 2 * i, "%2hhx", &bytes[i]);
    }
}

int main(int argc, char *argv[]) {
    FILE *fp, *out_fp;
    char buffer[1024];
    char *token;
    int Add, counter;
    char hash[65];  // SHA-256 hash is 64 characters long + null terminator
    int beta = atoi(argv[1]);
    char *input_file = argv[2];
    char *output_file = argv[3];
    printf("beta: %d\n", beta);
    Add = Add * beta;
    counter = counter * beta * beta;

    const blst_p1* gen = blst_p1_generator();
    blst_p1 point, add_point, hash_point, counter_point;
    unsigned char scalar[4]; // Assuming we don't need more than 32 bits for Add and counter

    fp = fopen(input_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return 1;
    }

    out_fp = fopen(output_file, "w");
    if (out_fp == NULL) {
        fprintf(stderr, "Unable to open output file\n");
        fclose(fp);
        return 1;
    }
    // Initialize point to zero by setting it to the generator multiplied by zero
    blst_p1_mult(&point, gen, scalar, 0); // Multiply generator by 0 
    unsigned char output[96]; // Serialized output
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Parse "Add" value
        token = strstr(buffer, "\"Add\": ");
        if (token) {
            sscanf(token, "\"Add\": %d", &Add);
        }

        // Parse "hash" value
        token = strstr(buffer, "\"hash\": \"");
        if (token) {
            sscanf(token, "\"hash\": \"%64s\"", hash);
            hash[64] = '\0'; // Ensure null termination
        }

        // Parse "counter" value
        token = strstr(buffer, "\"counter\": ");
        if (token) {
            sscanf(token, "\"counter\": %d", &counter);
        }

        unsigned char hash_bytes[32]; // SHA-256 hash size
        hex_to_bytes(hash, hash_bytes, sizeof(hash_bytes));

        // 计算 hash*G
        blst_hash_to_g1(&hash_point, hash_bytes, sizeof(hash_bytes), NULL, 0, NULL, 0);

        // Convert integer to byte and calculate Add*G
        scalar[0] = Add & 0xFF;
        scalar[1] = (Add >> 8) & 0xFF;
        scalar[2] = (Add >> 16) & 0xFF;
        scalar[3] = (Add >> 24) & 0xFF;
        blst_p1_mult(&add_point, gen, scalar, sizeof(Add) * 8); // Pass bit count

        // Convert integer to byte and calculate counter*G
        scalar[0] = counter & 0xFF;
        scalar[1] = (counter >> 8) & 0xFF;
        scalar[2] = (counter >> 16) & 0xFF;
        scalar[3] = (counter >> 24) & 0xFF;
        blst_p1_mult(&counter_point, gen, scalar, sizeof(counter) * 8); // Pass bit count


        blst_p1_add(&point, &point, &add_point);
        blst_p1_add(&point, &point, &hash_point);
        blst_p1_add(&point, &point, &counter_point);

        
        blst_p1_serialize(output, &point);
        for (int i = 0; i < sizeof(output); i++) {
            fprintf(out_fp, "%02x", output[i]);
        }
        fprintf(out_fp, "\n");
        printf("init\n");
    }


    fclose(fp);
    fclose(out_fp);

    return 0;
}