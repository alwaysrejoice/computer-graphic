#ifndef _MAT_H_
#define _MAT_H_

float rad(float deg);
void normalize(float v[]);
void add(float v1[], float v2[], float res[]);
void sub(float v1[], float v2[], float res[]);
void cross (float a[], float b[], float res[]);
void zero(float m[]);
void identity(float m[]);
void translate(float m[], float t[3]);
void scale(float m[], float s[3]);
void rotate(float m[], float angle, float x, float y, float z);
void vector_rot(float m[], float angle, float x, float y, float z);
void vector_mult(float v[],float m[]);
void mult(float A[], float B[]);

void lookat(float m[], float view[], float eye[], float at[], float up[]);
void perspective(float m[], float fovy, float aspect, float near, float far);
void frustum(float m[], float left, float right, float bottom, float top, float znear, float zfar);

#endif
