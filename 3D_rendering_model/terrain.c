#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void square(float v1[], float v2[], float out[], int max){
        out[0]=(v1[0]+v2[0])/2;
		out[1]=(v1[1]+v2[1])/2;
    out[2]=(rand()%max)*0.5;
}

void diamond(float v1[], int half, float up[],float down[], float left[], float right[]){
    up[0]=v1[0];
    up[1]=v1[1]+half;
    up[2]=(rand()%half);
    down[0]=v1[0];
    down[1]=v1[1]-half;
    down[2]=(rand()%half);
    left[0]=v1[0]-half;
    left[1]=v1[1];
    left[2]=(rand()%half);
    right[0]=v1[0]+half;
    right[1]=v1[1];
    right[2]=(rand()%half);

}
void divide(int offset, int size, float arr[][3], int max){
    int space=(size+1)*(size+1)-1;
    int dist=space+offset;
    int length=max+1;
    int half=size/2;
    if(half<1) return;
    for (int j=0; j<max; j+=size){
    	for (int i=0; i<max; i+=size){
    		square(arr[length*j+i],arr[length*j+i+dist],arr[length*j+i+dist/2],size);
    	}
    }
    for(int j=half; j<max; j+=size){
        for(int i=half; i<max; i+=size){
        diamond(arr[length*j+i],half,arr[length*(j-half)+i],arr[length*(j+half)+i],arr[length*j+i-half],arr[length*j+i+half]);
        }
    }
    offset=(max-half)*half;
    divide(offset,size/2,arr,max);
}

int main(int argc, char **argv ){
    
    FILE *fp;
    if(argc<3){
        printf("Usage %s <terrain level> <output file name>\n",argv[0]);
        return 1;
    };
    fp=fopen(argv[2],"w+");
    int arg = strtol(argv[1], NULL, 10);
    printf("%d\n",arg);
    int size=(int)pow(2,(double)arg)+1;
    int max=size-1;
    int numPoints=size*size;
    float points[numPoints][3];
    int numFaces=max*max*2;
    int face[numFaces][4];

    points[0][0]=0;
    points[0][1]=max;
    points[0][2]=0;
    points[max][0]=max;
    points[max][1]=max;
    points[max][2]=0;
    points[max*size][0]=0;
    points[max*size][1]=0;
    points[max*size][2]=0;
    points[numPoints-1][0]=max;
    points[numPoints-1][1]=0;
    points[numPoints-1][2]=0;
    divide(0,max, points,max);
    
    int k=0;
    for(int i=0; i<max; i++){
        for(int j=0; j<max; j++){
            face[k][0]=3;
            face[k][1]=size*i+j;
            face[k][2]=size*(i+1)+j;
            face[k++][3]=size*(i+1)+j+1;
            face[k][0]=3;
            face[k][1]=size*i+j;
            face[k][2]=size*(i+1)+j+1;
            face[k++][3]=size*i+j+1;
        }
    }
    
    fprintf(fp,"OFF\n");
    fprintf(fp, "%d %d 0\n",numPoints,numFaces);
    for(int i=0; i<numPoints; i++)	fprintf(fp,"%f %f %f\n",points[i][0],points[i][1],points[i][2]);
    for(int i=0; i<numFaces; i++) fprintf(fp,"%d %d %d %d\n",face[i][0],face[i][1],face[i][2],face[i][3]);
    fclose(fp);
    return 0;
    
    }