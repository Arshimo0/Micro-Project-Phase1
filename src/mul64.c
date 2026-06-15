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

int main(void) {
char sa[80], sb[80], out[129];
if (scanf("%79s %79s", sa, sb) != 2) return 1;

uint32_t aH, aL, bH, bL;
parse64(sa, &aH, &aL);
parse64(sb, &bH, &bL);

uint32_t w0, w1, w2, w3; /* 128-bit result, little-endian limbs */
uint32_t t0, t1; /* RDTSC samples */

/* ===== MEASURED REGION: nothing but RDTSC + multiply ===== */
__asm__ volatile (
        "rdtsc                  \n\t"
        "mov    %%eax, %[t0]    \n\t"

        /* w0:w1 = aL * bL */
        "mov    %[aL], %%eax    \n\t"
        "mul    %[bL]           \n\t"   /* edx:eax = aL*bL */
        "mov    %%eax, %[w0]    \n\t"
        "mov    %%edx, %[w1]    \n\t"

        /* w2:w3 = aH * bH */
        "mov    %[aH], %%eax    \n\t"
        "mul    %[bH]           \n\t"
        "mov    %%eax, %[w2]    \n\t"
        "mov    %%edx, %[w3]    \n\t"

        /* cross = aL * bH, add into w1:w2:w3 */
        "mov    %[aL], %%eax    \n\t"
        "mul    %[bH]           \n\t"
        "add    %%eax, %[w1]    \n\t"
        "adc    %%edx, %[w2]    \n\t"
        "adc    $0,   %[w3]     \n\t"

        /* cross = aH * bL, add into w1:w2:w3 */
        "mov    %[aH], %%eax    \n\t"
        "mul    %[bL]           \n\t"
        "add    %%eax, %[w1]    \n\t"
        "adc    %%edx, %[w2]    \n\t"
        "adc    $0,   %[w3]     \n\t"

        "rdtsc                  \n\t"
        "mov    %%eax, %[t1]    \n\t"
        : [w0]"=&rm"(w0), [w1]"=&rm"(w1), [w2]"=&rm"(w2), [w3]"=&rm"(w3),
          [t0]"=&rm"(t0), [t1]"=&rm"(t1)
        : [aL]"rm"(aL), [aH]"rm"(aH), [bL]"rm"(bL), [bH]"rm"(bH)
        : "eax", "edx", "cc"
    );
    /* ===== END MEASURED REGION ===== */

    format128(w3, w2, w1, w0, out);

    uint32_t cycles = t1 - t0;        /* 32-bit delta is plenty for this */
    double hz = cpu_hz();
    printf("%s\n", out);
    printf("cycles = %u\n", cycles);
    if (hz > 0.0) printf("time   = %.3e s\n", (double)cycles / hz);
    else          printf("time   = (set base clock; CPUID 15h unavailable)\n");
    return 0;

return 0;
}

