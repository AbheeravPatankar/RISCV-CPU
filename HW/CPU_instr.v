\m4_TLV_version 1d: tl-x.org
\SV
   // This code can be found in: https://github.com/stevehoover/LF-Building-a-RISC-V-CPU-Core/risc-v_shell.tlv
   
   m4_include_lib(['https://raw.githubusercontent.com/stevehoover/LF-Building-a-RISC-V-CPU-Core/main/lib/risc-v_shell_lib.tlv'])

`define READONLY_MEM(ADDR, DATA) \
  logic [31:0] instrs [0:63]; \
  initial begin \
    instrs[0] = 32'b00000000000000000001000100110111; \
    instrs[1] = 32'b00000000000000000000000010010111; \
    instrs[2] = 32'b00000000110000001000000010010011; \
    instrs[3] = 32'b00001011010000000000000001101111; \
    instrs[4] = 32'b00000000000000000000000001101111; \
    instrs[5] = 32'b11111110000000010000000100010011; \
    instrs[6] = 32'b00000000100000010010111000100011; \
    instrs[7] = 32'b00000010000000010000010000010011; \
    instrs[8] = 32'b11111110101001000010011000100011; \
    instrs[9] = 32'b11111110101101000010010000100011; \
    instrs[10] = 32'b11111110100001000010011110000011; \
    instrs[11] = 32'b11111110110001000010011100000011; \
    instrs[12] = 32'b00000000111001111010000000100011; \
    instrs[13] = 32'b00000000000000000000000000010011; \
    instrs[14] = 32'b00000001110000010010010000000011; \
    instrs[15] = 32'b00000010000000010000000100010011; \
    instrs[16] = 32'b00000000000000001000000001100111; \
    instrs[17] = 32'b11111110000000010000000100010011; \
    instrs[18] = 32'b00000000100000010010111000100011; \
    instrs[19] = 32'b00000010000000010000010000010011; \
    instrs[20] = 32'b01000000000000000010011110000011; \
    instrs[21] = 32'b11111110111101000010011000100011; \
    instrs[22] = 32'b11111110000001000010010000100011; \
    instrs[23] = 32'b00000011000000000000000001101111; \
    instrs[24] = 32'b01000001000000000000011100010011; \
    instrs[25] = 32'b11111110100001000010011110000011; \
    instrs[26] = 32'b00000000001001111001011110010011; \
    instrs[27] = 32'b00000000111101110000011110110011; \
    instrs[28] = 32'b00000000000001111010011110000011; \
    instrs[29] = 32'b11111110110001000010011100000011; \
    instrs[30] = 32'b00000000111101110000011110110011; \
    instrs[31] = 32'b11111110111101000010011000100011; \
    instrs[32] = 32'b11111110100001000010011110000011; \
    instrs[33] = 32'b00000000000101111000011110010011; \
    instrs[34] = 32'b11111110111101000010010000100011; \
    instrs[35] = 32'b11111110100001000010011100000011; \
    instrs[36] = 32'b00000000010000000000011110010011; \
    instrs[37] = 32'b11111100111001111101011011100011; \
    instrs[38] = 32'b11111110110001000010011100000011; \
    instrs[39] = 32'b00001010000000000000011110010011; \
    instrs[40] = 32'b00000000111101110001011001100011; \
    instrs[41] = 32'b00000000000100000000011110010011; \
    instrs[42] = 32'b00000000100000000000000001101111; \
    instrs[43] = 32'b00000000000000000000011110010011; \
    instrs[44] = 32'b00000000000001111000010100010011; \
    instrs[45] = 32'b00000001110000010010010000000011; \
    instrs[46] = 32'b00000010000000010000000100010011; \
    instrs[47] = 32'b00000000000000001000000001100111; \
    instrs[48] = 32'b11111110000000010000000100010011; \
    instrs[49] = 32'b00000000000100010010111000100011; \
    instrs[50] = 32'b00000000100000010010110000100011; \
    instrs[51] = 32'b00000010000000010000010000010011; \
    instrs[52] = 32'b11110111010111111111000011101111; \
    instrs[53] = 32'b11111110101001000010011000100011; \
    instrs[54] = 32'b00000000000000000001011110110111; \
    instrs[55] = 32'b11000000000001111000010110010011; \
    instrs[56] = 32'b11111110110001000010010100000011; \
    instrs[57] = 32'b11110011000111111111000011101111; \
    instrs[58] = 32'b00000000000000000000011110010011; \
    instrs[59] = 32'b00000000000001111000010100010011; \
    instrs[60] = 32'b00000001110000010010000010000011; \
    instrs[61] = 32'b00000001100000010010010000000011; \
    instrs[62] = 32'b00000010000000010000000100010011; \
    instrs[63] = 32'b00000000000000001000000001100111; \
  end \
  assign DATA = instrs[ADDR >> 2];
\SV
   m4_makerchip_module   // (Expanded in Nav-TLV pane.)
   /* verilator lint_on WIDTH */
\TLV
   //---------------------------------------------PC------------------------------------------------------
   $reset = *reset;
   $pc[31:0] = >>1$next_pc;
   $next_pc[31:0] = $reset ? 0 :
                   $is_branch_taken ? $br_tgt_pc :
                   $is_jalr_taken ? $jalr_tgt_pc :
                   $pc + 4; // default case 
   // Select IMEM or DMEM and virtual to physical address mapping
   $is_imem = $pc < 1024 ? 1 : 0;
   $imem_addr[31:0] = $pc;
   $dmem_addr[31:0] = $pc - 1024;
   $aaaaaa[31:0] = $pc;
   //--------------------------------------------IMEM-----------------------------------------------------
   `READONLY_MEM($imem_addr, $$imem_instr[31:0])
   
   //----------------------------------------Fetch INSTR------------------------------------------------
   $dmem_word[31:0] = $dmem_addr >> 2 ;
   $dmem_instr[31:0] = /dmem[$dmem_word]$value;
   $instr[31:0] = $is_imem ? $$imem_instr[31:0] : $dmem_instr;
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

   $is_load = $is_lb | $is_lh | $is_lw | $is_lbu | $is_lhu ;
   $is_store = $is_sb | $is_sh | $is_sw ;
   // Assert these to end simulation (before Makerchip cycle limit).
   *passed = 1'b0;
   *failed = *cyc_cnt > 1000;

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
                  $is_load ? ($src_value1 + $imm):
                  $is_store ? ($src_value1 + $imm): 
                  32'b0; // default
   
   //----------------------------------------------Branch Unit--------------------------------------------
   $br_tgt_pc[31:0] = $pc + $imm;
   $jalr_tgt_pc[31:0] = $src_value1 + $imm;

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
                     $is_jal ? 1 :
                     0; //default
   $is_jalr_taken = $is_jalr;

   //------------------------------------------------------Register File------------------------------------------------------
   // check what is valid from the instruction 
   $rd_en1 = $is_rs1_valid;
   $rd_en2 = $is_rs2_valid;
   $wr_en = $is_rd_valid & $rd != 0;

   // assign proper values to the macro 
   m4+rf(32, 32, $reset, $wr_en, $rd, $wr_data_rf, $rd_en1, $rs1, $rd_data1, $rd_en2, $rs2, $rd_data2)
   
   $wr_data_rf[31:0] = $is_load ? $ld_data : $result;
   // rename the values from the macro 
   $src_value1[31:0] = $rd_data1;
   $src_value2[31:0] = $rd_data2;
   
   //--------------------------------------------------------------DMEM-----------------------------------------------------
   
   $wr_en_dmem = $is_store ? 1 : 0;
   $rd_en_dmem = $is_load | $is_store ? 1 : 0;
   //virtual to physicial address mapping 
   $dmem_data_addr[31:0] = $result - 1024;
   // handle the bit shifting logic 
   $word_index[29:0] = $dmem_data_addr[31:2];
   $byte_index[1:0] = $dmem_data_addr[1:0];
   $filter[31:0] = ~( 255 << $byte_index * 8 );
   $rd_data_pre_process[31:0] = $byte_index != 0 ? ( $filter & $rd_data_dmem ) : 0 & ( $rd_data_dmem ) ;
   $write_value[31:0] = ($src_value2 << $byte_index * 8) | $rd_data_pre_process; 
   
   // Allow expressions for most inputs, so define input signals.
   $dmem1_wr_en = $wr_en_dmem;
   $dmem1_addr[\$clog2(1024)-1:0] = $word_index;
   $dmem1_wr_data[32-1:0] = $write_value;
   
   $dmem1_rd_en = $rd_en_dmem;
   /dmem[1023:0]
      $wr = /top$dmem1_wr_en && (/top$dmem1_addr == #dmem);
      <<1$value[32-1:0] =  $wr  ? /top$dmem1_wr_data :
                                   $RETAIN;
   
   $rd_data_dmem[32-1:0] = $dmem1_rd_en ? /dmem[$dmem1_addr]$value : 'X;
   /dmem[1023:0]
   $ld_data[31:0] = $byte_index > 0 ? ($rd_data_dmem >> $byte_index * 8) & 255 : $rd_data_dmem ;   
\SV_plus  
initial begin
    /dmem[0]$value = 32'h0000000A;
    /dmem[1]$value = 32'h00000067;
    /dmem[2]$value = 32'h00000016;
    /dmem[3]$value = 32'h00000066;
    /dmem[4]$value = 32'h0000000A;
    /dmem[5]$value = 32'h00000014;
    /dmem[6]$value = 32'h0000001E;
    /dmem[7]$value = 32'h00000028;
    /dmem[8]$value = 32'h00000032;
end

\SV_plus

// Call preload_dmem on reset
   

                   
   //------------------------------------------------------------------------------------------------------------------------
\SV
   endmodule
   