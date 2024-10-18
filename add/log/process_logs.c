#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#define MAX_ADDRESSES 100

// 计算 SHA-256 哈希值的函数
void compute_sha256(const char *str, char outputBuffer[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, strlen(str));
    SHA256_Final(hash, &sha256);

    int i = 0;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;  // Ensure null termination
}

int main() {
    FILE *file = fopen("./add/log/log", "r");
    FILE *readLog = fopen("./add/log/read_log", "w");
    FILE *writeLog = fopen("./add/log/write_log", "w");
    FILE *initialLog = fopen("./add/log/initial_log", "w");
    FILE *finalLog = fopen("./add/log/final_log", "w");
    if (file == NULL || readLog == NULL || writeLog == NULL) {
        perror("File opening failed");
        return 1;
    }

    int usedAddresses[MAX_ADDRESSES] = {0};
    char lastHashes[MAX_ADDRESSES][65] = {{0}};
    int addressCounts[MAX_ADDRESSES] = {0};
    int addressCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        char key[50], value[256], hashOutput[65];
        int add;

        if (sscanf(line, "{ \"Add\": %d, \"%[^\"]\": \"%[^\"]\" }", &add, key, value) == 3 ||
            sscanf(line, "{ \"Add\": %d, \"%[^\"]\": %s }", &add, key, value) == 3) {
            compute_sha256(value, hashOutput);
            int foundIndex = -1;
            for (int i = 0; i < addressCount; i++) {
                if (usedAddresses[i] == add) {
                    foundIndex = i;
                    break;
                }
            }
            if (foundIndex == -1) {
                if (addressCount < MAX_ADDRESSES) {
                    foundIndex = addressCount;
                    usedAddresses[addressCount] = add;
                    strcpy(lastHashes[addressCount], hashOutput);
                    addressCounts[addressCount] = 1;
                    addressCount++;
                    // Log the first appearance with "0" hash and count
                    fprintf(readLog, "{ \"Add\": %d, \"hash\": \"0\", \"counter\": 0 }\n", add);
                    fprintf(writeLog, "{ \"Add\": %d, \"hash\": \"%s\", \"counter\": 1 }\n", add, hashOutput);
                }
            } else {
                // Log subsequent appearances with last hash and incremented counter
                fprintf(readLog, "{ \"Add\": %d, \"hash\": \"%s\", \"counter\": %d }\n", add, lastHashes[foundIndex], addressCounts[foundIndex]);
                addressCounts[foundIndex]++;
                strcpy(lastHashes[foundIndex], hashOutput);
                fprintf(writeLog, "{ \"Add\": %d, \"hash\": \"%s\", \"counter\": %d }\n", add, hashOutput, addressCounts[foundIndex]);
            }
        }
    }
    for (int i = 0; i < addressCount; i++) {
        fprintf(writeLog, "{ \"Add\": %d, \"hash\": \"0\", \"counter\": 0 }\n", usedAddresses[i]);
        fprintf(initialLog, "{ \"Add\": %d, \"hash\": \"0\", \"counter\": 0 }\n", usedAddresses[i]);
    }
    for (int i = 0; i < addressCount; i++) {
        fprintf(readLog, "{ \"Add\": %d, \"hash\": \"%s\", \"counter\": %d }\n", usedAddresses[i], lastHashes[i], addressCounts[i]);
        fprintf(finalLog, "{ \"Add\": %d, \"hash\": \"%s\", \"counter\": %d }\n", usedAddresses[i], lastHashes[i], addressCounts[i]);
    }
    fclose(file);
    fclose(readLog);
    fclose(writeLog);
    fclose(initialLog);
    fclose(finalLog);
    printf("Processing complete. Read log is read_log.txt and write log is write_log.txt\n");

    return 0;
}