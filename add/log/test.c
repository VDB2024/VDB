// #include "blst.h"
// #include <stdio.h>
// #include <stdint.h>

// // 将一个64位整数转换为256位（32字节）的小端字节序列
// void int_to_scalar(unsigned char scalar[32], uint64_t num) {
//     for (int i = 0; i < 32; i++) {
//         if (i < sizeof(num)) {
//             scalar[i] = (num >> (8 * i)) & 0xFF;
//         } else {
//             scalar[i] = 0;  // 填充高位为0
//         }
//     }
// }

// int main() {
//     // 获取BLS12-381曲线的生成元
//     const blst_p1* gen = blst_p1_generator();

//     // 接收用户输入的整数
//     uint64_t num;
//     printf("Enter an integer: ");
//     scanf("%lu", &num);  // 使用 %lu 来匹配 uint64_t

//     // 创建一个256位的标量
//     unsigned char scalar[32];
//     int_to_scalar(scalar, num);

//     // 创建点来存储结果
//     blst_p1 result;

//     // 标量乘法：将生成元乘以标量
//     blst_p1_mult(&result, gen, scalar, 256); // 256位标量

//     // 序列化点并打印结果
//     unsigned char output[96];
//     blst_p1_serialize(output, &result);
//     for (int i = 0; i < sizeof(output); i++) {
//         printf("%02x", output[i]);
//     }
//     printf("\n");

//     return 0;
// }

// #include "blst.h"
// #include <stdio.h>
// #include <stdint.h>

// int main() {
//     // 获取BLS12-381曲线的生成元
//     const blst_p1* gen = blst_p1_generator();

//     // 提供一个固定的哈希值，例如SHA-256哈希
//     unsigned char hash[32] = {
//         0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab,
//         0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd,
//         0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef,
//         0xab, 0xcd
//     };

//     // 创建点来存储结果
//     blst_p1 result;
//     printf("%d\n",sizeof(hash));
//     // 将哈希映射到曲线上
//     blst_hash_to_g1(&result, hash, 32, NULL, 0, NULL, 0);

//     // 序列化点并打印结果
//     unsigned char output[96];
//     blst_p1_serialize(output, &result);
//     for (int i = 0; i < sizeof(output); i++) {
//         printf("%02x", output[i]);
//     }
//     printf("\n");

//     return 0;
// }
#include <stdio.h>
#include "blst.h"

int main() {
    // 16进制字符串转换为字节序列
    unsigned char hex_str[] = {
        0x0f, 0x81, 0xda, 0x25, 0xec, 0xf1, 0xc8, 0x4b, 0x57, 0x7f, 0xef, 0xbe, 0xdd, 0x61, 0x07, 0x7a,
        0x81, 0xdc, 0x43, 0xb0, 0x03, 0x04, 0x01, 0x5b, 0x2b, 0x59, 0x6a, 0xb6, 0x7f, 0x00, 0xe4, 0x1c,
        0x86, 0xbb, 0x00, 0xeb, 0xd0, 0xf9, 0x0d, 0x4b, 0x12, 0x5e, 0xb0, 0x53, 0x98, 0x91, 0xae, 0xed,
        0x11, 0xaf, 0x62, 0x95, 0x91, 0xec, 0x86, 0x91, 0x6d, 0x6c, 0xe3, 0x78, 0x77, 0xb7, 0x43, 0xfe,
        0x20, 0x9a, 0x3a, 0xf6, 0x11, 0x47, 0x99, 0x6c, 0x1d, 0xf7, 0xfd, 0x1c, 0x47, 0xb0, 0x31, 0x81,
        0xcd, 0x80, 0x6f, 0xd3, 0x1c, 0x30, 0x71, 0xb7, 0x39, 0xe4, 0xde, 0xb2, 0x34, 0xbd, 0x9e, 0x19
    };

    blst_p1_affine point;
    BLST_ERROR err;

    // 反序列化G1点
    err = blst_p1_deserialize(&point, hex_str);
    if (err != BLST_SUCCESS) {
        fprintf(stderr, "Deserialization failed with error %d\n", err);
        return 1;
    }

    unsigned char output[96];
    blst_p1_affine_serialize(output, &point);
    for (int i = 0; i < sizeof(output); i++) {
       printf("%02x", output[i]);
    }
    printf("\n");

    return 0;
}