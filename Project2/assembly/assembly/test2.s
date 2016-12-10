! misctest.s
!
! A few assorted test cases you may find helpful
!
! We assume you've already made a few of your own
! If your BR instruction doesn't work correctly, these won't help much
!

        ADD $zero, $zero, $zero             ! do nothing, start at test1
test1:  LEA $t0, compare
		JALR $t0, $ra
		ADDI $s1,$s1,1

success:HALT
		HALT
		HALT

compare: ADDI $s1,$s1,11
		 RET

fail:
        ADDI $t0, $zero, -1                 ! set $t registers to signal error
        ADDI $t1, $zero, -1
        ADDI $t2, $zero, -1
        HALT