// #pragma once
#ifndef  _IP_H_
#define  _IP_H_

#include "stdafx.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glaux.h>

#include <windows.h>   // include important windows stuff
#define   EPS    0.00001
#define   GU_BITMAP_ID            0x4D42 // universal id for a bitmap
#define   BOOL int 

using namespace std;



class IP_Image
{
public:	
	int WIDTH ;        // ͼ����
	int HEIGHT;        // ͼ��߶�
	double PPI ;       // LCD��PPI 
	double LENSPITCH ;      // LCD��PPI 
	int    LENSTYPE  ;      // ͸������״
	double LENSTHICK ;
	unsigned char* DATA ;          // IP�洢
	
	char  * IP_FILE_NAME ;  // �����ļ�����
	int   LENS_NUMBER_X;
	int   LENS_NUMBER_Y;
	int   RADIUS_X;
	int   RADIUS_Y;
	double PIXEL_DX ;
	// ����͹��ƽ��ľ���
	double FLYOUT_DISTANCE ;
	GLfloat WINDOW_SIZESET;
	float  **Rotation_Matrix;
	double RATIO ;  // ������ʾ�ı���
	double SCALE ;  // ʵ��IPAD����ϵ �� OpenGLͼ������ϵ
	// ����ʼ���Ķ���
	GLenum lastBuffer;           // �������õĻ�����
	unsigned char * pBits;        // Perspectiveͼ�����ݵ���ʱ�洢
	unsigned long lImageSize;    
	GLint iViewport[4]; // ��ͼ��С
	double LENSPITCHY;
	// 
	int FIRST_FLAG_OF_IP;  // ��ʼ��Ϊ0�������ڼ��ν������ط���Function
	float *subimage_Indexs;
	int   DIRECTION_TEST[2];
	float Magnitudes_4_Interp [4] ;
public:	
	IP_Image(); // ���캯��
	IP_Image(int width,int height, double ppi, double lenspitch, double lenstype,double lensthick, double flyout_distance,char* filename ); // ���캯��
	// ********************************************************** // 
	void Rotation_Matrix_Update(float eyex , float eyey, float eyez, float centerx, float centery, float centerz, float upx,float upy,float upz); // 
	void Cross_Matrix( GLfloat*Matrix ,GLfloat*Matrix1,GLfloat*Matrix2 ) ; 
	BOOL IP_make_FCN(int i,int j , int sizex, int sizey); // 
	void Pixel_Redistribution(int i,int j);
	void Normalize_Fcn(float *Vector) ;
	void Cal_Index(float x, float y, float z);
	void Interp_Bilinear_Fcn () ;
	void SET_IMAGE_SIZE (int x_init, int y_init, int x_width, int y_height) ; 
	bool IPwriteBMP();
	~IP_Image(); // ��������
	void IP_Parameter_Show();
};
#endif
