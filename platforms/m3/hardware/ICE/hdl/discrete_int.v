module discrete_int(
	input clk,
	input reset,
	
	input SCL_DISCRETE_BUF,
	output SCL_PD,
	output SCL_PU,
	output SCL_TRI,
	
	input SDA_DISCRETE_BUF,
	output SDA_PD,
	output SDA_PU,
	output SDA_TRI,
	
	//I2C Settings
	input [7:0] i2c_speed,
	input [15:0] i2c_addr,
	
	//Master input bus
	input [7:0] ma_data,
	input [7:0] ma_addr,
	input ma_data_valid,
	input ma_frame_valid,
	inout sl_overflow,

	//Slave output bus
	inout [7:0] sl_data,
	output [1:0] sl_arb_request,
	input [1:0] sl_arb_grant,
	input sl_data_latch,
	
	//Global counter for 'time-tagging'
	input [7:0] global_counter,
	output reg incr_ctr,
	
	output [7:0] debug
);

//Local wires/buses
wire [7:0] tx_char;
wire tx_char_valid;
reg tx_data_latch;

reg [7:0] rx_char;
reg rx_char_latch;
reg rx_req;

//Bus interface takes care of all buffering, etc for discrete data...
wire hd_data_valid, hd_frame_valid, hd_data_latch, hd_header_done, hd_is_fragment, hd_is_empty;
wire [7:0] hd_header_eid;
reg hd_header_done_clear;
reg send_flag, send_ctr;
wire [7:0] rx_frame_data = (send_flag) ? 8'h64 : (send_ctr) ? global_counter : rx_char;
wire [7:0] bi_debug;
wire rx_frame_data_latch = rx_char_latch | send_ctr;
bus_interface #(8'h64,1,1,1) bi0(
	.clk(clk),
	.rst(reset),
	.ma_data(ma_data),
	.ma_addr(ma_addr),
	.ma_data_valid(ma_data_valid),
	.ma_frame_valid(ma_frame_valid),
	.sl_overflow(sl_overflow),
	.sl_data(sl_data),
	.sl_arb_request(sl_arb_request[0]),
	.sl_arb_grant(sl_arb_grant[0]),
	.sl_data_latch(sl_data_latch),
	.in_frame_data(tx_char),
	.in_frame_data_valid(hd_data_valid),//tx_char_valid),
	.in_frame_valid(hd_frame_valid),//tx_req),
	.in_frame_data_latch(tx_data_latch | hd_data_latch),
	.out_frame_data(rx_frame_data),
	.out_frame_valid(rx_req),
	.out_frame_data_latch(rx_frame_data_latch),
	.debug(bi_debug)
);
header_decoder hd0(
	.clk(clk),
	.rst(reset),
	.in_frame_data(tx_char),
	.in_frame_data_valid(hd_data_valid),
	.in_frame_valid(hd_frame_valid),
	.header_eid(hd_header_eid),
	.is_fragment(hd_is_fragment),
	.frame_data_latch(hd_data_latch),
	.header_done(hd_header_done),
	.packet_is_empty(hd_is_empty),
	.header_done_clear(hd_header_done_clear)
);
assign tx_char_valid = hd_data_valid & hd_header_done;

/************************************************************
 *Ack generator is used to easily create ACK & NAK sequences*
 ************************************************************/
reg ackgen_generate_ack, ackgen_generate_nak;
wire [7:0] ack_message_data;
wire ack_message_data_valid;
wire ack_message_frame_valid;
ack_generator ag0(
	.clk(clk),
	.reset(reset),
	
	.generate_ack(ackgen_generate_ack),
	.generate_nak(ackgen_generate_nak),
	.eid_in(hd_header_eid),
	
	.message_data(ack_message_data),
	.message_data_valid(ack_message_data_valid),
	.message_frame_valid(ack_message_frame_valid)
);

