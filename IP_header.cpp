//  ��Χ���� ���ڹ���
#pragma region IPHEADER
#include "stdafx.h"     // �κ�cpp��ͷ�ļ�����Ҫ����
#include "IP_header.h"

// Ĭ��construction ����
#pragma region  IP_Image::IP_Image()
IP_Image::IP_Image()
{
	// test
	WIDTH        = 2048 ; 
	HEIGHT       = 1536 ; 
	PPI          = 264  ;  
	LENSPITCH    = 2.32 ;
	LENSTYPE     = 1      ; 
	LENSPITCHY   = LENSPITCH*sqrt(3.0)/2.0;
	LENSTHICK    = 2.4;
	PIXEL_DX      = (25.4/PPI);    // PIXEL�ĳߴ�
	FLYOUT_DISTANCE = 0.18;
	
	DATA = (unsigned char*) calloc(WIDTH*HEIGHT*3,sizeof(unsigned char)) ;
	
	LENS_NUMBER_X = WIDTH * PIXEL_DX/LENSPITCH;                 // X�����͸������
	LENS_NUMBER_Y = HEIGHT* PIXEL_DX/LENSPITCH*2.0/sqrt(3.0);   // Y�����͸������
	RADIUS_X      = LENSPITCH / PIXEL_DX / 2.0       ;          // X����İ뾶
	RADIUS_Y      = LENSPITCH / PIXEL_DX / sqrt(3.0) ;          // Y�������ĸ���

	IP_FILE_NAME   = "Guowen_IP.bmp";
	WINDOW_SIZESET = 0.8 ;              // �趨IP�����д��ڴ�С
	RATIO          = 1.3 ;              // �趨����ߴ����IPad�ļ���֮��
	SCALE          = (0.5/WINDOW_SIZESET*iViewport[2])/(PIXEL_DX*HEIGHT*RATIO ) ;  
	// �ٶ�windowsize=0.7f*2���ʹ��ڳߴ� 400*400 �����С��1
	Rotation_Matrix = (float  **) malloc(3*sizeof(float *));
	for (int i=0;i<3;i++) //
		Rotation_Matrix[i]=(float *) malloc(3*sizeof(float)); 
	//
	FIRST_FLAG_OF_IP = 0 ;                                    // ���ط������д�����־
	subimage_Indexs  = (float*)calloc(8 , sizeof(float));     //  �洢��ֵ�����±�
	DIRECTION_TEST[0] =-1 ; 
	DIRECTION_TEST[1] =-1 ;
}
#pragma endregion

#pragma region  IP_Image::IP_Image 2
// ��ʽconstruction ����
IP_Image::IP_Image(int width,int height, double ppi, double lenspitch,double lenstype,
double lensthick, double flyout_distance ,char* filename)
{
	WIDTH  = width;
	HEIGHT = height;
	PPI    = ppi;      // pixel per inch
	LENSPITCH = lenspitch;   // ͸��������
	LENSTYPE  = lenstype ;   // ͸������
	LENSTHICK = lensthick;   // ͸����� 
	FLYOUT_DISTANCE = flyout_distance ;
	PIXEL_DX  = 25.4/PPI ;   // PIXEL�ĳߴ� mm
	LENSPITCHY   = LENSPITCH*sqrt(3.0)/2.0;

	DATA = (unsigned char*) calloc(WIDTH*HEIGHT*3,sizeof(unsigned char)) ;

	LENS_NUMBER_X = WIDTH *PIXEL_DX/LENSPITCH;                // X�����͸������
	LENS_NUMBER_Y = HEIGHT* PIXEL_DX/LENSPITCH*2.0/sqrt(3.0); // Y�����͸������  // Modified 2015-07-28 
	RADIUS_X      = LENSPITCH / PIXEL_DX / 2.0       ;        // X����İ뾶
	RADIUS_Y      = LENSPITCH / PIXEL_DX / sqrt(3.0) ;        // Y�������ĸ���
	IP_FILE_NAME  = filename;
	
	RATIO        = 1; // �趨����ߴ����IPad�ļ���֮��
	SCALE        = (0.5/0.7*iViewport[2])/(PIXEL_DX*HEIGHT*RATIO ) ;  
	// �ٶ�windowsize=0.7f*2���ʹ��ڳߴ� 400*400 �����С��1
	Rotation_Matrix = (float  **) malloc(3*sizeof(float *));
	for (int i=0;i<3;i++) //
		Rotation_Matrix[i]=(float *) malloc(3*sizeof(float)); 
	//
	FIRST_FLAG_OF_IP =0;            // ���ط������д�����־
	subimage_Indexs  = (float*)calloc(2,sizeof(float)); // 
}
#pragma endregion

