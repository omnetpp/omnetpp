    #################################################################
    # STJPOWER.S
    #
    #    setjmp()/longjmp() functions
    #
    #   Contents:
    #     int my_setjmp(my_jmp_buf buf);          
    #     void my_longjmp(my_jmp_buf buf, int v); 
    #
    #   Power/PowerPC processor (IBM RISC/6000, AIX, XL C compiler)
    #################################################################

    #
    # Register usage conventions:
    #  GPR1: stack pointer, must be multiple of 16
    #  GPR2: TOC
    #  GPR3: 1st word of function arg list / 1st word of function return
    #  GPR4: 2nd word of function arg list / 2nd word of function return
    #
            .file   "stjpower.s"
    #Static data entry in T(able)O(f)C(ontents)

            .toc
            .globl  my_setjmp[ds]
            .globl  my_longjmp[ds]

            .csect  my_setjmp[ds]
            .long   .my_setjmp[PR]
        
            .csect  my_longjmp[ds]
            .long   .my_longjmp[PR]
       
            .long   TOC[tc0]
            .long   0


    #Function entry in T(able)O(f)C(ontents)
            .toc
    T.my_setjmp:  .tc     .my_setjmp[tc], my_setjmp[ds]

    #Routine: my_setjmp
            .globl  .my_setjmp[PR]
            .csect  .my_setjmp[PR]

            # store general purpose registers
            stw 1,  0(3)
            stw 2,  4(3)
            stw 13, 8(3)
            stw 14, 12(3)
            stw 15, 16(3)
            stw 16, 20(3)
            stw 17, 24(3)
            stw 18, 28(3)
            stw 19, 32(3)
            stw 20, 36(3)
            stw 21, 40(3)
            stw 22, 44(3)
            stw 23, 48(3)
            stw 24, 52(3)
            stw 25, 56(3)
            stw 26, 60(3)
            stw 27, 64(3)
            stw 28, 68(3)
            stw 29, 72(3)
            stw 30, 76(3)
            stw 31, 80(3)

            # store floating point registers
            stfd 14, 84(3)
            stfd 15, 92(3)
            stfd 16, 100(3)
            stfd 17, 108(3)
            stfd 18, 116(3)
            stfd 19, 124(3)
            stfd 20, 132(3)
            stfd 21, 140(3)
            stfd 22, 148(3)
            stfd 23, 156(3)
            stfd 24, 164(3)
            stfd 25, 172(3)
            stfd 26, 180(3)
            stfd 27, 188(3)
            stfd 28, 196(3)
            stfd 29, 204(3)
            stfd 30, 212(3)
            stfd 31, 220(3)

            # store link register
            mflr 0
            stw  0, 228(3) 

            # store condition register
            mfcr 0
            stw  0, 232(3)

            # set 0 return value
            andil. 3,3,0
            
            # return
            blr  

    #Function entry in T(able)O(f)C(ontents)
            .toc
    T.my_longjmp:  .tc     .my_longjmp[tc], my_longjmp[ds]

    #Routine: my_longjmp
            .globl  .my_longjmp[PR]
            .csect  .my_longjmp[PR]

            # restore general purpose registers
            lwz 1,  0(3)
            lwz 2,  4(3)
            lwz 13, 8(3)
            lwz 14, 12(3)
            lwz 15, 16(3)
            lwz 16, 20(3)
            lwz 17, 24(3)
            lwz 18, 28(3)
            lwz 19, 32(3)
            lwz 20, 36(3)
            lwz 21, 40(3)
            lwz 22, 44(3)
            lwz 23, 48(3)
            lwz 24, 52(3)
            lwz 25, 56(3)
            lwz 26, 60(3)
            lwz 27, 64(3)
            lwz 28, 68(3)
            lwz 29, 72(3)
            lwz 30, 76(3)
            lwz 31, 80(3)

            # restore floating point registers
            lfd 14, 84(3)
            lfd 15, 92(3)
            lfd 16, 100(3)
            lfd 17, 108(3)
            lfd 18, 116(3)
            lfd 19, 124(3)
            lfd 20, 132(3)
            lfd 21, 140(3)
            lfd 22, 148(3)
            lfd 23, 156(3)
            lfd 24, 164(3)
            lfd 25, 172(3)
            lfd 26, 180(3)
            lfd 27, 188(3)
            lfd 28, 196(3)
            lfd 29, 204(3)
            lfd 30, 212(3)
            lfd 31, 220(3)

            # restore link register
            lwz  0, 228(3) 
            mtlr 0 

            # restore condition register
            lwz  0, 232(3)
            mtcrf 0xFF, 0
            
            # move 2nd argument to return value
            addic 3,4,0000

            # return
            blr  

