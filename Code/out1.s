.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
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

main:
sw $fp, 0($sp)
addi $sp, $sp, -4
addi $fp, $sp, 4
addi $sp, $sp, -4
li $t0, 55
sw $t0, -4($fp)
li $t0, 1
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