#pragma region  IP_Image::IP_Image()  // // дIPͼ�����
bool IP_Image::IPwriteBMP()
{
unsigned char* data = DATA ; 
unsigned int w = WIDTH;
unsigned int h = HEIGHT;
std::ofstream out_file;
/** ���data */
if(!data) 
{
	std::cerr << "data corrupted! " << std::endl;
	out_file.close();
	return false;
}

/** ����λͼ�ļ���Ϣ��λͼ�ļ�ͷ�ṹ */
BITMAPFILEHEADER header;
BITMAPINFOHEADER bitmapInfoHeader;
//unsigned char textureColors = 0;/**< ���ڽ�ͼ����ɫ��BGR�任��RGB */

/** ���ļ�,�������� */
out_file.open(IP_FILE_NAME, std::ios::out | std::ios::binary);
if (!out_file)
{
	std::cerr << "Unable to open file " << IP_FILE_NAME << std::endl;
	return false;
}

/** ���BITMAPFILEHEADER */
header.bfType = GU_BITMAP_ID;  // ���ķ�ֹ��������
header.bfSize = w*h*3 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
header.bfReserved1 = 0;
header.bfReserved2 = 0;
header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
/** д��λͼ�ļ�ͷ��Ϣ */
out_file.write((char*)&header, sizeof(BITMAPFILEHEADER));

/** ���BITMAPINFOHEADER */
bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
bitmapInfoHeader.biWidth = w;
bitmapInfoHeader.biHeight = h;
bitmapInfoHeader.biPlanes = 1;
bitmapInfoHeader.biBitCount = 24;
bitmapInfoHeader.biCompression = BI_RGB; // BI_RLE4 BI_RLE8
bitmapInfoHeader.biSizeImage = w * h * 3; // ��ѹ������ΪBI_RGB��Ҳ��������Ϊ0
bitmapInfoHeader.biXPelsPerMeter = 0;
bitmapInfoHeader.biYPelsPerMeter = 0;
bitmapInfoHeader.biClrUsed = 0;
bitmapInfoHeader.biClrImportant = 0;
/** д��λͼ�ļ���Ϣ */
out_file.write((char*)&bitmapInfoHeader, sizeof(BITMAPINFOHEADER));

/** ��ָ���Ƶ����ݿ�ʼλ�� */
out_file.seekp(header.bfOffBits, std::ios::beg);

/** д��ͼ������ */
out_file.write((char*)data, bitmapInfoHeader.biSizeImage);

out_file.close();
return true;
}
#pragma endregion


// ��������
IP_Image::~IP_Image()
{
	free(DATA);            // ��̬����������ͷ� 
	for (int i=0;i<3;i++)  // ��̬����������ͷ�
	{
	   free(Rotation_Matrix[i]);
	}
	free(Rotation_Matrix); 
	free(pBits);
	free(subimage_Indexs);
}

