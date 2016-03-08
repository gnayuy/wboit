
#include "glwidget.h"
#include <iostream>

//
GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    shaderProgram1 = NULL;
    shaderProgram2 = NULL;
    vertexShader = NULL;
    fragmentShader = NULL;

    xRot = 0;
    yRot = 0;
    zRot = 0;
    scale = 1.0f;

    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f,0.0f,0.0f));
    model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f,1.0f,0.0f));

    view = glm::mat4(1.0f);
    view = glm::lookAt(glm::vec3(4.0f,3.0f,3.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));

    projection = glm::mat4(1.0f);

    vaos = 0;
    vbos = 0;

    fb = 0;
    db = 0;

    accumTexture = 0;
    revealageTexture = 0;

    //installEventFilter(this);

    m_MousePressed = false;
    b_rot = false;
    b_scale = false;

    screen.setPositions(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f));

    m_frameCount = 0;

    mouseSpeed = 8; //
}

GLWidget::~GLWidget()
{
    delete vertexShader;
    vertexShader = NULL;

    delete fragmentShader;
    fragmentShader = NULL;

    if (shaderProgram1)
    {
        shaderProgram1->release();
        delete shaderProgram1;
        shaderProgram1 = NULL;
    }

    if (shaderProgram2)
    {
        shaderProgram2->release();
        delete shaderProgram2;
        shaderProgram2 = NULL;
    }

    glDeleteBuffers(nQuad, &vbos[0]);
    glDeleteVertexArrays(nQuad, &vaos[0]);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glDeleteFramebuffers(1, &fb);
    glDeleteTextures(1, &accumTexture);
    glDeleteTextures(1, &revealageTexture);
    glDeleteRenderbuffers(1, &db);
}

