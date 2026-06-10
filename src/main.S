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

    .lcomm num1_hi, 4
    .lcomm num1_lo, 4
    .lcomm num2_hi, 4
    .lcomm num2_lo, 4
.section .text
parse_32bit_str:
    xor eax, eax
    mov ecx, 32
parse_loop:
    shl eax,1
    movzx edx, byte ptr [esi]
    sub edx, '0'
    or eax, edx
    inc esi
    dec ecx
    jnz parse_loop
    ret
_start:
    lea esi, [num1_str]
    call parse_32bit_str
    mov [num1_lo],eax

    call parse_32bit_str
    mov [num1_lo],eax

    lea esi,[num2_str]
    call parse_32bit_str
    mov [num2_lo],eax

    call parse_32bit_str
    mov [num2_lo],eax
    
    mov eax, 1
    xor ebx, ebx
    int 0x80