// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "glInit.h"
#include "myCam.hpp"
#include "myCube.hpp"

myGLInterface myGL;

int main( void )
{
    system ("pwd");
    
    myGL.init();
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
        glfwSwapBuffers(myGL.window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(myGL.window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(myGL.window) == 0 );

    myGL.close();
    return 0;
}

// EOF //