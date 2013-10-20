module pint_int(
	input clk,
	input reset,
	output busy,
	
	//Controller logic signals
	input tx_req,
	input tx_cmd_type,
	input [7:0] tx_char,
	input tx_char_latch,
	
	output reg rx_latch,
	output reg rx_req,
	output reg [7:0] rx_data,

	//PINT signals
	output reg PINT_WRREQ,
	output PINT_WRDATA,
	output PINT_CLK,
	output PINT_RESETN,
	output reg PINT_RDREQ,
	input PINT_RDRDY,
	input PINT_RDDATA,
	
	output [7:0] debug
);

parameter BAUD_DIV = 100; //TODO: What does this need to be set to???

//FIFO to queue up outgoing transmissions.  An extra bit to denote frame boundaries...
//NOTE: Make sure this doesn't overfill
wire [7:0] fifo_char;
wire fifo_valid;
wire fifo_req;
reg fifo_latch;
fifo #(9,4) f1(
	.clk(clk),
	.reset(reset),
	.in({tx_req, tx_char}),
	.in_latch(tx_char_latch | tx_req),
	.out({fifo_req, fifo_char}),
	.out_latch(fifo_latch),
	.out_valid(fifo_valid)
);

//For now, I'm just going to assume that the RESETN signal works by itself
assign PINT_RESETN = ~reset;

reg pint_rdrdy2, pint_rdrdy1;
reg tx_in_progress, rx_in_progress;
reg [8:0] tx_data_latched;
reg [15:0] baud_ctr;
reg [2:0] rx_bit_count;
reg [3:0] tx_bit_count;
reg pint_clk_mute, pint_clk_int;

assign PINT_WRDATA = tx_data_latched[8];
assign PINT_CLK = (pint_clk_mute) ? 1'b0 : pint_clk_int;

assign busy = (tx_in_progress | rx_in_progress);
wire baud_tick = (baud_ctr == BAUD_DIV-1);

always @(posedge clk) begin
	//Sync logic for PINT_RDRDY signal
	pint_rdrdy1 <= PINT_RDRDY;
	pint_rdrdy2 <= pint_rdrdy1;
	PINT_RDREQ <= 1'b0;
	rx_latch <= 1'b0;
	rx_req <= 1'b0;
	fifo_latch <= 1'b0;

	if(reset) begin
		tx_in_progress <= 1'b0;
		rx_in_progress <= 1'b0;
		baud_ctr <= 16'd0;
		pint_clk_int <= 1'b0;
		pint_clk_mute <= 1'b1;
		PINT_WRREQ <= 1'b0;
	end else begin
		//Default behavior is to let the PINT clock run
		baud_ctr <= baud_ctr + 1;
		if(baud_tick ) begin
			baud_ctr <= 0;
			pint_clk_int <= ~pint_clk_int;
		end
		
		//Read data back on PINT_RDDATA line, framed by PINT_RDREQ
		if(rx_in_progress) begin
			PINT_RDREQ <= 1'b1;
			if(baud_tick && pint_clk_int == 1'b0) begin
				//Rising edge of generated clock, shift in the received data
				rx_data[7:1] <= rx_data[6:0];
				rx_data[0] <= PINT_RDDATA;
				rx_bit_count <= rx_bit_count + 1;
				if(rx_bit_count == 3'd7)
					rx_latch <= 1'b1;
			end
				
			//We end when the PINT_RDRDY line says we're done (RDRDY goes low...)
			else if(baud_tick && pint_clk_int == 1'b1 && ~pint_rdrdy2) begin
				rx_in_progress <= 1'b0;
				rx_req <= 1'b1;
				pint_clk_mute <= 1'b1;
			end
		end
			
		//Transmit data on PINT_WRDATA line, framed by WRREQ
		else if(tx_in_progress) begin
			if(baud_tick && pint_clk_int == 1'b0) begin
				PINT_WRREQ <= 1'b1;
			end 
			
			else if(baud_tick && pint_clk_int == 1'b1) begin
				//Falling edge of generated clock
				tx_data_latched[8:1] <= tx_data_latched[7:0];
				
				//Unmute clock after command bit has gone through
				pint_clk_mute <= 1'b0;
				
				//Check to see if we're done sending all the bytes
				tx_bit_count <= tx_bit_count + 1;
				if(tx_bit_count == 4'd8) begin
					tx_bit_count <= 1;
					tx_data_latched <= {fifo_char,1'b0};
					fifo_latch <= 1'b1;
					if(fifo_req) begin //END Case: Got a dummy fifo_req bit...
						pint_clk_mute <= 1'b1;
						PINT_WRREQ <= 1'b0;
						tx_in_progress <= 1'b0;
						fifo_latch <= 1'b1;
					end
				end
			end
		end
			
		//Wait for either the controller to initiate a write or the PINT to initiate an interrupt
		else begin
			baud_ctr <= 16'd0;
			rx_bit_count <= 6'd0;
			if(pint_rdrdy2) begin
				rx_in_progress <= 1'b1;
				pint_clk_mute <= 1'b0;
			end else if(tx_req) begin
				tx_in_progress <= 1'b1;
				tx_data_latched <= {tx_cmd_type, fifo_char};
				fifo_latch <= 1'b1;
				pint_clk_mute <= 1'b1;
				tx_bit_count <= 0;
			end
		end
	end
end

assign debug = {tx_in_progress, rx_in_progress, tx_req, tx_cmd_type};

endmodule
