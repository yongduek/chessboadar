//
//  myCam.hpp
//  cbarXcode
//
//  Created by Yongduek Seo on 2015. 3. 25..
//  Copyright (c) 2015년 Yongduek Seo. All rights reserved.
//

#ifndef cbarXcode_myCam_hpp
#define cbarXcode_myCam_hpp

static void print_shader_log(GLuint shader)
{
    std::string str;
    GLint len;
    
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len != 0) {
        str.resize(len);
        glGetShaderInfoLog(shader, len, NULL, &str[0]);
    }
}


struct myCam {

    myCam () {
        std::string visionDataFileName = "visionData-test.yaml";
        cv::FileStorage fs(visionDataFileName, cv::FileStorage::READ);
        if (fs.isOpened()==false) {
            cerr << "vision data file not exist: " << visionDataFileName << endl;
            exit (0);
        }
        fs["K"] >> K;
        fs["rvec"] >> rvec;
        fs["tvec"] >> tvec;
        fs["imageFileName"] >> imageFileName; cerr << "imagefilename: " << imageFileName << endl;
        fs.release();
        
        string ifname = imageFileName;
        image = cv::imread(imageFileName);
        if (image.empty()) {
            for (int i=0; i<imageFileName.size(); i++)
                cerr << imageFileName[i] << endl;
            cerr << "image file open error: " << ifname << endl;
            exit (0);
        }
        
        cv::Rodrigues(rvec, Rmat);
        
        cerr << "K:" << K << endl;
        cerr << "R:" << Rmat << endl;
        cerr << "T:" << tvec << endl;
        cerr << "image:" << imageFileName << endl;
        cerr << " size=" << image.size() << endl;
        
        texImage = cv::Mat(texHeight=512,texWidth=1024,CV_8UC3, cv::Scalar(0,0,0));
        cv::Mat simg = texImage(cv::Rect(0,512-image.rows-1, image.cols, image.rows));
        image.copyTo(simg);
//        cv::flip(texImage, texImage, 0);
        //        cv::imshow("textureImage", texImage); // will be upside down
        //        cv::imshow("srcImage", image);
        
        if (texImage.isContinuous()==false) {
            cerr << "! the image pixel align is NOT continuous" << endl;
        }
        
        width = image.cols;
        height = image.rows;
        
        prepareGL();
    }
    
    void prepareGL() {
        // Create one OpenGL texture
        glGenTextures(1, &textureID);
        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        bool flagSynth=true;
        
        glTexStorage2D (GL_TEXTURE_2D, //2D
                        1,             // 1 MIPMAP LEVELS
                        flagSynth? GL_RGBA32F : GL_BGR,
                        texWidth, texHeight);
        
        float * data = flagSynth ? new float[texWidth*texHeight*4] : 0;
        generate_texture(data, texWidth, texHeight);
        // Give the image to OpenGL
        glTexSubImage2D(GL_TEXTURE_2D,  // 2D texture
                        0,              // Level 0
                        0, 0,           // Offset 0, 0
                        texWidth, texHeight,       // 256 x 256 texels, replace entire image
                        flagSynth? GL_RGBA : GL_RGB,        // 3 channel data
                        flagSynth? GL_FLOAT : GL_UNSIGNED_BYTE,       // data type
                        flagSynth? (unsigned char*)data : texImage.data);          // Pointer to data
        
        delete [] data;
        
        static const char * vs_source[] =
        {
        "#version 330 core                              \n"
        "                                                                               \n"
        "uniform mat4 MVP;                                                              \n"
        "                                                                               \n"
        "void main(void)                                                                \n"
        "{                                                                              \n"
        "    const vec4 vertices[] = vec4[](vec4( 0., 0., 0., 1.0),               \n"
        "                                   vec4( 1., 0., 0., 1.0),               \n"
        "                                   vec4( 1., 1., 0., 1.0),              \n"
        "                                   vec4( 0., 1., 0., 1.0));              \n"
        "                                                                               \n"
        "    gl_Position = MVP * vertices[gl_VertexID];                                 \n"
        "}                                                                              \n"
        };
        
        static const char * fs_source[] =
        {
        "#version 330 core                                                              \n"
        "                                                                               \n"
        "uniform sampler2D s;                                                           \n"
        "                                                                               \n"
        "out vec4 color;                                                                \n"
        "                                                                               \n"
        "void main(void)                                                                \n"
        "{                                                                              \n"
        "    color = texture(s, vec2(gl_FragCoord.x, 1-gl_FragCoord.y) /textureSize(s,0) ); \n"
        "    //color = texture(s, gl_FragCoord.xy / textureSize(s, 0));                   \n"
        "    //color = vec4(0.5,0,0,0.5); //texture(s, gl_FragCoord.xy / textureSize(s, 0)); \n"
        "}                                                                              \n"
        };
        
        programID = glCreateProgram();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, NULL);
        glCompileShader(fs);
        
