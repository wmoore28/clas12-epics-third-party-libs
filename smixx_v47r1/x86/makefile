#
# global makefile
#


all:
	echo " "; echo " " ; echo ==rtl==; gmake -f makefile_rtl all
	echo " "; echo " " ; echo ==generator==; gmake -f makefile_generator all
	echo " "; echo " " ; echo ==preprocessor==; gmake -f makefile_preprocessor all
	echo " "; echo " " ; echo ==stateManager==; gmake -f makefile_stateManager all
	echo " "; echo " " ; echo ==translator==; gmake -f makefile_translator all
	echo " "; echo " " ; echo ==gui==; gmake -f makefile_gui all
	echo " "; echo " " ; echo ==utilities==; gmake -f makefile_utilities all
	echo " "; echo " " ; echo ==tcl/Tk GUI==; gmake -f makefile_tclTkGUI all


clean:
	gmake -f makefile_rtl clean
	gmake -f makefile_generator clean
	gmake -f makefile_preprocessor clean
	gmake -f makefile_stateManager clean
	gmake -f makefile_translator clean
	gmake -f makefile_gui clean
	gmake -f makefile_utilities clean
	gmake -f makefile_tclTkGUI clean








