// created 10/27/2015 by Yang Yu <gnayuy@gmail.com>

#include <QApplication>
#include "glwidget.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    //
    QApplication a(argc, argv);

    //
    QGLFormat glFormat;
    glFormat.setVersion( 3, 3 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );

    //
    GLWidget widget(0);
    widget.resize(640, 480);

    // gray
//    widget.addQuad(glm::vec3(-1,-1,-1), glm::vec3(+1,-1,-1), glm::vec3(+1,+1,-1), glm::vec3(-1,+1,-1), glm::vec4(0.1,0.1,0.1,0.05));
//    widget.addQuad(glm::vec3(-1,-1, 0), glm::vec3(+1,-1, 0), glm::vec3(+1,+1, 0), glm::vec3(-1,+1, 0), glm::vec4(0.05,0.05,0.05,0.05));
//    widget.addQuad(glm::vec3(-1,-1,+1), glm::vec3(+1,-1,+1), glm::vec3(+1,+1,+1), glm::vec3(-1,+1,+1), glm::vec4(0.02,0.02,0.02,0.05));

    // color
    widget.addQuad(glm::vec3(-1,-1,-1), glm::vec3(+1,-1,-1), glm::vec3(+1,+1,-1), glm::vec3(-1,+1,-1), glm::vec4(1.0,0.0,0.0,0.75));
    widget.addQuad(glm::vec3(-1,-1, 0), glm::vec3(+1,-1, 0), glm::vec3(+1,+1, 0), glm::vec3(-1,+1, 0), glm::vec4(1.0,1.0,0.0,0.75));
    widget.addQuad(glm::vec3(-1,-1,+1), glm::vec3(+1,-1,+1), glm::vec3(+1,+1,+1), glm::vec3(-1,+1,+1), glm::vec4(0.0,0.0,1.0,0.75));

    widget.show();

    //
    const GLubyte *renderer = glGetString( GL_RENDERER );
    const GLubyte *vendor = glGetString( GL_VENDOR );
    const GLubyte *version = glGetString( GL_VERSION );
    const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

//    GLint major, minor;
//    glGetIntegerv(GL_MAJOR_VERSION, &major);
//    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("GL Vendor : %s\n", vendor);
    printf("GL Renderer : %s\n", renderer);
    printf("GL Version (string) : %s\n", version);
//    printf("GL Version (integer) : %d.%d\n", major, minor);
    printf("GLSL Version : %s\n", glslVersion);

    //
    return a.exec();
}