//Only using an output message fifo here because we should be able to keep up with requests in real-time
wire [7:0] mf_sl_data;
reg [7:0] message_idx;
reg local_frame_valid, send_idx;
assign sl_data = (sl_arb_grant[1]) ? mf_sl_data : 8'bzzzzzzzz;
message_fifo #(8) mf1(
	.clk(clk),
	.rst(reset),
	
	.in_data((send_idx) ? message_idx : ack_message_data),
	.in_data_latch(ack_message_data_valid | send_idx),
	.in_frame_valid(ack_message_frame_valid | local_frame_valid),
	.populate_frame_length(1'b1),

	.out_data(mf_sl_data),
	.out_frame_valid(sl_arb_request[1]),
	.out_data_latch(sl_data_latch & sl_arb_grant[1])
);
/************************************************************/

assign debug = {rx_req,rx_frame_data_latch,bi_debug[1:0],rx_frame_data[3:0]};//{hd_data_valid, hd_frame_valid, hd_is_fragment, tx_char_valid, tx_data_latch, SCL_DISCRETE_BUF, SDA_DISCRETE_BUF};

reg sda_drive_real;
reg ack;

reg SDA_mpu;
reg SDA_mpd;
assign SDA_PD = (ack) ? 1'b1 : SDA_mpd;
assign SDA_PU = (ack) ? 1'b1 : ~SDA_mpu;
assign SDA_TRI = (ack) ? 1'b1 : sda_drive_real;

reg SCL_mpu;
reg SCL_mpd;
assign SCL_PD = SCL_mpd;
assign SCL_PU = ~SCL_mpu;
assign SCL_TRI = SCL_mpd | SCL_mpu;

