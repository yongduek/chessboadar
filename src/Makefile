
CXXFLAGS = -I /usr/local/include -I /usr/local/include/GLFW 
LDFLAGS = -L /usr/local/lib -lglew -lglfw3 -framework opengl 

SRCS = main.cpp shader.cpp

arshow : $(SRCS) $(HFILES)
	clang++ -o $@ $(CXXFLAGS) $(LDFLAGS) $(SRCS)
