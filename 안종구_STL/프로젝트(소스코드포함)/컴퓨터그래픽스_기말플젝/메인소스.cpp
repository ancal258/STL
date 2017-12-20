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

// 컨테이너 비교
#include <vector>
#include <list>
#include <deque>
// map, set X


#define ENEMYMAX 6

// STL 리플레이 사용 
using namespace std;
using namespace chrono;
using ReplayPair = pair<char, double>;

vector<ReplayPair> input;	// 입력할 때
vector<ReplayPair> myTime;	// 시간 출력할 때
vector<ReplayPair> stepTime;	// 단계별 출력할 때

ofstream out; // 파일 입력
ifstream in; // 파일 출력
string fileName = "myRep.replay"; // 파일 이름

system_clock::time_point time_start, time_play;	//입력하기 누르고 시간 재기.
duration<double, micro> d;

char charRead; // 입력 읽기
char step; // 스텝 읽기

double timeRead; // 시간 읽기
bool oCheck; // o입력 검사
bool iCheck; // i입력 검사
vector<ReplayPair>::iterator i;
vector<ReplayPair>::iterator i2;


// 함수 정리

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid TimerFunction(int value);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUP(unsigned char key, int x, int y);
GLvoid gun();
GLvoid inputKey(char key);
static bool g_keyState[0xff] = { 0, };

//통일해서 쓰는 함수
void FloorCoC(float x, float z, int w, int s);
void CharInit();
void DrawMap(float x, float y, float z, int w, int h, int s); //벽
void CheckCollision_Side(); //벽 충돌체크
bool Collision_state = false;
void SetUP();
//0탄 함수
GLvoid Level0();
void DrawFloor0(); //0번방 바닥
void Decoration0(); //0번방 장식품 총집합
//1탄 함수
GLvoid Level1();
void DrawFloor1(); //1번방 바닥
void Tile(float x, float z, int w, int s, float alpha); //타일

// 2탄 함수
GLvoid Level2(); //2번 레벨
void DrawFloor2(); //2번방 바닥
void Decoration2(); //2번방 장식품 총집합
void Enemy2(); //2번방 적
double theta_2 = 0.0; //2번방 장식품 회전
double moveZ_Enemy2 = 0.0; //2번방 장애물 이동
bool moveZ_Enemy_state = true;

// 3탄 함수
GLvoid Level3(); //3번 레벨
void DrawFloor3(); //3번방 바닥
void DrawBuilding(float x, float y, float z, int w, int h, int s); //건물
void CheckCollision(float x1, float x2, float z1, float z2, float r); //건물 충돌체크

// 4탄 함수
void DrawFloor4(); //4번방 바닥
GLvoid Level4();

int Shot_Gun = 100;
char draw_FPS[100];
void update_FPS();

float g_fMu; // 베지어
int muCount; // 베지어

