    #################################################################
    # SWPOWER.S
    #   contains:
    #     void set_sp_reg(int *sp);   -- overwrite stack pointer
    #     int *get_sp_reg();          -- return stack pointer
    #     void use_stack(int *sp);    -- switch to another stack
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
            .file   "swpower.s"
    #Static data entry in T(able)O(f)C(ontents)

            .toc
            .globl  set_sp_reg[ds]
            .globl  use_stack[ds]
            .globl  get_sp_reg[ds]

            .csect  set_sp_reg[ds]
            .long   .set_sp_reg[PR]
        
            .csect  use_stack[ds]
            .long   .use_stack[PR]
       
            .csect  get_sp_reg[ds]
            .long   .get_sp_reg[PR]
 
            .long   TOC[tc0]
            .long   0


    #Function entry in T(able)O(f)C(ontents)
            .toc
    T.set_sp_reg:  .tc     .set_sp_reg[tc],set_sp_reg[ds]

    #Routine: set_sp_reg
    #   Moves GPR3 to GPR1, clears GPR1's lower 4 bits and returns.
            .globl  .set_sp_reg[PR]
            .csect  .set_sp_reg[PR]
                # GPR1 <- GPR3 + 0
            addic 1, 3, 0x0000
                # GPR1 <- GPR1 | 0x000f
            ori   1, 1, 0x000f
                # GPR1 <- GPR1 - 0x000f
            addic 1, 1, -0x000f
                # Return to address held in Link Register.
            brl

    #Function entry in T(able)O(f)C(ontents)
            .toc
    T.use_stack:  .tc     .use_stack[tc],use_stack[ds]

    #Routine: use_stack
    #   Moves GPR3 to GPR1, clears GPR1's lower 4 bits and returns.
            .globl  .use_stack[PR]
            .csect  .use_stack[PR]
                # GPR1 <- GPR3 + 0
            addic 1, 3, 0x0000
                # GPR1 <- GPR1 | 0x000f
            ori   1, 1, 0x000f
                # GPR1 <- GPR1 - 0x000f
            addic 1, 1, -0x000f 
                # Return to address held in Link Register.
            brl

    #Function entry in T(able)O(f)C(ontents)
            .toc
    T.get_sp_reg:  .tc     .get_sp_reg[tc],get_sp_reg[ds]

    #Routine: get_sp_reg
    #   Moves GPR3 to GPR1, clears GPR1's lower 4 bits and returns.
            .globl  .get_sp_reg[PR]
            .csect  .get_sp_reg[PR]
                # GPR3 <- GPR1 + 0
            addic 3, 1, 0x0000
                # Return to address held in Link Register.
            brl