        print_shader_log(fs);
        
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source, NULL);
        glCompileShader(vs);
        
        print_shader_log(vs);
        
        glAttachShader(programID, vs);
        glAttachShader(programID, fs);
        
        glLinkProgram(programID);
    }
    
    void drawGL() {
        glUseProgram (programID);
        glEnable (GL_DEPTH_TEST);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        //
        // sign due to a wierd implementation of glm::ortho().
        //
        glm::mat4 Proj = glm::ortho(0.f, 1.f, 0.f, 1.f, 1.f, 10.f);
        //glm::mat4 Proj = glm::mat4(1.0f);
        static float tz = -9.9f;
        static float dz = .01f;
//        cerr << "tz= " << tz << endl;
        glm::mat4 Tx = glm::translate(glm::mat4(1.0f), glm::vec3(0.f,0.f, tz));
        glm::mat4 MVP = Proj*Tx;
        //        cerr << "ortho:=" << endl << glm::to_string (MVP) << endl;
        
        GLuint matrixID = glGetUniformLocation(programID, "MVP");
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(MVP));
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        
        //        tz += dz;
        //        if (tz<-10.f) dz = 0.01f;
        //        if (tz>-9.f)  dz = -0.01f;
    }
    
    
    

    void drawWorldAxes ()
    {
    
    struct glcontext {
        GLuint program;
        bool flagInit;
        glcontext() { flagInit=false; }
    };

    static glcontext glc;
    

    if (glc.flagInit==false) {
        glc.flagInit=true;
        
        static const char * vs_source_drawCoordAxes[] =
        {
        "#version 330 core                              \n"
        "                                                                               \n"
        "uniform mat4 MVP;                                                              \n"
        "out vec4 color_vs;                                                                \n"
        "void main(void)                                                                \n"
        "{                                                                              \n"
        "    const vec4 vertices[] = vec4[](vec4( 0., 0., 0., 1.0),      //origin         \n"
        "                                   vec4( 1., 0., 0., 1.0),      //x-axis         \n"
        "                                   vec4( 0., 0., 0., 1.0),      //origin         \n"
        "                                   vec4( 0., 1., 0., 1.0),      //y-axis         \n"
        "                                   vec4( 0., 0., 0., 1.0),      //origin         \n"
        "                                   vec4( 0., 0., 1., 1.0)      //z-axis         \n"
        "                                   );              \n"
        "    const vec4 colors[] = vec4[](vec4( 1., 0., 0., 1.0),      // red         \n"
        "                                 vec4( 1., 0., 0., 1.0),      // green         \n"
        "                                 vec4( 0., 1., 0., 1.0),      // green         \n"
        "                                 vec4( 0., 1., 0., 1.0),      // green         \n"
        "                                 vec4( 0., 0., 1., 1.0)      //  blue         \n"
        "                                 vec4( 0., 0., 1., 1.0)      //  blue         \n"
        "                                 );              \n"
        "                                                                               \n"
        "    gl_Position = MVP * vertices[gl_VertexID];                                 \n"
        "    color_vs = colors[gl_VertexID];                                 \n"
        "}                                                                              \n"
        };
        
        static const char * fs_source_drawCoordAxes[] =
        {
        "#version 330 core                                                              \n"
        "                                                                               \n"
        "in  vec4 color_vs;                                                             \n"
        "out vec4 color;                                                                \n"
        "                                                                               \n"
        "void main(void)                                                                \n"
        "{                                                                              \n"
        "    color = vec4(1.,1.,.1,1.); //color_vs;                                                          \n"
        "    color = color_vs;                                                          \n"
        "}                                                                              \n"
        };
        

        glc.program = glCreateProgram();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source_drawCoordAxes, NULL);
        glCompileShader(fs);
        
        print_shader_log(fs);
        
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source_drawCoordAxes, NULL);
        glCompileShader(vs);
        
        print_shader_log(vs);
        
        glAttachShader(glc.program, vs);
        glAttachShader(glc.program, fs);
        
        glLinkProgram(glc.program);
        
        cerr << "myCam::drawWorldAxes::glc.program= " << glc.program << endl;
    }
    
    // draw main
    glUseProgram (glc.program);
    glEnable (GL_DEPTH_TEST);
    
    //
    //
    cv::Point3f ilt(0,0,1), ilb(0,height-1,1), irt(width-1,0,1), irb(width-1,height-1,1);
    cv::Mat_<double> Ci = this->K.inv();
    cv::Mat_<double> lt = Ci*cv::Mat_<double>(ilt); //, lb=Ci*ilb, rt=Ci*irt, rb=Ci*irb;
    cv::Mat_<double> rb = Ci*cv::Mat_<double>(irb); //, lb=Ci*ilb, rt=Ci*irt, rb=Ci*irb;
                                                    //    cerr << "Ci: " << Ci << endl;
                                                    //    cerr << "lt: " << (ilt) << " --> " << (lt.t()) << endl;
                                                    //    cerr << "rb: " << (irb) << " --> " << (rb.t()) << endl;
    float left = lt(0);
    float right = rb(0);
    float bottom = rb(1);
    float top = lt(1);
    cerr << "lrbt=(" << left << "," << right << "," << -bottom << "," << -top << ")" << endl;
    
    glm::mat4 Proj = glm::frustum(left, right, -bottom, -top, 0.9f, 100.f);
    Proj = glm::perspective(40.0f, 4.0f / 3.0f, 0.1f, 50.0f);
    //Proj=glm::mat4(1.0);
    
    cv::Mat_<double> A = cv::Mat_<double>::eye(3, 3);
    A(1,1)=A(2,2)= -1.0;
    cv::Mat_<double> R2 = A*this->Rmat;
    cv::Mat_<double> t2 = A*this->tvec;
    
    glm::mat4 Model = glm::mat4(1.0f);
    
    for (int i=0; i<3; i++) {
        //        for (int j=0; j<3; j++)
        //            Model[i][j] = R2(i,j);
        //        Model[i][3]=t2(i);
    }
    //    cerr << "R2=" << R2 << endl;
    //    cerr << "t2=" << t2.t() << endl;
    //    exit (0);
    static float z=-1, dz=-0.1;
    if (z>-1) dz = -0.1;
    if (z<-30) dz = +0.1;
    z += dz;
    //    cerr << "z=" << z << endl;
    //
    Model[1][3] = z;
    Model[2][3] = -2.5;
    
    cerr << "M: " << to_string(Model) << endl;
    
    
    glm::mat4 MVP = Proj*Model;
    
    GLuint mvpID = glGetUniformLocation(glc.program, "MVP");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(MVP));
    
    glPointSize(16);
    glDrawArrays(GL_POINTS, 0, 6);
    
    return;
    } // drawFrameAxes()
    

public:
    cv::Mat_<double> K, rvec, Rmat, tvec;
    cv::Mat image;
    std::string imageFileName;
    cv::Mat texImage;
    
    int width, height;
    int texWidth, texHeight;
    
    GLuint vertexbufferTexture;
    GLuint vertexbufferTextureUV;
    GLuint programID;
    GLuint textureID;
    
private:
    void generate_texture(float * data, int width, int height) {
        int x, y;
        
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                data[(y * width + x) * 4 + 0] = (float) texImage.at<cv::Vec3b>(y,x)[2] / 255.0f;
                data[(y * width + x) * 4 + 1] = (float) texImage.at<cv::Vec3b>(y,x)[1] / 255.0f;
                data[(y * width + x) * 4 + 2] = (float) texImage.at<cv::Vec3b>(y,x)[0] / 255.0f;
                data[(y * width + x) * 4 + 3] = 1.0f;
            }
        }
    } //
    
};

#endif
