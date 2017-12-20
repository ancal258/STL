// C,OpenGL
#include <GL/glut.h> // includes gl.h glu.h
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
//c++
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <chrono>

// �����̳� ��
#include <vector>
#include <list>
#include <deque>
// map, set X


#define ENEMYMAX 6

// STL ���÷��� ��� 
using namespace std;
using namespace chrono;
using ReplayPair = pair<char, double>;

vector<ReplayPair> input;	// �Է��� ��
vector<ReplayPair> myTime;	// �ð� ����� ��
vector<ReplayPair> stepTime;	// �ܰ躰 ����� ��

ofstream out; // ���� �Է�
ifstream in; // ���� ���
string fileName = "myRep.replay"; // ���� �̸�

system_clock::time_point time_start, time_play;	//�Է��ϱ� ������ �ð� ���.
duration<double, micro> d;

char charRead; // �Է� �б�
char step; // ���� �б�

double timeRead; // �ð� �б�
bool oCheck; // o�Է� �˻�
bool iCheck; // i�Է� �˻�
vector<ReplayPair>::iterator i;
vector<ReplayPair>::iterator i2;


// �Լ� ����

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid TimerFunction(int value);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUP(unsigned char key, int x, int y);
GLvoid gun();
GLvoid inputKey(char key);
static bool g_keyState[0xff] = { 0, };

//�����ؼ� ���� �Լ�
void FloorCoC(float x, float z, int w, int s);
void CharInit();
void DrawMap(float x, float y, float z, int w, int h, int s); //��
void CheckCollision_Side(); //�� �浹üũ
bool Collision_state = false;
void SetUP();
//0ź �Լ�
GLvoid Level0();
void DrawFloor0(); //0���� �ٴ�
void Decoration0(); //0���� ���ǰ ������
//1ź �Լ�
GLvoid Level1();
void DrawFloor1(); //1���� �ٴ�
void Tile(float x, float z, int w, int s, float alpha); //Ÿ��

// 2ź �Լ�
GLvoid Level2(); //2�� ����
void DrawFloor2(); //2���� �ٴ�
void Decoration2(); //2���� ���ǰ ������
void Enemy2(); //2���� ��
double theta_2 = 0.0; //2���� ���ǰ ȸ��
double moveZ_Enemy2 = 0.0; //2���� ��ֹ� �̵�
bool moveZ_Enemy_state = true;

// 3ź �Լ�
GLvoid Level3(); //3�� ����
void DrawFloor3(); //3���� �ٴ�
void DrawBuilding(float x, float y, float z, int w, int h, int s); //�ǹ�
void CheckCollision(float x1, float x2, float z1, float z2, float r); //�ǹ� �浹üũ

// 4ź �Լ�
void DrawFloor4(); //4���� �ٴ�
GLvoid Level4();

int Shot_Gun = 100;
char draw_FPS[100];
void update_FPS();

float g_fMu; // ������
int muCount; // ������

double ambient = 0.4; // ������ ambient light ����
double specular = 1; // ������ specular light ����
double diffuse = 0.8; // ������ diffuse light ����
float alpha1[11];
int rotateCount = 0;
int BC = 0;
bool DownAni, Arm_B, Head_B;
bool Floorcheck = true;
bool Rabbit_CC;
bool BossCheck;
int LevelCheck = 0;
double Arm_R = 38, Head_T = 4;
int BossCount = 0;
int BossHP = 20;
double a;

float Sight_Y = 0;
float Sight_Z = 0;

struct Vector3 {
	float x, y, z;
	bool state;
};
Vector3 g_Enemy[ENEMYMAX];
Vector3 g_MyPosition;
Vector3 g_Shot[100];

GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info);
GLuint texture_object[6]; //��(2)
GLubyte *pBytes; //�����͸� ����ų ������
BITMAPINFO *info; // ��Ʈ�� ��� ������ ����



typedef struct Bg
{
	double x;
	double y;
	double z;
}Bg;

Bg P[19];
Bg p;
Bg Bezier4(Bg p1, Bg p2, Bg p3, Bg p4, double mu)
{
	double mum1, mum13, mu3;
	mum1 = 1 - mu;
	mum13 = mum1 * mum1 * mum1;
	mu3 = mu * mu * mu;
	p.x = mum13*p1.x + 3 * mu*mum1*mum1*p2.x + 3 * mu*mu*mum1*p3.x + mu3*p4.x;
	p.y = mum13*p1.y + 3 * mu*mum1*mum1*p2.y + 3 * mu*mu*mum1*p3.y + mu3*p4.y;
	p.z = mum13*p1.z + 3 * mu*mum1*mum1*p2.z + 3 * mu*mu*mum1*p3.z + mu3*p4.z;
	return (p);
}


GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	// ���̳ʸ� �б� ���� ������ ����
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	// ��Ʈ�� ���� ����� �д´�.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}

	// ������ BMP �������� Ȯ���Ѵ�.
	if (header.bfType != 'MB') {
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER ��ġ�� ����.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// ��Ʈ�� �̹��� �����͸� ���� �޸� �Ҵ��� �Ѵ�.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		fclose(fp);
		exit(0);
		return NULL;
	}

	// ��Ʈ�� ���� ����� �д´�.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// ��Ʈ���� ũ�� ����
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
		(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);

	// ��Ʈ���� ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// ��Ʈ�� �����͸� bit(GLubyte Ÿ��)�� �����Ѵ�.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	return bits;
}

