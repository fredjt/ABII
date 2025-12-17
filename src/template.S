.section .text
.global @SYMBOL@

.type @SYMBOL@, @function

@SYMBOL@:
    jmp abii_@SYMBOL@

.section .note.GNU-stack, "", @progbits
