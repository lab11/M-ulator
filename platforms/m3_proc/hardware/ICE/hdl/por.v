module por(
	input clk,
	inout PAD,
	output reset
);
	
//This circuit follows the design guidelines from http://www.actel.com/documents/LPF_AC380_AN.pdf
// to make a simple PoR circuit using just a single external pull-up resistor (~100kOhm)

wire RST_p;
BIBUF b0(
	.PAD(PAD),
	.Y(RST_p),
	.D(1'b0),
	.E(1'b1)
);

reg [1:0] chain;
assign reset = chain[1];

always @(posedge clk or posedge RST_p) begin
	if(RST_p) begin
		chain <= 2'b0;
	end else begin
		chain <= {chain[0], 1'b1};
	end
end

endmodule