void IP_Image::IP_Parameter_Show()
{
	cout<<" ͼ���� ="<<WIDTH <<endl ;       // 
	cout<<" ͼ��߶� ="<<HEIGHT<<endl ;       // ͼ��߶�
	cout<< " PPI =   " <<PPI   <<endl ;       // LCD��PPI 

	cout<<"LENSPITCH��� = "   <<LENSPITCH     <<"mm"<<endl ;
	cout<<"LENSTHICK��� = "   <<LENSTHICK     <<"mm"<<endl ;
	cout<<"�����ļ����� = "    <<IP_FILE_NAME        <<endl;
	cout<<"LENS_NUMBER_X = "   <<LENS_NUMBER_X <<"��"<<endl;
	cout<<"LENS_NUMBER_Y = "   <<LENS_NUMBER_Y <<"��"<<endl;
	cout<<"RADIUS_X = "        <<RADIUS_X      <<"��"<<endl;
	cout<<"RADIUS_Y = "        <<RADIUS_Y      <<"��"<<endl;
	cout<<"PIXEL_DX"           <<PIXEL_DX      <<"mm"<<endl;
	cout<<"����͹��ƽ��ľ��� FLYOUT_DISTANCE"<<FLYOUT_DISTANCE<<endl;
	// 
	cout<<"SCALE = "<<SCALE<<endl;
	cout<<"RATIO = "<<RATIO<<endl;
 }


void IP_Image::Normalize_Fcn(float *Vector)
{
	// cout<<"Vector[0]="<<Vector[0]<<"  Vector[1]="<<Vector[1]<<"  Vector[2]="<<Vector[2]<<endl;
	float sum = sqrt ( Vector[0]*Vector[0] + Vector[1]*Vector[1] + Vector[2]*Vector[2] ) ;
	Vector[0] = Vector[0] / sum ;
	Vector[1] = Vector[1] / sum ;
	Vector[2] = Vector[2] / sum ;
}


// ��������
void IP_Image::Cross_Matrix( GLfloat*Matrix ,GLfloat*Matrix1,GLfloat*Matrix2)
{
	Matrix[0] = 1 * ( Matrix1[1]*Matrix2[2]-Matrix1[2]*Matrix2[1] );
	Matrix[1] = 1 * ( Matrix1[0]*Matrix2[2]-Matrix1[2]*Matrix2[0] );
	Matrix[2] = 1 * ( Matrix1[1]*Matrix2[0]-Matrix1[0]*Matrix2[1] );
	double sum = sqrt(  Matrix[0] *Matrix[0]  +  Matrix[1] *Matrix[1] +  Matrix[2] *Matrix[2] );
	Matrix[1]  = Matrix[1] /sum;
	Matrix[0]  = Matrix[0] /sum;
	Matrix[2]  = Matrix[2] /sum;
}

 #pragma endregion 

// ��ת����ÿ�θ���һ�¼���
void IP_Image::Rotation_Matrix_Update(float eyex,float  eyey,float  eyez,float  centerx, float centery, float centerz, float upx,float  upy,float  upz) // ���������λ�ú�Ŀ��λ�ü�����ת����
{ // ���簴�ո�ʽд����  
    Rotation_Matrix[2][0] = centerx -eyex ;
	Rotation_Matrix[2][1] = centery -eyey ;
	Rotation_Matrix[2][2] = centerz -eyez ;  // Z'��
	Normalize_Fcn( Rotation_Matrix[2] );  // ��ù�һ����Z'��
	float up_dot_Z_axis = (upx*Rotation_Matrix[2][0]+upy*Rotation_Matrix[2][1]+upz*Rotation_Matrix[2][2]); 
	Rotation_Matrix[1][0] = upx -Rotation_Matrix[2][0]*up_dot_Z_axis ;
	Rotation_Matrix[1][1] = upy -Rotation_Matrix[2][1]*up_dot_Z_axis ;
	Rotation_Matrix[1][2] = upz -Rotation_Matrix[2][2]*up_dot_Z_axis ; // Y'��
	Normalize_Fcn( Rotation_Matrix[1] );  // ��ù�һ����Y'��
	Cross_Matrix(Rotation_Matrix[0],Rotation_Matrix[1],Rotation_Matrix[2]); // ���X'��
}

