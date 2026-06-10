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
void parse_64bit_string(const char *str, uint32_t *high, uint32_t *low) {
    char high_str[33];
    char low_str[33];

    strncpy(high_str, str, 32);
    high_str[32] = '\0';
    strncpy(low_str, str + 32, 32);
    low_str[32] = '\0';

    *high = bin_str_to_uint32(high_str);
    *low  = bin_str_to_uint32(low_str);
}

int main() {
    return 0;
}