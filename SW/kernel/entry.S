.section .text
.global _entry
_entry:
    addi sp,x0,0    #load sp with 0
    la   ra, exit
    j    main       # call main function 

exit:
    j exit
