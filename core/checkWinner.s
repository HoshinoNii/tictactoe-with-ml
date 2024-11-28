.section .data
.align 2                      // Ensure 4-byte alignment
win_patterns:
    .word 0, 1, 2             // Row 1
    .word 3, 4, 5             // Row 2
    .word 6, 7, 8             // Row 3
    .word 0, 3, 6             // Column 1
    .word 1, 4, 7             // Column 2
    .word 2, 5, 8             // Column 3
    .word 0, 4, 8             // Diagonal 1
    .word 2, 4, 6             // Diagonal 2

.section .text
.global check_winner

check_winner:
    stp x29, x30, [sp, -16]!  // Save frame pointer and return address
    mov x29, sp

    // Initialize loop counter for win_patterns
    mov x1, 0
loop_patterns:
    cmp x1, 8                 // Check if all win patterns are checked
    b.ge check_tie

    ldr x2, =win_patterns     // Load base address of win_patterns
    mov x4, #12               // Mov 12 into temp register
    mul x3, x1, x4            // Offset = i × 12 bytes
    add x2, x2, x3            // Address of win_patterns[i]
    ldr w3, [x2]              // win_patterns[i][0]
    ldr w4, [x2, #4]          // win_patterns[i][1]
    ldr w5, [x2, #8]          // win_patterns[i][2]

    // Load board values for the current win pattern
    uxtw x6, w3
    lsl x6, x6, #2            // Offset = win_patterns[i][0] × 4 bytes
    add x6, x0, x6            // Address of board[win_patterns[i][0]]
    ldr w6, [x6]              // Load board[win_patterns[i][0]]

    uxtw x7, w4
    lsl x7, x7, #2            // Offset = win_patterns[i][1] × 4 bytes
    add x7, x0, x7            // Address of board[win_patterns[i][1]]
    ldr w7, [x7]              // Load board[win_patterns[i][1]]

    uxtw x8, w5
    lsl x8, x8, #2            // Offset = win_patterns[i][2] × 4 bytes
    add x8, x0, x8            // Address of board[win_patterns[i][2]]
    ldr w8, [x8]              // Load board[win_patterns[i][2]]

    // Check if the current pattern is a winning pattern
    cmp w6, #0
    b.eq next_pattern
    cmp w6, w7
    b.ne next_pattern
    cmp w7, w8
    b.ne next_pattern

    mov x0, x6                // Winner found
    b end

next_pattern:
    add x1, x1, 1             // Increment index
    b loop_patterns

check_tie:
    mov x1, 0
loop_board:
    cmp x1, 9                 // Check if all board positions are checked
    b.ge tie

    lsl x2, x1, #2            // Offset = index × 4 bytes
    add x2, x0, x2            // Address of board[index]
    ldr w3, [x2]              // Load board[index]
    cmp w3, #0
    b.eq ongoing

     add x1, x1, 1             // Increment index
    b loop_board

tie:
    mov x0, 2                 // Return TIE
    b end

ongoing:
    mov x0, 0                 // Return EMPTY
    b end

end:
    ldp x29, x30, [sp], 16    // Restore frame pointer and return address
    ret
