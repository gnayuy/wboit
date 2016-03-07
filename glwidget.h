
#include "mesh.h"

//#include <GL/glew.h>

#include <QtOpenGL>
#include <QtCore>
#include <QtGui>

#include <QGLShader>
#include <QGLShaderProgram>

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#define GL_RGBA32F  GL_RGBA32F_ARB
#define GL_RGB32F  GL_RGB32F_ARB
#define GL_RGBA16F GL_RGBA16F_ARB
#endif

//
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent);
    ~GLWidget();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

public:
    void addQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color);

public slots:

public:
    QGLShader *vertexShader;
    QGLShader *fragmentShader;
    QGLShader *compVertShader;
    QGLShader *compFragShader;
    QGLShaderProgram *shaderProgram1, *shaderProgram2;

private:
    bool m_MousePressed;
    bool b_rot, b_scale;

private:
    QPoint lastPos;

    float xRot;
    float yRot;
    float zRot;

    float scale;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    std::vector<Quadrilateral> quads;

    GLuint *vaos; // vertex array object(s)
    GLuint *vbos; // vertex buffer object(s)
    GLuint fb; // frame buffer
    GLuint db; // depth buffer
    GLuint accumTexture, revealageTexture; // rendered texture

    GLint currentFB;

    GLuint vao, vbo; // composite

    int nQuad;
    GLuint loc;

    Quadrilateral screen;

    QTime m_time;
    int m_frameCount;

    float mouseSpeed;

    int weight, height;
};

