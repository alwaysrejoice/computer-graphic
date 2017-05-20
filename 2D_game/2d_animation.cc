#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <openGL/glu.h>
#include <GLFW/glfw3.h>

GLint winwidth=500, winheight=700;
GLint scaledirection, rotatedirection, translatedirection_x,translatedirection_y;
GLint animation=GL_TRUE;
GLint rotate=GL_FALSE;
GLint to_right=GL_FALSE;
GLint to_left=GL_FALSE;
GLdouble ang = 0.0;
GLdouble now, last_time;
int num_bombs=0;
int num_missile=0;
int num_aliens=0;
int currentpos=250;
int currenty=50;
int speed=3;
int counter=0;
int step=0;
int down=0;
int pause_game=1;
int level=1;
#define MAX_ALIENS 50
#define MAX_NUM 2000
enum {WHITE, RED, GREEN, BLUE};
typedef struct
{
    float x, y;
} Vertex;

typedef struct{
    Vertex v1;
    Vertex v2;
    float distance;
    int color;
} circle;
typedef struct
{
    Vertex v1;
    Vertex v2;
    int color;
    int hit;
} line;

int currentcolor = WHITE;
GLfloat drawingcolor[][3] = {{1.0, 1.0, 1.0},
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}};

circle CrlSegments[MAX_ALIENS];
line LineSegments[MAX_NUM];
circle bombs[MAX_NUM];
Vertex firstvert, secondvert;

Vertex make_vertex(float x, float y)
{
    return (Vertex){x, y};
}
line make_line(Vertex v1, Vertex v2) {
    Ln tmp;
    tmp->v1 = v1;
    tmp->v2 = v2;
    tmp->hit = 0;
    return tmp;
}

circle make_circle(Vertex v1, Vertex v2){
    Crl tmp;
    tmp->v1 = v1;
    tmp->v2 = v2;
    tmp->distance = sqrt((float)(v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y));
    return tmp;
}


void myInit(void)
{
    glViewport(0,0,winwidth,winheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, winwidth, 0.0, winheight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0,0.0,0.0,1.0);
    glPointSize(10.0);
    glLineWidth(4.0);
    last_time = glfwGetTime();
}

void aliens_init(int pos, int num_col, int num_row){
    float x=250-pos;
    float y=550;
    float x1=270-pos;
    
    int space_y=0;
    for (int i=0; i<20*num_row; i+=20){
        int space_x=0;
        if(i>0){
            space_y+=5;
        }
        for (int j=0; j<20*num_col; j+=20){
            if(j>0){
                space_x+=5;
            }
            
        	firstvert=make_vertex(x+2*j+space_x,y+2*i+space_y);
        	secondvert=make_vertex(x1+2*j+space_x,y+2*i+space_y);
            CrlSegments[num_aliens]=make_circle(firstvert, secondvert);
            CrlSegments[num_aliens++]->color = currentcolor;
        }
    }
}
GLint object_hit(Crl *aliens,Ln *missile){
    if(missile->v1.y>=aliens->v1.y-20 &&
       missile->v1.y<=aliens->v1.y+20 &&
       missile->v1.x>=aliens->v1.x-20 && missile->v1.x<=aliens->v1.x+20) return GL_TRUE;
    if(missile->v2.y>=aliens->v1.y-20 &&
       missile->v2.y<=aliens->v1.y+20 &&
       missile->v2.x>=aliens->v1.x-20 && missile->v2.x<=aliens->v1.x+20) return GL_TRUE;
    return GL_FALSE;
}
GLint object_hitship(Crl *aliens,int pos){
    int y=50;
    int y1=10;
    if(y>=aliens->v1.y-5 &&
       y<=aliens->v1.y+5 &&
       pos>=aliens->v1.x-25 && pos<=aliens->v1.x+25) return GL_TRUE;
    if(y1>=aliens->v1.y-5 &&
       y1<=aliens->v1.y+5 &&
       pos+25>=aliens->v1.x-5 && pos+25<=aliens->v1.x+5) return GL_TRUE;
    if(y1>=aliens->v1.y-5 &&
       y1<=aliens->v1.y+5 &&
       pos-25>=aliens->v1.x-5 && pos-25<=aliens->v1.x+5) return GL_TRUE;
    return GL_FALSE;
}
void really_draw()
{
    // Draw aliens
    for (int i = 0; i<num_aliens; i++)
    {
        if(CrlSegments[i]->distance!=0)
        {
            glColor3fv(drawingcolor[CrlSegments[i]->color]);
            float theta = 2 * 3.1415926 / 300.0;
            float tangetial_factor = tanf(theta);//calculate the tangential factor
            
            float radial_factor = cosf(theta);//calculate the radial factor
            
            float x = CrlSegments[i]->distance;//we start at angle = 0
            float cx = CrlSegments[i]->v1.x;
            float cy = CrlSegments[i]->v1.y;
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
        if(animation)
        {
            bombs[i]->v1.y = bombs[i]->v1.y +translatedirection_y;
        }
        
        glColor3fv(drawingcolor[bombs[i]->color]);
        float theta = 2 * 3.1415926 / 300.0;
        float tangetial_factor = tanf(theta);//calculate the tangential factor
        
        float radial_factor = cosf(theta);//calculate the radial factor
        
        float x = bombs[i]->distance;//we start at angle = 0
        float cx = bombs[i]->v1.x;
        float cy = bombs[i]->v1.y-20;
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
        if(LineSegments[i]->hit==0)
        {
            if(animation){
                LineSegments[i]->v1.y+=speed;
                LineSegments[i]->v2.y+=speed;
            }
            glColor3fv(drawingcolor[0]);
            glBegin(GL_LINES);
            glVertex2i(LineSegments[i]->v1.x, LineSegments[i]->v1.y);
            glVertex2i(LineSegments[i]->v2.x, LineSegments[i]->v2.y);
            glEnd();
        }
    }
}
void draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    now = glfwGetTime();
    
    if(counter==num_aliens && level==1){
        animation=GL_FALSE;
        level++;
        memset(&LineSegments[0],0,sizeof(LineSegments));
        memset(&bombs[0],0,sizeof(bombs));
        memset(&CrlSegments[0],0,sizeof(CrlSegments));
        num_aliens=0;
        num_bombs=0;
        num_missile=0;
        aliens_init(110, 6, 4);
    }
    
    for (int i = 0; i<num_aliens; i++){
        if(CrlSegments[i]->distance!=0){
        	if(CrlSegments[i]->v1.x>480)
        	{
            	translatedirection_x=-1;
        	}
        	if(CrlSegments[i]->v1.x<=20)
        	{
            	translatedirection_x=1;
        	}
            if(animation)
            {
        		if(CrlSegments[i]->v1.y>=20)
        		{
        			CrlSegments[i]->v1.x = CrlSegments[i]->v1.x +translatedirection_x;
            		CrlSegments[i]->v1.y = CrlSegments[i]->v1.y +translatedirection_y*0.2;
            	}
            }
    
        
        }
    }
    
   
    if(animation){
    double timestep=(rand()%(200/level))*0.01;
    
    if (now-last_time>timestep){
        //printf("bomb,%d\n",num_bombs);
        int i=rand()%num_aliens;
        if(CrlSegments[i]->distance!=0){
        bombs[num_bombs]=make_circle(CrlSegments[i]->v1,CrlSegments[i]->v2);
        bombs[num_bombs]->distance = 5;
        bombs[num_bombs++]->color = currentcolor;
        }
    }
    }
    if(to_right && currentpos<475){
        currentpos += speed;
    }
    if(to_left && currentpos>25){
        currentpos -= speed;
    }
    if(rotate){
        ang+=2.0;
        currenty-=1;
        animation=GL_FALSE;
    }
    
    //check if they hit each other
    for(int i=0; i<num_aliens;i++)
    {
        if(CrlSegments[i]->distance!=0){
        	for (int j=0; j<num_missile;j++)
        	{
                if(LineSegments[j]->hit==0 && LineSegments[j]->v2.y<700){
            		if(object_hit(CrlSegments[i],LineSegments[j]))
            		{
                		CrlSegments[i]->distance=0;
                		LineSegments[j]->hit=1;
                		counter++;
                    }
                }
        	}
        }
    }
    for(int i=0; i<num_bombs;i++)
    {
        if(CrlSegments[i]->distance!=0){
            
            if(object_hitship(bombs[i],currentpos))
            {
                bombs[i]->distance=0;
               	rotate=GL_TRUE;
                
            }
        }
        
    
    }

	last_time = now;
    if(pause_game==0){
        animation=GL_FALSE;
    }
    really_draw();
}

