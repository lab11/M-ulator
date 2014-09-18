module message_fifo(
	input clk,
	input rst,

	input [7:0] in_data,
	input in_data_latch,
	input in_frame_valid,
	output in_data_overflow,
	input populate_frame_length,

	output [7:0] out_data,
	output out_frame_valid,
	input out_data_latch,
	
	output [7:0] debug
);
parameter DEPTH_LOG2=9;
parameter SUPPORTS_FRAGMENTATION = 0;
parameter DEPTH = (1 << DEPTH_LOG2);

wire eof_marker;

//State machine signals
reg latch_old_head;
reg insert_in_data;
reg insert_fvbit;
reg revert_head_ptr;
reg incr_byte_ctr;
reg incr_frame_ctr;
reg insert_frame_ctr;
reg insert_old_header;

reg [7:0] old_header;

//Inferred ram block
reg [DEPTH_LOG2-1:0] head, old_head, tail, num_valid_frames;
reg [8:0] num_frame_bytes;
wire [DEPTH_LOG2-1:0] ram_wr_addr;
wire [8:0] ram_wr_data;
wire ram_wr_latch;
ram #(9,DEPTH_LOG2) fr1(
	.clk(clk),
	.reset(rst),
	.in_data(ram_wr_data),
	.in_addr(ram_wr_addr),
	.in_latch(ram_wr_latch),
	.out_data({eof_marker, out_data}),
	.out_addr(tail)
);

assign ram_wr_addr = (insert_frame_ctr) ? old_head + 2 : head;
assign ram_wr_data = (insert_frame_ctr) ? {1'b0, num_frame_bytes[7:0]} : {insert_fvbit,in_data};
assign ram_wr_latch = (insert_frame_ctr | insert_in_data | insert_fvbit | insert_old_header); 

reg last_frame_valid;

assign in_data_overflow = (head == tail-1);
assign out_frame_valid = (num_valid_frames > 0);

//Main state machine logic
parameter STATE_IDLE = 0;
parameter STATE_RECORDING_FRAME = 1;
parameter STATE_POPULATE_FLEN = 2;
parameter STATE_FRAGMENT0 = 3;
parameter STATE_FRAGMENT1 = 4;
parameter STATE_FRAGMENT2 = 5;

reg [3:0] state, next_state;
assign debug = {eof_marker,state[2:0]};
always @(posedge clk) begin
	last_frame_valid <= in_frame_valid;

	if(rst) begin
		state <= STATE_IDLE;
		head <= 0;
		old_head <= 0;
		tail <= 0;
		num_valid_frames <= 0;
		num_frame_bytes <= 0;
	end else begin
		state <= next_state;
	
		if(latch_old_head) begin
			old_head <= head;
			num_frame_bytes <= -2;
		end
		if(insert_in_data | insert_fvbit | insert_old_header)
			head <= head + 1;
		if(revert_head_ptr)
			head <= old_head;
		if(incr_byte_ctr)
			num_frame_bytes <= num_frame_bytes + 1;
		if(incr_frame_ctr)
			num_valid_frames <= num_valid_frames + 1;
			
		//Save a header in case we need it for a fragment later
		if(state == STATE_IDLE)
			old_header <= in_data;

		if(out_data_latch)
			tail <= tail + 1;
		if(eof_marker && (num_valid_frames > 0)) begin
			tail <= tail + 1;
			num_valid_frames <= num_valid_frames - 1;
		end
	end
end
always @* begin
	next_state = state;
	latch_old_head = 1'b0;
	insert_in_data = 1'b0;
	insert_fvbit = 1'b0;
	revert_head_ptr = 1'b0;
	incr_byte_ctr = 1'b0;
	incr_frame_ctr = 1'b0;
	insert_frame_ctr = 1'b0;
	insert_old_header = 1'b0;

	case(state)
		STATE_IDLE: begin
			latch_old_head = 1'b1;
			if(in_frame_valid && ~last_frame_valid) begin
				insert_in_data = 1'b1;
				next_state = STATE_RECORDING_FRAME;
			end
		end
		
		STATE_RECORDING_FRAME: begin
			//Check to see if we have to fragment a packet
			if(in_data_overflow) begin
				revert_head_ptr = 1'b1;
				next_state = STATE_IDLE;
			end else if(in_data_latch) begin
				incr_byte_ctr = 1'b1;
				insert_in_data = 1'b1;
			end
			if(~in_frame_valid) begin
				incr_frame_ctr = 1'b1;
				insert_fvbit = 1'b1;
				if(populate_frame_length)
					next_state = STATE_POPULATE_FLEN;
				else
					next_state = STATE_IDLE;
			end
		end
		
		STATE_POPULATE_FLEN: begin
			insert_frame_ctr = 1'b1;
			next_state = STATE_IDLE;
		end
		
	endcase
end

endmodule

