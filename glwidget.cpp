
#include "glwidget.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLFunctions_ES2>
#include <ext.hpp>
#include <glm.hpp>
#include <iostream>

//
GLWidget::GLWidget() : QOpenGLWidget() {
    shaderProgram1 = NULL;
    shaderProgram2 = NULL;
    vertexShader = NULL;
    fragmentShader = NULL;

    xRot = 0;
    yRot = 0;
    zRot = 0;
    scale = 1.0f;

    model = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    view = glm::mat4(1.0f);
    view = glm::lookAt(glm::vec3(4.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    projection = glm::mat4(1.0f);

    vaos = 0;
    vbos = 0;

    fb = 0;
    db = 0;

    accumTexture = 0;
    revealageTexture = 0;

    // installEventFilter(this);

    m_MousePressed = false;
    b_rot = false;
    b_scale = false;

    screen.setPositions(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f));

    m_frameCount = 0;

    mouseSpeed = 8;
}

GLWidget::~GLWidget() {
    delete vertexShader;
    vertexShader = NULL;

    delete fragmentShader;
    fragmentShader = NULL;

    if (shaderProgram1) {
        shaderProgram1->release();
        delete shaderProgram1;
        shaderProgram1 = NULL;
    }

    if (shaderProgram2) {
        shaderProgram2->release();
        delete shaderProgram2;
        shaderProgram2 = NULL;
    }

    f->initializeOpenGLFunctions();

    f->glDeleteBuffers(nQuad, &vbos[0]);
    f->glDeleteVertexArrays(nQuad, &vaos[0]);

    f->glDeleteBuffers(1, &vbo);
    f->glDeleteVertexArrays(1, &vao);

    f->glDeleteFramebuffers(1, &fb);
    f->glDeleteTextures(1, &accumTexture);
    f->glDeleteTextures(1, &revealageTexture);
    f->glDeleteRenderbuffers(1, &db);
}

void GLWidget::initializeGL() {
    // objects
    QOpenGLContext *c = QOpenGLContext::currentContext();
    f = 0;
    f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_5_Core>(c);
    if (!f) {
        // qDebug() << "Context doesn't support the requested version+profile";
        exit(-1);
    }

    // shaders
    vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!vertexShader->compileSourceFile(":/vshader.glsl")) {
        qWarning() << vertexShader->log();
        close();
    }

    fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if (!fragmentShader->compileSourceFile(":/fshader.glsl")) {
        qWarning() << fragmentShader->log();
        close();
    }

    compVertShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!compVertShader->compileSourceFile(":/compose_vert.glsl")) {
        qWarning() << compVertShader->log();
        close();
    }

    compFragShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if (!compFragShader->compileSourceFile(":/compose_frag.glsl")) {
        qWarning() << compFragShader->log();
        close();
    }

    shaderProgram1 = new QOpenGLShaderProgram;
    shaderProgram1->addShader(vertexShader);
    shaderProgram1->addShader(fragmentShader);

    if (!shaderProgram1->link()) {
        qWarning() << shaderProgram1->log() << "\n";
        close();
    }
    
    if (!shaderProgram1->bind())
    {
        qWarning() << shaderProgram1->log();
        close();
    }

    shaderProgram2 = new QOpenGLShaderProgram;
    shaderProgram2->addShader(compVertShader);
    shaderProgram2->addShader(compFragShader);

    if (!shaderProgram2->link()) {
        qWarning() << shaderProgram2->log();
        close();
    }

    int linked;
    f->glGetProgramiv(shaderProgram1->programId(), GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        qDebug() << "check shader link error" << linked;

        GLchar infoLog[512];
        GLint size; //gives 0 when checked in debugger
        f->glGetProgramInfoLog(shaderProgram1->programId(), 512, &size, infoLog);
        qDebug() << "Failed to link shader program:" << infoLog;
    }

    if (!shaderProgram2->bind())
    {
        qWarning() << shaderProgram2->log();
        close();
    }

    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_TEXTURE_2D);
    f->glEnable(GL_MULTISAMPLE);

    // geometry
    nQuad = (int)quads.size();

    try {
        vaos = new GLuint[nQuad];
        vbos = new GLuint[nQuad];
    } catch (...) {
        std::cout << "Fail to allocate memory for vaos/vbos." << std::endl;
        return;
    }

    f->glGenVertexArrays(nQuad, vaos);

    f->glGenBuffers(nQuad, vbos);

    for (int i = 0; i < nQuad; i++) {
        f->glBindVertexArray(vaos[i]);
        f->glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);

        f->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * quads[i].positions.size() + sizeof(glm::vec4) * quads[i].colors.size(), NULL,
                        GL_STATIC_DRAW);
        f->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * quads[i].positions.size(), &(quads[i].positions[0]));
        f->glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * quads[i].positions.size(), sizeof(glm::vec4) * quads[i].colors.size(),
                           &(quads[i].colors[0]));

        loc = f->glGetAttribLocation(shaderProgram1->programId(), "vPosition");
        f->glEnableVertexAttribArray(loc);
        f->glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(0));

        loc = f->glGetAttribLocation(shaderProgram1->programId(), "vColor");
        f->glEnableVertexAttribArray(loc);
        f->glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(glm::vec3) * quads[i].positions.size()));

        f->glBindVertexArray(0);
    }

    f->glGenVertexArrays(1, &vao);
    f->glBindVertexArray(vao);

    f->glGenBuffers(1, &vbo);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    f->glBufferData(GL_ARRAY_BUFFER, screen.positions.size() * sizeof(glm::vec3), &(screen.positions[0]), GL_STATIC_DRAW);

    loc = f->glGetAttribLocation(shaderProgram2->programId(), "vPosition");
    f->glEnableVertexAttribArray(loc);
    f->glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(0));

    f->glBindVertexArray(0);

    f->glGenFramebuffers(1, &fb);
    f->glBindFramebuffer(GL_FRAMEBUFFER, fb);

    f->glGenTextures(1, &accumTexture);
    f->glBindTexture(GL_TEXTURE_2D, accumTexture);

    f->glGenTextures(1, &revealageTexture);
    f->glBindTexture(GL_TEXTURE_2D, revealageTexture);

    f->glGenRenderbuffers(1, &db);
    f->glBindRenderbuffer(GL_RENDERBUFFER, db);

    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLWidget::resizeGL(int w, int h) {
    weight = w;
    height = h;

    f->glViewport(0, 0, w, h);

    projection = glm::perspective(glm::pi<float>() * 0.25f, (float)w / (float)h, 0.1f, 1000.0f);

    f->glBindFramebuffer(GL_FRAMEBUFFER, fb);

    f->glBindTexture(GL_TEXTURE_2D, accumTexture);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);

    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);

    f->glBindTexture(GL_TEXTURE_2D, revealageTexture);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, 0);

    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealageTexture, 0);

    f->glBindTexture(GL_TEXTURE_2D, 0);

    f->glBindRenderbuffer(GL_RENDERBUFFER, db);
    f->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    f->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, db);
    f->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Set the list of draw buffers.
    //    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    //    glDrawBuffers(2, (GLenum*)buffers);

    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLWidget::paintGL() {
    if (f->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    // FPS
    //    if (m_frameCount == 0)
    //    {
    //        m_time.start();
    //    }
    //    else
    //    {
    //        printf("FPS is %f\n", m_frameCount / (float(m_time.elapsed()) / 1000.0f));
    //    }
    //    m_frameCount++;

    if (b_rot) {
        model = glm::rotate(model, glm::radians(xRot / 16), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(yRot / 16), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(zRot / 16), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // std::cout<<"model "<<glm::to_string(model)<<" scale "<<scale<<" rot "<<b_rot<<std::endl;
    if (b_scale) {
        model = glm::scale(model, glm::vec3(scale, scale, scale));
    }

    b_scale = false;

    // opaque surfaces
    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);  // screen background
    float alpha = 0.5;
    f->glClearColor(alpha, alpha, alpha, 1.0);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    f->glDepthMask(GL_TRUE);
    f->glDisable(GL_BLEND);

    
    // transparent surfaces
    f->glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFB);  // in this case, currentFB = 0

    f->glBindFramebuffer(GL_FRAMEBUFFER, fb);  // render to an offscreen framebuffer
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    f->glClearDepth(1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_TEXTURE_2D);
    f->glEnable(GL_MULTISAMPLE);

    f->glDepthMask(GL_FALSE);
    f->glEnable(GL_BLEND);

    // 3D Transparency Pass
    if (!shaderProgram1->bind()) {
        qWarning() << shaderProgram1->log();
        close();
    }

    loc = f->glGetUniformLocation(shaderProgram1->programId(), "modelview");
    f->glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view * model));

    loc = f->glGetUniformLocation(shaderProgram1->programId(), "projection");
    f->glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

    //    std::cout<<"red    depth ... "<<glm::to_string( view * model * glm::vec4(-1,-1,-1, 1) )<< std::endl;
    //    std::cout<<"yellow depth ... "<<glm::to_string( view * model * glm::vec4(-1,-1,0, 1) )<< std::endl;
    //    std::cout<<"blue   depth ... "<<glm::to_string( view * model * glm::vec4(-1,-1,1, 1) )<< std::endl;
    //    qDebug()<<" ... ";

    f->glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

    // glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // with primitives sorted from farthest to nearest, for rendering antialiased points and
    // lines in arbitrary order glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE); // with polygons sorted from nearest to farthest glShadeModel (GL_FLAT);
    // glBlendFunc(GL_ONE,  GL_ZERO);
    // glBlendEquation(GL_FUNC_ADD);

    for (int i = 0; i < nQuad; i++) {
        f->glBindVertexArray(vaos[i]);
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    f->glBindVertexArray(0);

    // set the framebuffer back
    f->glBindFramebuffer(GL_FRAMEBUFFER, currentFB);
    f->glClearColor(alpha, alpha, alpha, 1.0);
    f->glClearDepth(1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glDepthMask (GL_TRUE); 

    // 2D Compositing Pass
    if (!shaderProgram2->bind()) {
        qWarning() << shaderProgram2->log();
        close();
    }

    f->glActiveTexture(GL_TEXTURE0);
    f->glEnable(GL_TEXTURE_2D);
    f->glBindTexture(GL_TEXTURE_2D, accumTexture);
    loc = f->glGetUniformLocation(shaderProgram2->programId(), "accumTexture");
    f->glUniform1i(loc, 0);

    f->glActiveTexture(GL_TEXTURE1);
    f->glEnable(GL_TEXTURE_2D);
    f->glBindTexture(GL_TEXTURE_2D, revealageTexture);
    loc = f->glGetUniformLocation(shaderProgram2->programId(), "revealageTexture");
    f->glUniform1i(loc, 1);


    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);


    f->glBindVertexArray(vao);
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    f->glBindVertexArray(0);


    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_TEXTURE_2D);

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_TEXTURE_2D);


    f->glDisable(GL_TEXTURE_2D);
    f->glDisable(GL_BLEND);
    f->glEnable(GL_DEPTH_TEST);
}