// IP ��������
BOOL IP_Image::IP_make_FCN(int xx,int yy,int sizex, int sizey)
{
	// *******************  Store the Matrix after sheer **********************  //
	FIRST_FLAG_OF_IP++;
	if( FIRST_FLAG_OF_IP==1 ) // ����ǵ�һ�ν��� �����һЩ��ʼ���Ĳ���
	{
		lImageSize = iViewport[2] * iViewport[3] * 3; 
		cout<<"iViewport[2] = "<<iViewport[2]<<endl; // 
		cout<<"iViewport[3] = "<<iViewport[3]<<endl; 
		cout<<"lImageSize = "<<lImageSize<<endl;
		pBits = (unsigned char*)new unsigned char[lImageSize];
		if (!pBits)
		return false;
		system("pause"); // �鿴���� 
	}
	
	//****************************************************_***************// 
	// ��color buffer�ж�ȡ����
	glPixelStorei(GL_PACK_ALIGNMENT, 1);  // OpenGL�汾��IP�Ķ�ȡ���ݵĺ������Լ��ڲ����Բ�������Screen_Shot header
	glPixelStorei(GL_PACK_ROW_LENGTH, 0); 
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	//
	glGetIntegerv(GL_READ_BUFFER, (GLint*)&lastBuffer);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits); // GL_UNSIGNED_BYTE 
	glReadBuffer(lastBuffer);
	Pixel_Redistribution( xx, yy); // 
	return 0;
}

