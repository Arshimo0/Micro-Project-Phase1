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
    const char *num1_str = "0000000000000000000000000000000000000000000000000000000000000010"; 
    const char *num2_str = "0000000000000000000000000000000000000000000000000000000000000011";

    uint32_t a_high, a_low;
    uint32_t b_high, b_low;
    
    parse_64bit_string(num1_str, &a_high, &a_low);
    parse_64bit_string(num2_str, &b_high, &b_low);
    
    printf("Number 1 -> High: %u, Low: %u\n", a_high, a_low);
    printf("Number 2 -> High: %u, Low: %u\n", b_high, b_low)
    return 0;
}