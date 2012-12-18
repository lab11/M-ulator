//----------------------------------------------------------------------------------------
// Design Name : uart 
// File Name   : uart.v
// Function    : Simple UART
// Coder       : Deepak Kumar Tala (w/ significant annotations and modifications from Ben Kempke)
// Pulled from : http://www.asic-world.com/examples/verilog/uart.html on 11/1/2012
//----------------------------------------------------------------------------------------
module uart (
	// Port declarations
	input reset             ,//Reset signal for entire UART module (active high)

	input clk               ,//Clock corresponding to TX baudrate

	input rx_in             ,//UART RX line
	output reg tx_out       ,//UART TX line

	input tx_latch          ,//TX Data Latch
	input [7:0] tx_data     ,//8-bit TX Data
	output reg tx_empty     ,//Ready for new TX Data

	output reg [7:0] rx_data,//8-bit RX Data
	output reg rx_latch      //RX Data ready line (high for one rxclk cycle)
);

parameter BAUD_DIV = 8;

// Internal Variables 
reg [7:0] tx_reg;
reg [3:0] tx_cnt;
reg [3:0] rx_cnt;
reg [15:0] rx_sample_cnt;
reg [15:0] tx_sample_cnt;
reg rx_d1;
reg rx_d2;
reg rx_busy;

// UART RX Logic 
always @ (posedge clk) begin
	if (reset) begin
		rx_data       <= 0; 
		rx_sample_cnt <= 0;
		rx_cnt        <= 0;
		rx_latch      <= 0;
		rx_d1         <= 1;
		rx_d2         <= 1;
		rx_busy       <= 0;
	end else begin
		// Synchronize the asynch signal
		rx_d1 <= rx_in;
		rx_d2 <= rx_d1;
		rx_latch <= 0;
		// Check if just received start of frame
		if (!rx_busy && !rx_d2) begin
			rx_busy       <= 1;
			rx_sample_cnt <= 1;
			rx_cnt        <= 0;
		end
		// Start of frame detected, Proceed with rest of data
		if (rx_busy) begin
			rx_sample_cnt <= rx_sample_cnt + 1;
			// Logic to sample at middle of data
			if (rx_sample_cnt == BAUD_DIV/2) begin
				if ((rx_d2 == 1) && (rx_cnt == 0)) begin
					rx_busy <= 0;
				end else begin
					rx_cnt <= rx_cnt + 1; 
					// Start storing the rx data
					if (rx_cnt > 0 && rx_cnt < 9) begin
						rx_data[rx_cnt - 1] <= rx_d2;
					end
					if (rx_cnt == 9) begin
						rx_busy <= 0;
						// Check if End of frame received correctly
						if (rx_d2 == 1) begin
							rx_latch     <= 1;
						end
					end
				end
			end else if(rx_sample_cnt == BAUD_DIV-1) begin
				rx_sample_cnt <= 0;
			end
		end 
	end
end

// UART TX Logic
always @ (posedge clk) begin
	if (reset) begin
		tx_reg        <= 0;
		tx_empty      <= 1;
		tx_sample_cnt <= 0;
		tx_out        <= 1;
		tx_cnt        <= 0;
	end else begin
		if (!tx_empty) begin
			tx_sample_cnt <= tx_sample_cnt + 1;
			if(tx_sample_cnt == BAUD_DIV-1) begin
				tx_sample_cnt <= 0;
				tx_cnt <= tx_cnt + 1;
				if (tx_cnt == 0) begin
					tx_out <= 0;
				end
				if (tx_cnt > 0 && tx_cnt < 9) begin
					tx_out <= tx_reg[tx_cnt -1];
				end
				if (tx_cnt == 9) begin
					tx_out <= 1;
					tx_cnt <= 0;
					tx_empty <= 1;
				end
			end
		end else if (tx_latch) begin
			tx_reg   <= tx_data;
			tx_empty <= 0;
			tx_sample_cnt <= 0;
		end
	end
end

endmodule
