V:BUDS is written in MATLAB (GUI and front end) and C (computational engine).

The MATLAB executable, VBUDS.m, executes from the MATLAB command line.
Prior to executing V:BUDS, it may be necessary to run the command

mex -setup

from the command line.  V:BUDS functions under the MATLAB-bundled C compiler,
Lcc C version 2.4.

Once V:BUDS is successfully run for the first time, it is possible to comment
out line 378 of the VBUDS.m file.  This command compiles the c code, 
VBUDS_solve_rx.c, which performs most of the computation.  Removing the compilation
command will speed execution, but remember to recompile after changing VBUDS_solve_rx.c!

This release does not offer a C-standalone executable.

Some options shown in the GUI menus are not yet active; contact the developers for
further information.  

&&&&&&&&  FILES INCLUDED IN THIS DISTRIBUTION &&&&&&&&&&&&
VBUDS_solve_rx.c	Main computational module of V:BUDS
VBUDS_solve_rx.dll	Dynamic linked library created by the MATLAB mex c-compiler option
VBUDS.m			MATLAB executable script for running V:BUDS
get_case.m
VBUDSgui.m
VBUDSplotgui.m
VBUDSgui.fig
VBUDSplotgui.fig	Scripts and controls for the two GUI windows
costdata
lookup			Data files for Dancoff factor and cost database lookup.  costdata not currently used.
nr.h
nrutil.h		VBUDS uses methods and subroutines from Teukolsky et. al., "Numerical Recipes in C, 2nd Ed." 
saved_input_key.txt	Legend / description for saveable inputs.
catnames.mat		Category names for saved inputs.
save####.mat		VBUDS.m-created saved case files.
readme.txt		

