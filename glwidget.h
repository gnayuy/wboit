#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QPoint>
#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <glm.hpp>

#include "mesh.h"

class GLWidget : public QOpenGLWidget {
    Q_OBJECT

   public:
    GLWidget();
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

   public:
    QOpenGLShader *vertexShader;
    QOpenGLShader *fragmentShader;
    QOpenGLShader *compVertShader;
    QOpenGLShader *compFragShader;
    QOpenGLShaderProgram *shaderProgram1, *shaderProgram2;
    QOpenGLFunctions_4_5_Core *f;

   private:
    bool m_MousePressed;
    bool b_rot, b_scale;

    QPoint lastPos;

    float xRot;
    float yRot;
    float zRot;

    float scale;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    std::vector<Quadrilateral> quads;

    GLuint *vaos;                           // vertex array object(s)
    GLuint *vbos;                           // vertex buffer object(s)
    GLuint fb;                              // frame buffer
    GLuint db;                              // depth buffer
    GLuint accumTexture, revealageTexture;  // rendered texture

    GLint currentFB;

    GLuint vao, vbo;  // composite

    int nQuad;
    GLuint loc;

    Quadrilateral screen;

    QTime m_time;
    int m_frameCount;

    float mouseSpeed;

    int weight, height;
};
