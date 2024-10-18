#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blst.h"


void hex_to_bytes(const char* hex, unsigned char* bytes, size_t bytes_len) {
    for (size_t i = 0; i < bytes_len; i++) {
        sscanf(hex + 2 * i, "%2hhx", &bytes[i]);
        // printf("bytes[%d]: %02x\n", i, bytes[i]);
    }
}


void deserialize_point(blst_p1 *point, const unsigned char *bytes) {
    blst_p1_deserialize(point, bytes);
}

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
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }
    int beta = atoi(argv[1]);

    FILE *fp_in = fopen(argv[2], "r");
    if (fp_in == NULL) {
        fprintf(stderr, "Unable to open input file %s\n", argv[2]);
        return 1;
    }

    FILE *fp_out = fopen(argv[3], "w");
    if (fp_out == NULL) {
        fprintf(stderr, "Unable to open output file %s\n", argv[3]);
        fclose(fp_in);
        return 1;
    }

    char line[193]; 
    
    blst_p1_affine points_affine[3], out;
    int point_index = 0;
    unsigned char bytes[96]; 

    blst_p1 result;
    blst_p1 points[3];


    while (fgets(line, sizeof(line), fp_in)) {
        hex_to_bytes(line, bytes, sizeof(bytes));
        blst_p1_deserialize(&points_affine[point_index], bytes);
        point_index++;

        if (point_index == 3) {
       
            // add_three_points(&result, &points[0], &points[1], &points[2]);

        unsigned char output[96];

        blst_p1_from_affine(&points[0], &points_affine[0]);
        blst_p1_from_affine(&points[1], &points_affine[1]);
        blst_p1_from_affine(&points[2], &points_affine[2]);

        calculate_expression(&result, &points[0], &points[1], &points[2], beta);

        blst_p1_to_affine(&out, &result);
        blst_p1_affine_serialize(output, &out);
        for (int i = 0; i < sizeof(output); i++) {
           fprintf(fp_out, "%02x", output[i]);
        }
        fprintf(fp_out, "\n");

        point_index = 0;
        }
        fgets(line, sizeof(line), fp_in);
    }

    fclose(fp_in);
    fclose(fp_out);

    return 0;
}