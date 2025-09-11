\m4_TLV_version 1d: tl-x.org
\SV
   // This code can be found in: https://github.com/stevehoover/LF-Building-a-RISC-V-CPU-Core/risc-v_shell.tlv
   
   m4_include_lib(['https://raw.githubusercontent.com/stevehoover/LF-Building-a-RISC-V-CPU-Core/main/lib/risc-v_shell_lib.tlv'])



   //---------------------------------------------------------------------------------
   // /====================\
   // | Sum 1 to 9 Program |
   // \====================/
   //
   // Program to test RV32I
   // Add 1,2,3,...,9 (in that order).
   //
   // Regs:
   //  x12 (a2): 10
   //  x13 (a3): 1..10
   //  x14 (a4): Sum
   // 
   m4_asm(ADDI, x14, x0, 0)             // Initialize sum register a4 with 0
   m4_asm(ADDI, x12, x0, 1010)          // Store count of 10 in register a2.
   m4_asm(ADDI, x13, x0, 1)             // Initialize loop count register a3 with 0
   // Loop:
   m4_asm(ADD, x14, x13, x14)           // Incremental summation
   m4_asm(ADDI, x13, x13, 1)            // Increment loop count by 1
   m4_asm(BLT, x13, x12, 1111111111000) // If a3 is less than a2, branch to label named <loop>
   // Test result value in x14, and set x31 to reflect pass/fail.
   m4_asm(ADDI, x30, x14, 111111010100) // Subtract expected value of 44 to set x30 to 1 if and only iff the result is 45 (1 + 2 + ... + 9).
   m4_asm(BGE, x0, x0, 0) // Done. Jump to itself (infinite loop). (Up to 20-bit signed immediate plus implicit 0 bit (unlike JALR) provides byte address; last immediate bit should also be 0)
   m4_asm_end()
   m4_define(['M4_MAX_CYC'], 50)
   //---------------------------------------------------------------------------------



\SV
   m4_makerchip_module   // (Expanded in Nav-TLV pane.)
   /* verilator lint_on WIDTH */
\TLV
   //---------------------------------------------PC------------------------------------------------------
   $reset = *reset;
   $pc[31:0] = >>1$next_pc;
   $next_pc[31:0] = $reset ? 0 : ($pc + 4);
   
   //--------------------------------------------IMEM-----------------------------------------------------
   $addr[31:0] = $pc; 
   `READONLY_MEM($addr, $$instr[31:0])
   

   $instr[31:0] = $$instr[31:0];
   //-------------------------------------------Decoder---------------------------------------------------
   // decode the instruction type
   $is_u_type = $instr[6:2] == 5'b00101 | $instr[6:2] == 5'b01101;
   $is_r_type = $instr[6:2] == 5'b01100 | $instr[6:2] == 5'b01011 | $instr[6:2] == 5'b01110 | $instr[6:2] == 5'b10100;
   $is_i_type = $instr[6:2] == 5'b00000 | $instr[6:2] == 5'b00001 | $instr[6:2] == 5'b00100 | $instr[6:2] == 5'b00110 | $instr[6:2] == 5'b11001;
   $is_s_type = $instr[6:2] == 5'b01000 | $instr[6:2] == 5'b01001;
   $is_b_type = $instr[6:2] == 5'b11000;
   $is_j_type = $instr[6:2] == 5'b11001;

   // instruction component extraction 
   $opcode[6:0] = $instr[6:0];
   $rd[4:0]     = $instr[11:7];
   $funct3[2:0] = $instr[14:12];
   $rs1[4:0]    = $instr[19:15];
   $rs2[4:0]    = $instr[24:20];

   // check component validity
   $is_rd_valid    = $is_r_type | $is_i_type | $is_u_type | $is_j_type;
   $is_funct3_valid = $is_r_type | $is_i_type | $is_s_type | $is_b_type;
   $is_rs1_valid   = $is_r_type | $is_i_type | $is_s_type | $is_b_type;
   $is_rs2_valid   = $is_r_type | $is_s_type | $is_b_type;

   // extract the immediate component 
   $imm[31:0] = $is_i_type ? { {21{$instr[31]}},  $instr[30:20] } : 
               $is_s_type ? { {21{$instr[31]}}, {$instr[30:25]}, {$instr[11:7]} } :
               $is_b_type ? { {20{$instr[31]}}, $instr[7], $instr[30:25], $instr[11:8], 1'b0 } :
               $is_u_type ? { {$instr[31:12]}, {21'b0}}:
               $is_j_type ? { {12{$instr[31]}} , {$instr[19:12]}, {$instr[20]}, {$instr[30:21]}, 1'b0} :
               32'b0; //defimault case 

   // identify the actual instruction
   $is_beq = ($funct3 == 3'b000 & $opcode == 7'b1100011);
   $is_bne = ($funct3 == 3'b001 & $opcode == 7'b1100011);
   $is_blt = ($funct3 == 3'b100 & $opcode == 7'b1100011);
   $is_bge = ($funct3 == 3'b101 & $opcode == 7'b1100011);
   $is_bltu = ($funct3 == 3'b110 & $opcode == 7'b1100011);
   $is_bgeu = ($funct3 == 3'b111 & $opcode == 7'b1100011);

   $is_addi = ($funct3 == 3'b000 & $opcode == 7'b0010011);
   $is_add = ($funct3 == 3'b000 & $opcode == 7'b0110011 & $instr[30] == 1'b0 );
   

   // Assert these to end simulation (before Makerchip cycle limit).
   *passed = 1'b0;
   *failed = *cyc_cnt > M4_MAX_CYC;

   //--------------------------------------------------ALU--------------------------------------------------
   $result[31:0] = $is_add ? $src_value1 + $src_value2 :
                  $is_addi ? $src_value1 + $imm        :
                  32'b0; //default;
   
   //----------------------------------------------Branch Unit--------------------------------------------
   $br_tgt_pc = $pc + $imm;

   $beq =  src_value1 == src_value2 ? 1 : 0;
   $bne =  src_value1 != src_value2 ? 1 : 0;
   $blt =  (src_value1 < src_value2) ^ (src_value1[31] != src_value2[31]) ? 1 : 0;
   $bge =  (src_value1 >= src_value2) ^ (src_value1[31] != src_value2[31]) ? 1 : 0;
   $bltu = (src_value1 < src_value2) ? 1 : 0;
   $bgeu = (src_value1 >= src_value2) ? 1 : 0;

   $is_branch_taken = $is_beq & $beq ? 1 :
                     $is_bne & $bne ? 1  :
                     $is_blt & blt ? 1 :
                     $is_bge & bge ? 1 :
                     $is_bltu & bltu ? 1 :
                     $is_bgeu & bgeu ? 1:
                     0; //default

   $next_pc = $is_branch_taken ? br_tgt_pc : ($pc + 4);

   //------------------------------------------------------Register File------------------------------------------------------
   // check what is valid from the instruction 
   $rd_en1 = $is_rs1_valid;
   $rd_en2 = $is_rs2_valid;
   $wr_en = $is_rd_valid & $rd != 0;

   // assign proper values to the macro 
   m4+rf(32, 32, $reset, $wr_en, $rd, $result, $rd_en1, $rs1, $rd_data1, $rd_en2, $rs2, $rd_data2)
   
   // rename the values from the macro 
   $src_value1[31:0] = $rd_data1;
   $src_value2[31:0] = $rd_data2;
   
   //m4+dmem(32, 32, $reset, $addr[4:0], $wr_en, $wr_data[31:0], $rd_en, $rd_data)
   m4+cpu_viz()
\SV
   endmodule