void GLWidget::initializeGL()
{
    /// objects

    // shaders
    vertexShader = new QGLShader(QGLShader::Vertex);
    if (!vertexShader->compileSourceFile(":/vshader.glsl"))
    {
        qWarning() << vertexShader->log();
        close();
    }

    fragmentShader = new QGLShader(QGLShader::Fragment);
    if (!fragmentShader->compileSourceFile(":/fshader.glsl"))
    {
        qWarning() << fragmentShader->log();
        close();
    }

    compVertShader = new QGLShader(QGLShader::Vertex);
    if (!compVertShader->compileSourceFile(":/compose_vert.glsl"))
    {
        qWarning() << compVertShader->log();
        close();
    }

    compFragShader = new QGLShader(QGLShader::Fragment);
    if (!compFragShader->compileSourceFile(":/compose_frag.glsl"))
    {
        qWarning() << compFragShader->log();
        close();
    }

    //
    shaderProgram1 = new QGLShaderProgram;
    shaderProgram1->addShader(vertexShader);
    shaderProgram1->addShader(fragmentShader);


    if (!shaderProgram1->link())
    {
        qWarning() << shaderProgram1->log() << endl;
        close();
    }
//    if (!shaderProgram1->bind())
//    {
//        qWarning() << shaderProgram1->log() << endl;
//        close();
//    }

    shaderProgram2 = new QGLShaderProgram;
    shaderProgram2->addShader(compVertShader);
    shaderProgram2->addShader(compFragShader);

    if (!shaderProgram2->link())
    {
        qWarning() << shaderProgram2->log() << endl;
        close();
    }

    //
//    int linked;
//    glGetProgramiv(shaderProgram1->programId(), GL_LINK_STATUS, &linked);
//    qDebug()<<"check shader link error"<<linked;

//    if (!shaderProgram2->bind())
//    {
//        qWarning() << shaderProgram2->log() << endl;
//        close();
//    }

    //
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_MULTISAMPLE);

    // geometry
    nQuad = quads.size();

    try
    {
        vaos = new GLuint [nQuad];
        vbos = new GLuint [nQuad];
    }
    catch(...)
    {
        std::cout<<"Fail to allocate memory for vaos/vbos."<<std::endl;
        return;
    }
    //
    glGenVertexArrays( nQuad, vaos );

    //
    glGenBuffers( nQuad, vbos );

    //
    for(int i=0; i<nQuad; i++)
    {
        glBindVertexArray( vaos[i] );
        glBindBuffer( GL_ARRAY_BUFFER, vbos[i] );

        //
        glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec3)*quads[i].positions.size() + sizeof(glm::vec4)*quads[i].colors.size(), NULL, GL_STATIC_DRAW );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*quads[i].positions.size(), &(quads[i].positions[0]) );
        glBufferSubData( GL_ARRAY_BUFFER, sizeof(glm::vec3)*quads[i].positions.size(), sizeof(glm::vec4)*quads[i].colors.size(), &(quads[i].colors[0]) );

        //
        loc = glGetAttribLocation( shaderProgram1->programId(), "vPosition" );
        glEnableVertexAttribArray( loc );
        glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0) );

        loc = glGetAttribLocation( shaderProgram1->programId(), "vColor" );
        glEnableVertexAttribArray( loc );
        glVertexAttribPointer( loc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)*quads[i].positions.size()));

        //
        glBindVertexArray(0);
    }

    //
    glGenVertexArrays(1, &vao);
    glBindVertexArray( vao );

    glGenBuffers(1, &vbo);
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, screen.positions.size()*sizeof(glm::vec3), &(screen.positions[0]), GL_STATIC_DRAW );

    loc = glGetAttribLocation( shaderProgram2->programId(), "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0) );

    //
    glBindVertexArray(0);

    //
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glGenTextures(1, &accumTexture);
    glBindTexture(GL_TEXTURE_2D, accumTexture);

    glGenTextures(1, &revealageTexture);
    glBindTexture(GL_TEXTURE_2D, revealageTexture);

    glGenRenderbuffers(1, &db);
    glBindRenderbuffer(GL_RENDERBUFFER, db);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLWidget::resizeGL( int w, int h )
{
    weight = w;
    height = h;

    glViewport(0,0,w,h);
    //projection = glm::perspective(glm::radians(45.0f), (float)w/(float)h, 0.3f, 100.0f);
    projection = glm::perspective(glm::pi<float>() * 0.25f, (float)w/(float)h, 0.1f, 1000.0f);

    //
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    //
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);

    //
    glBindTexture(GL_TEXTURE_2D, revealageTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealageTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    //
    glBindRenderbuffer(GL_RENDERBUFFER, db);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, db);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Set the list of draw buffers.
//    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
//    glDrawBuffers(2, (GLenum*)buffers);

    //
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLWidget::paintGL()
{
    //
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
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

    //
    if(b_rot)
    {
        model = glm::rotate(model, glm::radians(xRot/16), glm::vec3(1.0f,0.0f,0.0f));
        model = glm::rotate(model, glm::radians(yRot/16), glm::vec3(0.0f,1.0f,0.0f));
        model = glm::rotate(model, glm::radians(zRot/16), glm::vec3(0.0f,0.0f,1.0f));
    }

    //std::cout<<"model "<<glm::to_string(model)<<" scale "<<scale<<" rot "<<b_rot<<std::endl;

    if(b_scale)
    {
        model = glm::scale(model, glm::vec3(scale, scale, scale));
    }

    b_scale = false;

    //
    /// opaque surfaces
    //
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // screen background
    float alpha = 0.5;
    glClearColor(alpha,alpha,alpha,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    //
    /// transparent surfaces
    //
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFB); // in this case, currentFB = 0

    glBindFramebuffer(GL_FRAMEBUFFER, fb); // render to an offscreen framebuffer
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClearDepth(1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_MULTISAMPLE);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);

    // 3D Transparency Pass
    if (!shaderProgram1->bind())
    {
        qWarning() << shaderProgram1->log() << endl;
        close();
    }

    loc = glGetUniformLocation(shaderProgram1->programId(), "modelview");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr (view * model) );

    loc = glGetUniformLocation(shaderProgram1->programId(), "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection) );

//    std::cout<<"red    depth ... "<<glm::to_string( view * model * glm::vec4(-1,-1,-1, 1) )<< std::endl;
//    std::cout<<"yellow depth ... "<<glm::to_string( view * model * glm::vec4(-1,-1,0, 1) )<< std::endl;
//    std::cout<<"blue   depth ... "<<glm::to_string( view * model * glm::vec4(-1,-1,1, 1) )<< std::endl;
//    qDebug()<<" ... ";

    //
    glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

    //glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // with primitives sorted from farthest to nearest, for rendering antialiased points and lines in arbitrary order
    //glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE); // with polygons sorted from nearest to farthest
    //glShadeModel (GL_FLAT);
    //glBlendFunc(GL_ONE,  GL_ZERO);
    //glBlendEquation(GL_FUNC_ADD);

    //
    for(int i=0; i<nQuad; i++)
    {
        glBindVertexArray(vaos[i]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(0);

    // set the framebuffer back
    glBindFramebuffer(GL_FRAMEBUFFER, currentFB);
    glClearColor(alpha,alpha,alpha,1.0);
    glClearDepth(1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glDepthMask (GL_TRUE); //

    // 2D Compositing Pass
    if (!shaderProgram2->bind())
    {
        qWarning() << shaderProgram2->log() << endl;
        close();
    }

    //
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    loc = glGetUniformLocation(shaderProgram2->programId(), "accumTexture");
    glUniform1i(loc, 0);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, revealageTexture);
    loc = glGetUniformLocation(shaderProgram2->programId(), "revealageTexture");
    glUniform1i(loc, 1);

    //
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    //
    glBindVertexArray( vao );
    glDrawArrays( GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    //
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    //
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->pos();
    m_MousePressed = true;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    if (e->button() == Qt::LeftButton)    // Left button...
    {
        // Do something related to the left button
    }
    else if (e->button() == Qt::RightButton)   // Right button...
    {
        // Do something related to the right button
    }
    else if (e->button() == Qt::MidButton)   // Middle button...
    {
        // Do something related to the middle button
    }

    m_MousePressed = false;

    b_rot = false;

    xRot = 0;
    yRot = 0;
    zRot = 0;
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(m_MousePressed)
    {
        int dx = e->x() - lastPos.x();
        int dy = e->y() - lastPos.y();

        if (e->buttons() & Qt::LeftButton) {

            xRot += dy*mouseSpeed;
            yRot += dx*mouseSpeed;

        } else if (e->buttons() & Qt::RightButton) {

            xRot += dy*mouseSpeed;
            zRot += dx*mouseSpeed;

        }
        lastPos = e->pos();

        // normalize
        while (xRot < 0)
            xRot += 360 * 16;
        while (xRot > 360 * 16)
            xRot -= 360 * 16;

        while (yRot < 0)
            yRot += 360 * 16;
        while (yRot > 360 * 16)
            yRot -= 360 * 16;

        while (zRot < 0)
            zRot += 360 * 16;
        while (zRot > 360 * 16)
            zRot -= 360 * 16;

        b_rot = true;
    }
    else
    {
        b_rot = false;
    }

    update();
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    //e->delta() > 0 ? scale += scale*0.01f : scale -= scale*0.01f;

    e->delta() > 0 ? scale = 1.05f : scale = 0.95f;

    b_scale = true;

    update();
}

void GLWidget::addQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color)
{
    Quadrilateral quad;

    quad.setColor(color);
    quad.setPositions(a,b,c,d);
    quads.push_back(quad);
}
