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
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));

    if (eax >= 0x16) {
        __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x16));

        uint32_t freq_MHz = eax & 0xFFFF;
        if (freq_MHz > 0) {
            return (double)freq_MHz * 1000000.0;
        }
    }

    return 2600000000.0;
}

int main(void) {
    char sa[80], sb[80], out[129];
    if (scanf("%79s %79s", sa, sb) != 2) 
        return 1;

    uint32_t aH, aL, bH, bL;
    parse64(sa, &aH, &aL);
    parse64(sb, &bH, &bL);

    uint32_t w0, w1, w2, w3; /* 128-bit result, little-endian limbs */
    uint32_t t0, t1; /* RDTSC samples */

/* ===== MEASURED REGION: nothing but RDTSC + multiply ===== */
__asm__ volatile (
            "rdtsc\n\t"
            "movl %%eax, %[t0]\n\t"

            "movl %[aL], %%eax\n\t"
            "mull %[bL]\n\t"
            "movl %%eax, %[w0]\n\t"
            "movl %%edx, %%ecx\n\t"

            "movl %[aH], %%eax\n\t"
            "mull %[bH]\n\t"
            "movl %%eax, %%ebx\n\t"
            "movl %%edx, %[w3]\n\t"

            "movl %[aL], %%eax\n\t"
            "mull %[bH]\n\t"
            "addl %%eax, %%ecx\n\t"
            "adcl %%edx, %%ebx\n\t"
            "adcl $0, %[w3]\n\t"

            "movl %[aH], %%eax\n\t"
            "mull %[bL]\n\t"
            "addl %%eax, %%ecx\n\t"
            "adcl %%edx, %%ebx\n\t"
            "adcl $0, %[w3]\n\t"

            "movl %%ecx, %[w1]\n\t"
            "movl %%ebx, %[w2]\n\t"

            "rdtsc\n\t"
            "movl %%eax, %[t1]\n\t"

            : [w0]"=m"(w0), [w1]"=m"(w1), [w2]"=m"(w2), [w3]"=m"(w3),
              [t0]"=r"(t0), [t1]"=r"(t1)
            : [aL]"m"(aL), [aH]"m"(aH), [bL]"m"(bL), [bH]"m"(bH)
            : "eax", "edx", "ecx", "ebx", "cc"
    );
    /* ===== END MEASURED REGION ===== */

    format128(w3, w2, w1, w0, out);

    uint32_t cycles = t1 - t0;        /* 32-bit delta is plenty for this */
    double hz = cpu_hz();
    printf("%s\n", out);
    printf("cycles = %u\n", cycles);
    if (hz > 0.0) 
        printf("time   = %.9f s\n", (double)cycles / hz);    
    else          
        printf("time   = (set base clock; CPUID 15h unavailable)\n");
    return 0;

return 0;
}

