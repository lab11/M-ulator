module ack_generator(
	input clk,
	input reset,
	
	input generate_ack,
	input generate_nak,
	input [7:0] eid_in,
	
	output reg [7:0] message_data,
	output reg message_data_valid,
	output reg message_frame_valid
);

parameter STATE_IDLE = 0;
parameter STATE_ACK0 = 1;
parameter STATE_NAK0 = 2;
parameter STATE_EID = 3;
parameter STATE_LEN = 4;
reg [2:0] state, next_state;

always @* begin
	next_state = state;
	message_data = 8'h00;
	message_data_valid = 1'b1;
	message_frame_valid = 1'b1;
	
	case(state)
		STATE_IDLE: begin
			message_data_valid = 1'b0;
			message_frame_valid = 1'b0;
			if(generate_ack)
				next_state = STATE_ACK0;
			else if(generate_nak)
				next_state = STATE_NAK0;
		end
		
		STATE_ACK0: begin
			message_data = 8'h00;
			next_state = STATE_EID;
		end
		
		STATE_NAK0: begin
			message_data = 8'h01;
			next_state = STATE_EID;
		end
		
		STATE_EID: begin
			message_data = eid_in;
			next_state = STATE_LEN;
		end
		
		STATE_LEN: begin
			next_state = STATE_IDLE;
		end
	endcase
end

always @(posedge clk) begin
	if(reset) begin
		state <= STATE_IDLE;
	end else begin
		state <= next_state;
	end
end
	
endmodule
