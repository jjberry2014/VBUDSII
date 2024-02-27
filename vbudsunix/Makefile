# We'll compile in 32 bit mode otherwise valgrind can't analyze vbuds
# invoking mex from matlab compiles in 64 bit as standard.

#INCLUDES = -I/Applications/MATLAB_R2009a.app/extern/include -I/Applications/MATLAB_R2009a.app/simulink/include
INCLUDES = -I/usr/local/MATLAB/R2011a/extern/include -I/usr/local/MATLAB/R2011a/simulink/include
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
CXX = gcc
LIBS = -lmx -lmex -lmat -lstdc++
#LIBS_DIR = /Applications/MATLAB_R2009a.app/bin/maci64 
#LIBS_DIR = /Applications/MATLAB_R2009a.app/bin/maci
LIBS_DIR = /usr/local/MATLAB/R2011a/bin/glnxa64
RUNTIME_LIBS_DIR = $(LIBS_DIR)
LIBS_STRING = -L$(LIBS_DIR)
RUNTIME_LIBS_STRING = -R$(RUNTIME_LIBS_DIR)

#CXXFLAGS = -Wall -g -c -arch x86_64
#LDFLAGS = -Wall -g -O -arch x86_64
#CXXFLAGS = -Wall -g -c -m32
#LDFLAGS = -Wall -g -O -m32
CXXFLAGS = -Wall -g -c
LDFLAGS = -Wall -g -O
EXE_STR = $(LIBS_STRING) $(RUNTIME_LIBS_STRING) $(LIBS)

vbuds : $(OBJS)
	$(CXX) $(LDFLAGS) -o vbuds $(OBJS) $(EXE_STR)
	#echo 'remember to export DYLD_LIBRARY_PATH=/Applications/MATLAB_R2009b.app/bin/maci64/'
	echo 'remember to export LD_LIBRARY_PATH=/usr/local/MATlAB/R2011a/bin/glnxa64/'

.c.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCLUDES)

clean:
	rm vbuds VBUDS_solve_rx.o

