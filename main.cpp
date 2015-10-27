// created 10/27/2015 by Yang Yu <gnayuy@gmail.com>

#include <QApplication>
#include "glwidget.h"

#include <stdio.h>

int main(int argc, char **argv)
{

    QApplication a(argc, argv);

    GLWidget widget(0);
    widget.resize(640, 480);

    //
    widget.addQuad(glm::vec3(-1,-1,-1), glm::vec3(+1,-1,-1), glm::vec3(+1,+1,-1), glm::vec3(-1,+1,-1), glm::vec4(1.0,0.0,0.0,0.75));
    widget.addQuad(glm::vec3(-1,-1, 0), glm::vec3(+1,-1, 0), glm::vec3(+1,+1, 0), glm::vec3(-1,+1, 0), glm::vec4(0.0,1.0,0.0,0.75));
    widget.addQuad(glm::vec3(-1,-1,+1), glm::vec3(+1,-1,+1), glm::vec3(+1,+1,+1), glm::vec3(-1,+1,+1), glm::vec4(0.0,0.0,1.0,0.75));

    widget.show();

    //
    return a.exec();
}

