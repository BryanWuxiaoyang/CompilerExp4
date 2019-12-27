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
lw $t0, 8($fp)
li $t1, 1
bne $t0, $t1, L2
lw $t0, 8($fp)
move $v0, $t0
move $sp, $fp
lw $fp, 0($fp)
jr $ra
j L3
L2:
addi $sp, $sp, -4
lw $t1, 8($fp)
addi $t0, $t1, -1
sw $t0, -4($fp)
lw $t0, -4($fp)
sw $t0, -8($fp)
sw $t0, 0($sp)
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
jal fact
lw $ra, 4($sp)
addi $sp, $sp, -4
move $t0, $v0
sw $t0, -16($fp)
addi $sp, $sp, -4
lw $t1, 8($fp)
lw $t2, -16($fp)
mul $t0, $t1, $t2
sw $t0, -20($fp)
lw $t0, -20($fp)
move $v0, $t0
move $sp, $fp
lw $fp, 0($fp)
jr $ra
L3:

main:
sw $fp, 0($sp)
addi $sp, $sp, -4
addi $fp, $sp, 4
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
jal read
move $t0, $v0
sw $t0, -4($fp)
lw $ra, 4($sp)
addi $sp, $sp, -4
lw $t1, -4($fp)
move $t0, $t1
sw $t0, -12($fp)
lw $t0, -12($fp)
li $t1, 1
ble $t0, $t1, L5
lw $t0, -12($fp)
sw $t0, -16($fp)
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
j L6
L5:
li $t0, 1
sw $t0, -28($fp)
L6:
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
