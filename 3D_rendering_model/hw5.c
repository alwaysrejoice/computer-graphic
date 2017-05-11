#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "mat.h"

#define REALLOC(ptr, size) ptr = realloc(ptr, size * sizeof(*ptr));

GLuint vao,vbo[2],program,pos=0,norm=1,ModelView,Projection,View,Sp,Dp,Lp,Ap,Sn;
int num_of_object=0;
int current_object=0;
int animation=FALSE;
GLfloat tmp;
GLfloat current_f[3]={0.0,0.0,-2.0};
int mode=0;
int render_mode=1;
GLfloat eye[3] = {0.0f, 0.0f, 2.0f};
GLfloat at[3] = {0.0f, 0.0f, 0.0f};
GLfloat up[3] = {0.0f, 1.0f, 0.0f};
GLfloat eye_spinx=0.0;
GLfloat eye_spiny=0.0;
GLfloat spinx = 0.0;
GLfloat spiny = 0.0;
GLfloat t[3] = {0.0f, 0.0f, 0.0f};
GLfloat s[3] = {1.0f, 1.0f, 1.0f};
GLfloat distance = 2.0;
// fixed point light properties
float light_position_world[] = {2.0, 2.0, 5.0, 1.0};
float Ls[] = {1.0, 1.0, 0.0}; // white specular light color
float Ld[] = {1.0, 1.0, 1.0}; // white diffuse light color
float La[] = {0.2, 0.2, 0.2}; // grey ambient light colour

// surface reflectance
float Ks[] = {1.0, 1.0, 1.0}; // fully reflect specular light
float Kd[] = {1.0, 1.0, 1.0}; // orange diffuse surface reflectance
float Ka[] = {1.0, 0.0, 0.0}; // fully reflect ambient light
float material_shininess = 76.8;

float specular_product[4];
float diffuse_product[4];
float ambient_product[4];

GLfloat transform[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};
GLfloat vector_rotate[9]={
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f
};
GLfloat projection[16];
GLfloat view[16];

typedef struct vertex
{
    GLfloat at[3];
} vx;

typedef struct faces
{
    GLint at[4];
}fa;

typedef struct object{
    size_t size, capacity;
    vx* points;
} obj;
typedef struct object_faces{
    size_t size, capacity;
    fa* faces;
} obj_f;

typedef struct image_vertex{
    int* index;
    size_t size, capacity;
    vx* p;
    vx* normal;
    GLfloat box_size[3];
    GLfloat center[3];
} img_v;


obj image_data[7];
obj_f image_face[7];
img_v image_vertices[7];

void flat_shading(vx* p, vx* normal, int size){
    float result[3],v1[3],v2[3];
    for(int j=0; j<size/3; j++){
        sub(p[j*3+1].at,p[j*3].at,v1);
        sub(p[j*3+2].at,p[j*3].at,v2);
    	cross(v1,v2, result);
    	normalize(result);
    	for(int i=0; i<3; i++){
    		normal[j*3+i].at[0]=result[0];
    		normal[j*3+i].at[1]=result[1];
    		normal[j*3+i].at[2]=result[2];
    	}
    }
}



void gourand_shading(img_v* a){
    int flag[a->size];
    int tmp[100];
    memset(tmp,0,100*sizeof(int));
    memset(flag,0,a->size*sizeof(int));
    for(int j=0; j<a->size; j++){
        int k=0;
        if(flag[j]==0){
            flag[j]=1;
        	int current=a->index[j];
        	vx curr_n;
        	curr_n.at[0]=a->normal[j].at[0];
        	curr_n.at[1]=a->normal[j].at[1];
        	curr_n.at[2]=a->normal[j].at[2];
            tmp[k++]=j;
    		for (int i=j+1; i<a->size; i++){
            	if(current==a->index[i]){
                	flag[i]=1;
                	curr_n.at[0]+=a->normal[i].at[0];
                	curr_n.at[1]+=a->normal[i].at[1];
                	curr_n.at[2]+=a->normal[i].at[2];
                    tmp[k++]=i;
            	}
            }
            for (int i=0; i<k; i++){
                a->normal[tmp[i]].at[0]=curr_n.at[0];
                a->normal[tmp[i]].at[1]=curr_n.at[1];
                a->normal[tmp[i]].at[2]=curr_n.at[2];
            }
    	}
    }
}
    
