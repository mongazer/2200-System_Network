        LEA $sp, stackpointer
        LDR $sp, 0($sp)
stackpointer:
        .word 0xFFFFFF