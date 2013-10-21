module header_decoder(
	input clk,
	input rst,
	input [7:0] in_frame_data,
	input in_frame_data_valid,
	input in_frame_valid,
	output reg [7:0] header_eid,
	output reg frame_data_latch,
	output reg header_done,
	output reg packet_is_empty,
	output reg is_fragment,
	input header_done_clear
);

parameter STATE_IDLE = 0;
parameter STATE_RECORD_EID = 1;
parameter STATE_SKIP_LEN = 2;
parameter STATE_WAIT = 3;

reg [3:0] state, next_state;
reg latch_eid, latch_is_fragment, set_header_done;
always @(posedge clk) begin
	if(rst) begin
		state <= STATE_IDLE;
		header_done <= 1'b0;
		is_fragment <= 1'b0;
		packet_is_empty <= 1'b0;
	end else begin
		state <= next_state;
		if(latch_eid)
			header_eid <= in_frame_data;
		if(set_header_done) begin
			header_done <= 1'b1;
			if(in_frame_data == 8'h00)
				packet_is_empty <= 1'b1;
			else
				packet_is_empty <= 1'b0;
		end
		if(header_done_clear)
			header_done <= 1'b0;
		if(latch_is_fragment) begin
			if(in_frame_data == 8'hFF)
				is_fragment <= 1'b1;
			else
				is_fragment <= 1'b0;
		end
	end
end

always @* begin
	next_state = state;
	frame_data_latch = 1'b0;
	set_header_done = 1'b0;
	latch_is_fragment = 1'b0;
	latch_eid = 1'b0;
	
	case(state)
		STATE_IDLE: begin
			frame_data_latch = in_frame_valid;
			if(in_frame_valid) begin
				next_state = STATE_RECORD_EID;
			end
		end
		
		STATE_RECORD_EID: begin
			frame_data_latch = in_frame_data_valid;
			latch_eid = 1'b1;
			if(in_frame_data_valid) begin
				next_state = STATE_SKIP_LEN;
			end
		end
		
		STATE_SKIP_LEN: begin
			frame_data_latch = in_frame_data_valid;
			latch_is_fragment = in_frame_data_valid;
			set_header_done = in_frame_data_valid;
			if(in_frame_data_valid) begin
				next_state = STATE_WAIT;
			end
		end
		
		STATE_WAIT: begin
			if(~in_frame_valid) begin
				next_state = STATE_IDLE;
			end
		end
	endcase
end

endmodule
