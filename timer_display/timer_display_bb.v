
module timer_display (
	clk_clk,
	display_7_segments_export,
	reset_reset_n,
	reset_button_export,
	start_button_export);	

	input		clk_clk;
	output	[27:0]	display_7_segments_export;
	input		reset_reset_n;
	input		reset_button_export;
	input		start_button_export;
endmodule
