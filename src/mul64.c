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

/* ---- format 128-bit value (4 limbs, MSW first) -> 128-char binary ---- */
static void format128(uint32_t w3, uint32_t w2, uint32_t w1, uint32_t w0, char *out) {
uint32_t w[4] = { w3, w2, w1, w0 };
char *p = out;
for (int k = 0; k < 4; ++k)
for (int b = 31; b >= 0; --b)
*p++ = (char)('0' + ((w[k] >> b) & 1u));
*p = '\0';
}

static double cpu_hz(void) {
    uint32_t a, b, c, d;
    __asm__ volatile("cpuid" : "=a"(a),"=b"(b),"=c"(c),"=d"(d) : "a"(0));
    uint32_t maxleaf = a;
    if (maxleaf >= 0x15) {
        __asm__ volatile("cpuid" : "=a"(a),"=b"(b),"=c"(c),"=d"(d) : "a"(0x15));
        /* a = denom, b = numer, c = core crystal Hz */
        if (a && b && c) return (double)c * (double)b / (double)a;
    }
    return 0.0; /* caller substitutes a known base clock if 0 */
}
