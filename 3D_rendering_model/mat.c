#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mat.h"
#include "common.h"

float rad(float deg){
    return (M_PI*deg)/180;
}
void normalize(float v[]){
    float base=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    v[0]=v[0]/base;
    v[1]=v[1]/base;
    v[2]=v[2]/base;
}
void add(float v1[], float v2[], float res[])
{
    for (int i =0; i<3; i++){
        res[i]=v1[i]+v2[i];
    }
}
void sub(float v1[], float v2[], float res[])
{
    for (int i =0; i<3; i++){
        res[i]=v1[i]-v2[i];
    }
}
void cross (float v1[], float v2[], float res[])
{
    res[0]=v1[1]*v2[2]-v1[2]*v2[1];
    res[1]=v1[2]*v2[0]-v1[0]*v2[2];
    res[2]=v1[0]*v2[1]-v1[1]*v2[0];
}
void mult(float A[], float B[]){
    int row, col, inner, i;
    float res[16];
    float sum;
    for (col=0; col<4; col++) {
        for (row=0; row<4; row++) {
            sum = 0;
            for (inner=0; inner<4; inner++) {
                sum += A[inner*4+col]*B[row*4+inner];
            }
            res[row*4+col] = sum;
        }
    }
    
    for (i=0; i<16; i++)
        A[i] = res[i];
}

void zero(float mat[])
{
    int i;
    for (i=0; i<16; i++) {
        mat[i] = 0.0f;
    }
}
void identity(float mat[])
{
    zero(mat);
    mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}
void translate(float mat[], float t[3])
{
    float m[16];
    zero(m);
    identity(m);
    
    m[12] = t[0];
    m[13] = t[1];
    m[14] = t[2];
    mult(mat,m);
}
void scale(float mat[], float s[3]){
    float m[16];
    zero(m);
    identity(m);
    m[0]=s[0];
    m[5]=s[1];
    m[10]=s[2];
    mult(mat,m);
}

void rotate(float m[], float angle, float x, float y, float z)
{
    float mat[16];
    zero(mat);
    float len, nx, ny, nz, c, s;
    identity(mat);
    len = sqrt(x*x+y*y+z*z);
    nx = x/len;
    ny = y/len;
    nz = z/len;
    c = (float) cos((angle/180)*M_PI);
    s = (float) sin((angle/180)*M_PI);
    
    mat[0]=nx*nx*(1-c)+c;
    mat[1]=ny*nx*(1-c)+nz*s;
    mat[2]=nx*nz*(1-c)-ny*s;
    mat[4]=ny*nx*(1-c)-nz*s;
    mat[5]=ny*ny*(1-c)+c;
    mat[6]=ny*nz*(1-c)+nx*s;
    mat[8]=nz*nx*(1-c)+ny*s;
    mat[9]=nz*ny*(1-c)-nx*s;
    mat[10]=nz*nz*(1-c)+c;
    mult(m,mat);
}
void vector_rot(float m[], float angle, float x, float y, float z)
{
    float c,s;
    for (int i=0; i<9; i++) {
        m[i] = 0.0f;
    }
    c=(float) cos((angle/180)*M_PI);
    s=(float) sin((angle/180)*M_PI);
    if(x==1){
        m[0]=1;
        m[4]=c;
        m[5]=-s;
        m[7]=s;
        m[8]=c;
    }
    else if(y==1){
        m[0]=c;
        m[2]=s;
        m[4]=1;
        m[6]=-s;
        m[8]=c;
    }
    else if(z==1){
        m[0]=c;
        m[1]=-s;
        m[3]=s;
        m[4]=c;
        m[8]=1;
    }
}
void vector_mult(float v[],float m[]){
    float vec[3]={0,0,0};
    for (int i=0; i<3; i++){
        vec[i]=m[i*3]*v[0]+m[i*3+1]*v[1]+m[i*3+2]*v[2];
    }
    for (int i=0;i<3;i++){
        v[i]=vec[i];
    }
}
void lookat(float m[], float view[], float eye[], float at[], float up[]){
    float f[3], s[3], u[3], eyet[3];
    
    sub(at, eye, f);
    normalize(f);
    cross(f, up, s);
    normalize(s);
    cross(s, f, u);
    normalize(u);
    
    identity(view);
    view[0] = s[0];
    view[4] = s[1];
    view[8] = s[2];
    view[1] = u[0];
    view[5] = u[1];
    view[9] = u[2];
    view[2] = -f[0];
    view[6] = -f[1];
    view[10] = -f[2];
    

    for (int i=0; i<3; i++){
        eyet[i] = -eye[i];
    }
    translate(view, eyet);
    mult(m,view);
}

void frustum(float m[], float left, float right, float bottom, float top, float znear, float zfar){
        float temp, temp2, temp3, temp4;
        zero(m);
        temp = 2.0*znear;
        temp2 = right - left;
        temp3 = top - bottom;
        temp4 = zfar - znear;
        m[0] = temp / temp2;
        m[5] = temp / temp3;
        m[8] = (right + left) / temp2;
        m[9] = (top + bottom) / temp3;
        m[10] = (-zfar - znear) / temp4;
        m[11] = -1.0;
    	 //m[11] = 0.0;
        m[14] = (-temp * zfar) / temp4;
    //m[14] = 0;
    }
void perspective(float m[], float fovy, float aspect, float near, float far){
    float ymax = near*tan(rad(fovy)/2);
    float xmax = ymax*aspect;
    frustum(m, -xmax, xmax, -ymax, ymax, near, far);
}
    
    
    