void keyboard(GLFWwindow *w, int key, int scancode, int action, int mods) {
    
    if (action == GLFW_PRESS){
        switch (key) {
            case GLFW_KEY_RIGHT:
                if(pause_game) to_right=GL_TRUE;
                break;
            case GLFW_KEY_LEFT:
                if(pause_game) to_left=GL_TRUE;
                break;
            case GLFW_KEY_SPACE:
                if(pause_game)
                {
                	firstvert=make_vertex(currentpos,45);
                	secondvert=make_vertex(currentpos,50);
                    LineSegments[num_missile++]=make_line(firstvert, secondvert);
                }
                break;
            case GLFW_KEY_D:
                animation=!animation;
                pause_game=0;
                if(animation==GL_TRUE){
                for (int i =0; i<num_aliens; i++){
                    if(CrlSegments[i]->distance!=0){
                    printf("Alian at (%f,%f)\n",CrlSegments[i]->v1.x,CrlSegments[i]->v1.y);
                    }
                }
                for (int i =0; i<num_bombs; i++){
                    if(bombs[i]->v1.y>0){
                    printf("Bombs at (%f,%f)\n",bombs[i]->v1.x,bombs[i]->v1.y);
                    }
                }
                for (int i =0; i<num_missile; i++){
                    if(LineSegments[i]->v1.y<700&& LineSegments[i]->hit==0){
                    printf("Missile at (%f,%f)\n",LineSegments[i]->v1.x,LineSegments[i]->v1.y);
                    }
                }
                printf("Space ship at (%d,%d)\n",currentpos, currenty);
                }
                break;
            case GLFW_KEY_R:
                animation=GL_TRUE;
                pause_game=1;
                break;
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(w, GL_TRUE);
                break;
        }
    }
    if(action==GLFW_RELEASE)
    {
        switch (key)
        {
            case GLFW_KEY_RIGHT:
                to_right=GL_FALSE;
                
            case GLFW_KEY_LEFT:
                to_left=GL_FALSE;
   
        }
    }
}

int main(int argc,char **argv){
    GLFWwindow* window;
    const GLubyte* renderer;
    const GLubyte* version;
    
    if (!glfwInit()){
        exit(EXIT_FAILURE);
    }
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(winwidth, winheight, "2d_animation_hw2", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* get version info */
    renderer = glGetString (GL_RENDERER); /* get renderer string */
    version = glGetString (GL_VERSION); /* version as a string */
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
    
    myInit();
    aliens_init(90,5,3);
    glfwSetKeyCallback(window, keyboard);     /* Callback functions */
    //glfwSetMouseButtonCallback(window, mouse);
    
    /* Loop until the user closes the window */
    translatedirection_x=1;
    translatedirection_y=-1;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        
        draw();
        
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}