reg cur_bit_decr;
reg cur_bit_reset;
reg [8:0] clock_counter;
wire [8:0] clock_div = {1'b0, i2c_speed};
reg [2:0] cur_bit;

reg [3:0] rx_counter;
reg [7:0] rx_char_ctr;
reg reset_rx_char_ctr;
reg rx_counter_incr;
reg rx_counter_reset;
reg rx_shift_out;
reg next_rx_char_latch;
reg sda_db, sda_db_last, sda_db_0;
reg scl_db, scl_db_last, scl_db_0;
reg addr_enable, addr_disable, cur_addr_flag;
reg [7:0] cur_addr;
reg ack_set, ack_reset;

wire [7:0] ones_mask = i2c_addr[15:8];
wire [7:0] zeros_mask = i2c_addr[7:0];
wire addr_match = ((cur_addr & ones_mask) == ones_mask) && ((~cur_addr & zeros_mask) == zeros_mask);

//Defining states as parameters since `defines have global scope
parameter STATE_IDLE = 0;
parameter STATE_TX_START = 1;
parameter STATE_TX_SCL_LOW = 2;
parameter STATE_TX_SCL_HIGH = 3;
parameter STATE_TX_ACK_SCL_LOW = 4;
parameter STATE_TX_ACK_SCL_HIGH = 5;
parameter STATE_TX_STOP0 = 6;
parameter STATE_TX_STOP1 = 7;
parameter STATE_TX_STOP2 = 8;
parameter STATE_FRAGMENT_IDLE = 9;
parameter STATE_RX_DATA=10;
parameter STATE_RX_ACK=11;
parameter STATE_NAK0=12;
parameter STATE_NAK1=13;
parameter STATE_NAK2=14;
parameter STATE_CLEAR_FIFO=15;
parameter STATE_RX_HDR0=16;
parameter STATE_RX_HDR1=17;
parameter STATE_RX_FRAGMENT=18;

reg [4:0] state;
reg [4:0] next_state;
reg sda_drive, scl_drive;
reg sda_drive_val, scl_drive_val;
reg reset_idx, incr_idx;
reg fail, fail_set, fail_reset;

always @* begin
	next_state = state;
	cur_bit_decr = 1'b0;
	cur_bit_reset = 1'b0;
	tx_data_latch = 1'b0;
	hd_header_done_clear = 1'b0;
	local_frame_valid = 1'b0;
	send_idx = 1'b0;
	reset_idx = 1'b0;
	incr_idx = 1'b0;

	sda_drive = 1'b0;
	scl_drive = 1'b0;
	sda_drive_val = 1'b1;
	scl_drive_val = 1'b1;
	
	ackgen_generate_ack = 1'b0;
	ackgen_generate_nak = 1'b0;
	
	rx_counter_incr = 1'b0;
	rx_counter_reset = 1'b0;
	next_rx_char_latch = 1'b0;
	rx_req = 1'b0;
	rx_shift_out = 1'b0;
	addr_enable = 1'b0;
	addr_disable = 1'b0;
	ack_set = 1'b0;
	ack_reset = 1'b0;
	send_flag = 1'b0;
	send_ctr = 1'b0;
	incr_ctr = 1'b0;
	fail_set = 1'b0;
	fail_reset = 1'b0;
	reset_rx_char_ctr = 1'b0;
	
	case(state)
		STATE_IDLE: begin
			fail_reset = 1'b1;
			rx_counter_reset = 1'b1;
			addr_enable = 1'b1;
			ack_reset = 1'b1;
			reset_idx = 1'b1;
			if(sda_db == 1'b0 && scl_db == 1'b1)
				next_state = STATE_RX_HDR0;
			else if(hd_header_done & ~hd_is_empty)
				next_state = STATE_TX_START;
			else if(hd_header_done) begin //The packet IS empty
				hd_header_done_clear = 1'b1;
				ackgen_generate_ack = 1'b1;
			end
		end
		
		STATE_FRAGMENT_IDLE: begin
			scl_drive = 1'b1;
			scl_drive_val = 1'b0;
			reset_idx = 1'b1;
			if(hd_header_done & ~hd_is_empty)
				next_state = STATE_TX_SCL_LOW;
			else if(hd_header_done) begin //The packet IS empty
				hd_header_done_clear = 1'b1;
				ackgen_generate_ack = 1'b1;
				next_state = STATE_IDLE;
			end
		end

		STATE_TX_START: begin
			sda_drive = 1'b1;
			scl_drive = 1'b1;
			if(clock_counter <= clock_div)
				sda_drive_val = 1'b1;
			else begin
				sda_drive_val = 1'b0;
			end
			cur_bit_reset = 1'b1;
			if(clock_counter == clock_div*2) begin
				next_state = STATE_TX_SCL_LOW;
			end
		end

		STATE_TX_SCL_LOW: begin
			sda_drive = 1'b1;
			scl_drive = 1'b1;
			sda_drive_val = tx_char[cur_bit];
			scl_drive_val = 1'b0;
			if(clock_counter == clock_div)
				next_state = STATE_TX_SCL_HIGH;
		end

		STATE_TX_SCL_HIGH: begin
			sda_drive = 1'b1;
			scl_drive = 1'b1;
			sda_drive_val = tx_char[cur_bit];
			if(clock_counter == clock_div) begin
				cur_bit_decr = 1'b1;
				if(cur_bit > 0) begin
					next_state = STATE_TX_SCL_LOW;
				end else begin
					next_state = STATE_TX_ACK_SCL_LOW;
				end
			end
		end

		STATE_TX_ACK_SCL_LOW: begin
			scl_drive = 1'b1;
			scl_drive_val = 1'b0;
			if(clock_counter == clock_div) begin
				tx_data_latch = 1'b1; //TODO: This should only be done conditionally (if it received an ACK)
				next_state = STATE_TX_ACK_SCL_HIGH;
			end
		end

		STATE_TX_ACK_SCL_HIGH: begin
			scl_drive = 1'b1;
			scl_drive_val = 1'b1;
			cur_bit_reset = 1'b1;
			if(clock_counter == clock_div) begin
				incr_idx = 1'b1;
				if(sda_db == 1'b1) begin
					next_state = STATE_TX_STOP0;
					fail_set = 1'b1;
				end else if(hd_frame_valid & tx_char_valid) begin
					next_state = STATE_TX_SCL_LOW;
				end else if(hd_is_fragment) begin
					hd_header_done_clear = 1'b1;
					ackgen_generate_ack = 1'b1;
					next_state = STATE_FRAGMENT_IDLE;
				end else
					next_state = STATE_TX_STOP0;
			end
		end

		STATE_TX_STOP0: begin
			scl_drive = 1'b1;
			scl_drive_val = 1'b0;
			sda_drive = 1'b1;
			sda_drive_val = 1'b0;
			if(clock_counter == clock_div)
				next_state = STATE_TX_STOP1;
		end

		STATE_TX_STOP1: begin
			scl_drive = 1'b1;
			scl_drive_val = 1'b1;
			sda_drive = 1'b1;
			sda_drive_val = 1'b0;
			if(clock_counter == clock_div)
				next_state = STATE_TX_STOP2;
		end

		STATE_TX_STOP2: begin
			scl_drive = 1'b1;
			scl_drive_val = 1'b1;
			sda_drive = 1'b1;
			sda_drive_val = 1'b1;
			if(clock_counter == clock_div) begin
				if(fail)
					next_state = STATE_NAK0;
				else begin
					hd_header_done_clear = 1'b1;
					ackgen_generate_ack = 1'b1;
					next_state = STATE_IDLE;
				end
			end
		end
		
		STATE_NAK0: begin
			ackgen_generate_nak = 1'b1;
			next_state = STATE_NAK1;
		end
		
		STATE_NAK1: begin
			local_frame_valid = 1'b1;
			if(ack_message_frame_valid == 1'b0)
				next_state = STATE_NAK2;
		end
		
		STATE_NAK2: begin
			local_frame_valid = 1'b1;
			send_idx = 1'b1;
			next_state = STATE_CLEAR_FIFO;
		end
		
		STATE_CLEAR_FIFO: begin
			tx_data_latch = 1'b1;
			if((~hd_frame_valid) | (~tx_char_valid)) begin
				tx_data_latch = 1'b0;
				hd_header_done_clear = 1'b1;
				next_state = STATE_IDLE;
			end
		end
		
		STATE_RX_HDR0: begin
			send_flag = 1'b1;
			rx_req = 1'b1;
			next_state = STATE_RX_HDR1;
		end
		
		STATE_RX_HDR1: begin
			reset_rx_char_ctr = 1'b1;
			rx_req = 1'b1;
			send_ctr = 1'b1;
			if(clock_counter[0]) begin
				incr_ctr = 1'b1;
				next_state = STATE_RX_DATA;
			end
		end
		
		STATE_RX_DATA: begin
			rx_req = 1'b1;
			if(scl_db_last == 1'b0 && scl_db == 1'b1) begin
				rx_counter_incr = 1'b1;
				rx_shift_out = 1'b1;
				if(rx_counter == 4'd7) begin
					rx_counter_reset = 1'b1;
					next_rx_char_latch = 1'b1;
					next_state = STATE_RX_ACK;
				end
			end
			//If we've seen a stop bit, proceed to go, collect $200
			if(sda_db_last == 1'b0 && sda_db == 1'b1 && scl_db == 1'b1) begin
				next_state = STATE_IDLE;
			end
		end
		
		STATE_RX_ACK: begin
			rx_req = 1'b1;
			//According to the 'mm3 sensor node - I2C.pptx' documentation, I can just pull down for ACK during the whole cycle without any repercussions
			addr_disable = 1'b1;
			if(sda_db_last == 1'b0 && sda_db == 1'b1 && scl_db == 1'b1) begin
				next_state = STATE_IDLE;
			end else if(scl_db_last == 1'b1 && scl_db == 1'b0) begin
				if(addr_match)
					ack_set = 1'b1;
				//TODO: Need some sort of address matching in here!!!
				rx_counter_incr = 1'b1;
				if(rx_counter == 4'd1) begin
					ack_reset = 1'b1;
					rx_counter_reset = 1'b1;
					if(rx_char_ctr == 8'hFF)
						next_state = STATE_RX_FRAGMENT;
					else
						next_state = STATE_RX_DATA;
				end
			end
		end
		
		STATE_RX_FRAGMENT: begin
			if(clock_counter[0]) //Give the message fifo a cycle to populate the length data
				next_state = STATE_RX_HDR0;
		end
	endcase
end

reg [4:0] sda_drive_counter;
always @(posedge clk) begin
	
	SCL_mpu <= (scl_drive) ? ((scl_drive_val) ? 1'b1 : 1'b0) : 1'b0;
	SCL_mpd <= (scl_drive) ? ((scl_drive_val) ? 1'b0 : 1'b1) : 1'b0;
	
	//Every time we stop driving SDA, we should pull it high first
	if(!sda_drive) begin
		if(sda_drive_counter <= 11) begin
			sda_drive_counter <= sda_drive_counter + 1;
			sda_drive_real <= 1'b1;
		end else if(sda_drive_counter <= 23) begin
			sda_drive_counter <= sda_drive_counter + 1;
			sda_drive_real <= 1'b1;
			SDA_mpu <= 1'b1;
			SDA_mpd <= 1'b0;
		end else if(sda_drive_counter <= 24) begin
			sda_drive_counter <= sda_drive_counter + 1;
			sda_drive_real <= 1'b0;
			SDA_mpu <= 1'b1;
			SDA_mpd <= 1'b0;
		end else begin
			SDA_mpu <= 1'b0;
			SDA_mpd <= 1'b0;
		end
	end else if(sda_drive) begin
		//Everything SDA is delayed by 1/4 cycle
		if(clock_counter >= (clock_div >> 2)) begin
			SDA_mpu <= (sda_drive) ? ((sda_drive_val) ? 1'b1 : 1'b0) : 1'b0;
			SDA_mpd <= (sda_drive) ? ((sda_drive_val) ? 1'b0 : 1'b1) : 1'b0;
			sda_drive_counter <= 0;
			sda_drive_real <= 1'b1;
		end
	end

	if(reset) begin
		clock_counter <= 0;
		state <= STATE_IDLE;
		sda_drive_real <= 1'b0;
		sda_drive_counter <= 0;
		message_idx <= 0;
		fail <= 1'b0;
	end else begin
		state <= next_state;
		
		//Failure flag tells whether to send a NAK to host
		if(fail_set)
			fail <= 1'b1;
		else if(fail_reset)
			fail <= 1'b0;
	
		clock_counter <= clock_counter + 1;
		if(state != next_state)
			clock_counter <= 0;
			
		//message_idx keeps track of where we are in the current i2c transaction (for NAKing)
		if(reset_idx)
			message_idx <= 0;
		else if(incr_idx)
			message_idx <= message_idx + 1;

		if(cur_bit_reset)
			cur_bit <= 7;
		else if(cur_bit_decr)
			cur_bit <= cur_bit - 1;
	end
end

always @(posedge clk) begin
	
	//Debouncing for SDA & SCL
	sda_db_0 <= SDA_DISCRETE_BUF;
	sda_db <= sda_db_0;
	scl_db_0 <= SCL_DISCRETE_BUF;
	scl_db <= scl_db_0;
	
	sda_db_last <= sda_db;
	scl_db_last <= scl_db;
	
	rx_char_latch <= next_rx_char_latch;
	if(reset)
		rx_char_ctr <= 0;
	else begin
		if(reset_rx_char_ctr)
			rx_char_ctr <= 0;
		if(next_rx_char_latch)
			rx_char_ctr <= rx_char_ctr + 1;
	end
	
	if(rx_counter_reset)
		rx_counter <= 0;
	else if(rx_counter_incr)
		rx_counter <= rx_counter + 1;
		
	if(rx_shift_out)
		rx_char <= {rx_char[6:0], sda_db};

	//First byte of the I2C transaction is the I2C address
	if(addr_enable)
		cur_addr_flag <= 1'b1;
	else if(addr_disable)
		cur_addr_flag <= 1'b0;
	if(rx_shift_out && cur_addr_flag)
		cur_addr <= {cur_addr[6:0], sda_db};

	//Ack overrides the tx state machine logic to pull SDA down
	if(ack_reset)
		ack <= 1'b0;
	else if(ack_set)
		ack <= 1'b1;

	//Increment through the address match RAM when loading stuff in
	/*if(~addr_match_nreset)
		addr_match_addr <= 0;
	else if(addr_match_latch)
		addr_match_addr <= addr_match_addr + 1;

	if(reset) begin
		addr_match_addr <= 4'd0;
	end*/
end

endmodule

