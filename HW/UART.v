
\m5_TLV_version 1d: tl-x.org
\m5
   // ============================================
   // Welcome, new visitors! Try the "Learn" menu.
   // ============================================

   //use(m5-1.0)   /// uncomment to use M5 macro library.
\SV
   // Macro providing required top-level module definition, random
   // stimulus support, and Verilator config.
   m5_makerchip_module          // (Expanded in Nav-TLV pane.)
\TLV
   // ------------------------------------------------------------
   // Test-bench stimulus
   // ------------------------------------------------------------
   $reset = *reset;
   $aa[5:0] = $reset ? 0 : >>1$aa + 1;
   // -----------------------Receiver Module -----------------------------------
   
   $rx = $aa % 4 == 0 ? 1 : 0;
   $counter[2:0] = $reset ? 0 : >>1$counter + 1;
   $sampling_reg[7:0] = ( >>1$sampling_reg << 1 ) | $rx;

// ----------------------------------------------------------------------------------------------
   $fifo_wr_en         = $counter == 7 ? 1 : 0;
   $fifo_rd_en         = 0;
   $fifo_rd_index[3:0] = $reset ? 0
                       : $fifo_rd_en ? >>1$fifo_rd_index + 1
                                     : >>1$fifo_rd_index;

   $is_fifo_full_prev = ( ( ( >>1$fifo_wr_index + 4'd1 ) % 16) == $fifo_rd_index );
   $is_wr_index_inc   = $fifo_wr_en && !$is_fifo_full_prev;

   $fifo_wr_index[3:0] = $reset ? 0 :
                         $is_wr_index_inc  ? >>1$fifo_wr_index + 1 :
                                     >>1$fifo_wr_index;

   $wr_data[7:0]       = $counter == 7 ? $sampling_reg : 0;
   $is_fifo_empty      = ($fifo_rd_index == $fifo_wr_index)       ? 1 : 0;
   $is_fifo_full = (( $fifo_wr_index + 4'd1 ) % 16 == $fifo_rd_index) ? 1 : 0;  

   // -------------------------------FIFO storage array--------------------------------------------------
   /fifo_tx[15:0]
      // Write strobe for this location
      $fifo_wr = /top$fifo_wr_en && (/top$fifo_wr_index == #fifo_tx);
      // Store data on write, retain otherwise
      <<1$value[7:0] = $fifo_wr && !/top$is_fifo_full
                       ? /top$wr_data
                       : $RETAIN;

   $rd_data[7:0] = /top$fifo_rd_en && !/top$is_fifo_empty
                   ? /fifo_tx[$fifo_rd_index]$value
                   : 'X;

   *passed = *cyc_cnt > 500;
   *failed = 1'b0;
\SV
endmodule