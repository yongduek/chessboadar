// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFWsvn checkout http://morethantechnical.googlecode.com/svn/trunk/OpenCVAR morethantechnical-OpenCVAR

#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.hpp"

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "myCam.hpp"

#include "myCube.hpp"

int main( void )
{
    system ("pwd");
    
    // Initialise GLFW
    if( !glfwInit() )
        {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
        }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Chessboard AR", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    
    //
    // now opengl
    //
    std::cerr << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // -0----0
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // load computer vison part
    //
    myCam mycam;

    // ogl object to draw on the pattern
    Cube mycube;
    
    cerr << "! Rendering Loop Start." << endl;
    
    do{
        
        //
        // the origin is at the lower-left corner
        glViewport(0, 0, mycam.width, mycam.height);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        
        const bool drawAR=true;
        
        if (drawAR) {
            mycube.draw();
        } // if (drawAR)

        
        // draw video frame
        const bool drawVideoFrame = true;
        if (drawVideoFrame) {
            if (/* DISABLES CODE */ (0)) {
                // removed in GL > 3.2
                glDrawPixels(mycam.width, mycam.height, GL_BGR, GL_UNSIGNED_BYTE, mycam.image.data);
            }
            else {
                mycam.drawGL();
            }
        } // if (drawVideoFrame)
        

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

// EOF //