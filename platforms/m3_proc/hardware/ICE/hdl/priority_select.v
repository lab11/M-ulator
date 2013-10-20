module priority_select(clk, rst, enable, latch, requests, grants, granted);
parameter WIDTH=8;	

input clk;
input rst;
input enable;
input latch;

input [WIDTH-1:0] requests;
output reg [WIDTH-1:0] grants;
output granted;

reg [WIDTH-1:0] temp_grants;

assign granted = (grants & requests) > 0;

integer ii;
reg temp_granted;
always @* begin
	temp_grants = 0;
	temp_granted = 1'b0;
	
	//Only grant one request, and only if the priority selector is enabled...
	if(enable) begin
		for(ii = 0; ii < WIDTH; ii=ii+1) begin
			if(requests[ii] && ~temp_granted) begin
				temp_grants[ii] = 1'b1;
				temp_granted = 1'b1;
			end
		end
	end
end

always @(posedge clk) begin
	if(rst) begin
		grants <= 0;
	end else begin
		if(latch) begin
			grants <= temp_grants;
		end
	end
end

endmodule