void update(void) {
    spiny += 1.0;
    if (spiny > 360.0)
        spiny -= 360.0;
}



void init_img(img_v* data, int size){
    data->size=0;
    data->capacity=size+2;
    data->p=(vx*)malloc(sizeof(vx)*data->capacity);
    data->normal=(vx*)malloc(sizeof(vx)*data->capacity);
    data->index=(int*)malloc(sizeof(int)*data->capacity);
}
void img_add(img_v* a, obj* v, obj_f* f){
    for(int i=0; i<f->size; i++){
        a->p[a->size].at[0]=v->points[f->faces[i].at[1]].at[0];
        a->p[a->size].at[1]=v->points[f->faces[i].at[1]].at[1];
        a->p[a->size].at[2]=v->points[f->faces[i].at[1]].at[2];
        a->index[a->size]=f->faces[i].at[1];
        a->size++;
        a->p[a->size].at[0]=v->points[f->faces[i].at[2]].at[0];
        a->p[a->size].at[1]=v->points[f->faces[i].at[2]].at[1];
        a->p[a->size].at[2]=v->points[f->faces[i].at[2]].at[2];
        a->index[a->size]=f->faces[i].at[2];
        a->size++;
        a->p[a->size].at[0]=v->points[f->faces[i].at[3]].at[0];
        a->p[a->size].at[1]=v->points[f->faces[i].at[3]].at[1];
        a->p[a->size].at[2]=v->points[f->faces[i].at[3]].at[2];
        a->index[a->size]=f->faces[i].at[3];
        a->size++;
    }
}

void init_obj(obj* data, int vertices)
{
    memset(data,0,sizeof(obj));
    data->points=(vx*)malloc(sizeof(vx)*vertices);
    data->size=0;
    data->capacity=vertices;
    
}
void init_obj_face(obj_f* data, int faces)
{
    memset(data,0,sizeof(obj_f));
    data->faces=(fa*)malloc(sizeof(fa)*faces);
    data->size=0;
    data->capacity=faces;
    
}
void obj_add(obj* a, GLfloat x, GLfloat y, GLfloat z){
    if(a->size>= a->capacity){
        a->capacity *= 2;
        REALLOC(a->points, a->capacity);
    }
    a->points[a->size].at[0]=x;
    a->points[a->size].at[1]=y;
    a->points[a->size].at[2]=z;
    a->size++;
}
void objface_add(obj_f* a, GLint num, GLint v1, GLint v2, GLint v3){
    if(a->size>= a->capacity){
        a->capacity *= 2;
        REALLOC(a->faces, a->capacity);
    }
    a->faces[a->size].at[0]=num;
    a->faces[a->size].at[1]=v1;
    a->faces[a->size].at[2]=v2;
    a->faces[a->size].at[3]=v3;
    a->size++;
}

void init( vx* points, vx* normals, int size) {
    // Create and initialize 2 buffer objects
    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vx)*size, points, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vx)*size, normals, GL_STATIC_DRAW);
    
    // Load shaders and use the resulting shader program
    if(render_mode==1 || render_mode==2) program = initshader( "hw5_pv_vs.glsl", "hw5_pv_fs.glsl" );
    else program = initshader("hw5_pf_vs.glsl","hw5_pf_fs.glsl");
    glUseProgram(program);
    
    // Create a vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Initialize the vertex position attribute from the vertex shader
    pos = glGetAttribLocation(program, "vPos");
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    norm = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(norm);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(norm, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    
    // Specify the values of Uniform variables for illumination
    // model in shader program
    Sp=glGetUniformLocation(program, "SpecularProduct");
    
    Dp=glGetUniformLocation(program, "DiffuseProduct");
    
    Ap=glGetUniformLocation(program, "AmbientProduct");
    
    Lp=glGetUniformLocation(program, "LightPosition");
    
    Sn=glGetUniformLocation(program, "Shininess");
    

    //Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "modelview");
    Projection = glGetUniformLocation(program, "projection");
    View = glGetUniformLocation(program,"view");
    
    
    glEnable (GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); //black background

}
void reset(){
    current_f[0]=0.0;
	current_f[1]=0.0;
	current_f[2]=-2.0;
    mode=0;
    eye[0] = 0.0f;
    eye[1] = 0.0f;
    eye[2] = 2.0f;
    at[0] = 0.0;
    at[1] = 0.0;
    at[2] = 0.0;
    up[0] = 0.0;
    up[1] = 1.0;
    up[2] = 0.0;
    eye_spinx=0.0;
    eye_spiny=0.0;
    spinx = 0.0;
    spiny = 0.0;
    t[0] = 0.0;
    t[1] = 0.0;
    t[2] = 0.0;
    s[0] = 1.0;
    s[1] = 1.0;
    s[2]= 1.0;
    distance = 2.0;
    animation=FALSE;
}

