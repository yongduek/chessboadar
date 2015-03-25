SRCS = opticalFlow.cpp
HFILES = opticalFlow.h

CFLAGS = -I /usr/local/include

arview : $(SRCS) $(HFILES)
	clang++ -o $@ $(CFLAGS) $(SRCS) -framework glut -framework opengl /usr/local/lib/libopencv_*
