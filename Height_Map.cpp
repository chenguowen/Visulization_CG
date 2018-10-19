// Height_Map.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"

#include "IP_header.h"
#include "Screen_Shot.h"
#include "LoadObj.h"

#define SQRT3 1.732050807568877
 // ���������۵�����
IP_Image *ip_image = new IP_Image();
int FIRST_TIME_FLAG = 0 ;
#pragma region Rotation_Control  
TriangleMesh mesh;
static GLfloat spin_x=0.0, spin_y=0.0 ;
int    Latest_State = 1 ;
int    Last_xy[2],Now_xy[2] ;
double ANGLE_SUB = 20;
GLuint  texture[1]  ;

float Move_Z= 0.0 ;
float Rotation_INdex = 0;

GLfloat Transform_Matrix[16]={1.0f, 0.0f,  0.0f,  0.0f,				// NEW: Final Transform
                              0.0f, 1.0f,  0.0f,  0.0f,
                              0.0f,  0.0f,  1.0f,  0.0f,
                              0.0f,  0.0f,  0.0f,  1.0f } ;
GLfloat *Now_Matrix = (GLfloat *) calloc(9,sizeof(GLfloat));

double rotxyz[3] ;  
// glMatrix g;  glMultMatrixf(Transform_Matrix);	
//�������� 
GLdouble Orth_Window=1.0;
void myDisplay();
void IP_MAKE_Display();
//double 
void Refresh_4f_Transform_Matrix(GLfloat* Transform_Matrix, GLfloat* RotationMatrix)
{
	GLfloat Result[9];
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
	    {
		   Result[i*3+j] = Transform_Matrix[i*4+j];
		}
	}
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
	    {
			Transform_Matrix[i*4+j] = 0 ;
			for (int k=0;k<3;k++){
		        Transform_Matrix[i*4+j] =  Transform_Matrix[i*4+j] + Result[i*3+k] * RotationMatrix[k*3+j];
			}
		}
	}
}
//  
void Cross_Matrix( GLfloat*Matrix ,GLfloat*Matrix1,GLfloat*Matrix2)
{
	Matrix[0] = 1 * ( Matrix1[1]*Matrix2[2]-Matrix1[2]*Matrix2[1] );
	Matrix[1] = 1 * ( Matrix1[0]*Matrix2[2]-Matrix1[2]*Matrix2[0] );
	Matrix[2] = 1 * ( Matrix1[1]*Matrix2[0]-Matrix1[0]*Matrix2[1] );
	double sum = sqrt(  Matrix[0] *Matrix[0]  +  Matrix[1] *Matrix[1] +  Matrix[2] *Matrix[2] );
	Matrix[1]  = Matrix[1] /sum;
	Matrix[0]  = Matrix[0] /sum;
	Matrix[2]  = Matrix[2] /sum;
}