void IP_Image::Pixel_Redistribution(int xx,int yy)
{
	float CENTER1[2] ; // ͸������
	int   INDEX_i,INDEX_j ;       // DATA�е�����
	int   index_ij[8] ;         // pBits�е�����
	
	for(int i=0;i<LENS_NUMBER_X;i++)
	{
		for(int j=0;j<LENS_NUMBER_Y;j+=2)
		{
			// ��һ��
			INDEX_j    = (i+0.0)*LENSPITCH /PIXEL_DX + DIRECTION_TEST[0] * xx ; // x���� ��Ӧ��ͼ���е�j ��ͼ�����
			INDEX_i    = (j+0.0)*LENSPITCHY/PIXEL_DX + DIRECTION_TEST[1] * yy ; // y���� ��Ӧ��ͼ���е�i ��ͼ�����
			if( INDEX_j>=0 && INDEX_i>=0  &&  INDEX_j< WIDTH  && INDEX_i <HEIGHT)
			{
				CENTER1[0] = (i+0.0)*LENSPITCH*SCALE  - WIDTH  * PIXEL_DX/2.0*SCALE  ; // x����
			    CENTER1[1] = (j+0.0)*LENSPITCHY*SCALE - HEIGHT * PIXEL_DX/2.0*SCALE  ; // y����

				Cal_Index( CENTER1[0] ,  CENTER1[1], 0.0f );   // ������Othographicͼ�е�Index
				Interp_Bilinear_Fcn ();                        // �������ֵϵ��  Magnitudes_4_Interp 

				index_ij[0] = subimage_Indexs[0];    // ȡ��
	            index_ij[1] = subimage_Indexs[1];    // ȡ��

				index_ij[2] = subimage_Indexs[0];    // ȡ��
	            index_ij[3] = subimage_Indexs[1]+1;  // ȡ��

				index_ij[4] = subimage_Indexs[0]+1;  // ȡ��
	            index_ij[5] = subimage_Indexs[1];    // ȡ��

				index_ij[6] = subimage_Indexs[0]+1;  // ȡ��
	            index_ij[7] = subimage_Indexs[1]+1;  // ȡ��

				if(index_ij[1]>=0 && index_ij[1]< iViewport[2] && index_ij[0]>=0 && index_ij[0]< iViewport[3] )
				{
					DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 0  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 0 ]   + 
						                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[3]*iViewport[2]*3 + index_ij[2]*3 + 0 ]   + 
															  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[5]*iViewport[2]*3 + index_ij[4]*3 + 0 ]   + 
															  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[7]*iViewport[2]*3 + index_ij[6]*3 + 0 ]   ;

					DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 1  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 1 ]   + 
						                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[3]*iViewport[2]*3 + index_ij[2]*3 + 1 ]   + 
															  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[5]*iViewport[2]*3 + index_ij[4]*3 + 1 ]   + 
															  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[7]*iViewport[2]*3 + index_ij[6]*3 + 1 ]   ;

					DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 2  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 2 ]   + 
						                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[3]*iViewport[2]*3 + index_ij[2]*3 + 2 ]   + 
															  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[5]*iViewport[2]*3 + index_ij[4]*3 + 2 ]   + 
															  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[7]*iViewport[2]*3 + index_ij[6]*3 + 2 ]   ;
				}
			}
			// �ڶ���
			INDEX_j    = (i+0.5)*LENSPITCH /PIXEL_DX + DIRECTION_TEST[0] * xx ; // x���� ��Ӧ��ͼ���е�j ��ͼ�����
			INDEX_i    = (j+1.0)*LENSPITCHY/PIXEL_DX + DIRECTION_TEST[1] * yy ; // y���� ��Ӧ��ͼ���е�i ��ͼ�����
			if( INDEX_j>=0 && INDEX_i>=0  &&  INDEX_j< WIDTH  && INDEX_i <HEIGHT)
			{
				CENTER1[0] = (i+0.5)*LENSPITCH*SCALE  - WIDTH  * PIXEL_DX/2.0*SCALE  ;   // 
			    CENTER1[1] = (j+1.0)*LENSPITCHY*SCALE - HEIGHT * PIXEL_DX/2.0*SCALE  ;   // 
				// ����Ipad����xyƽ���ϣ���camtarget��z���ϣ���Ipad��������ͬ�����z������0
				Cal_Index( CENTER1[0] ,  CENTER1[1], 0.0f );  
				Interp_Bilinear_Fcn ();  // �������ֵϵ��  Magnitudes_4_Interp 

				index_ij[0] = subimage_Indexs[0];    // ȡ��
	            index_ij[1] = subimage_Indexs[1];    // ȡ��

				index_ij[2] = subimage_Indexs[0];    // ȡ��
	            index_ij[3] = subimage_Indexs[1]+1;  // ȡ��

				index_ij[4] = subimage_Indexs[0]+1;  // ȡ��
	            index_ij[5] = subimage_Indexs[1];    // ȡ��

				index_ij[6] = subimage_Indexs[0]+1;  // ȡ��
	            index_ij[7] = subimage_Indexs[1]+1;  // ȡ��

				if(index_ij[1]>=0 && index_ij[1]< iViewport[2] && index_ij[0]>=0 && index_ij[0]< iViewport[3] )
				{
					DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 0  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 0 ]   + 
						                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[3]*iViewport[2]*3 + index_ij[2]*3 + 0 ]   + 
															  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[5]*iViewport[2]*3 + index_ij[4]*3 + 0 ]   + 
															  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[7]*iViewport[2]*3 + index_ij[6]*3 + 0 ]   ;

					DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 1  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 1 ]   + 
						                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[3]*iViewport[2]*3 + index_ij[2]*3 + 1 ]   + 
															  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[5]*iViewport[2]*3 + index_ij[4]*3 + 1 ]   + 
															  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[7]*iViewport[2]*3 + index_ij[6]*3 + 1 ]   ;

					DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 2  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 2 ]   + 
						                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[3]*iViewport[2]*3 + index_ij[2]*3 + 2 ]   + 
															  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[5]*iViewport[2]*3 + index_ij[4]*3 + 2 ]   + 
															  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[7]*iViewport[2]*3 + index_ij[6]*3 + 2 ]   ;
				}

				//if(index_ij[1]>=0 && index_ij[1]< iViewport[2] && index_ij[0]>=0 && index_ij[0]< iViewport[3] )
				//{
				//	DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 0  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 0 ]   +         // index_ij[1] mean the y direction  //  Modified 15-07-28
				//		                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + iViewport[2]*3 ]   + 
				//											  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 3 ]   + 
				//											  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + iViewport[2]*3 +1*3 ]   ;

				//	DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 1  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 0     + 1  ]   + 
				//		                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + iViewport[2]*3 + 1  ]   + 
				//											  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 3     + 1  ]   + 
				//											  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + iViewport[2]*3 +1*3 + 1  ]   ;

				//	DATA[INDEX_i*3*WIDTH + INDEX_j*3 + 2  ] = Magnitudes_4_Interp[0] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 0     + 2  ]   + 
				//		                                      Magnitudes_4_Interp[1] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + iViewport[2]*3 + 2  ]   + 
				//											  Magnitudes_4_Interp[2] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + 3     + 2  ]   + 
				//											  Magnitudes_4_Interp[3] * (float)pBits[ index_ij[1]*iViewport[2]*3 + index_ij[0]*3 + iViewport[2]*3 +1*3 + 2  ]   ;

				//}
			}
		}
	}
}


