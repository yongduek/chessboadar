//
//  glInit.h
//  cbarXcode
//
//  Created by Yongduek Seo on 3/27/15.
//  Copyright (c) 2015 Yongduek Seo. All rights reserved.
//

// Include GLEW
#include <GL/glew.h>

// Include GLFWsvn checkout http://morethantechnical.googlecode.com/svn/trunk/OpenCVAR morethantechnical-OpenCVAR

#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.hpp"


#ifndef __cbarXcode__glInit__
#define __cbarXcode__glInit__

#include <stdio.h>

void myGLInit();

struct myGLInterface {
    GLFWwindow* window;
    GLuint VertexArrayID;

    void init();
    void close();
};
#endif /* defined(__cbarXcode__glInit__) */
