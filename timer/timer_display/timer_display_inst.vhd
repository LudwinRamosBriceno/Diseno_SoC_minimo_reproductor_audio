	component timer_display is
		port (
			clk_clk                   : in  std_logic                     := 'X'; -- clk
			display_7_segments_export : out std_logic_vector(27 downto 0);        -- export
			pause_button_export       : in  std_logic                     := 'X'; -- export
			reset_reset_n             : in  std_logic                     := 'X'; -- reset_n
			reset_button_export       : in  std_logic                     := 'X'; -- export
			start_button_export       : in  std_logic                     := 'X'  -- export
		);
	end component timer_display;

	u0 : component timer_display
		port map (
			clk_clk                   => CONNECTED_TO_clk_clk,                   --                clk.clk
			display_7_segments_export => CONNECTED_TO_display_7_segments_export, -- display_7_segments.export
			pause_button_export       => CONNECTED_TO_pause_button_export,       --       pause_button.export
			reset_reset_n             => CONNECTED_TO_reset_reset_n,             --              reset.reset_n
			reset_button_export       => CONNECTED_TO_reset_button_export,       --       reset_button.export
			start_button_export       => CONNECTED_TO_start_button_export        --       start_button.export
		);