void main(int argc, char *argv[])
{


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ����
	glutInitWindowSize(800, 800); // �������� ũ�� ����
	glutCreateWindow("1����"); // ������ ���� (������ �̸�)

	SetUP();
	glutReshapeFunc(Reshape);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUP);
	glutTimerFunc(300, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene(GLvoid)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // ��� ��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (LevelCheck == 0){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(15.0 + g_MyPosition.x, 14 + g_MyPosition.y*1.4, 47.0 + g_MyPosition.z, 15.0 + g_MyPosition.x, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	//1��
	if (LevelCheck == 1){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(65.0 + g_MyPosition.x, 14 + g_MyPosition.y*1.4, 48.0 + g_MyPosition.z,
			65.0 + g_MyPosition.x, 0.0, 0.0, 0.0, 1.0, 0.0);
	}//2��
	else if (LevelCheck == 2){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(115.0, 7.0, 48.0 + g_MyPosition.z, 115.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}//3��
	else if (LevelCheck == 3){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(165.0 + g_MyPosition.x, 7.0 + -(g_MyPosition.z) + 5, 45.0 + g_MyPosition.z + 5, 165.0 + g_MyPosition.x, 0.0, 0.0, 0.0, 1.0, 0.0);
	}//4��
	else if (LevelCheck == 4){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(215.0 + g_MyPosition.x, 14 + g_MyPosition.y + Sight_Y, 47.0 + g_MyPosition.z + Sight_Z, 215.0 + g_MyPosition.x, 7 + g_MyPosition.y, 0.0, 0.0, 1.0, 0.0);
	}

	glEnable(GL_LIGHTING); // ����ȿ�� ����
	GLfloat ambientLight[] = { 0.2f, 0.2, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	GLfloat AmbientLight[] = { ambient, ambient, ambient, 1.0f };
	GLfloat DiffuseLight[] = { diffuse, diffuse, diffuse, 1.0f };
	GLfloat SpecularLight[] = { specular, specular, specular, 1.0 };
	GLfloat SPOTD[] = { 0, -1, 0 };

	/// 1�� ���� ///
	GLfloat lightPos1[] = { 4.0, 4.0, 5.0, 1.0 };
	GLfloat lightPos[] = { 165 + g_MyPosition.x, 2, 29 + g_MyPosition.z, 1.0 };

	GLfloat lightPos2[] = { p.x, 2, p.z, 1.0 };

	// 1�� ���� ���� //
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);

	glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, SpecularLight);
	if (LevelCheck != 4)
		glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	if (LevelCheck == 4)
		glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, SPOTD);

	if (LevelCheck == 0 || LevelCheck == 1 || LevelCheck == 2 || (LevelCheck == 4 && a<-40)){
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT0);
	}
	else if (LevelCheck == 3 || LevelCheck == 4){
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
	}

	/// �� ��ü�� ���� ������ ���� ///
	GLfloat specref1[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientLight);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref1);
	glMateriali(GL_FRONT, GL_SHININESS, 60);
	/// �� ��ü�� ���� ������ ���� ///

	if (Sight_Y < 300){
		glPushMatrix();//FPS ǥ��
		glColor3f(1, 1, 0);
		if (LevelCheck == 0)
			glTranslatef(g_MyPosition.x + 13, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 1)
			glTranslatef(g_MyPosition.x + 63, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 2)
			glTranslatef(g_MyPosition.x + 113, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 3)
			glTranslatef(g_MyPosition.x + 163, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 4)
			glTranslatef(g_MyPosition.x + 213, 6 + g_MyPosition.y, g_MyPosition.z + 30);

		glRasterPos2f(0.0, 0.0);
		sprintf(draw_FPS, "bullet Count : %d", Shot_Gun);

		//glRasterPos2f(0.0, 3.0f);

		int len = (int)strlen(draw_FPS);
		for (int i = 0; i < len; i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, draw_FPS[i]);
		glPopMatrix();
		glPopMatrix();
	}

	if (Sight_Y < 300) {
		glPushMatrix();//FPS ǥ��
		glColor3f(1, 0, 0);
		if (LevelCheck == 0)
			glTranslatef(g_MyPosition.x + 13, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 1)
			glTranslatef(g_MyPosition.x + 63, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 2)
			glTranslatef(g_MyPosition.x + 113, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 3)
			glTranslatef(g_MyPosition.x + 163, 6 + g_MyPosition.y, g_MyPosition.z + 30);
		else if (LevelCheck == 4)
			glTranslatef(g_MyPosition.x + 213, 6 + g_MyPosition.y, g_MyPosition.z + 30);

		glTranslatef(-5, 8, 0);
		glRasterPos2f(0.0, 0.0);
		sprintf(draw_FPS, "Save : p // Load(Time) : o // Load(Step) : i (Front : m , Behind : n)");

		//glRasterPos2f(0.0, 3.0f);

		int len = (int)strlen(draw_FPS);
		for (int i = 0; i < len; i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, draw_FPS[i]);
		glPopMatrix();
		glPopMatrix();
	}


	gun();

	glEnable(GL_BLEND);
	if (LevelCheck == 0){
		Level0();
	}
	else if (LevelCheck == 1){
		Level1();
	}
	else if (LevelCheck == 2){
		Level2();

		for (int i = 0; i < ENEMYMAX; i++){
			if (g_Enemy[i].x - 1.5 < 115 + g_MyPosition.x &&
				g_Enemy[i].x + 1.5 > 115 + g_MyPosition.x)
				if (g_Enemy[i].z + 0.5 + moveZ_Enemy2 > g_MyPosition.z + 29 &&
					g_Enemy[i].z - 0.5 + moveZ_Enemy2 < g_MyPosition.z + 29)
					CharInit();
		}
	}
	else if (LevelCheck == 3){
		Level3();
	}
	else if (LevelCheck == 4){
		Level4();
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid TimerFunction(int value)
{
	


	//�̰� �ϴ� ���ÿ��� �����°�
	for (int i = 0; i < ENEMYMAX; i++){
		if (g_Enemy[i].state == false){ //2���� ��
			g_Enemy[i].z += 0.5;
			if (g_Enemy[i].z > 27){
				for (int i = 0; i < ENEMYMAX; i++){
					g_Enemy[i].x = (rand() % 30) + 100;
					g_Enemy[i].z = -4;
				}
			}
		}
	}
	for (int i = 0; i < 100; i++){
		if (g_Shot[i].state == true){ // �Ѿ�
			g_Shot[i].z -= 0.5;
			if (g_Shot[i].z < -10)
				g_Shot[i].z = -1000;
		}
	}

	if (Arm_B == false)
		Arm_R += 1;
	if (Arm_R > 60)
		Arm_B = true;
	if (Arm_B == true)
		Arm_R -= 1;
	if (Arm_R <= 20)
		Arm_B = false;

	if (Head_T > 4.2)
		Head_B = true;
	if (Head_T <= 3.8)
		Head_B = false;
	if (Head_B == false)
		Head_T += 0.03;
	if (Head_B == true)
		Head_T -= 0.03;

	if (DownAni == false) {
		if (g_keyState['l']) {
			inputKey('l');
		}

		if (g_keyState['w'])
			inputKey('w');
		if (g_keyState['s'])
			inputKey('s');
		if (g_keyState['a'])
			inputKey('a');
		if (g_keyState['d'])
			inputKey('d');
		if (g_keyState['t']) { // ���� ���� 
			out.open(fileName, ios::out, ios::trunc);
			out.clear();
			time_start = system_clock::now();
		}
		if (g_keyState['p'])  // ����
			inputKey('p');
		if (g_keyState['i']) {
			inputKey('`');
			inputKey('i');		
		}
		if (g_keyState['`'])
			inputKey('`');
	}
	if (oCheck == true) {
		time_play = system_clock::now();
		d = time_play - time_start;
		cout << d.count() << endl;
		if (d.count() >= i->second) {
			cout << i->first << endl;
			inputKey(i->first);
			i++;
		}	

		if (i == myTime.end() - 1) {
			cout << "�� ���常��" << endl;
			inputKey('`');
			oCheck = false;
		}
	}
	if (iCheck == true) {
		time_play = system_clock::now();
		d = time_play - time_start;
		if (g_keyState['m']) { // -> Ű
			i2++;
			inputKey(i2->first);
		}
		else if (g_keyState['n']){
			if (i2 != stepTime.begin() + 1) {
				i2--;
				if (i2->first == 'd')
					inputKey('a');
				if (i2->first == 'a')
					inputKey('d');
				if (i2->first == 'w')
					inputKey('s');
				if (i2->first == 's')
					inputKey('w');
				if (i2->first == 'l')
					Shot_Gun++;
			}
		}
		if (i2 == stepTime.end() - 1) {
			cout << " ���� ���� " << endl;
			inputKey('`');
			iCheck = false;
		}
	}


	if (g_keyState['o']) {

		in.open(fileName, ios::in);
		cout << " ���� ��� " << endl;
		myTime.clear();
		if (in.is_open()) {
			while (!in.eof()) {
				in >> charRead;
				in >> timeRead;
				myTime.push_back(make_pair(charRead, timeRead));
			}cout << " ��� �Ϸ� " << endl;
			in.close();
			time_start = system_clock::now();

			i = myTime.begin();
			oCheck = true;
			//cout << "end" << endl;
		}
	}
	if (g_keyState['i']) {
		in.open(fileName, ios::in);
		cout << " ���� ��� (Step) " << endl;
		stepTime.clear();
		if (in.is_open()) {
			while (!in.eof()) {
				in >> charRead;
				in >> timeRead;
				stepTime.push_back(make_pair(charRead, timeRead));
			}
			in.close();
			i2 = stepTime.begin();
			iCheck = true;
		}
	}

	for (int i = 0; i < 11; i++){
		if (alpha1[i] > 0)
			alpha1[i] -= 0.03;
	}
	glutPostRedisplay(); // ȭ�� �� ���
	glutTimerFunc(30, TimerFunction, 1); // Ÿ�̸��Լ� �� ����
}

GLvoid inputKey(char key) {
	//char key = '.';
	static  float cubeSpeed = 0.2;

	if (DownAni == false) {
		if (key == '`') { // 0ź���� ��ȯ
			LevelCheck = 0;
			Shot_Gun = 100;
			BossHP = 20;
			Sight_Y = 0;
			Sight_Z = 0;
			CharInit();
		}

		if (key == 'w') {
			g_MyPosition.z -= cubeSpeed;
			input.push_back(make_pair(key, d.count()));
		}
		if (key == 's') {
			g_MyPosition.z += cubeSpeed;
			input.push_back(make_pair(key, d.count()));

		}
		if (key == 'a') {
			g_MyPosition.x -= cubeSpeed;
			input.push_back(make_pair(key, d.count()));

		}
		if (key == 'd') {
			g_MyPosition.x += cubeSpeed;
			input.push_back(make_pair(key, d.count()));
		}
		if (key == 'l') {
			for (int i = 0; i<100; i++) // 2�� �� ��ǥ ī��Ʈ
			{
				if (g_Shot[i].state == false) {
					if (LevelCheck == 0) {
						g_Shot[i].x = 15 + g_MyPosition.x + 2;
						g_Shot[i].y = g_MyPosition.y + 2;
						g_Shot[i].z = 29 + g_MyPosition.z;
					}
					else if (LevelCheck == 1) {
						g_Shot[i].x = 65 + g_MyPosition.x + 2;
						g_Shot[i].y = g_MyPosition.y + 2;
						g_Shot[i].z = 29 + g_MyPosition.z;
					}
					else if (LevelCheck == 2) {
						g_Shot[i].x = 115 + g_MyPosition.x + 2;
						g_Shot[i].y = g_MyPosition.y + 2;
						g_Shot[i].z = 29 + g_MyPosition.z;
					}
					else if (LevelCheck == 3) {
						g_Shot[i].x = 165 + g_MyPosition.x + 2;
						g_Shot[i].y = g_MyPosition.y + 2;
						g_Shot[i].z = 29 + g_MyPosition.z;
					}
					else if (LevelCheck == 4) {
						g_Shot[i].x = 215 + g_MyPosition.x + 2;
						g_Shot[i].y = g_MyPosition.y + 2;
						g_Shot[i].z = 29 + g_MyPosition.z;
					}
					g_Shot[i].state = true;
					Shot_Gun -= 1;
					break;
				}
			}
			input.push_back(make_pair(key, d.count()));
		}
		d = system_clock::now() - time_start;
		if (key == 'p') {
			for (auto i = input.begin(); i != input.end(); ++i) {
				if (i == input.begin())
					out << "`" << "\t" << i->second << endl;
				out << i->first << "\t" << i->second << endl;
			}
			out.close();
			cout << "������ �Ϸ�Ǿ���" << endl;
		}





	}
}

GLvoid Keyboard(unsigned char key, int x, int y){
	
	char kkey;
	

	
	
	if (Collision_state == false)
		g_keyState[key] = true;

	//if (g_keyState['p']){ // ġƮŰ p  ( ���� 1�� �ʱ�ȭ) , �Ѿ� 100�� ������
	//	for (int i = 0; i < 11; i++)
	//		alpha1[i] = 1 + i*0.1;
	//	for (int i = 0; i < 100; i++){
	//		g_Shot[i].state = false;
	//		g_Shot[i].x = -100;
	//		g_Shot[i].y = 0;
	//		g_Shot[i].z = 0;
	//	}
	//}
	//


	//if (g_keyState['`']){ // 0ź���� ��ȯ
	//	LevelCheck = 0;
	//	Shot_Gun = 100;
	//	BossHP = 20;
	//	Sight_Y = 0;
	//	Sight_Z = 0;
	//	CharInit();
	//}
	if (g_keyState['1']){ // 1ź���� ��ȯ
		LevelCheck = 1;
		CharInit();
	}
	if (g_keyState['2']){ // 2ź���� ��ȯ
		LevelCheck = 2;
		CharInit();
	}
	if (g_keyState['3']){ // 3ź���� ��ȯ
		LevelCheck = 3;
		CharInit();
	}
	if (g_keyState['4']){ // 4ź���� ��ȯ
		BossHP = 20;
		LevelCheck = 4;
		CharInit();
	}
}



GLvoid KeyboardUP(unsigned char key, int x, int y){
	g_keyState[key] = false;


}

void FloorCoC(float x, float z, int w, int s){
	if (z  < g_MyPosition.z + 29 && z + s > g_MyPosition.z + 29){
		if (x < g_MyPosition.x + 65 && x + w > g_MyPosition.x + 65){
			BC = 1;
		}
		else {
			BC = 0;
			DownAni = true;
		}
	}
	// printf("%d", BC);
}

void CharInit(){
	g_MyPosition.x = 0;
	g_MyPosition.y = -6;
	g_MyPosition.z = 0;
	DownAni = false;
	for (int i = 0; i < 11; i++){
		alpha1[i] = 1 + (i*0.1);
	}
	muCount = 0;
	g_fMu = 0;
	Rabbit_CC = false;
	a = 0;
	Rabbit_CC = false;
	BossCheck = false;

	for (int i = 0; i < 100; i++){
		g_Shot[i].state = false;
		g_Shot[i].x = -100;
	}
	BossCount = 0;
}

void DrawMap(float x, float y, float z, int w, int h, int s){ // �� ����� �Լ�
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	//�޸�
	glTexCoord2f(0, 1);
	glVertex3f(x, y + h, z);
	glTexCoord2f(0, 0);
	glVertex3f(x, y, z);
	glTexCoord2f(1, 0);
	glVertex3f(x + w, y, z);
	glTexCoord2f(1, 1);
	glVertex3f(x + w, y + h, z);

	//�޸�
	glTexCoord2f(0, 1);
	glVertex3f(x, y + h, z + s);
	glTexCoord2f(0, 0);
	glVertex3f(x, y, z + s);
	glTexCoord2f(1, 0);
	glVertex3f(x, y, z);
	glTexCoord2f(1, 1);
	glVertex3f(x, y + h, z);

	//������
	glTexCoord2f(0, 1);
	glVertex3f(x + w, y + h, z);
	glTexCoord2f(0, 0);
	glVertex3f(x + w, y, z);
	glTexCoord2f(1, 0);
	glVertex3f(x + w, y, z + s);
	glTexCoord2f(1, 1);
	glVertex3f(x + w, y + h, z + s);

	//�ո�
	/* glVertex3f(x + w, y + h, z + s);
	glVertex3f(x, y + h, z + s);
	glVertex3f(x, y, z + s);
	glVertex3f(x + w, y, z + s);*/
	glEnd();
}

void CheckCollision_Side()
{
	//printf("x = %lf, z = %lf\n",g_MyPosition.x , g_MyPosition.z);

	if (g_MyPosition.z <= 1 && g_MyPosition.z >= -28)
	{
		if (g_MyPosition.x > 13){
			g_MyPosition.x = 13;
			Collision_state = true;
			printf("�浹\n");
		}
		else
			Collision_state = false;
		if (g_MyPosition.x  < -13){
			g_MyPosition.x = -13;
			Collision_state = true;
			printf("�浹\n");
		}
		else
			Collision_state = false;
	}
	else
		Collision_state = false;

	if (g_MyPosition.x <= 13 && g_MyPosition.x >= -13)
	{
		if (g_MyPosition.z > 1){
			g_MyPosition.z = 1;
			Collision_state = true;
			printf("�浹\n");
		}
		else
			Collision_state = false;
		if (g_MyPosition.z  < -28){
			g_MyPosition.z = -28;
			Collision_state = true;
			printf("�浹\n");
		}
		else
			Collision_state = false;
	}
	else
		Collision_state = false;
}

void SetUP()
{
	glEnable(GL_DEPTH_TEST); // ��������

	g_MyPosition.y = -6; //�� ĳ����

	glGenTextures(5, texture_object);


	glBindTexture(GL_TEXTURE_2D, texture_object[0]);
	pBytes = LoadDIBitmap("1���� ����.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 350, 391, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[1]);
	pBytes = LoadDIBitmap("2���� ��.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 450, 412, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[2]);
	pBytes = LoadDIBitmap("3���� ����.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 350, 350, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[3]);
	pBytes = LoadDIBitmap("0���� ����.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 413, 375, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[4]);
	pBytes = LoadDIBitmap("4���� ����.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 424, 410, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[5]);
	pBytes = LoadDIBitmap("TEST1.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 390, 360, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	for (int i = 0; i<ENEMYMAX; i++) // 2�� �� ��ǥ ī��Ʈ
	{
		g_Enemy[i].x = (rand() % 30) + 100;
		g_Enemy[i].z = 4;
		g_Enemy[i].y = -5;
	}

	for (int i = 0; i < 11; i++){ // ���İ� �ʱ�ȭ
		alpha1[i] = 1 + (i*0.1);
	}
}
////////////////////////0ź �Լ���
GLvoid Level0(){
	DrawFloor0(); // �ٴ�
	Decoration0();  // ���

	glPushMatrix();
	if (Sight_Y > 300)
		glTranslatef(-200 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);
	glTranslatef(15 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);

	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix(); {
		glPushMatrix(); {//����
			glTranslatef(-1.5, 2, 0);
			glRotatef(-Arm_R, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(-0.5, 0, 0);

			glutSolidCube(1);
		}   glPopMatrix();
		glPushMatrix(); {// ������
			glTranslatef(1.5, 2, 0);
			glRotatef(Arm_R, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(0.5, 0, 0);
			glutSolidCube(1);
		}glPopMatrix();
		glPushMatrix(); { // �Ӹ�
			glTranslatef(0, Head_T, 0);
			glutSolidSphere(1, 20, 20);
		}glPopMatrix();
		glPushMatrix(); // �׸���
		glColor3f(0, 0, 0);
		glTranslatef(0, -0.7, 0);
		glScalef(0.2, 0.01, 0.2);
		glutSolidSphere(1, 20, 20);
		glPopMatrix();
		glPushMatrix(); { // �ٵ�
			glColor3f(1, 1, 1);
			glTranslatef(0, 3, 0);
			glRotated(90, 1, 0, 0);
			glutSolidCone(1, 3, 20, 20);
		}glPopMatrix();

	}glPopMatrix();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D); // ��
	glBindTexture(GL_TEXTURE_2D, texture_object[3]);
	DrawMap(0, -7, 0, 30, 30, 30);
	CheckCollision_Side();
	glDisable(GL_TEXTURE_2D);
}
void Decoration0()
{
	glPushMatrix(); //���漮
	glColor3f(1.0, 0.2, 0.2);
	glTranslatef(10.0, -7, 15.0);
	glutSolidSphere(3.0, 20, 20);
	glColor3f(0.2, 1.0, 0.2);
	glTranslatef(4.0, 0, 2.0);
	glutSolidSphere(3.0, 20, 20);
	glColor3f(0.2, 0.2, 1.0);
	glTranslatef(-1.0, 0, -4.0);
	glutSolidSphere(3.0, 20, 20);
	glPopMatrix();

	glPushMatrix(); //å��
	glColor3f(0.6, 0.2, 0.0);
	glTranslatef(3.0, -7, 1.0);
	glScalef(5.0, 15.0, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.0, -7.0, 2.0);
	glScalef(5.0, 0.5, 3);
	glutSolidCube(1.0);
	glTranslatef(0.0, 7.0, 0.0);
	glutSolidCube(1.0);
	glTranslatef(0.0, 8.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, -4.0, 2.0);
	glScalef(0.5, 9.5, 3);
	glutSolidCube(1.0);
	glTranslatef(9.5, 0.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix(); //�Ʒ�ĭ å
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(1.2, -6.0, 2.0);
	glRotatef(10, 0.0, 0.0, 1.0);
	glScalef(0.5, 2.5, 3);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix(); //��ĭ å
	glColor3f(1.0, 1.0, 0.0);
	glTranslatef(4.7, -2.0, 2.0);
	glRotatef(-10, 0.0, 0.0, 1.0);
	glScalef(0.5, 2.5, 3);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix(); //�ƹ��Բ� ħ���ϳ� ������߰ھ��
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(25.0, -4, 25.0);
	glScalef(5.0, 3.0, 8.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 0.5, 1.0);
	glTranslatef(25.0, -4, 26.0);
	glScalef(5.2, 3.2, 5.2);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();  //����
	glColor3f(1.0, 0.2, 1.0);
	glTranslatef(25.0, -4, 22.0);
	glScalef(2.5, 3.5, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();

	////////Ŭ���� �� ������ �����ִ� ĳ���� 
	if (Sight_Y > 300){
		glPushMatrix();
		glTranslatef(25.0, -2.3, 26.5);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();
		{
			glPushMatrix(); //����
			glTranslatef(-1.5, 2, 0);
			glRotatef(-15, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(-0.5, 0, 0);
			glutSolidCube(1);
			glPopMatrix();
			glPushMatrix();// ������
			glTranslatef(1.5, 2, 0);
			glRotatef(15, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(0.5, 0, 0);
			glutSolidCube(1);
			glPopMatrix();
			glPushMatrix();  // �Ӹ�
			glTranslatef(0, 4, 0);
			glutSolidSphere(1, 20, 20);
			glPopMatrix();
			glPushMatrix();  // �ٵ�
			glColor3f(1, 1, 1);
			glTranslatef(0, 3, 0);
			glRotated(90, 1, 0, 0);
			glutSolidCone(1, 3, 20, 20);
			glPopMatrix();

		}
		glPopMatrix();
	}
	if (Sight_Y < 300){
		glPushMatrix();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.2, 0.7, 1.0, 0.3);
		glTranslatef(25, -6, 2);
		glutSolidCube(2);
		glPopMatrix();

		if (1 < 29 + g_MyPosition.z && 3 >29 + g_MyPosition.z){
			if (24 < 15 + g_MyPosition.x && 26 >15 + g_MyPosition.x){
				g_MyPosition.y -= 0.1;
				if (g_MyPosition.y < -2){
					LevelCheck = 1;
					CharInit();
				}
			}
		}
	}

}


void DrawFloor0(){
	glColor3f(0.2, 0.2, 0.2);
	glBegin(GL_QUADS);
	glVertex3f(0, -7, 0);
	glVertex3f(0, -7, 30);
	glVertex3f(30, -7, 30);
	glVertex3f(30, -7, 0);
	glEnd();
}

//////////////////////1ź �Լ���
GLvoid Level1(){
	DrawFloor1(); //�ٴ�

	glPushMatrix(); //1�� TEST ĳ����
	{
		glTranslatef(65 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);
		if (DownAni == true){
			g_MyPosition.y -= 0.1;
			if (g_MyPosition.y < -5){
				CharInit();
			}
		}
		glPushMatrix(); {
			//glRotatef(theta, rotate_BX, 0.0, rotate_BZ);
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix(); {
				glPushMatrix(); {//����
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);

					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// ������
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // �Ӹ�
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
				}glPopMatrix();
				glPushMatrix(); // �׸���
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // �ٵ�
					glColor3f(1, 1, 1);
					glTranslatef(0, 3, 0);
					glRotated(90, 1, 0, 0);
					glutSolidCone(1, 3, 20, 20);
				}glPopMatrix();
			}glPopMatrix();
		}glPopMatrix();
	}
	glPopMatrix();

	glEnable(GL_TEXTURE_2D); // 1�� ��
	glBindTexture(GL_TEXTURE_2D, texture_object[0]);
	DrawMap(50, -7, 0, 30, 30, 30);
	CheckCollision_Side(); // �� �浹üũ
	glDisable(GL_TEXTURE_2D);
}

void DrawFloor1(){
	for (int z = 25; z>0; z = z - 3){
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(50, -7, z);
		glVertex3f(80, -7, z);
		glEnd();
	}

	for (int x = 55; x < 80; x = x + 5){
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(x, -7, 4);
		glVertex3f(x, -7, 25);
		glEnd();
	}

	glColor3f(1.0, 0.5, 0.5);
	glBegin(GL_QUADS); // ���� ��
	glVertex3f(80, -7, 30);
	glVertex3f(50, -7, 30);
	glVertex3f(50, -7, 25);
	glVertex3f(80, -7, 25);
	glEnd();
	FloorCoC(50, 25, 30, 5);

	glBegin(GL_QUADS); // ���ʱ�
	glVertex3f(80, -7, 4);
	glVertex3f(50, -7, 4);
	glVertex3f(50, -7, 0);
	glVertex3f(80, -7, 0);
	glEnd();
	FloorCoC(50, 0, 30, 4);

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.2, 0.7, 1.0, 0.3);
	glTranslatef(65, -6, 2);
	glutSolidCube(2);
	glPopMatrix();
	if (1 < g_MyPosition.z + 29 && 3 > g_MyPosition.z + 29){
		if (64 < g_MyPosition.x + 65 && 66 > g_MyPosition.x + 65){
			LevelCheck = 2;
			CharInit();
		}
	}

	Tile(55, 22, 5, 3, alpha1[0]); //1
	FloorCoC(55, 22, 5, 3);
	Tile(55, 19, 5, 3, alpha1[1]); //2
	FloorCoC(55, 19, 5, 3);
	Tile(55, 16, 5, 3, alpha1[2]); //3
	Tile(60, 16, 5, 3, alpha1[3]); //4
	FloorCoC(55, 16, 10, 3); // 3,4 ���� üũ
	Tile(60, 13, 5, 3, alpha1[4]); //5
	Tile(65, 13, 5, 3, alpha1[5]); //6
	Tile(70, 13, 5, 3, alpha1[6]); //7
	FloorCoC(60, 13, 15, 3);
	Tile(70, 10, 5, 3, alpha1[7]); //8
	FloorCoC(70, 10, 5, 3);
	Tile(70, 7, 5, 3, alpha1[8]); //9
	Tile(65, 7, 5, 3, alpha1[9]); //10
	FloorCoC(65, 7, 10, 3);
	Tile(65, 4, 5, 3, alpha1[10]); //11
	FloorCoC(65, 4, 5, 3);
}

void Tile(float x, float z, int w, int s, float alpha)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glColor4f(0.2, 0.7, 1.0, alpha);
	glVertex3f(x, -7, z);
	glVertex3f(x, -7, z + s);
	glVertex3f(x + w, -7, z + s);
	glVertex3f(x + w, -7, z);
	glEnd();
}

///////////////2ź �Լ��� 


GLvoid Level2(){
	Enemy2(); //2���� ��ֹ�
	DrawFloor2(); // �ٴ�
	Decoration2(); // 2���� ���ǰ

	glPushMatrix(); //1�� TEST ĳ����
	{
		glTranslatef(115 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);

		glPushMatrix(); {
			//glRotatef(theta, rotate_BX, 0.0, rotate_BZ);
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix(); {
				glPushMatrix(); {//����
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);
					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// ������
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // �Ӹ�
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
				}glPopMatrix();
				glPushMatrix(); // �׸���
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // �ٵ�
					glColor3f(1, 1, 1);
					glTranslatef(0, 3, 0);
					glRotated(90, 1, 0, 0);
					glutSolidCone(1, 3, 20, 20);
				}glPopMatrix();
			}glPopMatrix();
		}glPopMatrix();
	}glPopMatrix();

	glEnable(GL_TEXTURE_2D); // ��
	glBindTexture(GL_TEXTURE_2D, texture_object[1]);
	DrawMap(100, -7, 0, 30, 30, 30);
	CheckCollision_Side(); // �� �浹üũ
	glDisable(GL_TEXTURE_2D);
}

void DrawFloor2(){
	glColor3f(1.0, 0.5, 0.5);
	glBegin(GL_QUADS); // ��� ��
	glVertex3f(118, -7, 27);
	glVertex3f(112, -7, 27);
	glVertex3f(112, -7, 3);
	glVertex3f(118, -7, 3);
	glEnd();

	glBegin(GL_QUADS); // ������ ��
	glVertex3f(126, -7, 27);
	glVertex3f(121, -7, 27);
	glVertex3f(121, -7, 3);
	glVertex3f(126, -7, 3);
	glEnd();

	glBegin(GL_QUADS); // ���� ��
	glVertex3f(109, -7, 27);
	glVertex3f(104, -7, 27);
	glVertex3f(104, -7, 3);
	glVertex3f(109, -7, 3);
	glEnd();

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_QUADS); // ���� ��
	glVertex3f(130, -7, 30);
	glVertex3f(100, -7, 30);
	glVertex3f(100, -7, 27);
	glVertex3f(130, -7, 27);
	glEnd();

	glBegin(GL_QUADS); // ���ʱ�
	glVertex3f(130, -7, 3);
	glVertex3f(100, -7, 3);
	glVertex3f(100, -7, 0);
	glVertex3f(130, -7, 0);
	glEnd();


	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.2, 0.7, 1.0, 0.3);
	glTranslatef(115, -6, 2);
	glutSolidCube(2);
	glPopMatrix();
	if (1 < g_MyPosition.z + 29 && 3 > g_MyPosition.z + 29){
		if (114 < g_MyPosition.x + 115 && 116 > g_MyPosition.x + 115){
			LevelCheck = 3;
			CharInit();
		}
	}

	for (int i = 0; i < ENEMYMAX; i++){
		for (int j = 0; j < 100; j++){
			if (g_Shot[i].state == true){ // �Ѿ�
				if (g_Enemy[i].x - 1.5 <  g_Shot[j].x &&
					g_Enemy[i].x + 1.5 >  g_Shot[j].x)
					if (g_Enemy[i].z + 1.5 > g_Shot[j].z &&
						g_Enemy[i].z - 1.5  < g_Shot[j].z){
						g_Enemy[i].z = -4;
					}
			}
		}
	}
}

void Decoration2(){
	theta_2 += 2;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	for (int i = 0; i<24; i = i + 3){
		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();      // �����ʱ��
		glTranslatef(128.2, 18.0 - i, 1.4);
		glRotatef(theta_2, 0.0, 1.0, 0.0);
		glRotatef(45, 1.0, 1.0, 1.0);
		glutSolidCube(1.5);
		glPopMatrix();

		glPushMatrix();      // ���ʱ��
		glTranslatef(102.2, 18.0 - i, 1.4);
		glRotatef(theta_2, 0.0, 1.0, 0.0);
		glRotatef(45, 1.0, 1.0, 1.0);
		glutSolidCube(1.5);
		glPopMatrix();
	}

	glColor4f(1.0, 0.5, 1.0, 0.5);
	glPushMatrix();      // �����ʱ��
	glTranslatef(128, 7.0, 2.0);
	glScalef(3.0, 28.0, 2.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();      // ���ʱ��
	glTranslatef(102.2, 7.0, 2.0);
	glScalef(3.0, 28.0, 2.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glDisable(GL_BLEND);
}

void Enemy2()
{
	for (int i = 0; i<ENEMYMAX; ++i)
	{
		glPushMatrix();
		{
			glColor3f(0.5, 1.0, 1.0);
			glTranslatef(g_Enemy[i].x, g_Enemy[i].y, g_Enemy[i].z);
			glutSolidCube(1.5);
			glColor3f(0.0, 0.0, 0.0);
			glutWireCube(1.51);

			// ������������
			glColor3f(1.0, 0.0, 0.0);
			glRotatef(-90, 1.0, 0.0, 0.0);
			glutSolidCone(0.5, 2.0, 20, 20);
			glColor3f(1.0, 0.0, 0.0);
			glRotatef(90, 1.0, 0.0, 0.0);
			glutSolidCone(0.5, 2.0, 20, 20);
			//glColor3f(1.0, 0.0, 0.0);
			//glRotatef(90, 0.0, 1.0, 0.0);
			//glutSolidCone(0.5, 2.0, 20, 20);
			//glColor3f(1.0, 0.0, 0.0);
			//glRotatef(180, 0.0, 1.0, 0.0);
			//glutSolidCone(0.5, 2.0, 20, 20);
		}
		glPopMatrix();
	}
}

void gun()
{
	for (int i = 0; i < 100; ++i)
	{
		glPushMatrix();
		{
			glColor3f(0.5, 1.0, 1.0);
			glTranslatef(g_Shot[i].x, g_Shot[i].y, g_Shot[i].z);

			//  �� ����
			glutWireCube(0.51);
			glColor3f(1, 0, 1);
			glutSolidCube(0.5);
		}
		glPopMatrix();
	}
}
//////////////3ź �Լ���

GLvoid Level3(){
	glPushMatrix(); //3�� TEST ĳ����
	{
		glTranslatef(165 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);

		glPushMatrix(); {
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix(); {
				glPushMatrix(); {//����
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);

					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// ������
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // �Ӹ�
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
					glPushMatrix(); {
						glColor3f(0, 1, 0);
						glTranslatef(0, 6, 0);
						glScalef(1, 50, 1);
						glutSolidCube(0.2);
					}glPopMatrix();
				}glPopMatrix();
				glPushMatrix(); // �׸���
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // �ٵ�
					glColor3f(1, 1, 1);
					glTranslatef(0, 3, 0);
					glRotated(90, 1, 0, 0);
					glutSolidCone(1, 3, 20, 20);
				}glPopMatrix();
			}glPopMatrix();
		}glPopMatrix();
	}
	glPopMatrix();

	DrawFloor3();

	glEnable(GL_TEXTURE_2D); // ��
	glBindTexture(GL_TEXTURE_2D, texture_object[2]);
	CheckCollision_Side(); // �� �浹üũ
	DrawMap(150, -7, 0, 30, 30, 30);
	glDisable(GL_TEXTURE_2D);

}

void CheckCollision(float x1, float x2, float z1, float z2, float r)
{
	// (z2 < Z < z1) �϶� -> ����,������ �浹üũ
	if (g_MyPosition.z < z1 && g_MyPosition.z > z2)
	{
		if (g_MyPosition.x > (x1 + x2) / 2) // ������ �浹üũ (= (������x��+����x��)/2)
		{
			if (g_MyPosition.x < x1 + r + 1.5) // ((������ �� x�� = 4.0) + (���� ������ = 0.5))
			{
				Collision_state = true;
				g_MyPosition.x = x1 + r + 1.5;
			}
			if (g_MyPosition.x >= x1 + r + 1.5)
				Collision_state = false;
		}
		if (g_MyPosition.x < (x1 + x2) / 2) // ���� �浹üũ
		{
			if (g_MyPosition.x > x2 - r - 1.5) // ( (���� �� x�� = -3.0) - (���� ������ = 0.5))
			{
				Collision_state = true;
				g_MyPosition.x = x2 - r - 1.5;
			}
			if (g_MyPosition.x <= x2 - r - 1.5)
				Collision_state = false;
		}
	}
	else
		Collision_state = false;

	// (x2 < X < x1) �϶� -> ����, ���� �浹üũ
	if (g_MyPosition.x < x1 + r && g_MyPosition.x > x2 - r)
	{
		if (g_MyPosition.z > (z1 + z2) / 2) // ���� �浹üũ ( -7.5 = (����z��+����z��/2))
		{
			if (g_MyPosition.z < z1 + r) // (����z�� = -7.0) + (�������� = 0.5)
			{
				Collision_state = true;
				g_MyPosition.z = z1 + r;
			}
			if (g_MyPosition.z >= z1 + r)
				Collision_state = false;
		}
		if (g_MyPosition.z < (z1 + z2) / 2) // ���� �浹üũ
		{
			if (g_MyPosition.z > z2 - r) // (����z�� = -8.0) - (�������� = 0.5)
			{
				Collision_state = true;
				g_MyPosition.z = z2 - r;
			}
			if (g_MyPosition.z <= z2 - r)
				Collision_state = false;
		}
	}
	else
		Collision_state = false;
}

void DrawFloor3(){
	glColor3f(0.2, 0.5, 0.5);

	//�ǹ�
	glColor4f(1.0, 0.6, 0.6, 0.9);
	DrawBuilding(150, -6, 4, 7, 9, 2); //1
	DrawBuilding(162, -6, 4, 7, 9, 2); //2
	DrawBuilding(174, -6, 0, 2, 9, 8); //3
	DrawBuilding(160, -6, 17, 2, 9, 13); //4
	DrawBuilding(155, -6, 17, 5, 9, 2); //5
	DrawBuilding(155, -6, 11, 26, 9, 2); //6
	DrawBuilding(167, -6, 18, 7, 9, 2); //7
	DrawBuilding(174, -6, 18, 2, 9, 9); //8
	DrawBuilding(167, -6, 24, 2, 9, 6); //9
	DrawBuilding(150, -6, 23, 5, 9, 2); //10

	//�浹üũ
	CheckCollision(-8, -15, -23, -25, 1); //1
	CheckCollision(4, -3, -23, -25, 1); //2
	CheckCollision(11, 9, -22, -30, 1); //3
	CheckCollision(-3, -5, 1, -12, 1); //4
	CheckCollision(-5, -10, -10, -12, 1); //5
	CheckCollision(16, -10, -16, -18, 1); //6
	CheckCollision(9, 2, -9, -11, 1); //7
	CheckCollision(11, 9, -2, -11, 1); //8
	CheckCollision(4, 2, 1, -5, 1); //9
	CheckCollision(-10, -15, -4, -6, 1); //10


	//DrawMap(150, -7, 0, 30, 30, 30);
	glPushMatrix();
	glColor3f(0.5, 1, 0);
	for (double i = 0; i < 30; i += 0.05)  //   ������ �ɰ��� 
	{
		for (double j = 150; j < 180; j += 0.05)
		{
			glBegin(GL_QUADS);
			{
				glVertex3f(j, -7, i);
				glVertex3f(j + 1, -7, i);
				glVertex3f(j + 1, -7, i - 1);
				glVertex3f(j, -7, i - 1);
			}
			glEnd();
		}
	}glPopMatrix();



	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 0, 0, 0.6);
	glTranslatef(178, -6, 2);
	glutSolidCube(2);
	glPopMatrix();

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 0, 0, 0.6);
	glTranslatef(152, -6, 14);
	glutSolidCube(2);
	glPopMatrix();

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 0, 0, 0.6);
	glTranslatef(167, -6, 8);
	glutSolidCube(2);
	glPopMatrix();

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 0, 0, 0.6);
	glTranslatef(152, -6, 28);
	glutSolidCube(2);
	glPopMatrix();

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 0, 0, 0.6);
	glTranslatef(177, -6, 28);
	glutSolidCube(2);
	glPopMatrix();
	if (1 < g_MyPosition.z + 29 && 3 > g_MyPosition.z + 29){
		if (177 < g_MyPosition.x + 165 && 179 > g_MyPosition.x + 165){
			LevelCheck = 4;
			CharInit();
		}
	}
}

