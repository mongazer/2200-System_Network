ADDI $a2,$a2, 1
ADDI $a1, $a2, 3
beq $a1, $a2, gg
bne $a1, $a2, gg2
ADDI $a0, $a0,1
halt
gg: ADDI $a0, $a0,3
halt
gg2: ADDI $a0, $a0,4
halt