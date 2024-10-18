#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blst.h"

void hex_to_bytes(const char* hex, unsigned char* bytes, size_t bytes_len) {
    for (size_t i = 0; i < bytes_len; i++) {
        sscanf(hex + 2 * i, "%2hhx", &bytes[i]);
    }
}

// p1 + beta*p2 + beta^2*p3
void calculate_expression(blst_p1 *result, const blst_p1 *p1, const blst_p1 *p2, const blst_p1 *p3, int beta) {
    unsigned char scalar[32]={0};
    blst_p1 beta_p2, beta2_p3;

    // beta * p2
    int_to_scalar(scalar, beta);
    blst_p1_mult(&beta_p2, p2, scalar, 256);

    // beta^2 * p3
    int beta2 = beta * beta;
    int_to_scalar(scalar, beta2);
    blst_p1_mult(&beta2_p3, p3, scalar, 256);

    // Add all together: p1 + beta*p2 + beta^2*p3
    blst_p1_add(result, p1, &beta_p2);
    blst_p1_add(result, result, &beta2_p3);
}

void int_to_scalar(unsigned char scalar[32], uint64_t num) {
    for (int i = 0; i < 32; i++) {
        if (i < sizeof(num)) {
            scalar[i] = (num >> (8 * i)) & 0xFF;
        } else {
            scalar[i] = 0;  
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *fp, *out_fp;
    char buffer[1024];
    char *token;
    int Add, counter;
    char hash[65];  // SHA-256 hash is 64 characters long + null terminator
    char *input_file = argv[1];
    char *output_file = argv[2];

    const blst_p1* gen = blst_p1_generator();

    blst_p1 point, add_point, hash_point, counter_point;
    unsigned char scalar[32]={0}; // Assuming we don't need more than 32 bits for Add and counter

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
    blst_p1_affine out;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Parse "Add" value
        token = strstr(buffer, "\"Add\": ");
        if (token) {
            sscanf(token, "\"Add\": %d", &Add);
        }

        // Parse "hash" value
        token = strstr(buffer, "\"hash\": \"");
        if (token) {
            sscanf(token, "\"hash\": \"%64[^\"]\"", hash); 
        }

        // Parse "counter" value
        token = strstr(buffer, "\"counter\": ");
        if (token) {
            sscanf(token, "\"counter\": %d", &counter);
        }


        
        // Convert integer to byte and calculate Add*G
        int_to_scalar(scalar, Add);    
        blst_p1_mult(&add_point, gen, scalar, 256);
        blst_p1_to_affine(&out, &add_point);
        blst_p1_affine_serialize(output, &out);
        for (int i = 0; i < sizeof(output); i++) {
           fprintf(out_fp, "%02x", output[i]);
        }
        fprintf(out_fp, "\n");

        

        unsigned char hash_bytes[32]; // SHA-256 hash size
        if (strcmp(hash, "0") == 0) {
            int_to_scalar(scalar, 0);
            blst_p1_mult(&hash_point, gen, scalar, 256);
            blst_p1_to_affine(&out, &hash_point);
            blst_p1_affine_serialize(output, &out);
            for (int i = 0; i < sizeof(output); i++) {
                fprintf(out_fp, "%02x", output[i]);
            }
            fprintf(out_fp, "\n");
        }
        else{
            hex_to_bytes(hash, hash_bytes, sizeof(hash_bytes));
            blst_hash_to_g1(&hash_point, hash_bytes, 32, NULL, 0, NULL, 0);
            blst_p1_to_affine(&out, &hash_point);
            blst_p1_affine_serialize(output, &out);
            for (int i = 0; i < sizeof(output); i++) {
                fprintf(out_fp, "%02x", output[i]);
            }
            fprintf(out_fp, "\n");
        }


        // Convert integer to byte and calculate counter*G
        int_to_scalar(scalar, counter);  
        blst_p1_mult(&counter_point, gen, scalar, 256);
        blst_p1_to_affine(&out, &counter_point);
        blst_p1_affine_serialize(output, &out);
        for (int i = 0; i < sizeof(output); i++) {
            fprintf(out_fp, "%02x", output[i]);
        }
        fprintf(out_fp, "\n");

        //blst_p1 result;
        //calculate_expression(&result, &add_point, &hash_point, &counter_point, beta);

        //blst_p1_serialize(output, &point);
        //for (int i = 0; i < sizeof(output); i++) {
        //    fprintf(out_fp, "%02x", output[i]);
        //}
        //fprintf(out_fp, "\n");
    }

    fclose(fp);
    fclose(out_fp);

    return 0;
}