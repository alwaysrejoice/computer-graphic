#include <openGL/glu.h>
#include <math.h>
#include "Common.h"

static GLfloat drawingcolor[][3] = {{1.0, 1.0, 1.0},
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}};

void init_graphics()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glPointSize(10.0);
    glLineWidth(4.0);
}
void resize_graphics(uint width, uint height)
{
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw_graphics(size_t num_aliens, circle* CrlSegments, size_t num_bombs, circle* bombs, size_t num_missile, line* LineSegments, float currentpos, float currenty, float ang)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw aliens
    for (int i = 0; i<num_aliens; i++)
    {
        if(CrlSegments[i].distance!=0)
        {
            glColor3fv(drawingcolor[CrlSegments[i].color]);
            float theta = 2 * 3.1415926 / 300.0;
            float tangetial_factor = tanf(theta);//calculate the tangential factor
            
            float radial_factor = cosf(theta);//calculate the radial factor
            
            float x = CrlSegments[i].distance;//we start at angle = 0
            float cx = CrlSegments[i].v1.x;
            float cy = CrlSegments[i].v1.y;
            float y = 0;
            glBegin(GL_POLYGON);
            for(int ii = 0; ii < 300; ii++)
            {
                glVertex2f(x + cx, y + cy);//output vertex
                
                float tx = -y;
                float ty = x;
                
                //add the tangential vector
                
                x += tx * tangetial_factor;
                y += ty * tangetial_factor;
                
                //correct using the radial factor
                
                x *= radial_factor;
                y *= radial_factor;
            }
            glEnd();
        }
    }
    
    // Draw Bombs
    for (int i = 0; i<num_bombs; i++)
    {
        
        glColor3fv(drawingcolor[bombs[i].color]);
        float theta = 2 * 3.1415926 / 300.0;
        float tangetial_factor = tanf(theta);//calculate the tangential factor
        
        float radial_factor = cosf(theta);//calculate the radial factor
        
        float x = bombs[i].distance;//we start at angle = 0
        float cx = bombs[i].v1.x;
        float cy = bombs[i].v1.y-20;
        float y = 0;
        glBegin(GL_POLYGON);
        for(int ii = 0; ii < 300; ii++)
        {
            glVertex2f(x + cx, y + cy);//output vertex
            
            float tx = -y;
            float ty = x;
            
            //add the tangential vector
            
            x += tx * tangetial_factor;
            y += ty * tangetial_factor;
            
            //correct using the radial factor
            
            x *= radial_factor;
            y *= radial_factor;
        }
        glEnd();
    }
    
    // Draw somethings??
    glColor3fv(drawingcolor[1]);
    glPushMatrix();
    glTranslatef(currentpos, currenty, 0);
    glRotatef(ang, 0, 0, 1);
    glTranslatef(-currentpos, -currenty, 0);
    glBegin(GL_POLYGON);
    glVertex2i(currentpos, currenty);
    glVertex2i(currentpos-25, currenty-40);
    glVertex2i(currentpos+25, currenty-40);
    glEnd();
    glPopMatrix();
    
    // Draw missiles
    for (int i=0; i<num_missile; i++)
    {
        if(LineSegments[i].hit==0)
        {
            glColor3fv(drawingcolor[0]);
            glBegin(GL_LINES);
            glVertex2i(LineSegments[i].v1.x, LineSegments[i].v1.y);
            glVertex2i(LineSegments[i].v2.x, LineSegments[i].v2.y);
            glEnd();
        }
    }
}