void GLWidget::mousePressEvent(QMouseEvent *e) {
    lastPos = e->pos();
    m_MousePressed = true;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e) {
    e->accept();

    if (e->button() == Qt::LeftButton)  // Left button...
    {
        // Do something related to the left button
    } else if (e->button() == Qt::RightButton)  // Right button...
    {
        // Do something related to the right button
    } else if (e->button() == Qt::MiddleButton)  // Middle button...
    {
        // Do something related to the middle button
    }

    m_MousePressed = false;

    b_rot = false;

    xRot = 0;
    yRot = 0;
    zRot = 0;
}

void GLWidget::mouseMoveEvent(QMouseEvent *e) {
    if (m_MousePressed) {
        int dx = e->position().x() - lastPos.x();
        int dy = e->position().y() - lastPos.y();

        if (e->buttons() & Qt::LeftButton) {
            xRot += dy * mouseSpeed;
            yRot += dx * mouseSpeed;

        } else if (e->buttons() & Qt::RightButton) {
            xRot += dy * mouseSpeed;
            zRot += dx * mouseSpeed;
        }
        lastPos = e->pos();

        // normalize
        while (xRot < 0) xRot += 360 * 16;
        while (xRot > 360 * 16) xRot -= 360 * 16;

        while (yRot < 0) yRot += 360 * 16;
        while (yRot > 360 * 16) yRot -= 360 * 16;

        while (zRot < 0) zRot += 360 * 16;
        while (zRot > 360 * 16) zRot -= 360 * 16;

        b_rot = true;
    } else {
        b_rot = false;
    }

    update();
}

void GLWidget::wheelEvent(QWheelEvent *e) {
    // e->delta() > 0 ? scale += scale*0.01f : scale -= scale*0.01f;

    e->angleDelta().y() > 0 ? scale = 1.05f : scale = 0.95f;

    b_scale = true;

    update();
}

void GLWidget::addQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color) {
    Quadrilateral quad;

    quad.setColor(color);
    quad.setPositions(a, b, c, d);
    quads.push_back(quad);
}