void IP_Image:: Cal_Index(float x, float y, float z)
{
	// ************************  ������ȷ�İ汾 ************************ //
	subimage_Indexs[0] =  -(Rotation_Matrix[0][0]*x + Rotation_Matrix[0][1]*y+ Rotation_Matrix[0][2]*z) + (iViewport[2]-1)/2.0; // Modified 15-07-28
	subimage_Indexs[1] =  +(Rotation_Matrix[1][0]*x + Rotation_Matrix[1][1]*y+ Rotation_Matrix[1][2]*z) + (iViewport[3]-1)/2.0; // Modified 15-07-28
}


void IP_Image::Interp_Bilinear_Fcn ()  // 
{
	float Sum = 0;
	float x_1 = (subimage_Indexs[0]-int(subimage_Indexs[0]+0 ));  // 
	float x_2 = (subimage_Indexs[0]-int(subimage_Indexs[0]+1 ));  //

	float y_1 = (subimage_Indexs[1]-int(subimage_Indexs[1]+0 ));  // 
	float y_2 = (subimage_Indexs[1]-int(subimage_Indexs[1]+1 ));  // 

	Magnitudes_4_Interp[0] = 1/( x_1*x_1+(y_1*y_1) + EPS ) ;
	Magnitudes_4_Interp[1] = 1/( x_1*x_1+(y_2*y_2))+ EPS ;
	Magnitudes_4_Interp[2] = 1/( x_2*x_2+(y_1*y_1) + EPS) ;
	Magnitudes_4_Interp[3] = 1/( x_2*x_2+(y_2*y_2) + EPS) ;

	Sum  = Sum + Magnitudes_4_Interp[0] ;
	Sum  = Sum + Magnitudes_4_Interp[1] ;
	Sum  = Sum + Magnitudes_4_Interp[2] ;
	Sum  = Sum + Magnitudes_4_Interp[3] ;

	Magnitudes_4_Interp[0] = Magnitudes_4_Interp[0] / Sum;
	Magnitudes_4_Interp[1] = Magnitudes_4_Interp[1] / Sum;
	Magnitudes_4_Interp[2] = Magnitudes_4_Interp[2] / Sum;
	Magnitudes_4_Interp[3] = Magnitudes_4_Interp[3] / Sum;
}


void IP_Image::SET_IMAGE_SIZE (int x_init, int y_init, int x_width, int y_height) 
{
	iViewport[0] = x_init ;  // �ݶ� x_init �Ǻ�����ʼλ�� // Modified 15-07-28
	iViewport[1] = y_init ;  // �ݶ� y_init ��������ʼλ�� // Modified 15-07-28
	iViewport[2] = x_width ; // �ݶ� x_width �Ǻ�����    // Modified 15-07-28
	iViewport[3] = y_height; // �ݶ� y_height������߶�    // Modified 15-07-28
	RATIO        = 1; // �趨����ߴ����IPad�ļ���֮��
	SCALE        = (0.5/0.7*iViewport[3])/(PIXEL_DX*HEIGHT*RATIO ) ;  // Modified 15-07-28 Vital important
}
