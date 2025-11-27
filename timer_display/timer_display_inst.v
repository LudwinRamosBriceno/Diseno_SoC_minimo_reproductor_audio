	timer_display u0 (
		.clk_clk                   (<connected-to-clk_clk>),                   //                clk.clk
		.display_7_segments_export (<connected-to-display_7_segments_export>), // display_7_segments.export
		.pause_button_export       (<connected-to-pause_button_export>),       //       pause_button.export
		.reset_reset_n             (<connected-to-reset_reset_n>),             //              reset.reset_n
		.reset_button_export       (<connected-to-reset_button_export>),       //       reset_button.export
		.start_button_export       (<connected-to-start_button_export>)        //       start_button.export
	);

