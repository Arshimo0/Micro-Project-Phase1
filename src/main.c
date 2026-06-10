#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint32_t bin_str_to_uint32(const char *str) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) + (str[i] - '0');
    }
    return result;
}

int main() {
    return 0;
}