void Roderigues(GLfloat* RotationMatrix,GLfloat *NowRotation_Matrix , double* rotxyz)
{
	double TMP[3] ;

	TMP[0] = rotxyz[0];
	TMP[1] = rotxyz[1];
	TMP[2] = rotxyz[2];
	if(TMP[0]*TMP[0] + TMP[1]*TMP[1] + TMP[2]*TMP[2] > 0 ) 
	{
		GLfloat theta= sqrt( rotxyz[0]*rotxyz[0] + rotxyz[1]*rotxyz[1] + rotxyz[2]*rotxyz[2] );
		rotxyz[0] = rotxyz[0] / theta ;
		rotxyz[1] = rotxyz[1] / theta ;
		rotxyz[2] = rotxyz[2] / theta ;
		// Matlab Version
		//rotation=cos(theta)*eye(3)+(1-cos(theta))*r*r'+[0 -r(3) r(2); r(3) 0  -r(1); -r(2) r(1) 0]*sin(theta);
		RotationMatrix[0] = cos(theta)+ (1-cos(theta))*rotxyz[0]*rotxyz[0] + 0        *sin(theta);
		RotationMatrix[1] = 0         + (1-cos(theta))*rotxyz[0]*rotxyz[1] - rotxyz[2]*sin(theta);
		RotationMatrix[2] = 0         + (1-cos(theta))*rotxyz[0]*rotxyz[2] + rotxyz[1]*sin(theta);

		RotationMatrix[3] = 0         + (1-cos(theta))*rotxyz[1]*rotxyz[0] + rotxyz[2]*sin(theta);
		RotationMatrix[4] = cos(theta)+ (1-cos(theta))*rotxyz[1]*rotxyz[1] + 0        *sin(theta);
		RotationMatrix[5] = 0         + (1-cos(theta))*rotxyz[1]*rotxyz[2] - rotxyz[0]*sin(theta);

		RotationMatrix[6] = 0         + (1-cos(theta))*rotxyz[2]*rotxyz[0] - rotxyz[1]*sin(theta);
		RotationMatrix[7] = 0         + (1-cos(theta))*rotxyz[2]*rotxyz[1] + rotxyz[0]*sin(theta);
		RotationMatrix[8] = cos(theta)+ (1-cos(theta))*rotxyz[2]*rotxyz[2] + 0        *sin(theta);
	}
	else 
	{
		RotationMatrix[0] = 1.0;
		RotationMatrix[1] = 0;
		RotationMatrix[2] = 0;

		RotationMatrix[3] = 0;
		RotationMatrix[4] = 1.0;
		RotationMatrix[5] = 0;

		RotationMatrix[6] = 0;
		RotationMatrix[7] = 0;
		RotationMatrix[8] = 1.0;
	}
}

// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures()
{
	// Load Texture
	AUX_RGBImageRec *texture1;
	texture1 = auxDIBImageLoad(L"Data/yb_10k.bmp");
	if (!texture1) exit(1);
	// Create Texture
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture1->sizeX, texture1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1->data);
};


void InitGL ( GLvoid )     // Create Some Everyday Functions
{
	LoadGLTextures();                                    // �������� read texture
	glShadeModel(GL_SMOOTH);						 	 // Enable Smooth Shading
	glEnable(GL_DEPTH_TEST);						 	 // Enables Depth Testing
	glEnable (GL_COLOR_MATERIAL );                       // ������ɫ
	glEnable(GL_TEXTURE_2D);                             // ����2D����ӳ��
    GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };          
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat position[] = { 0.0, 2.0, 10.0, 11.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);  // Setup The Ambient Light���û�����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);  // Setup The Diffuse Light���������
	glLightfv(GL_LIGHT1, GL_POSITION,position);  // Position The Light���ù�Դλ��
	glEnable(GL_LIGHTING);     // ��������
    glEnable(GL_LIGHT1);     // Enable Light One����һ�Ź�Դ
	GLfloat pos[4] = { -2.8, 5., 1.8, 1.};
	glLightfv (GL_LIGHT1, GL_POSITION, pos);
	glEnable (GL_LINE_SMOOTH);
    glHint (GL_LINE_SMOOTH, GL_NICEST);
	glEnable (GL_POLYGON_SMOOTH);
	glHint (GL_POLYGON_SMOOTH,GL_NICEST ); 
	glEnable (GL_POINT_SMOOTH);
	glHint (GL_POINT_SMOOTH,GL_NICEST ); 
}

void spinDisplay(void)
{
	 spin_x =  -(Now_xy[0]- Last_xy[0])/400.0f*4; 
	 spin_y =  -(Now_xy[1]- Last_xy[1])/400.0f*4;
	 rotxyz[0] =   spin_y;
	 rotxyz[1] =   spin_x;
	 rotxyz[2] =   0 ;
	 
	 Last_xy[0] = Now_xy[0];
	 Last_xy[1] = Now_xy[1];   //cout<<"spin_x =��"<<spin_x << "   spin_y ="<<spin_y<<endl;
	 Roderigues(Now_Matrix , Transform_Matrix , rotxyz);          // Transform_Matrix����ǰһ��ʱ�̵���ת����Now_Matrix����������
	 Refresh_4f_Transform_Matrix( Transform_Matrix, Now_Matrix);  // glutPostRedisplay(); // 5.31 chenguowen
	 myDisplay();
}

