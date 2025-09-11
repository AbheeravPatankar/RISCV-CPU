\m4_TLV_version 1d: tl-x.org
\SV
   // This code can be found in: https://github.com/stevehoover/LF-Building-a-RISC-V-CPU-Core/risc-v_shell.tlv
   
   m4_include_lib(['https://raw.githubusercontent.com/stevehoover/LF-Building-a-RISC-V-CPU-Core/main/lib/risc-v_shell_lib.tlv'])

	m4_test_prog()

\SV
   m4_makerchip_module   // (Expanded in Nav-TLV pane.)
   /* verilator lint_on WIDTH */
\TLV
   //---------------------------------------------PC------------------------------------------------------
   $reset = *reset;
   $pc[31:0] = >>1$next_pc;
   $next_pc[31:0] = $reset ? 0 :
                   $is_branch_taken ? $br_tgt_pc : ($pc + 4);
   
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
   $is_j_type = $instr[6:2] == 5'b11011;

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
   $is_imm_valid  =  $is_u_type | $is_i_type | $is_s_type | $is_b_type | $is_j_type;

   // extract the immediate component 
   $imm[31:0] = $is_i_type ? { {21{$instr[31]}},  $instr[30:20] } : 
               $is_s_type ? { {21{$instr[31]}}, {$instr[30:25]}, {$instr[11:7]} } :
               $is_b_type ? { {20{$instr[31]}}, $instr[7], $instr[30:25], $instr[11:8], 1'b0 } :
               $is_u_type ? { $instr[31:12], 12'b0 } :
               $is_j_type ? { {12{$instr[31]}} , {$instr[19:12]}, {$instr[20]}, {$instr[30:21]}, 1'b0} :
               32'b0; //defimault case 

   // identify the actual instruction
   $is_lui   = ($opcode == 7'b0110111);
   $is_auipc = ($opcode == 7'b0010111);
   $is_jal   = ($opcode == 7'b1101111);
   $is_jalr  = ($funct3 == 3'b000 & $opcode == 7'b1100111);

   $is_beq  = ($funct3 == 3'b000 & $opcode == 7'b1100011);
   $is_bne  = ($funct3 == 3'b001 & $opcode == 7'b1100011);
   $is_blt  = ($funct3 == 3'b100 & $opcode == 7'b1100011);
   $is_bge  = ($funct3 == 3'b101 & $opcode == 7'b1100011);
   $is_bltu = ($funct3 == 3'b110 & $opcode == 7'b1100011);
   $is_bgeu = ($funct3 == 3'b111 & $opcode == 7'b1100011);

   $is_addi  = ($funct3 == 3'b000 & $opcode == 7'b0010011);
   $is_slti  = ($funct3 == 3'b010 & $opcode == 7'b0010011);
   $is_sltiu = ($funct3 == 3'b011 & $opcode == 7'b0010011);
   $is_xori  = ($funct3 == 3'b100 & $opcode == 7'b0010011);
   $is_ori   = ($funct3 == 3'b110 & $opcode == 7'b0010011);
   $is_andi  = ($funct3 == 3'b111 & $opcode == 7'b0010011);

   $is_slli  = ($funct3 == 3'b001 & $opcode == 7'b0010011 & $instr[30] == 0);
   $is_srli  = ($funct3 == 3'b101 & $opcode == 7'b0010011 & $instr[30] == 0);
   $is_srai  = ($funct3 == 3'b101 & $opcode == 7'b0010011 & $instr[30] == 1);

   $is_add = ($funct3 == 3'b000 & $opcode == 7'b0110011 & $instr[30] == 0);
   $is_sub = ($funct3 == 3'b000 & $opcode == 7'b0110011 & $instr[30] == 1);
   $is_sll = ($funct3 == 3'b001 & $opcode == 7'b0110011 & $instr[30] == 0);
   $is_slt = ($funct3 == 3'b010 & $opcode == 7'b0110011 & $instr[30] == 0);
   $is_sltu = ($funct3 == 3'b011 & $opcode == 7'b0110011 & $instr[30] == 0);
   $is_xor = ($funct3 == 3'b100 & $opcode == 7'b0110011 & $instr[30]== 0);
   $is_srl = ($funct3 == 3'b101 & $opcode == 7'b0110011 & $instr[30] == 0);
   $is_sra = ($funct3 == 3'b101 & $opcode == 7'b0110011 & $instr[30] == 1);
   $is_or  = ($funct3 == 3'b110 & $opcode == 7'b0110011 & $instr[30]== 0);
   $is_and = ($funct3 == 3'b111 & $opcode == 7'b0110011 & $instr[30] == 0);

   $is_lb  = ($funct3 == 3'b000 & $opcode == 7'b0000011);
   $is_lh  = ($funct3 == 3'b001 & $opcode == 7'b0000011);
   $is_lw  = ($funct3 == 3'b010 & $opcode == 7'b0000011);
   $is_lbu = ($funct3 == 3'b100 & $opcode == 7'b0000011);
   $is_lhu = ($funct3 == 3'b101 & $opcode == 7'b0000011);

   $is_sb  = ($funct3 == 3'b000 & $opcode == 7'b0100011);
   $is_sh  = ($funct3 == 3'b001 & $opcode == 7'b0100011);
   $is_sw  = ($funct3 == 3'b010 & $opcode == 7'b0100011);


   // Assert these to end simulation (before Makerchip cycle limit).
   *passed = 1'b0;
   *failed = *cyc_cnt > M4_MAX_CYC;

   //--------------------------------------------------ALU--------------------------------------------------
   $sext_src1[63:0] = {{32{$src_value1[31]}}, $src_value1}; 
   $sltu_rslt = {31'b0 , $src_value1 < $src_value2};
   $sltiu_rslt = {31'b0 , $src_value1 < $imm};
   $sra_rslt[63:0] = $sext_src1 >> $src_value2[4:0];
   $srai_rslt[63:0] = $sext_src1 >> $imm[4:0];

   $result[31:0] = $is_add ? $src_value1 + $src_value2 :
                  $is_addi ? $src_value1 + $imm        :
                  $is_andi ? $src_value1 & $imm        :
                  $is_ori ? $src_value1 | $imm         :
                  $is_xori ? $src_value1 ^ $imm        :
                  $is_slli ? $src_value1 << $imm[4:0]  :
                  $is_srli ? $src_value1 >> $imm[4:0]  :
                  $is_and ? $src_value1 & $src_value2  :
                  $is_or ? $src_value1 | $src_value2   :
                  $is_xor ? $src_value1 ^ $src_value2  :
                  $is_sub ? $src_value1 - $src_value2  :
                  $is_sll ? $src_value1 << $src_value2 :
                  $is_srl ? $src_value1 >> $src_value2 :
                  $is_sltu ? $sltu_rslt                :
                  $is_sltiu ? $sltiu_rslt              :
                  $is_lui ? {$imm[31:12], 12'b0}       :
                  $is_auipc ? $imm + $pc               :
                  $is_jal ? $pc + 32'd4                :
                  $is_jalr ? $pc + 32'd4               :
                  $is_slt ? (($src_value1[31] == $src_value2[31]) ? $sltu_rslt : {31'b0, $src_value1[31]}) :
                  $is_slti ? (($src_value1[31] == $imm[31]) ? $sltiu_rslt : {31'b0, $src_value1[31]}) :
                  $is_sra ? $sra_rslt : 
                  $is_srai ? $srai_rslt :
                  32'b0; // default
   
   //----------------------------------------------Branch Unit--------------------------------------------
   $br_tgt_pc[31:0] = $pc + $imm;

   $beq =  $src_value1 == $src_value2 ? 1 : 0;
   $bne =  $src_value1 != $src_value2 ? 1 : 0;
   $blt =  ($src_value1 < $src_value2) ^ ($src_value1[31] != $src_value2[31]) ? 1 : 0;
   $bge =  ($src_value1 >= $src_value2) ^ ($src_value1[31] != $src_value2[31]) ? 1 : 0;
   $bltu = ($src_value1 < $src_value2) ? 1 : 0;
   $bgeu = ($src_value1 >= $src_value2) ? 1 : 0;

   $is_branch_taken = $is_beq & $beq ? 1 :
                     $is_bne & $bne ? 1  :
                     $is_blt & $blt ? 1 :
                     $is_bge & $bge ? 1 :
                     $is_bltu & $bltu ? 1 :
                     $is_bgeu & $bgeu ? 1:
                     0; //default


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
