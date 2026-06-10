.intel_syntax noprefix
.global _start

.section .data
    num1_str: .ascii "0000000000000000000000000000000000000000000000000000000000000010"
    num2_str: .ascii "0000000000000000000000000000000000000000000000000000000000000011"

.section .bss
    .lcomm result_str, 128

    .lcomm time_start_low, 4
    .lcomm time_start_high, 4
    .lcomm time_end_low, 4
    .lcomm time_end_high, 4

.section .text
_start:
    mov eax, 1
    xor ebx, ebx
    int 0x80