void DrawBuilding(float x, float y, float z, int w, int h, int s){ // �ǹ� ���� �Լ�
	glBegin(GL_QUADS);
	//�޸�
	glVertex3f(x, y, z);
	glVertex3f(x, y + h, z);
	glVertex3f(x + w, y + h, z);
	glVertex3f(x + w, y, z);
	//�޸�
	glVertex3f(x, y + h, z + s);
	glVertex3f(x, y + h, z);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + s);
	//������
	glVertex3f(x + w, y + h, z);
	glVertex3f(x + w, y + h, z + s);
	glVertex3f(x + w, y, z + s);
	glVertex3f(x + w, y, z);
	//�ո�
	glVertex3f(x + w, y + h, z + s);
	glVertex3f(x, y + h, z + s);
	glVertex3f(x, y, z + s);
	glVertex3f(x + w, y, z + s);
	//����
	glVertex3f(x + w, y + h, z);
	glVertex3f(x, y + h, z);
	glVertex3f(x, y + h, z + s);
	glVertex3f(x + w, y + h, z + s);
	glEnd();
}

////////////////// Level 4

GLvoid Level4(){

	DrawFloor4(); // �ٴ�

	glPushMatrix(); //4�� TEST ĳ����
	{
		glTranslatef(215 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);
		glPushMatrix(); {
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix(); {
				glPushMatrix(); {//����
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);

					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// ������
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // �Ӹ�
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
				}glPopMatrix();
				glPushMatrix(); // �׸���
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // �ٵ�
					glColor3f(1, 1, 1);
					glTranslatef(0, 3, 0);
					glRotated(90, 1, 0, 0);
					glutSolidCone(1, 3, 20, 20);
				}glPopMatrix();
			}glPopMatrix();
		}glPopMatrix();
	}
	glPopMatrix();


	glEnable(GL_TEXTURE_2D); // ��4
	glBindTexture(GL_TEXTURE_2D, texture_object[4]);
	if (BossCount < 21)
		DrawMap(200, -7, 0, 30, 30, 30);
	CheckCollision_Side();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D); // ��5
	glBindTexture(GL_TEXTURE_2D, texture_object[5]);
	DrawMap(200, -48, 0, 30, 30, 30);
	CheckCollision_Side();
	glDisable(GL_TEXTURE_2D);
}


