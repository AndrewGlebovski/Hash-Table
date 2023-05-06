global asm_find_node

extern is_equal

section .text

; <- register priority <-
; rdi rsi rdx rcx r8 r9

;----------------------------------------
; Finds node in list by its key
;----------------------------------------
; Enter:    rdi = Node *begin, rsi = okey_t key, rdx = Node **prev
; Exit:     rax = Pointer to node
; Destr:    rax, rdi, rsi, rdx
;----------------------------------------

asm_find_node:
    ; Check if begin is NULL
    test rdi, rdi
    jne .not_null

    xor rax, rax
    ret

.not_null
    ; Save args to prevent them from changing on is_equal calls (r12-r15 should not change)
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi
    mov r13, rsi
    mov r14, rdx

    ; Create prev_
    xor r15, r15

    ; Loop
    jmp .condition

.loop:
    mov r15, r12            ; prev_ = begin

    mov r12, [r12+16]       ; begin = begin -> next

    ; Check if begin is NULL
    test r12, r12
    je .restore             ; We don't need to set prev to prev_, cause nothing was found

.condition:
    ; Call is_equal
    mov rdi, [r12]
    mov rsi, r13
    call is_equal

    test rax, rax           ; Check if is_equal return zero
    je .loop

    ; Set prev to prev_
    test r14, r14
    je .restore             ; Caller doesn't need previous node

    mov [r14], r15          ; *prev = prev_

.restore:
    ; Restore and return
    mov rax, r12

    pop r15
    pop r14
    pop r13
    pop r12

    ret

;----------------------------------------
