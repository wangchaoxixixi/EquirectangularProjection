#include "equiretangularprojection.h"
#include <QDebug>
EquiretangularProjection::EquiretangularProjection(QWidget *parent):
    QGLWidget(parent)
{
    QGLFormat format;
    format.setVersion(3,3);
    format.setProfile(QGLFormat::CompatibilityProfile);
    setFormat(format);
    /*sphere variable*/
    drawSphere = false;
    xRot = xRot0 = 90.0;
    zRot = zRot0 = 0.0;
    zoom = -30.0;
    radius = 50;
    x0 = 0;
    y0 = 0;
    scale = 1.0;
    angle = 0.0;
}

void EquiretangularProjection::initializeGL()
{
    initializeOpenGLFunctions();
    shader_cal = new Shader("calculate.vert", "calculate.frag");
    shader_dis = new Shader("display.vert", "display.frag");
    shader_sph = new Shader("sphere.vert", "sphere.frag");

    GLfloat vertices[] = {
        // Positions          // Colors           // Texture Coords
         1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
         1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // Top Left
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO


    // Load and create a texture
    glGenTextures(1, &texture_src);
    glBindTexture(GL_TEXTURE_2D, texture_src); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    QImage img_src;
    img_src.load("data//src.jpg");
    width = img_src.width();
    height = img_src.height();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_src.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

    // Load and create logo
    glGenTextures(1, &texture_logo);
    glBindTexture(GL_TEXTURE_2D, texture_logo); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    QImage image_logo;
    image_logo.load("data//logo2.jpeg");
    w_logo = image_logo.width();
    h_logo = image_logo.height();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_logo, h_logo, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_logo.bits());
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

    // Framebuffers
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Create a color attachment texture
    glGenTextures(1, &texture_res);
    glBindTexture(GL_TEXTURE_2D, texture_res);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_res, 0);
    // Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qDebug("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void EquiretangularProjection::paintGL()
{
    // Render
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport( 0, 0, (GLint)width, (GLint)height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, (GLfloat)width/(GLfloat)height, 0.1, 100 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    shader_cal->Use();
    glUniform2f(glGetUniformLocation(shader_cal->Program, "size_src"), width, height);
    glUniform2f(glGetUniformLocation(shader_cal->Program, "size_logo"), w_logo, h_logo);
    glUniform1f(glGetUniformLocation(shader_cal->Program, "scale"), scale);
    glUniform1f(glGetUniformLocation(shader_cal->Program, "angle"), angle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_src);
    glUniform1i(glGetUniformLocation(shader_cal->Program, "texture_src"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_logo);
    glUniform1i(glGetUniformLocation(shader_cal->Program, "texture_logo"), 1);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if(drawSphere)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int screen_height = this->size().height();
        int screen_width = this->size().width();
        glViewport( 0, 0, (GLint)screen_width, (GLint)screen_height );
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluPerspective( 45.0, (GLfloat)screen_width/(GLfloat)screen_height, 0.1, 100 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        shader_sph->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_res);
        glUniform1i(glGetUniformLocation(shader_sph->Program, "ourTexture"), 0);
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricTexture(sphere, GL_TRUE);
        glLoadIdentity();
        glTranslatef(0.0f,0.0f,zoom);
        glRotatef( xRot, 1.0,  0.0,  0.0 );
        glRotatef( zRot, 0.0,  0.0,  1.0 );
        glColor3f(1.0,1.0,1.0);
        glPushMatrix();
        gluSphere(sphere, radius, 100, 100);
        glPopMatrix();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    else
    {
        //display
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glViewport( 0, 0, (GLint)this->size().width(), (GLint)this->size().height() );
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluPerspective( 45.0, (GLfloat)this->size().width()/(GLfloat)this->size().height(), 0.1, 100 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        shader_dis->Use();
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_res);
        glUniform1i(glGetUniformLocation(shader_dis->Program, "texture1"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
void EquiretangularProjection::resizeGL( int width, int height )
{
    if ( height == 0 )
    {
      height = 1;
    }
    glViewport( 0, 0, (GLint)width, (GLint)height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, (GLfloat)width/(GLfloat)height, 0.1, 100 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}
void EquiretangularProjection::mousePressEvent ( QMouseEvent * e )
{
    x0 = e->x();
    y0 = e->y();
}
void EquiretangularProjection::mouseMoveEvent ( QMouseEvent * e )
{
     int x1 = e->x();
     int y1 = e->y();
     int delta_x = x0 - x1;
     int delta_y = y0 - y1;
     zRot= zRot0 - (double)delta_x/9;
     xRot= xRot0 + (double)delta_y/9;
     x0 = e->x();
     y0 = e->y();
     zRot0 = zRot;
     xRot0 = xRot;
     updateGL();

}
void EquiretangularProjection::wheelEvent(QWheelEvent *event)
 {
     int delta = event->delta() / 15;
     zoom+=delta;
     if ((zoom<0?-zoom:zoom)>=radius)
         zoom =zoom<0?-(radius-1):radius-1;
     updateGL();
 }
