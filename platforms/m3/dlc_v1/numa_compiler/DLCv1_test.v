//*******************************************************************************************
//Author:         Yejoong Kim
//Last Modified:  July 01 2015
//Description:    Testbench for PRCv13 - FLSv2S
//Update History: July 01 2015 - First Commit by Yejoong
//******************************************************************************************* 
// Test environment configuration (PRC->9->A->B->PRC)
//  +-- PRCv13 (addr = 0xAAAAA)
//  +-- FLSv2S (addr = 0x00012)
//  +-- Node A (addr = 0x0000A)
//  +-- Node B (addr = 0x0000B)
//  +-- PMUv2  (addr = 0x00017)
`timescale 1ns/1ps
`include "DLCv1_test_def.v"

module DLCv1_test();
    
   // PRCv13 Pads
   reg         PAD_EP_ACTIVE;
   reg         PAD_EP_CLK;
   reg         PAD_EP_DATA;

   wire        PAD_WD_IRQ;

   //Test Harness
   reg         checking;

   wire        MBUS_CLK_DLC2M;  //MBUS_CLK_FLS2A,  MBUS_CLK_A2B,  MBUS_CLK_B2PMU,  MBUS_CLK_PMU2PRC;
   wire        MBUS_DATA_DLC2M; //MBUS_DATA_FLS2A, MBUS_DATA_A2B, MBUS_DATA_B2PMU, MBUS_DATA_PMU2PRC;
 
   wire        MBUS_CLK_M2DLC; 
   wire        MBUS_DATA_M2DLC; 
     
   reg         RESETn;
   reg         clk;
   integer     handle;
   
   reg [167:0] DATA;
   integer     i, j, k;
   integer     task_i, task_j, task_k;
   
    // NODE_M (Master Node)
    reg [31:0]  N_M_TX_ADDR;
    reg [31:0]  N_M_TX_DATA;
    reg         N_M_TX_PEND;
    reg         N_M_TX_REQ;
    reg         N_M_TX_PRIORITY;
    wire        N_M_TX_ACK; 
    wire [31:0] N_M_RX_ADDR; 
    wire [31:0] N_M_RX_DATA; 
    wire        N_M_RX_REQ; 
    reg         N_M_RX_ACK; 
    wire        N_M_RX_BROADCAST;
    wire        N_M_RX_FAIL;
    wire        N_M_RX_PEND; 
    wire        N_M_TX_FAIL; 
    wire        N_M_TX_SUCC; 
    reg         N_M_TX_RESP_ACK;
    wire        N_M_LRC_SLEEP;
    wire        N_M_LRC_CLKENB;
    wire        N_M_LRC_RESET;
    wire        N_M_LRC_ISOLATE;
    reg         N_M_WAKEUP_REQ;
    reg [19:0]  N_M_MBC_THRESHOLD;
    wire        N_M_MBC_IN_FWD;
   
   reg         CLK_CORE;
   reg         CLK_MBC;
   
   /*
   DLCv1 DLCv1_0
     (
      //Inputs
      .PAD_CIN       (MBUS_CLK_PMU2PRC),
      .PAD_DIN       (MBUS_DATA_PMU2PRC),
      .PAD_GOC_VSOL  (PAD_GOC_VSOL),
      .PAD_EP_ACTIVE (PAD_EP_ACTIVE),
      .PAD_EP_CLK    (PAD_EP_CLK),
      .PAD_EP_DATA   (PAD_EP_DATA),
      //Outputs
      .PAD_COUT         (MBUS_CLK_PRE2FLS),
      .PAD_DOUT         (MBUS_DATA_PRE2FLS),
      .PAD_WD_IRQ       (PAD_WD_IRQ)
      );
      */

    //****************************************************
    // MODULE INSTANTIATIONS
    //****************************************************
    mbus_master_wrapper_testbench N_M 
    (
        .CLK_MBC        (CLK_MBC),
        .MBC_CLKENB     (MBC_CLKENB),
        .CLKIN          (MBUS_CLK_DLC2M), 
        .RESETn         (RESETn), 
        .DIN            (MBUS_DATA_DLC2M), 
        .CLKOUT         (MBUS_CLK_M2DLC),
        .DOUT           (MBUS_DATA_M2DLC), 
        .TX_ADDR        (N_M_TX_ADDR), 
        .TX_DATA        (N_M_TX_DATA), 
        .TX_PEND        (N_M_TX_PEND), 
        .TX_REQ         (N_M_TX_REQ), 
        .TX_PRIORITY    (N_M_TX_PRIORITY),
        .TX_ACK         (N_M_TX_ACK), 
        .RX_ADDR        (N_M_RX_ADDR), 
        .RX_DATA        (N_M_RX_DATA), 
        .RX_REQ         (N_M_RX_REQ), 
        .RX_ACK         (N_M_RX_ACK), 
        .RX_BROADCAST   (N_M_RX_BROADCAST),
        .RX_FAIL        (N_M_RX_FAIL),
        .RX_PEND        (N_M_RX_PEND), 
        .TX_FAIL        (N_M_TX_FAIL), 
        .TX_SUCC        (N_M_TX_SUCC), 
        .TX_RESP_ACK    (N_M_TX_RESP_ACK),
        .LRC_SLEEP      (N_M_LRC_SLEEP),
        .LRC_CLKENB     (N_M_LRC_CLKENB),
        .LRC_RESET      (N_M_LRC_RESET),
        .LRC_ISOLATE    (N_M_LRC_ISOLATE),
        .WAKEUP_REQ     (N_M_WAKEUP_REQ),
        .MBC_THRESHOLD  (N_M_MBC_THRESHOLD),
        .MBC_IN_FWD     (N_M_MBC_IN_FWD)
    );
    //****************************************************
    // MBUS_SNOOPER
    //****************************************************
    reg SNOOPER_DISABLE;
    initial SNOOPER_DISABLE = 1'b0;
   `ifdef MBUS_SNOOPER
    mbus_snooper #(.SHORT_PREFIX(`DLC_ADDR))
    mbus_snooper_0 (
        .RESETn(RESETn),
        .DISABLE(SNOOPER_DISABLE),
        .CIN(MBUS_CLK_M2DLC),
        .DIN(MBUS_DATA_M2DLC),
        .COUT(MBUS_CLK_DLC2M),
        .DOUT(MBUS_DATA_DLC2M)
        );
    `endif

    DLCv1 DLCv1_0 
      (
       .resetn(RESETn),
       .clk_core(CLK_CORE),
       //Pads
       .cin(MBUS_CLK_M2DLC),
       .din(MBUS_DATA_M2DLC),
       //.ep_active(PAD_EP_ACTIVE),
       //.ep_clk(PAD_EP_CLK),
       //.ep_data(PAD_EP_DATA),

       .cout(MBUS_CLK_DLC2M),
       .dout(MBUS_DATA_DLC2M)
       //.wd_irq(PAD_WD_IRQ)
       );

    //****************************************************
    // CLOCKS
    //****************************************************
   initial begin
      CLK_CORE = 1'b0;
   end
   always begin
      #50;
      CLK_CORE = ~CLK_CORE;
   end
   // MBUS runs at 400KHz
   
   always @ (RESETn or MBC_CLKENB or CLK_MBC) begin
       if (~RESETn | MBC_CLKENB) 
           CLK_MBC <= `TEST_SD 1'b0;
       else begin
           `MBUS_HALF_PERIOD;
           CLK_MBC <= `TEST_SD ~CLK_MBC;
       end
   end

   /*
   // CORE runs at 10MHz
   always @ (RESETn or MBC_CLKENB or CLK_MBC) begin
       if (~RESETn | MBC_CLKENB) 
           CLK_MBC <= `TEST_SD 1'b0;
       else begin
           `MBUS_HALF_PERIOD;
           CLK_MBC <= `TEST_SD ~CLK_MBC;
       end
   end
   */

   //****************************************************
   // CHECK finish 
   //****************************************************

   always @ (CLK_CORE) begin
       if (DLCv1_0.dlcv1_ctrl_0.DNN_DONE) begin 
       #700000
            SHOW_DNN_MEM_WITH_HEX;
            SHOW_CODE_MEM_WITH_HEX;
            $finish;
       end
   end

   //****************************************************
   // CHECK debug 
   //****************************************************

   always @ (CLK_CORE) begin
       if (DLCv1_0.dlcv1_ctrl_0.DNN_DEBUG_PE_0) begin 
            SHOW_MEM_WITH_HEX_DEBUG(2'b00);
       end
       if (DLCv1_0.dlcv1_ctrl_0.DNN_DEBUG_PE_1) begin 
            SHOW_MEM_WITH_HEX_DEBUG(2'b01);
       end
       if (DLCv1_0.dlcv1_ctrl_0.DNN_DEBUG_PE_2) begin 
            SHOW_MEM_WITH_HEX_DEBUG(2'b10);
       end
       if (DLCv1_0.dlcv1_ctrl_0.DNN_DEBUG_PE_3) begin 
            SHOW_MEM_WITH_HEX_DEBUG(2'b11);
       end
   end

   //****************************************************
   // RESETS
   //****************************************************
   initial begin
      RESETn      <= 1'b0;
      `RESET_TIME;
      RESETn      <= `TEST_SD 1'b1;
   end
   
   //****************************************************
   // TEST HARNESS
   //****************************************************
   initial begin
        N_M_TX_ADDR         <= 32'b0;
        N_M_TX_DATA         <= 32'b0;
        N_M_TX_PEND         <= 1'b0;
        N_M_TX_REQ          <= 1'b0;
        N_M_TX_PRIORITY     <= 1'b0;
        N_M_TX_RESP_ACK     <= 1'b0;
        N_M_WAKEUP_REQ      <= 1'b0;
        N_M_MBC_THRESHOLD   <= `MBUS_THRESHOLD;

      
   end // initial begin

    /*
    always @ (posedge MBUS_CLK_A2B or negedge RESETn) begin
        if (~RESETn) N_A_RX_ACK <= `TEST_SD 1'b0;
        else         N_A_RX_ACK <= `TEST_SD N_A_RX_REQ;
    end
    */

   //****************************************************
   // MEMORY INITIAL VALUES
   //****************************************************
   /*
    integer idx_mem;
    initial begin
        #100;
        for (idx_mem=0; idx_mem<2048; idx_mem++) begin 
            DLCv1_0.TS1N40LPB16384X32M16M_0.memory[idx_mem] = $random; 
        end
    end
    */

   //****************************************************
   // DNN MEMORY FINESIME VECTORS
   //****************************************************
   
   integer finesim_file; 
   integer mem_dump_start_time;
    always@(negedge CLK_CORE) begin
        if(DLCv1_0.run_cpu) begin
            #10
            $fwrite(finesim_file, "%d\n", $time - mem_dump_start_time - 160000);
            //$fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.RESETN);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_DATA);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ADDR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_SEQ1RAN0);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_SEQ_INIT);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RD1WR0);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_CS);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_CS);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_CS);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_CS);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_THR_ADDR);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_THR_ADDR);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_THR_ADDR);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_THR_ADDR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L1_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L2_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L3_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L4_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L1);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L2);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L3);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L4);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L1);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L2);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L3);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L4);
            $fwrite(finesim_file, "+xxxxxxxxxxxxxxxxxxxxxxxx\n");
            //$fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.SRAM_L1_TUNE);
            //$fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.SRAM_L2_TUNE);
            //$fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.SRAM_L3_TUNE);
            //$fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.SRAM_L4_TUNE);
            //$fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.REG2PE_PG_STAB_CYCLES);
            #40
            $fwrite(finesim_file, "%d\n", $time - mem_dump_start_time - 160000);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_DATA);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ADDR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_SEQ1RAN0);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_SEQ_INIT);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RD1WR0);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_CS);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_CS);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_CS);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_CS);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_THR_ADDR);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_THR_ADDR);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_THR_ADDR);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_THR_ADDR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_THR_RD);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L1_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L2_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L3_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_L4_THR_WR);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L1_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L2_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L3_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_PG_L4_ENB);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L1);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L2);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L3);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_RESETN_L4);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L1);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L2);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L3);
            $fwrite(finesim_file, "+%b\n", DLCv1_0.PE_TOP_0.PE_MEM_0.ARB2MEM_ISOL_L4);
            $fwrite(finesim_file, "+%h\n", DLCv1_0.PE_TOP_0.PE_MEM_0.MEM2ARB_DATA);
        end
    end                                        
                                               
    //****************************************************
    // VCD Dump
    //****************************************************
`ifdef VCD_DUMP
    initial begin
        $dumpfile("DLCv1.vcd");
        `START_VCD_DUMP_FROM;
        `ifdef VCD_DUMP_ALL
        $dumpvars();
        `else
        $dumpvars(0, DLCv1_0);
        `endif
    end
`endif

  //****************************************************
   // RUNAWAY
   //****************************************************
   initial begin
      `RUNAWAY_TIME;
      $display("Runaway Timer");
      failure;
   end

   //****************************************************
   // SDF
   //****************************************************
   initial begin
`ifdef APR
    `ifdef SDFMAX
      $sdf_annotate("/afs/eecs.umich.edu/vlsida/projects/mm3_node/apr/PRCv13/PRCv13.apr.sdf", DLCv1_0,,, "MAXIMUM");
    `else 
      $sdf_annotate("/afs/eecs.umich.edu/vlsida/projects/mm3_node/apr/PRCv13/PRCv13.apr.sdf", DLCv1_0,,, "MINIMUM");
    `endif
`else
      $sdf_annotate("/afs/eecs.umich.edu/vlsida/projects/DNN/rtl/verilog/behavior/DLCv1/CORTEXM0INTEGRATION_TOP/CORTEXM0INTEGRATION_TOP.sdf", DLCv1_0.m0_0);
`endif
   end

   //****************************************************
   // load ARM code 
   //****************************************************
   reg [31: 0] CODE_SRAM_TO_WRITE_0 [2047:0];
   reg [31: 0] CODE_SRAM_TO_WRITE_1 [2047:0];
   reg [31: 0] CODE_SRAM_TO_WRITE_2 [2047:0];
   reg [31: 0] CODE_SRAM_TO_WRITE_3 [2047:0];
   initial begin
     #1000
     //mbus write
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_0.mem", CODE_SRAM_TO_WRITE_0);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_1.mem", CODE_SRAM_TO_WRITE_1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_2.mem", CODE_SRAM_TO_WRITE_2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_3.mem", CODE_SRAM_TO_WRITE_3);
     
     //load
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_0.mem", DLCv1_0.TS6N40LPA2048X32M4S_0.MX.mem);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_1.mem", DLCv1_0.TS6N40LPA2048X32M4S_1.MX.mem);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_2.mem", DLCv1_0.TS6N40LPA2048X32M4S_2.MX.mem);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/ARM_code/test_fft_full/SRAM_3.mem", DLCv1_0.TS6N40LPA2048X32M4S_3.MX.mem);
     

   end

   //****************************************************
   // load test memory 
   //****************************************************
   reg [95: 0] DNN_SRAM_TO_WRITE_0 [5759:0];
   reg [95: 0] DNN_SRAM_TO_WRITE_1 [5759:0];
   reg [95: 0] DNN_SRAM_TO_WRITE_2 [5759:0];
   reg [95: 0] DNN_SRAM_TO_WRITE_3 [5759:0];
   initial begin
     #1000
     //mbus write
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/FFT_INIT_MEM_0.data", DNN_SRAM_TO_WRITE_0);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/FFT_INIT_MEM_1.data", DNN_SRAM_TO_WRITE_1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/FFT_INIT_MEM_2.data", DNN_SRAM_TO_WRITE_2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/FFT_INIT_MEM_3.data", DNN_SRAM_TO_WRITE_3);
     

     
     //MEM 0
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L1B1.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L1B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L1B2.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L1B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L1B3.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L1B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L1B4.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L1B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L2B1.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L2B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L2B2.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L2B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L2B3.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L2B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L2B4.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L2B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L3B1.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L3B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L3B2.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L3B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L3B3.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L3B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L3B4.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L3B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L4B1.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L4B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L4B2.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L4B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L4B3.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L4B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_0L4B4.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM_L4B4);
     
     //MEM 1
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L1B1.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L1B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L1B2.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L1B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L1B3.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L1B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L1B4.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L1B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L2B1.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L2B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L2B2.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L2B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L2B3.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L2B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L2B4.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L2B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L3B1.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L3B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L3B2.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L3B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L3B3.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L3B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L3B4.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L3B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L4B1.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L4B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L4B2.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L4B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L4B3.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L4B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_1L4B4.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM_L4B4);
     
     //MEM 2
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L1B1.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L1B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L1B2.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L1B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L1B3.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L1B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L1B4.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L1B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L2B1.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L2B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L2B2.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L2B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L2B3.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L2B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L2B4.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L2B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L3B1.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L3B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L3B2.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L3B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L3B3.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L3B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L3B4.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L3B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L4B1.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L4B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L4B2.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L4B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L4B3.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L4B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_2L4B4.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM_L4B4);
     //MEM 3
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L1B1.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L1B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L1B2.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L1B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L1B3.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L1B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L1B4.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L1B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L2B1.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L2B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L2B2.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L2B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L2B3.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L2B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L2B4.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L2B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L3B1.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L3B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L3B2.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L3B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L3B3.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L3B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L3B4.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L3B4);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L4B1.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L4B1);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L4B2.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L4B2);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L4B3.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L4B3);
     $readmemh("/afs/eecs.umich.edu/vlsida/projects/mm3_node/git/M-ulator/platforms/m3/dlc_v1/numa_compiler/compiler/memory/FFT_INIT_MEM_3L4B4.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM_L4B4);
     
     

   end
   
   //****************************************************
   // GOC, EP, SPI, GPIO
   //****************************************************
   //assign CLK_CORE = PRCv13_0.clk_core;

    initial begin
        PAD_EP_ACTIVE   <= 1'b0;
        PAD_EP_CLK      <= 1'b0;
        PAD_EP_DATA     <= 1'b0;
    end

   //****************************************************
   // DEBUG DISPLAY
   //****************************************************
   `include "DLCv1_debug.v"

   integer inst_file;
   integer cycle;

   
   //****************************************************
   // EVERYTHING ELSE
   //****************************************************
   initial begin
      checking <= 0;
      finesim_file = $fopen("/afs/eecs.umich.edu/vlsida/projects/DNN/rtl/verilog/behavior/DLCv1/DLCv1_test/fft_full_finesim.vectors", "w+");
      //inst_file 	= $fopen("/afs/eecs.umich.edu/vlsida/projects/DNN/rtl/verilog/behavior/PE_FOUR/TB/MOV.inst", "r");


      //$readmemh("/afs/eecs.umich.edu/vlsida/projects/DNN/rtl/verilog/behavior/PE_FOUR/TB/MOV_MEM_0.data", DLCv1_0.PE_TOP_0.PE_MEM_0.INT_MEM);
      //$readmemh("/afs/eecs.umich.edu/vlsida/projects/DNN/rtl/verilog/behavior/PE_FOUR/TB/MOV_MEM_1.data", DLCv1_0.PE_TOP_0.PE_MEM_1.INT_MEM);
      //$readmemh("/afs/eecs.umich.edu/vlsida/projects/DNN/rtl/verilog/behavior/PE_FOUR/TB/MOV_MEM_2.data", DLCv1_0.PE_TOP_0.PE_MEM_2.INT_MEM);
      //$readmemh("/afs/eecs.umich.edu/vlsida/projects/DNN/rtl/verilog/behavior/PE_FOUR/TB/MOV_MEM_3.data", DLCv1_0.PE_TOP_0.PE_MEM_3.INT_MEM);

      
   //****************************************************
   // Start Testing
   //****************************************************
      @(posedge RESETn);
      $display("");
      $display("***********************************************");
      $display("***********************************************");
      $display("***************Simulation Start****************");
      $display("***********************************************");
      $display("***********************************************");
      $display("");
      checking <= 1;
      DLCv1_0.TS6N40LPA2048X32M4S_0.MX.show(0, 8);

      //send_MBus(32'hA0, 32'h1, 1);
      wakeup;
      enumerate(`DLC_LAYER);

      //write_reg(`DLC_LAYER, 8'h40, 24'h0F0F0F);

      #400000

      //write_reg(`DLC_LAYER, 8'h4F, 24'h0A0B0C);


      #400000

      //write_sram(`DLC_LAYER, 15'h1, 32'h0F0F0F0F);

      #400000

      //write_dnn_sram(`DLC_LAYER, 13'h40, 96'hDEADBEEF0000DEADBEEF1111, 2'b01);

      #400000

      //write_M0_instructions_B0(`DLC_LAYER);

      #400000

      //write_M0_instructions_B1(`DLC_LAYER);

      #400000

      //write_M0_instructions_B2(`DLC_LAYER);

      #400000

      //write_M0_instructions_B3(`DLC_LAYER);

      #400000
        
      //write_dnn_sram_bank_MEM0;

      #400000
        
      //write_dnn_sram_bank_MEM1;
      //debug_pe(2'b01);

      #400000
        
      //write_dnn_sram_bank_MEM2;

      #400000
        
      //write_dnn_sram_bank_MEM3;

      #400000

      //write_dnn_inputs(`DLC_LAYER);

      #400000

      //DLCv1_0.TS6N40LPA2048X32M4S_0.MX.show(2038, 2047);

      #400000
      
      mem_dump_start_time = $time;
      start_cpu;

      #200000

      //conflict
      //write_reg(`DLC_LAYER, 8'h40, 24'h0A0B0C);

      //write_sram(`DLC_LAYER, 15'hD04, 32'h0F0F0F0F);
      //read_sram(`DLC_LAYER, 15'hD04, 32'h0F0F0F0F);

      #100000

      //need to enable the power  
      //stall_pe(4'h1);
      //write_dnn_sram(`DLC_LAYER, 13'd2054, 96'hDEADBEEF0000DEADBEEF1111, 2'b01);
      //write_dnn_sram(`DLC_LAYER, 13'h2059, 96'hfbefbefbefaebaebaefbefb, 2'b01);
      //resume_pe(4'h1);

      //read_dnn_sram(`DLC_LAYER, 13'h40, 2'b01);
      //mbus_register_read(`DLC_LAYER, 8'h45, 8'h1, 8'h10, 8'h40);

      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000
      #400000

      #400000

      #400000

      #400000

      #400000

      #400000

      #400000

      #400000

      #400000

      #400000

      $finish;





      //read_reg(`DLC_LAYER, 8'h40, 24'h0F0F0F);


   end
   
`include "DLCv1_task.v"
endmodule // PRCv13_test