void DrawFloor4(){

	glBegin(GL_QUADS);
	for (int j = 0; j <= 24; j = j + 6)
	{
		for (int i = 0; i <= 24; i = i + 6)
		{
			glColor3f(0.0, 0.3, 0.3);
			glVertex3f(200 + i, -7 + a, j);
			glVertex3f(200 + i, -7 + a, j + 3);
			glVertex3f(200 + i + 3, -7 + a, j + 3);
			glVertex3f(200 + i + 3, -7 + a, j);

			glColor3f(0.0, 0.1, 0.1);
			glVertex3f(203 + i, -7 + a, j);
			glVertex3f(203 + i, -7 + a, j + 3);
			glVertex3f(203 + i + 3, -7 + a, j + 3);
			glVertex3f(203 + i + 3, -7 + a, j);
		}
	}

	for (int j = 3; j <= 27; j = j + 6)
	{
		for (int i = 0; i <= 24; i = i + 6)
		{
			glColor3f(0.0, 0.1, 0.1);
			glVertex3f(200 + i, -7 + a, j);
			glVertex3f(200 + i, -7 + a, j + 3);
			glVertex3f(200 + i + 3, -7 + a, j + 3);
			glVertex3f(200 + i + 3, -7 + a, j);

			glColor3f(0.0, 0.3, 0.3);
			glVertex3f(203 + i, -7 + a, j);
			glVertex3f(203 + i, -7 + a, j + 3);
			glVertex3f(203 + i + 3, -7 + a, j + 3);
			glVertex3f(203 + i + 3, -7 + a, j);
		}
	}
	glEnd();

	GLfloat ctrlpoints[18][3] = {
		{ 204, -6, 27 }, { 207, -6, 24 }, { 211, -6, 13 }, { 218, -6, 15 },
		{ 219, -6, 23 }, { 223, -6, 26 }, { 217, -6, 17 }, { 224, -6, 17 },
		{ 227, -6, 10 }, { 225, -6, 6 }, { 218, -6, 3 }, { 221, -6, 2 },
		{ 205, -6, 3 }, { 210, -6, 10 }, { 206, -6, 9 }, { 201, -6, 11 },
		{ 215, -6, 18 }, { 204, -6, 20 }
	};
	for (int i = 0; i < 18; i++){
		P[i].x = ctrlpoints[i][0];
		P[i].y = ctrlpoints[i][1];
		P[i].z = ctrlpoints[i][2];
	}
	if (BossCount < 21){
		if (muCount == 0){

			Bezier4(P[0], P[1], P[2], P[3], g_fMu);
			if (g_fMu > 1){
				muCount++;
				g_fMu = 0;
			}
		}
		if (muCount == 1){
			Bezier4(P[3], P[4], P[5], P[6], g_fMu);
			if (g_fMu > 1){
				muCount++;
				g_fMu = 0;

			}

		}
		if (muCount == 2){
			Bezier4(P[6], P[7], P[8], P[9], g_fMu);
			if (g_fMu > 1){
				muCount++;
				g_fMu = 0;
			}
		}
		if (muCount == 3){
			Bezier4(P[9], P[10], P[11], P[12], g_fMu);
			if (g_fMu > 1){
				muCount++;
				g_fMu = 0;

			}
		}
		if (muCount == 4){
			Bezier4(P[12], P[13], P[14], P[15], g_fMu);
			if (g_fMu > 1){
				muCount++;
				g_fMu = 0;
			}
		}
		if (muCount == 5){
			Bezier4(P[15], P[16], P[17], P[18], g_fMu);
			if (g_fMu > 1){
				muCount = 0;
				g_fMu = 0;
			}
		}
		glPushMatrix(); //4�� �䳢�����䳢
		{
			glTranslatef(p.x, p.y + a, p.z);
			if (BossCheck == true)
				glScalef(5, 5, 5);
			glPushMatrix(); //����
			if (BossCheck == false)
				glColor4f(0.9, 0.9, 0.9, 1);
			else
				glColor4f(1.0, 0.1, 0.1, 1);
			glScalef(1.4, 2.0, 1.4);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //�Ӹ�
			glTranslatef(0, 2.5, 0);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //���ʱ�
			glTranslatef(-0.5, 4, 0);
			glRotatef(20, 0.0, 0.0, 1.0);
			glScalef(0.4, 2.0, 0.2);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //�����ʱ�
			glTranslatef(0.5, 4, 0);
			glRotatef(-20, 0.0, 0.0, 1.0);
			glScalef(0.4, 2.0, 0.2);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //��
			glColor3f(0.0, 0.0, 0.0);
			glTranslatef(0.3, 3, 1);
			glutSolidSphere(0.2, 20, 20);
			glTranslatef(-0.6, 0, 0);
			glutSolidSphere(0.2, 20, 20);
			glPopMatrix();
			glPushMatrix(); //��
			glColor4f(1.0, 1.0, 1.0, 1);
			glScalef(1.3, 2.0, 1.5);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
		}
		glPopMatrix();
		g_fMu += 0.03;
	}
	if (p.x + 1.5 > 215 + g_MyPosition.x && p.x - 1.5 < 215 + g_MyPosition.x){
		if (p.z + 1.5 > 25 + g_MyPosition.z && p.z - 1.5 < 25 + g_MyPosition.z){
			Rabbit_CC = true;
		}
	}
	if (Rabbit_CC == true && BossCheck == false && BossCount <21){
		a -= 0.5;
		g_MyPosition.y -= 0.5;
		if (a < -40)
			BossCheck = true;
	}
	if (BossCheck == true){
		if (p.x + 1.4 * 5 > 215 + g_MyPosition.x && p.x - 1.4 * 5 < 215 + g_MyPosition.x){
			if (p.z + 1.4 * 5 > 25 + g_MyPosition.z && p.z - 1.4 * 5 < 25 + g_MyPosition.z){
				LevelCheck = 0;
				CharInit();
			}
		}
	}

	if (BossCheck == true){
		for (int i = 0; i < 100; i++){
			if (p.x + 1.4 * 5 > g_Shot[i].x && p.x - 1.4 * 5 < g_Shot[i].x){
				if (p.z + 1.4 * 5 > g_Shot[i].z && p.z - 1.4 * 5 < g_Shot[i].z){
					BossCount++;
					BossHP--;
					g_Shot[i].state = false;
					g_Shot[i].x = -100;
					g_Shot[i].y = -100;
					g_Shot[i].z = -100;
					printf("%d", BossCount);
					break;

				}
			}
		}
	}
	if (BossHP < 0)
		BossHP = 0;
	glPushMatrix();//FPS ǥ��
	glColor3f(1, 1, 1);
	if (LevelCheck == 4 && a < -40){
		glTranslatef(g_MyPosition.x + 220, 2 + g_MyPosition.y, g_MyPosition.z + 30);
		glRasterPos2f(0.0, 0.0);
		sprintf(draw_FPS, "Boss HP : %d", BossHP);
		int len = (int)strlen(draw_FPS);
		for (int i = 0; i < len; i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, draw_FPS[i]);
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();
	if (BossCount > 20){
		BossCheck = false;
		Sight_Y += 1;
		Sight_Z += 1;
	}
	// Sight_Y > 300 -> �޿��� ��� 
	if (Sight_Y > 300){
		LevelCheck = 0;
		CharInit();
	}
}