double ambient = 0.4; // 광원의 ambient light 조절
double specular = 1; // 광원의 specular light 조절
double diffuse = 0.8; // 광원의 diffuse light 조절
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
GLuint texture_object[6]; //벽(2)
GLubyte *pBytes; //데이터를 가리킬 포인터
BITMAPINFO *info; // 비트맵 헤더 저장할 변수



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

	// 바이너리 읽기 모드로 파일을 연다
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	// 비트맵 파일 헤더를 읽는다.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}

	// 파일이 BMP 파일인지 확인한다.
	if (header.bfType != 'MB') {
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER 위치로 간다.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// 비트맵 이미지 데이터를 넣을 메모리 할당을 한다.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		fclose(fp);
		exit(0);
		return NULL;
	}

	// 비트맵 인포 헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵의 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
		(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);

	// 비트맵의 크기만큼 메모리를 할당한다.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵 데이터를 bit(GLubyte 타입)에 저장한다.
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


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정
	glutInitWindowSize(800, 800); // 윈도우의 크기 지정
	glutCreateWindow("1번방"); // 윈도우 생성 (윈도우 이름)

	SetUP();
	glutReshapeFunc(Reshape);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUP);
	glutTimerFunc(300, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene(GLvoid)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // 배경 색
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
	//1번
	if (LevelCheck == 1){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(65.0 + g_MyPosition.x, 14 + g_MyPosition.y*1.4, 48.0 + g_MyPosition.z,
			65.0 + g_MyPosition.x, 0.0, 0.0, 0.0, 1.0, 0.0);
	}//2번
	else if (LevelCheck == 2){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(115.0, 7.0, 48.0 + g_MyPosition.z, 115.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}//3번
	else if (LevelCheck == 3){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(165.0 + g_MyPosition.x, 7.0 + -(g_MyPosition.z) + 5, 45.0 + g_MyPosition.z + 5, 165.0 + g_MyPosition.x, 0.0, 0.0, 0.0, 1.0, 0.0);
	}//4번
	else if (LevelCheck == 4){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, 1.0, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(215.0 + g_MyPosition.x, 14 + g_MyPosition.y + Sight_Y, 47.0 + g_MyPosition.z + Sight_Z, 215.0 + g_MyPosition.x, 7 + g_MyPosition.y, 0.0, 0.0, 1.0, 0.0);
	}

	glEnable(GL_LIGHTING); // 조명효과 설정
	GLfloat ambientLight[] = { 0.2f, 0.2, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	GLfloat AmbientLight[] = { ambient, ambient, ambient, 1.0f };
	GLfloat DiffuseLight[] = { diffuse, diffuse, diffuse, 1.0f };
	GLfloat SpecularLight[] = { specular, specular, specular, 1.0 };
	GLfloat SPOTD[] = { 0, -1, 0 };

	/// 1번 조명 ///
	GLfloat lightPos1[] = { 4.0, 4.0, 5.0, 1.0 };
	GLfloat lightPos[] = { 165 + g_MyPosition.x, 2, 29 + g_MyPosition.z, 1.0 };

	GLfloat lightPos2[] = { p.x, 2, p.z, 1.0 };

	// 1번 조명 설정 //
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

	/// 각 물체에 색을 입히기 위한 ///
	GLfloat specref1[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientLight);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref1);
	glMateriali(GL_FRONT, GL_SHININESS, 60);
	/// 각 물체에 색을 입히기 위한 ///

	if (Sight_Y < 300){
		glPushMatrix();//FPS 표시
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
		glPushMatrix();//FPS 표시
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

	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid TimerFunction(int value)
{
	


	//이건 일단 슈팅에서 가져온거
	for (int i = 0; i < ENEMYMAX; i++){
		if (g_Enemy[i].state == false){ //2번방 적
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
		if (g_Shot[i].state == true){ // 총알
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
		if (g_keyState['t']) { // 저장 시작 
			out.open(fileName, ios::out, ios::trunc);
			out.clear();
			time_start = system_clock::now();
		}
		if (g_keyState['p'])  // 저장
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
			cout << "끝 엔드만남" << endl;
			inputKey('`');
			oCheck = false;
		}
	}
	if (iCheck == true) {
		time_play = system_clock::now();
		d = time_play - time_start;
		if (g_keyState['m']) { // -> 키
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
			cout << " 스텝 엔드 " << endl;
			inputKey('`');
			iCheck = false;
		}
	}


	if (g_keyState['o']) {

		in.open(fileName, ios::in);
		cout << " 파일 출력 " << endl;
		myTime.clear();
		if (in.is_open()) {
			while (!in.eof()) {
				in >> charRead;
				in >> timeRead;
				myTime.push_back(make_pair(charRead, timeRead));
			}cout << " 출력 완료 " << endl;
			in.close();
			time_start = system_clock::now();

			i = myTime.begin();
			oCheck = true;
			//cout << "end" << endl;
		}
	}
	if (g_keyState['i']) {
		in.open(fileName, ios::in);
		cout << " 파일 출력 (Step) " << endl;
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
	glutPostRedisplay(); // 화면 재 출력
	glutTimerFunc(30, TimerFunction, 1); // 타이머함수 재 설정
}

GLvoid inputKey(char key) {
	//char key = '.';
	static  float cubeSpeed = 0.2;

	if (DownAni == false) {
		if (key == '`') { // 0탄으로 전환
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
			for (int i = 0; i<100; i++) // 2번 적 좌표 카운트
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
			cout << "저장이 완료되었음" << endl;
		}





	}
}

GLvoid Keyboard(unsigned char key, int x, int y){
	
	char kkey;
	

	
	
	if (Collision_state == false)
		g_keyState[key] = true;

	//if (g_keyState['p']){ // 치트키 p  ( 알파 1로 초기화) , 총알 100개 재충전
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


	//if (g_keyState['`']){ // 0탄으로 전환
	//	LevelCheck = 0;
	//	Shot_Gun = 100;
	//	BossHP = 20;
	//	Sight_Y = 0;
	//	Sight_Z = 0;
	//	CharInit();
	//}
	if (g_keyState['1']){ // 1탄으로 전환
		LevelCheck = 1;
		CharInit();
	}
	if (g_keyState['2']){ // 2탄으로 전환
		LevelCheck = 2;
		CharInit();
	}
	if (g_keyState['3']){ // 3탄으로 전환
		LevelCheck = 3;
		CharInit();
	}
	if (g_keyState['4']){ // 4탄으로 전환
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

void DrawMap(float x, float y, float z, int w, int h, int s){ // 맵 만드는 함수
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	//뒷면
	glTexCoord2f(0, 1);
	glVertex3f(x, y + h, z);
	glTexCoord2f(0, 0);
	glVertex3f(x, y, z);
	glTexCoord2f(1, 0);
	glVertex3f(x + w, y, z);
	glTexCoord2f(1, 1);
	glVertex3f(x + w, y + h, z);

	//왼면
	glTexCoord2f(0, 1);
	glVertex3f(x, y + h, z + s);
	glTexCoord2f(0, 0);
	glVertex3f(x, y, z + s);
	glTexCoord2f(1, 0);
	glVertex3f(x, y, z);
	glTexCoord2f(1, 1);
	glVertex3f(x, y + h, z);

	//오른면
	glTexCoord2f(0, 1);
	glVertex3f(x + w, y + h, z);
	glTexCoord2f(0, 0);
	glVertex3f(x + w, y, z);
	glTexCoord2f(1, 0);
	glVertex3f(x + w, y, z + s);
	glTexCoord2f(1, 1);
	glVertex3f(x + w, y + h, z + s);

	//앞면
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
			printf("충돌\n");
		}
		else
			Collision_state = false;
		if (g_MyPosition.x  < -13){
			g_MyPosition.x = -13;
			Collision_state = true;
			printf("충돌\n");
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
			printf("충돌\n");
		}
		else
			Collision_state = false;
		if (g_MyPosition.z  < -28){
			g_MyPosition.z = -28;
			Collision_state = true;
			printf("충돌\n");
		}
		else
			Collision_state = false;
	}
	else
		Collision_state = false;
}

void SetUP()
{
	glEnable(GL_DEPTH_TEST); // 은면제거

	g_MyPosition.y = -6; //내 캐릭터

	glGenTextures(5, texture_object);


	glBindTexture(GL_TEXTURE_2D, texture_object[0]);
	pBytes = LoadDIBitmap("1번방 벽지.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 350, 391, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[1]);
	pBytes = LoadDIBitmap("2번방 벽.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 450, 412, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[2]);
	pBytes = LoadDIBitmap("3번방 벽지.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 350, 350, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[3]);
	pBytes = LoadDIBitmap("0번방 벽지.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 413, 375, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture_object[4]);
	pBytes = LoadDIBitmap("4번방 벽지.bmp", &info);
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

	for (int i = 0; i<ENEMYMAX; i++) // 2번 적 좌표 카운트
	{
		g_Enemy[i].x = (rand() % 30) + 100;
		g_Enemy[i].z = 4;
		g_Enemy[i].y = -5;
	}

	for (int i = 0; i < 11; i++){ // 알파값 초기화
		alpha1[i] = 1 + (i*0.1);
	}
}
////////////////////////0탄 함수들
GLvoid Level0(){
	DrawFloor0(); // 바닥
	Decoration0();  // 장식

	glPushMatrix();
	if (Sight_Y > 300)
		glTranslatef(-200 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);
	glTranslatef(15 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);

	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix(); {
		glPushMatrix(); {//왼팔
			glTranslatef(-1.5, 2, 0);
			glRotatef(-Arm_R, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(-0.5, 0, 0);

			glutSolidCube(1);
		}   glPopMatrix();
		glPushMatrix(); {// 오른팔
			glTranslatef(1.5, 2, 0);
			glRotatef(Arm_R, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(0.5, 0, 0);
			glutSolidCube(1);
		}glPopMatrix();
		glPushMatrix(); { // 머리
			glTranslatef(0, Head_T, 0);
			glutSolidSphere(1, 20, 20);
		}glPopMatrix();
		glPushMatrix(); // 그림자
		glColor3f(0, 0, 0);
		glTranslatef(0, -0.7, 0);
		glScalef(0.2, 0.01, 0.2);
		glutSolidSphere(1, 20, 20);
		glPopMatrix();
		glPushMatrix(); { // 바디
			glColor3f(1, 1, 1);
			glTranslatef(0, 3, 0);
			glRotated(90, 1, 0, 0);
			glutSolidCone(1, 3, 20, 20);
		}glPopMatrix();

	}glPopMatrix();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D); // 벽
	glBindTexture(GL_TEXTURE_2D, texture_object[3]);
	DrawMap(0, -7, 0, 30, 30, 30);
	CheckCollision_Side();
	glDisable(GL_TEXTURE_2D);
}
void Decoration0()
{
	glPushMatrix(); //공방석
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

	glPushMatrix(); //책장
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

	glPushMatrix(); //아랫칸 책
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(1.2, -6.0, 2.0);
	glRotatef(10, 0.0, 0.0, 1.0);
	glScalef(0.5, 2.5, 3);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix(); //윗칸 책
	glColor3f(1.0, 1.0, 0.0);
	glTranslatef(4.7, -2.0, 2.0);
	glRotatef(-10, 0.0, 0.0, 1.0);
	glScalef(0.5, 2.5, 3);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix(); //아버님께 침대하나 놔드려야겠어요
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

	glPushMatrix();  //베개
	glColor3f(1.0, 0.2, 1.0);
	glTranslatef(25.0, -4, 22.0);
	glScalef(2.5, 3.5, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();

	////////클리어 시 나오는 누워있는 캐리터 
	if (Sight_Y > 300){
		glPushMatrix();
		glTranslatef(25.0, -2.3, 26.5);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();
		{
			glPushMatrix(); //왼팔
			glTranslatef(-1.5, 2, 0);
			glRotatef(-15, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(-0.5, 0, 0);
			glutSolidCube(1);
			glPopMatrix();
			glPushMatrix();// 오른팔
			glTranslatef(1.5, 2, 0);
			glRotatef(15, 0, 0, 1);
			glScalef(0.4, 1.5, 0.6);
			glTranslatef(0.5, 0, 0);
			glutSolidCube(1);
			glPopMatrix();
			glPushMatrix();  // 머리
			glTranslatef(0, 4, 0);
			glutSolidSphere(1, 20, 20);
			glPopMatrix();
			glPushMatrix();  // 바디
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

//////////////////////1탄 함수들
GLvoid Level1(){
	DrawFloor1(); //바닥

	glPushMatrix(); //1번 TEST 캐릭터
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
				glPushMatrix(); {//왼팔
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);

					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// 오른팔
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // 머리
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
				}glPopMatrix();
				glPushMatrix(); // 그림자
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // 바디
					glColor3f(1, 1, 1);
					glTranslatef(0, 3, 0);
					glRotated(90, 1, 0, 0);
					glutSolidCone(1, 3, 20, 20);
				}glPopMatrix();
			}glPopMatrix();
		}glPopMatrix();
	}
	glPopMatrix();

	glEnable(GL_TEXTURE_2D); // 1번 벽
	glBindTexture(GL_TEXTURE_2D, texture_object[0]);
	DrawMap(50, -7, 0, 30, 30, 30);
	CheckCollision_Side(); // 맵 충돌체크
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
	glBegin(GL_QUADS); // 앞쪽 길
	glVertex3f(80, -7, 30);
	glVertex3f(50, -7, 30);
	glVertex3f(50, -7, 25);
	glVertex3f(80, -7, 25);
	glEnd();
	FloorCoC(50, 25, 30, 5);

	glBegin(GL_QUADS); // 뒤쪽길
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
	FloorCoC(55, 16, 10, 3); // 3,4 동시 체크
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

///////////////2탄 함수들 


GLvoid Level2(){
	Enemy2(); //2번방 장애물
	DrawFloor2(); // 바닥
	Decoration2(); // 2번방 장식품

	glPushMatrix(); //1번 TEST 캐릭터
	{
		glTranslatef(115 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);

		glPushMatrix(); {
			//glRotatef(theta, rotate_BX, 0.0, rotate_BZ);
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix(); {
				glPushMatrix(); {//왼팔
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);
					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// 오른팔
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // 머리
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
				}glPopMatrix();
				glPushMatrix(); // 그림자
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // 바디
					glColor3f(1, 1, 1);
					glTranslatef(0, 3, 0);
					glRotated(90, 1, 0, 0);
					glutSolidCone(1, 3, 20, 20);
				}glPopMatrix();
			}glPopMatrix();
		}glPopMatrix();
	}glPopMatrix();

	glEnable(GL_TEXTURE_2D); // 벽
	glBindTexture(GL_TEXTURE_2D, texture_object[1]);
	DrawMap(100, -7, 0, 30, 30, 30);
	CheckCollision_Side(); // 맵 충돌체크
	glDisable(GL_TEXTURE_2D);
}

void DrawFloor2(){
	glColor3f(1.0, 0.5, 0.5);
	glBegin(GL_QUADS); // 가운데 길
	glVertex3f(118, -7, 27);
	glVertex3f(112, -7, 27);
	glVertex3f(112, -7, 3);
	glVertex3f(118, -7, 3);
	glEnd();

	glBegin(GL_QUADS); // 오른쪽 길
	glVertex3f(126, -7, 27);
	glVertex3f(121, -7, 27);
	glVertex3f(121, -7, 3);
	glVertex3f(126, -7, 3);
	glEnd();

	glBegin(GL_QUADS); // 왼쪽 길
	glVertex3f(109, -7, 27);
	glVertex3f(104, -7, 27);
	glVertex3f(104, -7, 3);
	glVertex3f(109, -7, 3);
	glEnd();

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_QUADS); // 앞쪽 길
	glVertex3f(130, -7, 30);
	glVertex3f(100, -7, 30);
	glVertex3f(100, -7, 27);
	glVertex3f(130, -7, 27);
	glEnd();

	glBegin(GL_QUADS); // 뒤쪽길
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
			if (g_Shot[i].state == true){ // 총알
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
		glPushMatrix();      // 오른쪽기둥
		glTranslatef(128.2, 18.0 - i, 1.4);
		glRotatef(theta_2, 0.0, 1.0, 0.0);
		glRotatef(45, 1.0, 1.0, 1.0);
		glutSolidCube(1.5);
		glPopMatrix();

		glPushMatrix();      // 왼쪽기둥
		glTranslatef(102.2, 18.0 - i, 1.4);
		glRotatef(theta_2, 0.0, 1.0, 0.0);
		glRotatef(45, 1.0, 1.0, 1.0);
		glutSolidCube(1.5);
		glPopMatrix();
	}

	glColor4f(1.0, 0.5, 1.0, 0.5);
	glPushMatrix();      // 오른쪽기둥
	glTranslatef(128, 7.0, 2.0);
	glScalef(3.0, 28.0, 2.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();      // 왼쪽기둥
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

			// 뾰족뾰족가시
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

			//  총 구현
			glutWireCube(0.51);
			glColor3f(1, 0, 1);
			glutSolidCube(0.5);
		}
		glPopMatrix();
	}
}
//////////////3탄 함수들

GLvoid Level3(){
	glPushMatrix(); //3번 TEST 캐릭터
	{
		glTranslatef(165 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);

		glPushMatrix(); {
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix(); {
				glPushMatrix(); {//왼팔
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);

					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// 오른팔
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // 머리
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
					glPushMatrix(); {
						glColor3f(0, 1, 0);
						glTranslatef(0, 6, 0);
						glScalef(1, 50, 1);
						glutSolidCube(0.2);
					}glPopMatrix();
				}glPopMatrix();
				glPushMatrix(); // 그림자
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // 바디
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

	glEnable(GL_TEXTURE_2D); // 벽
	glBindTexture(GL_TEXTURE_2D, texture_object[2]);
	CheckCollision_Side(); // 맵 충돌체크
	DrawMap(150, -7, 0, 30, 30, 30);
	glDisable(GL_TEXTURE_2D);

}

void CheckCollision(float x1, float x2, float z1, float z2, float r)
{
	// (z2 < Z < z1) 일때 -> 왼쪽,오른쪽 충돌체크
	if (g_MyPosition.z < z1 && g_MyPosition.z > z2)
	{
		if (g_MyPosition.x > (x1 + x2) / 2) // 오른쪽 충돌체크 (= (오른쪽x값+왼쪽x값)/2)
		{
			if (g_MyPosition.x < x1 + r + 1.5) // ((오른쪽 벽 x값 = 4.0) + (원의 반지름 = 0.5))
			{
				Collision_state = true;
				g_MyPosition.x = x1 + r + 1.5;
			}
			if (g_MyPosition.x >= x1 + r + 1.5)
				Collision_state = false;
		}
		if (g_MyPosition.x < (x1 + x2) / 2) // 왼쪽 충돌체크
		{
			if (g_MyPosition.x > x2 - r - 1.5) // ( (왼쪽 벽 x값 = -3.0) - (원의 반지름 = 0.5))
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

	// (x2 < X < x1) 일때 -> 앞쪽, 뒤쪽 충돌체크
	if (g_MyPosition.x < x1 + r && g_MyPosition.x > x2 - r)
	{
		if (g_MyPosition.z > (z1 + z2) / 2) // 앞쪽 충돌체크 ( -7.5 = (앞쪽z값+뒤쪽z값/2))
		{
			if (g_MyPosition.z < z1 + r) // (앞쪽z값 = -7.0) + (반지름값 = 0.5)
			{
				Collision_state = true;
				g_MyPosition.z = z1 + r;
			}
			if (g_MyPosition.z >= z1 + r)
				Collision_state = false;
		}
		if (g_MyPosition.z < (z1 + z2) / 2) // 뒤쪽 충돌체크
		{
			if (g_MyPosition.z > z2 - r) // (뒤쪽z값 = -8.0) - (반지름값 = 0.5)
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

	//건물
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

	//충돌체크
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
	for (double i = 0; i < 30; i += 0.05)  //   잘잘잘 쪼개서 
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

void DrawBuilding(float x, float y, float z, int w, int h, int s){ // 건물 짓는 함수
	glBegin(GL_QUADS);
	//뒷면
	glVertex3f(x, y, z);
	glVertex3f(x, y + h, z);
	glVertex3f(x + w, y + h, z);
	glVertex3f(x + w, y, z);
	//왼면
	glVertex3f(x, y + h, z + s);
	glVertex3f(x, y + h, z);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + s);
	//오른면
	glVertex3f(x + w, y + h, z);
	glVertex3f(x + w, y + h, z + s);
	glVertex3f(x + w, y, z + s);
	glVertex3f(x + w, y, z);
	//앞면
	glVertex3f(x + w, y + h, z + s);
	glVertex3f(x, y + h, z + s);
	glVertex3f(x, y, z + s);
	glVertex3f(x + w, y, z + s);
	//윗면
	glVertex3f(x + w, y + h, z);
	glVertex3f(x, y + h, z);
	glVertex3f(x, y + h, z + s);
	glVertex3f(x + w, y + h, z + s);
	glEnd();
}

////////////////// Level 4

GLvoid Level4(){

	DrawFloor4(); // 바닥

	glPushMatrix(); //4번 TEST 캐릭터
	{
		glTranslatef(215 + g_MyPosition.x, g_MyPosition.y, 29 + g_MyPosition.z);
		glPushMatrix(); {
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix(); {
				glPushMatrix(); {//왼팔
					glTranslatef(-1.5, 2, 0);
					glRotatef(-Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(-0.5, 0, 0);

					glutSolidCube(1);
				}   glPopMatrix();
				glPushMatrix(); {// 오른팔
					glTranslatef(1.5, 2, 0);
					glRotatef(Arm_R, 0, 0, 1);
					glScalef(0.4, 1.5, 0.6);
					glTranslatef(0.5, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
				glPushMatrix(); { // 머리
					glTranslatef(0, Head_T, 0);
					glutSolidSphere(1, 20, 20);
				}glPopMatrix();
				glPushMatrix(); // 그림자
				glColor3f(0, 0, 0);
				glTranslatef(0, -0.7, 0);
				glScalef(0.2, 0.01, 0.2);
				glutSolidSphere(1, 20, 20);
				glPopMatrix();
				glPushMatrix(); { // 바디
					glColor3f(1, 1, 1);
					glTranslatef(0, 3, 0);
					glRotated(90, 1, 0, 0);
					glutSolidCone(1, 3, 20, 20);
				}glPopMatrix();
			}glPopMatrix();
		}glPopMatrix();
	}
	glPopMatrix();


	glEnable(GL_TEXTURE_2D); // 벽4
	glBindTexture(GL_TEXTURE_2D, texture_object[4]);
	if (BossCount < 21)
		DrawMap(200, -7, 0, 30, 30, 30);
	CheckCollision_Side();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D); // 벽5
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
		glPushMatrix(); //4번 토끼에요토끼
		{
			glTranslatef(p.x, p.y + a, p.z);
			if (BossCheck == true)
				glScalef(5, 5, 5);
			glPushMatrix(); //몸통
			if (BossCheck == false)
				glColor4f(0.9, 0.9, 0.9, 1);
			else
				glColor4f(1.0, 0.1, 0.1, 1);
			glScalef(1.4, 2.0, 1.4);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //머리
			glTranslatef(0, 2.5, 0);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //왼쪽귀
			glTranslatef(-0.5, 4, 0);
			glRotatef(20, 0.0, 0.0, 1.0);
			glScalef(0.4, 2.0, 0.2);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //오른쪽귀
			glTranslatef(0.5, 4, 0);
			glRotatef(-20, 0.0, 0.0, 1.0);
			glScalef(0.4, 2.0, 0.2);
			glutSolidSphere(1.0, 20, 20);
			glPopMatrix();
			glPushMatrix(); //눈
			glColor3f(0.0, 0.0, 0.0);
			glTranslatef(0.3, 3, 1);
			glutSolidSphere(0.2, 20, 20);
			glTranslatef(-0.6, 0, 0);
			glutSolidSphere(0.2, 20, 20);
			glPopMatrix();
			glPushMatrix(); //배
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
	glPushMatrix();//FPS 표시
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
	// Sight_Y > 300 -> 꿈에서 깨어남 
	if (Sight_Y > 300){
		LevelCheck = 0;
		CharInit();
	}
}