void keyboard(GLFWwindow *w, int key, int scancode, int action, int mods){
    if (action == GLFW_PRESS){
    switch (key){
        case GLFW_KEY_M:
            if(Ks[0]==0){
            	Ks[0]=1;
            	Ks[1]=1;
                Ks[2]=1;
            }
            else{
                Ks[0]=0;
                Ks[1]=0;
                Ks[2]=0;
            }
            break;
        case GLFW_KEY_B:
            if(mods==GLFW_MOD_SHIFT)
            {
            	Kd[2]+=0.1;
                Kd[0]+=0.1;
            }
            else{
                Kd[2]-=0.1;
                Kd[0]-=0.1;
            }
            break;
        case GLFW_KEY_1:
            render_mode=1;
            flat_shading(image_vertices[current_object].p, image_vertices[current_object].normal, image_vertices[current_object].size);
            init(image_vertices[current_object].p,image_vertices[current_object].normal,image_vertices[current_object].size);
            break;
        case GLFW_KEY_2:
            if(render_mode==2 || render_mode==3){
                flat_shading(image_vertices[current_object].p, image_vertices[current_object].normal, image_vertices[current_object].size);}
            gourand_shading(&image_vertices[current_object]);
            render_mode=2;
            init(image_vertices[current_object].p,image_vertices[current_object].normal,image_vertices[current_object].size);
            break;
        case GLFW_KEY_3:
            if(render_mode==1) gourand_shading(&image_vertices[current_object]);
            render_mode=3;
            init(image_vertices[current_object].p,image_vertices[current_object].normal,image_vertices[current_object].size);
            
            break;
        case GLFW_KEY_N:
            current_object++;
            render_mode=1;
            if(current_object==num_of_object){
                current_object=0;
            }
            init(image_vertices[current_object].p,image_vertices[current_object].normal,image_vertices[current_object].size);
            reset();
            break;
        case GLFW_KEY_P:
            render_mode=1;
            current_object--;
            if(current_object==-1){
                current_object=num_of_object-1;
            }
            init(image_vertices[current_object].p,image_vertices[current_object].normal,image_vertices[current_object].size);
            reset();
            break;
        case GLFW_KEY_V:
            mode=2;
            break;
        case GLFW_KEY_E:
            mode=1;
            break;
        case GLFW_KEY_F:
            mode=0;
            break;
        case GLFW_KEY_S:
            if (mods == GLFW_MOD_SHIFT){
                s[0]*=2;
            	s[1]*=2;
                s[2]*=2;
            }
            else{
                s[0]*=0.5;
            	s[1]*=0.5;
                s[2]*=0.5;
            }
            break;
        case GLFW_KEY_X:
            if (mods == GLFW_MOD_SHIFT)
                light_position_world[0]-=0.1;
            else
            	light_position_world[0]+=0.1;
            break;
        case GLFW_KEY_Y:
            if (mods == GLFW_MOD_SHIFT)
                light_position_world[1]-=0.1;
            else
            	light_position_world[1]+=0.1;
            break;
        case GLFW_KEY_Z:
            if (mods == GLFW_MOD_SHIFT)
                light_position_world[2]-=0.1;
            else
            	light_position_world[2]+=0.1;
            break;
        case GLFW_KEY_I:
            tmp=distance;
            if(mods==GLFW_MOD_SHIFT)
                distance-=0.1;
            else
                distance+=0.1;
            if(tmp==0){
                at[0]=current_f[0]*-1;
                at[1]=current_f[1]*-1;
                at[2]=current_f[2]*-1;
            }
            else{
            sub(at, eye, current_f);
            current_f[0]=current_f[0]*(distance/tmp);
            current_f[1]=current_f[1]*(distance/tmp);
            current_f[2]=current_f[2]*(distance/tmp);
            sub(at, current_f, eye);
            }
            break;
        case GLFW_KEY_R:
            eye_spiny-=4.0;
            //if (eye_spiny < 360.0)
              //  eye_spiny += 360.0;
            vector_rot(vector_rotate, eye_spiny, 0.0, 1.0, 0.0);
            vector_mult(current_f,vector_rotate);
            vector_mult(up,vector_rotate);
            sub(at, current_f, eye);
            break;
        case GLFW_KEY_L:
            eye_spiny+=4.0;
           // if (eye_spiny > 360.0)
             //   eye_spiny-= 360.0;
            vector_rot(vector_rotate, eye_spiny, 0.0, 1.0, 0.0);
            vector_mult(current_f,vector_rotate);
            vector_mult(up,vector_rotate);
            sub(at, current_f, eye);
            break;
        case GLFW_KEY_U:
            eye_spinx-=4.0;
          //  if (eye_spinx < 360.0)
            //    eye_spinx += 360.0;
            vector_rot(vector_rotate, eye_spinx, 1.0, 0.0, 0.0);
            vector_mult(current_f,vector_rotate);
            vector_mult(up,vector_rotate);
            sub(at, current_f, eye);
            break;
        case GLFW_KEY_D:
            eye_spinx+=4.0;
          //  if (eye_spinx > 360.0)
            //    eye_spinx-= 360.0;
            vector_rot(vector_rotate, eye_spinx, 1.0, 0.0, 0.0);
            vector_mult(current_f,vector_rotate);
            vector_mult(up,vector_rotate);
            sub(at, current_f, eye);
            break;
        case GLFW_KEY_UP:
            spinx+=2.0;
            if(spinx>360){
                spinx-=360;
            }
            break;
        case GLFW_KEY_DOWN:
            spinx-=2.0;
            if(spinx<360){
                spinx+=360;
            }
            break;
        case GLFW_KEY_LEFT:
            spiny-=2.0;
            if(spiny<360){
                spiny+=360;
            }
            break;
        case GLFW_KEY_RIGHT:
            spiny+=2.0;
            if(spiny>360){
                spiny-=360;
            }
            break;
        case GLFW_KEY_A:
                animation = !animation;
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(w, GL_TRUE);
            break;
    	}
    }
}

