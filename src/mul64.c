#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ---- parse 64-char binary string -> uint64_t (split into two u32 limbs) ---- */
static inline void parse64(const char *s, uint32_t *hi, uint32_t *lo) {
uint32_t h = 0, l = 0;
/* high 32 chars -> h, low 32 chars -> l */
for (int i = 0; i < 32; ++i)
    h = (h << 1) | (uint32_t)(s[i] - '0');
for (int i = 0; i < 32; ++i) 
    l = (l << 1) | (uint32_t)(s[i + 32] - '0');
*hi = h; *lo = l;
}