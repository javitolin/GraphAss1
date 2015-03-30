/*
 * assign1.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: jdorfsman
 */
#include "GL/gl.h"
#include "GL/glut.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
GLuint texture;
GLubyte *pic;
unsigned char header[54];
GLuint dataPos;
GLuint width, height;
GLuint imageSize;
unsigned char * data;
unsigned char * edges;
char * filename;
using namespace std;
GLuint loadTexture() {
	FILE *f;
	f = fopen(filename, "rb");
	if (f == 0) {
		printf("Couldn't open file\n");
		exit(-1);
	}
	GLubyte header[54];
	fread(header, 54, 1, f);
	if (header[0] != 'B' || header[1] != 'M') {
		printf("File not bitmap\n");
		exit(1);
	}
	dataPos = *(int*) &(header[0x0A]);
	imageSize = *(int*) &(header[0x22]);
	width = *(int*) &(header[0x12]);
	height = *(int*) &(header[0x16]);
	if (imageSize == 0)
		imageSize = width * height * 3;
	if (dataPos == 0)
		dataPos = 54;
	data = new unsigned char[imageSize];
	edges = new unsigned char[imageSize];
	fread(data, 1, imageSize, f);
	fclose(f);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_LUMINANCE,
	GL_UNSIGNED_BYTE, data);
	glEnable(GL_TEXTURE_2D);
	return textureID;
}
GLuint applyEdgeFilter(){
	int gxSobel[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
	int gySobel[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
	GLuint threshold = 150;
	for(GLuint i = 0; i < width; i++){
		for(GLuint j = 0; j < height; j++){
			edges[i+j*width] = data[i+j*width];
			if(edges[i+j*width] > threshold) edges[i+j*width] = 255;
			else edges[i+j*width] = 0;
		}
	}
	for(GLuint i = 0; i < width; i++){
		for(GLuint j = 0; j < height; j++){
			int sobelXPixel = 0, sobelYPixel = 0;

			for(GLint sobelx = -1; sobelx < 2; sobelx++){
				for(GLint sobely = -1; sobely < 2; sobely++){
					if(i > 0 && j > 0 && i < width - 2 && j < height - 2){
						sobelXPixel += (gxSobel[sobelx+1][sobely+1]*edges[(i+sobelx)+(j+sobely)*width])/8;
						sobelYPixel += (gySobel[sobelx+1][sobely+1]*edges[(i+sobelx)+(j+sobely)*width])/8;
					}
				}
			}
			int sobelAns = sqrt(pow(sobelXPixel,2) + pow(sobelYPixel,2));
			edges[i+j*width] = min(sobelAns,255);
			edges[i+j*width] = max(sobelAns,0);
		}
	}
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_LUMINANCE,
	GL_UNSIGNED_BYTE, edges);
	return textureID;
}
void mydisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//BOTTOM LEFT
	glViewport(0, 0, 256, 256);
	GLuint edgeTexture = applyEdgeFilter();
	glBindTexture(GL_TEXTURE_2D, edgeTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 1);
	glVertex2f(1, 1);
	glEnd();

	//BOTTOM RIGHT
	glViewport(256, 0, 256, 256);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 1);
	glVertex2f(1, 1);
	glEnd();

	////TOP LEFT
	glViewport(0, 256, 256, 256);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 1);
	glVertex2f(1, 1);
	glEnd();

	//TOP RIGHT
	glViewport(256, 256, 256, 256);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(0, 0);
	glVertex2f(1, -1);
	glTexCoord2f(0, 1);
	glVertex2f(1, 1);
	glEnd();

	glFlush();
}
void init() {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Sample");

	glOrtho(-1.0, 1.0, -1.0, 1.0, 2.0, -2.0);
	texture = loadTexture();
	glutDisplayFunc(mydisplay);
	glutMainLoop();
}
int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: assign1 <img_file>");
		return -1;
	}
	filename = argv[1];
	glutInit(&argc, argv);
	init();
	return 0;
}
