! misctest.s
!
! A few assorted test cases you may find helpful
!
! We assume you've already made a few of your own
! If your BR instruction doesn't work correctly, these won't help much
!

        ADD $zero, $zero, $zero             ! do nothing, start at test1
test1:
        ! Test that the zero register stays zero
        LEA $k0, test1                      ! test #1 (if you fail, check $k0)
        ADDI $zero, $zero, 5                ! try to add 5 to $zero
        ADDI $s0, $zero, 0                  ! put $zero into $s0 to see if $zero changed
        BRnp fail

        ! Try some shifts
        LEA $at, compare

test2:
        LEA $k0, test2                      ! test #2 (if you fail, check $k0)
        ADDI $s0, $zero, 1                  ! 1 << 1 should == 2
        SHFLL $s0, $s0, 1
        ADD $a0, $s0, $zero                 ! compare and check
        ADDI $a1, $zero, 2
        JALR $at, $ra
        BRnp fail                           ! fail if not equal

test3:
        LEA $k0, test3                      ! test #3 (if you fail, check $k0)
        ADDI $s0, $zero, -4                 ! -4 >>A 1 should == -2
        SHFRA $s0, $s0, 1
        ADD $a0, $s0, $zero                 ! compare and check
        ADDI $a1, $zero, -2
        JALR $at, $ra
        BRnp fail                           ! fail if not equal

test4:
        LEA $k0, test4                      ! test #4 (if you fail, check $k0)
        ADDI $s0, $zero, -4                 ! -4 >>L 1 should == -2
        SHFRL $s0, $s0, 30
        ADD $a0, $s0, $zero                 ! compare and check
        ADDI $a1, $zero, 3
        JALR $at, $ra
        BRnp fail                           ! fail if not equal

success:
        LEA $k0, success                    ! clear error number
        HALT                                ! Yay! we have passed

compare:
        ! Compare $a0 to $a1
        NAND $a1, $a1, $a1                  ! take 2's complement of $a1
        ADDI $a1, $a1, 1                    ! (NOT $a1) + 1
        ADD $v0, $a0, $a1                   ! $v0 = $a0 - $a1
        RET

fail:
        ADDI $t0, $zero, -1                 ! set $t registers to signal error
        ADDI $t1, $zero, -1
        ADDI $t2, $zero, -1
        HALT