//  
void  Reshape(GLsizei width,GLsizei height)
{
    GLfloat w_aspect = (double) width / height;
	int w_width = width;
	int w_height = height;
	ip_image->SET_IMAGE_SIZE(0,0,w_width,w_height); // 
	cout<<"********************************** Size of Window by glGetIntegerv ******************"<<endl;

	cout<<"WIDTH of Window="<<ip_image->iViewport[2]<<"\tHeight of Window = "<<ip_image->iViewport[3]<<endl;
	int a[4];                      // 
	glGetIntegerv(GL_VIEWPORT,a);  // 
	cout<<"WIDTH  of Window="<<a[2]<<"\tHeight of Window = "<<a[3]<<endl;
	cout<<"Origin of Window="<<a[0]<<"\tOrigin of Window = "<<a[1]<<endl;

	if (w_aspect>1)  // ԭ���ڴ� �Ұ�ViewPort������һЩ���� �������λ���
	{
		glViewport( (w_width-w_height)/2, 0, w_width-(w_width-w_height), w_height);
	}
	else
	{
		glViewport( 0,-(w_width-w_height)/2,  w_width, w_height+(w_width-w_height));
	}
}


void mouse(int botton,int state,int x,int y)
{
 switch(botton)
 {
 case GLUT_LEFT_BUTTON:   //������������ʱ��ʼ��ת
   if(state==GLUT_DOWN )
   { 
	   Latest_State =1; 
	   Last_xy[0] = x;
	   Last_xy[1] = y;
   }
   if(state==GLUT_UP && Latest_State==1 )
   {
	   Now_xy[0]= x; 
	   Now_xy[1]= y;
	   Latest_State = 0;
	   spinDisplay();
   }
   break;

 case GLUT_MIDDLE_BUTTON:  
   if(state==GLUT_DOWN)
    glutIdleFunc(0);
   break;
 default:
  break;

 }
}
void Mouse_motion(int x,int y )
{
	if(Latest_State==1 )
   { 
	   Now_xy[0]= x; 
	   Now_xy[1]= y;
	   spinDisplay();
   }
}
// te
void SpecicalKey(int key, int x, int y) 
{  
   switch (key) {  
	case GLUT_KEY_DOWN:
		Move_Z = Move_Z-0.3;
		glutPostRedisplay(); 
		break;
	case GLUT_KEY_UP:  
		 Move_Z = Move_Z+0.3;
	     cout<<Move_Z <<endl;
	     glutPostRedisplay();       
         break;                     // ʧ�ܵĳ�����   // GLUT_KEY_DOWNVK_UP 
  case 'z':
  glutIdleFunc(NULL);
   break; 
      case 27:  
         exit(0);  
         break;  
   }  
}  

//  
void keyboard(unsigned char key, int x, int y)  
{  
   switch (key) {  
      case 'w':  
		 Move_Z = Move_Z+0.3;
	     //cout<<Move_Z <<endl;
	     glutPostRedisplay(); 
         break;  
     case 's':  
		 Move_Z = Move_Z-0.3;
		 glutPostRedisplay(); 
         break;  
	 case'd':
		 Rotation_INdex = Rotation_INdex - 3;
	     cout<<Rotation_INdex <<endl;
	     myDisplay();
         break;  
	 case'a':
		 Rotation_INdex = Rotation_INdex + 3;
		 glutPostRedisplay(); 
         break;  

	 case 'i':
		 IP_MAKE_Display();
		 break;

	case'b':  //
		cin.getline(filename_BMP,30);
		screenshot(filename_BMP);
        break;  
	/*case GLUT_KEY_DOWN:
		Move_Z = Move_Z-3;
		glutPostRedisplay(); 
		break;
	case GLUT_KEY_F6:  
		 Move_Z = Move_Z+3;
	     cout<<Move_Z <<endl;
	     glutPostRedisplay();         // GLUT_KEY_DOWNVK_UP 
         break;   */                  // ʧ�ܵĳ����� ��Ϊλ�ò��� Ӧ����SpecialFunc����
  case 'z':
  glutIdleFunc(NULL);
   break; 
      case 27:  
         exit(0);  
         break;  
   }  
}  