void reshape(GLFWwindow *w, int width, int height) {
    glViewport(0, 0, width, height);
    perspective(projection, 60, (float) width/((float) height), 0.1f, 100.0f);
}


int main(int argc, char* argv[])
{
     GLfloat T[16], R[16];
    if(argc < 2)
    {
        printf("usage: %s input_file\n",argv[0]);
        return 0;
    }
    if(argc > 8)
    {
        printf("7 input files are the maximum.\n");
    }
    num_of_object=argc-1;
    FILE *file[argc-1];
	int num_of_vertex[argc-1];
    int num_of_face[argc-1];
    int num_of_edge[argc-1];
    char first_line[3];
    
    
    for (int i=0; i<argc-1; i++)
    {
        GLfloat min_x, min_y, min_z, max_x, max_y, max_z;
        file[i]=fopen(argv[i+1],"r");
        if(file[i]==0)
        {
            printf("Could not open file\n");
        }
        else
        {
            fscanf(file[i],"%s",first_line);
            fscanf(file[i],"%d %d %d",&num_of_vertex[i],&num_of_face[i],&num_of_edge[i]);
            init_obj(&image_data[i],num_of_vertex[i]);
            init_obj_face(&image_face[i],num_of_face[i]);
            init_img(&image_vertices[i],num_of_face[i]*3);
            
            
            for (int j=0; j<num_of_vertex[i]; j++)
            {
                GLfloat tmp[3];
                fscanf(file[i],"%f %f %f",&tmp[0],&tmp[1],&tmp[2]);
                if(j==0)
                {
                	min_x=max_x=tmp[0];
                	min_y=max_y=tmp[1];
                	min_z=max_z=tmp[2];
                }
                else
                {
                    if(tmp[0]<min_x) min_x=tmp[0];
                    if(tmp[0]>max_x) max_x=tmp[0];
                    if(tmp[1]<min_y) min_y=tmp[1];
                    if(tmp[1]>max_y) max_y=tmp[1];
                    if(tmp[2]<min_z) min_z=tmp[2];
                    if(tmp[2]>max_z) max_z=tmp[2];
                }
                obj_add(&image_data[i],tmp[0],tmp[1],tmp[2]);
                
                    
            }
            float max=max_x-min_x;
            if(max_y-min_y>max){
                max=max_y-min_y;
            }
            if(max_z-min_z>max){
                max=max_z-min_z;
            }
            image_vertices[i].box_size[0]=1/max;
            image_vertices[i].box_size[1]=1/max;
            image_vertices[i].box_size[2]=1/max;
            image_vertices[i].center[0]=-1*(max_x+min_x)/2;
            image_vertices[i].center[1]=-1*(max_y+min_y)/2;
            image_vertices[i].center[2]=-1*(max_z+min_z)/2;
            for(int j=0;j<num_of_face[i];j++)
            {
                GLint tmp[4];
                fscanf(file[i],"%d %d %d %d",&tmp[0],&tmp[1],&tmp[2],&tmp[3]);
                objface_add(&image_face[i], tmp[0],tmp[1],tmp[2],tmp[3]);
            }
            img_add(&image_vertices[i],&image_data[i],&image_face[i]);
            flat_shading(image_vertices[i].p, image_vertices[i].normal, image_vertices[i].size);
        
        }
    }
    
    
    for(int i=0; i<argc-1; i++){
        printf("%f %f %f\n",image_vertices[i].box_size[0],image_vertices[i].box_size[1],image_vertices[i].box_size[2]);
         printf("%f %f %f\n",image_vertices[i].center[0],image_vertices[i].center[1],image_vertices[i].center[2]);
    }
    
    if(!glfwInit()){
        fprintf(stderr,"ERROR: could not start GLFW3\n");
        return 1;
    }
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *window = glfwCreateWindow (512, 512, "assignment5", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);
    
    // will segfault otherwise in init()
    glewExperimental = GL_TRUE;
    glewInit();
    
    printf("%s\n", glGetString(GL_VERSION));
    
    

    
    //GLfloat S[16];
    init(image_vertices[0].p,image_vertices[0].normal,image_vertices[0].size);
    
    reshape(window, 512, 512);
    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape);
    
    while (!glfwWindowShouldClose (window))
    {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        identity(transform);
        
        lookat(transform,view, eye, at, up);
        scale(transform,image_vertices[current_object].box_size);
        translate(transform,image_vertices[current_object].center);
        
        translate(transform, t);
        rotate(transform, spiny, 0.0, 1.0, 0.0);
        rotate(transform, spinx, 1.0, 0.0, 0.0);
        scale(transform,s);
        
        
        specular_product[0]=Ls[0]*Ks[0];
        specular_product[1]=Ls[1]*Ks[1];
        specular_product[2]=Ls[2]*Ks[2];
        specular_product[3]=1.0;
        diffuse_product[0]=Ld[0]*Kd[0];
        diffuse_product[1]=Ld[1]*Kd[1];
        diffuse_product[2]=Ld[2]*Kd[2];
        diffuse_product[3]=1.0;
        ambient_product[0] =La[0]*Ka[0];
        ambient_product[1]= La[1]*Ka[1];
        ambient_product[2]= La[2]*Ka[2];
        ambient_product[3]=1.0;

        
      
        glUseProgram(program);
        glBindVertexArray(vao);
        glUniformMatrix4fv(ModelView, 1, GL_FALSE, transform);
        glUniformMatrix4fv(Projection, 1, GL_FALSE, projection);
        glUniformMatrix4fv(View,1,GL_FALSE,view);
        glUniform4fv(Sp,1, specular_product );
        glUniform4fv(Dp,1, diffuse_product );
        glUniform4fv(Ap,1, ambient_product );
        glUniform4fv(Lp,1, light_position_world);
        
        glUniform1f(Sn,material_shininess );

        if(mode==1){
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }else if(mode==0){
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        } else if(mode==2){
            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
        }  // draw the points
        glDrawArrays(GL_TRIANGLES, 0, num_of_face[current_object]*3);
        glfwSwapBuffers (window);
        if (animation)
            update();
        // update other events like input handling
        glfwPollEvents ();
    }

    
    
    for(int i=0; i<argc-1; i++){
        fclose(file[i]);
    }
    glfwTerminate();
    return 0;
}




