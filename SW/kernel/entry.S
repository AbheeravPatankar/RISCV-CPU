.section .text
.global _entry
_entry:
    li  sp, 4096      # load sp with 4100
    la  ra, exit
    j   main       # call main function 

exit:
    j exit
