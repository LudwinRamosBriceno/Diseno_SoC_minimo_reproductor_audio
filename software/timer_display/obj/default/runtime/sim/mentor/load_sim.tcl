# ------------------------------------------------------------------------------
# Top Level Simulation Script to source msim_setup.tcl
# ------------------------------------------------------------------------------
set QSYS_SIMDIR obj/default/runtime/sim
source msim_setup.tcl
# Copy generated memory initialization hex and dat file(s) to current directory
file copy -force C:/Users/alber/OneDrive/Documentos/GitHub/Diseno_SoC_minimo_reproductor_audio/software/timer_display/mem_init/hdl_sim/timer_display_RAM.dat ./ 
file copy -force C:/Users/alber/OneDrive/Documentos/GitHub/Diseno_SoC_minimo_reproductor_audio/software/timer_display/mem_init/timer_display_RAM.hex ./ 