GLvoid draw_Scene()
{
    glBindTexture(GL_TEXTURE_2D, texture[0]);      // ѡ������
	for (int i=0;i<mesh.faces.size() ;i++)
	{
		glBegin(GL_TRIANGLES);
		int a = mesh.faces[i].v[0]-1,b = mesh.faces[i].v[1]-1, c=mesh.faces[i].v[2]-1;
		int D = mesh.faces[i].Define_index[0]-1,E = mesh.faces[i].Define_index[1]-1, F=mesh.faces[i].Define_index[2]-1;
		glTexCoord2f(mesh.Texture_2D[D].x , mesh.Texture_2D[D].y  );  
		glVertex3f(mesh.verts[a].x , mesh.verts[a].y ,mesh.verts[a].z);   // 
		glTexCoord2f(mesh.Texture_2D[E].x , mesh.Texture_2D[E].y  );  
		glVertex3f(mesh.verts[b].x, mesh.verts[b].y ,mesh.verts[b].z);
		glTexCoord2f(mesh.Texture_2D[F].x , mesh.Texture_2D[F].y  );  
		glVertex3f(mesh.verts[c].x, mesh.verts[c].y ,mesh.verts[c].z);
		glEnd();
	}
}
#pragma endregion 

// ����IP����ʾ����
void IP_MAKE_Display() // 
{
	//ip_image->IP_Parameter_Show(); //����չʾ
	//ip_image->IPwriteBMP();
	cout<<"������ɳ�����"<<endl; 
	cin>>ip_image->FLYOUT_DISTANCE ; //
	double DISTANCE = 10 ;
	GLfloat Window_size_ortho = ip_image->WINDOW_SIZESET;         // ���ڴ�Сorthographic

	for( int xx= -ip_image->RADIUS_X ; xx <ip_image->RADIUS_X; xx++)
	{
		for (int yy=-ip_image->RADIUS_Y ; yy <ip_image->RADIUS_Y; yy++)
		{
			if( (xx+0.5)/SQRT3 + (yy+0.5) <= +ip_image->RADIUS_Y + 0.5  &&    -(xx+0.5)/SQRT3+(yy+0.5) <= + ip_image->RADIUS_Y + 0.5 && 
				(xx+0.5)/SQRT3 + (yy+0.5) >= -ip_image->RADIUS_Y - 0.5  &&    -(xx+0.5)/SQRT3+(yy+0.5) >= - ip_image->RADIUS_Y - 0.5 )
			{   // ������Ȼ��� �� ��ɫ����
				glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
				glMatrixMode(GL_PROJECTION);              // �����������
				glLoadIdentity();                         // �ظ�ԭλ
				// ������ͶӰ���
				glOrtho(-Window_size_ortho,Window_size_ortho,-Window_size_ortho,Window_size_ortho,1.0f,120.0f); 
				// ���Ĳ����ڴ˴�,�������λ��
				GLdouble  eyex=xx*DISTANCE*ip_image->PIXEL_DX/ip_image->LENSTHICK;  // 
				GLdouble  eyey=yy*DISTANCE*ip_image->PIXEL_DX/ip_image->LENSTHICK;  //   
				GLdouble  eyez=0, centerx=0, centery =0, centerz = - DISTANCE  , upx = 0, upy =1, upz= 0;
				// ���Ĳ����ڴ˴�,�������λ��
				gluLookAt( eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
				ip_image->Rotation_Matrix_Update( eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz); // ���������λ�ú�Ŀ��λ�ü�����ת����
				glMatrixMode(GL_MODELVIEW);                         //  ��ģ������������ϵ�е�λ�ý�������
				glLoadIdentity();									//  Reset The Current Modelview Matrix ��������Ϊ�غ�
				glTranslatef(0.0f,.0f,ip_image->FLYOUT_DISTANCE -DISTANCE);             //  ��Z�᷽�����������ƶ�10f
				glPushMatrix();                                     //  ����ǰ��model�������ѹ���ջ 
				glMultMatrixf(Transform_Matrix);	                //  ��ջ���˵ľ����ҳ�����ת����Transform_Matrix 
			
				draw_Scene();                                        // ����С���ĳ���

				glPopMatrix();                                      // ��ģ�;����ջ 
				glutSwapBuffers();
				cout<<"xx="<<xx<<"   yy="<<yy<<endl; 
				ip_image->IP_make_FCN(xx,yy,ip_image->iViewport[2],ip_image->iViewport[3]);  
			}
		}
	}
	ip_image->IPwriteBMP();
}
// ������ʾ����
void myDisplay()
{
	if(FIRST_TIME_FLAG==0)  {	ip_image->IP_Parameter_Show();}   //����չʾ
	FIRST_TIME_FLAG++;
	double DISTANCE =5;
	// ������Ȼ��� �� ��ɫ����
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	glMatrixMode(GL_PROJECTION);              // �����������
	glLoadIdentity();                         // �ظ�ԭλ
	GLfloat Window_size_ortho = 0.2f;         // ���ڴ�Сorthographic
	// ������ͶӰ���
	glFrustum(-Window_size_ortho,Window_size_ortho,-Window_size_ortho,Window_size_ortho,2.0f,120.0f); 
	// ���Ĳ����ڴ˴�,�������λ��
	GLdouble  eyex=0 ;  // 
	GLdouble  eyey=0 ;  //   
	GLdouble  eyez=0, centerx=0, centery =0, centerz = - DISTANCE  , upx = 0, upy =1, upz= 0;
	// ���Ĳ����ڴ˴�,�������λ��
	gluLookAt( eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
			
	glMatrixMode(GL_MODELVIEW);                         //  ��ģ������������ϵ�е�λ�ý�������
	glLoadIdentity();									//  Reset The Current Modelview Matrix ��������Ϊ�غ�
	glTranslatef(0.0f,.0f,Move_Z-DISTANCE);             //  ��Z�᷽�����������ƶ�10f
	glPushMatrix();                                     //  ����ǰ��model�������ѹ���ջ 
	glMultMatrixf(Transform_Matrix);	                //  ��ջ���˵ľ����ҳ�����ת����Transform_Matrix 
	draw_Scene();                                        // ����С���ĳ���
	glPopMatrix();                                      // ��ģ�;����ջ 
	glutSwapBuffers();

}
  
int _tmain(int argc, char** argv)
{
	 AUX_RGBImageRec *texture1;
	 std::string filename = "Data\\yb_10k.obj";    // ��������С Data\\spongebob_bind.obj
	 loadObj(filename ,mesh);

	 cout<<ip_image->HEIGHT<<endl;
	 ip_image->SET_IMAGE_SIZE(100,100,100,100) ;

	 glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); //GLUT_SINGLE
	 glutInitWindowPosition(ip_image->iViewport[0], ip_image->iViewport[1]);   // All control by IP_Image class
     glutInitWindowSize    (ip_image->iViewport[2], ip_image->iViewport[3]);   // All control by IP_Image class
     glutCreateWindow("��һ��OpenGL����");
	 InitGL();
     glutDisplayFunc(&myDisplay);
	 glutMotionFunc(Mouse_motion);  // ���������ƶ��� �����
	 glutMouseFunc(mouse);          // ������̧����� �����
	 glutReshapeFunc(Reshape);
     glutKeyboardFunc(keyboard);    // Press specific key �����
	 glutSpecialFunc(SpecicalKey);  // ���ⰴ��
     glutMainLoop();
	 return 0;
}