module ram(clk,reset,in_data,in_addr,in_latch,out_data,out_addr);
//User-defined parameters
parameter DATA_WIDTH = 8;
parameter DEPTH_LOG2 = 4;
parameter DEPTH = (1 << DEPTH_LOG2);

//Ports
input clk;
input reset;
input [DATA_WIDTH-1:0] in_data;
input [DEPTH_LOG2-1:0] in_addr;
input in_latch;
output [DATA_WIDTH-1:0] out_data;
input [DEPTH_LOG2-1:0] out_addr;

//Locals
reg [DATA_WIDTH-1:0] ram [DEPTH-1:0];
assign out_data = ram[out_addr];

always @(posedge clk) begin
	if(reset) begin
		//Not sure if there's really anything to do in the reset state...
	end else begin
		if(in_latch) begin
			ram[in_addr] <= in_data;
		end
		//out_data <= ram[out_addr];
	end
end

endmodule

