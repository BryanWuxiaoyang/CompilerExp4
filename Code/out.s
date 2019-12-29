.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra

write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra

fact:
sw $fp, 0($sp)
addi $sp, $sp, -4
addi $fp, $sp, 4
addi $sp, $sp, -40
addi $sp, $sp, -4
li $t0, 0
sw $t0, -44($fp)
addi $sp, $sp, -4
li $t0, 0
sw $t0, -48($fp)
L1:
lw $t0, -44($fp)
lw $t1, 8($fp)
bge $t0, $t1, L3
addi $sp, $sp, -4
lw $t1, -44($fp)
li $t2, 4
mul $t0, $t1, $t2
sw $t0, -52($fp)
addi $sp, $sp, -4
addi $t1, $fp, 40
lw $t2, -52($fp)
add $t0, $t1, $t2
sw $t0, -56($fp)
lw $t0, -56($fp)
lw $t1, -44($fp)
sw $t1, 0($t0)
addi $sp, $sp, -4
lw $t1, -44($fp)
addi $t0, $t1, 1
sw $t0, -60($fp)
lw $t1, -60($fp)
move $t0, $t1
sw $t0, -44($fp)
j L1
L3:
li $t0, 0
sw $t0, -44($fp)
L4:
lw $t0, -44($fp)
lw $t1, 8($fp)
bge $t0, $t1, L6
addi $sp, $sp, -4
lw $t1, -44($fp)
li $t2, 4
mul $t0, $t1, $t2
sw $t0, -64($fp)
addi $sp, $sp, -4
addi $t1, $fp, 40
lw $t2, -64($fp)
add $t0, $t1, $t2
sw $t0, -68($fp)
addi $sp, $sp, -4
lw $t1, -48($fp)
lw $t4, -68($fp)
lw $t2, 0($t4)
add $t0, $t1, $t2
sw $t0, -72($fp)
lw $t1, -72($fp)
move $t0, $t1
sw $t0, -48($fp)
addi $sp, $sp, -4
lw $t1, -44($fp)
addi $t0, $t1, 1
sw $t0, -76($fp)
lw $t1, -76($fp)
move $t0, $t1
sw $t0, -44($fp)
j L4
L6:
lw $t0, -48($fp)
move $v0, $t0
move $sp, $fp
lw $fp, 0($fp)
jr $ra

main:
sw $fp, 0($sp)
addi $sp, $sp, -4
addi $fp, $sp, 4
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
jal read
lw $ra, 4($sp)
move $t0, $v0
sw $t0, -4($fp)
addi $sp, $sp, -4
lw $t1, -4($fp)
move $t0, $t1
sw $t0, -12($fp)
li $t0, 5
sw $t0, 0($sp)
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
jal fact
lw $ra, 4($sp)
addi $sp, $sp, -4
move $t0, $v0
sw $t0, -24($fp)
addi $sp, $sp, -4
lw $t1, -24($fp)
move $t0, $t1
sw $t0, -28($fp)
lw $t0, -28($fp)
move $a0, $t0
sw $ra, 0($sp)
addi $sp, $sp, -4
jal write
lw $ra, 4($sp)
li $t0, 0
move $v0, $t0
move $sp, $fp
lw $fp, 0($fp)
jr $ra
