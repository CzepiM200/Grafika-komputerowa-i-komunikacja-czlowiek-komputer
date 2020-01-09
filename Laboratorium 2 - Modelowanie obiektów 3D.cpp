#include "pch.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <gl/GLU.h>

using namespace std;

// Zdefiniowanie stalej pi, potrzebnej w obliczeniach
#define M_PI 3.14159265358979323846
// Liczba okreslajaca dokladnosc rysowanego jajka (N/2 = liczba poziomow jajka)
const int N = 60;

typedef float point3[3];
typedef float point2[2];

// Tablica odpowiedzialna za przechowywanie wyznaczonych wspolrzednych punktow na powierzchni jajka
static point3 matrixOfPoints[N][N];
// Tablica odpowiedzialna za przechowywanie kolorow potrzebnych do kolorowania powierzchni jajka
static point3 matrixOfColors[N][N];
// Tablica odpowiedzialna ze przechowywanie informacji na temat trzech katow, wzgledem ktorych obracane beda obiekty
static GLfloat theta[] = { 0.0, 0.0, 0.0 };
// Zmienna przechowujaca informacje o tym jaki obiekt ma byc rysowany
int mode = 4;

// Funkcja oblicza blicza numer z zakresu od 1.0 do 0.0
float GetRandomNumber()
{
	return ((float)(rand() % 101) / 100);
}

// Funkcja oblicza parametr x przy pomocy zadanego wzoru
float ParameterX(float u, float v) {
	return (float)(-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
}

// Funkcja oblicza parametr y przy pomocy zadanego wzoru
float ParameterY(float u, float v) {
	return (float)(160 * pow(u, 4)) - (320 * pow(u, 3)) + 160 * pow(u, 2);
}

// Funkcja oblicza parametr z przy pomocy zadanego wzoru
float ParameterZ(float u, float v) {
	return (float)((-90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - (45 * u))*sin(M_PI*v);
}

// Funkcja obliczajaca kat theta potrzebny do obraacnia obektami
void spinEgg()
{
	theta[0] += 0.02;
	//if (theta[0] > 360.0) theta[0] -= 360.0;

	theta[1] += 0.02;
	//if (theta[1] > 360.0) theta[1] -= 360.0;

	theta[2] += 0.02;
	//if (theta[2] > 360.0) theta[2] -= 360.0;

	glutPostRedisplay();
}

// Funkcja oblicza wszystkie wspolrzedne potrzebne do narysowania jajka i zapisue je w tablicy
void CalculatePoints()
{
	float u = 0, v = 0, gap = 1.0f/N;
	for (int x = 0; x < N; x++)
	{
		for (int y = 0; y < N; y++)
		{
			u = x * gap;	v = y * gap;
			matrixOfPoints[x][y][0] = ParameterX(u, v);
			matrixOfPoints[x][y][1] = ParameterY(u, v);
			matrixOfPoints[x][y][2] = ParameterZ(u, v);
		}
	}
}

// Funkcja przy pomocy obliczonych wczesnije punktow, rysuje jajko z kropek
void DotsEgg()
{
	int displacement = 5;
	glBegin(GL_POINTS);
	for (int x = 0; x < N; x++)
		for (int y = 0; y < N; y++)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - displacement, matrixOfPoints[x][y][2]);
		}
	glEnd();
}

// Funkcja przy pomocy obliczonych wczesnije punktow, rysuje jajko z linii
void LinesEgg()
{
	int displacement = 5;
	// x=0 dol jajka
	// N-1 brak rysowania polowy spodu
	glBegin(GL_LINES);
	for (int x = 0; x < N; x++) {
		if (x != N - 1)
		{
			for (int y = 0; y < N; y++) {
				glColor3f(1.0f, 1.0f, 1.0f);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - displacement, matrixOfPoints[x][y][2]);
				glVertex3f(matrixOfPoints[x + 1][y][0], matrixOfPoints[x + 1][y][1] - displacement, matrixOfPoints[x + 1][y][2]);
			}
		}
		else
		{
			for (int y = 0; y < N; y++) {
				glColor3f(1.0f, 1.0f, 1.0f);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - displacement, matrixOfPoints[x][y][2]);
				glVertex3f(0.0f, 0.0f - displacement, (0.0f));
			}
		}
	}
	glEnd();

	glBegin(GL_LINES);
	for (int y = 0; y < N; y++) {
		if (y != N - 1)
		{
			for (int x = 0; x < N; x++)
			{

				glColor3f(1.0f, 1.0f, 1.0f);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - displacement, matrixOfPoints[x][y][2]);
				glVertex3f(matrixOfPoints[x][y + 1][0], matrixOfPoints[x][y + 1][1] - displacement, matrixOfPoints[x][y + 1][2]);
			}
		}
		else
		{
			for (int x = 0; x < N; x++)
			{
				glBegin(GL_LINES);
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - displacement, matrixOfPoints[x][y][2]);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - displacement, 0);
			}
		}
	}
	glEnd();
}

// Funkcja przy pomocy obliczonych wczesnije punktow, rysuje jajko z kolorowych trojkatow
void ColorfulEgg()
{
	glBegin(GL_TRIANGLES);
	// Petla odpowiadajaca za rysowanie pierwszej czesci jajka
	for (int x = N / 2; x < N; x++) {
		for (int y = 0; y < N - 1; y++) {
			// Przypadek odpowiadajacy za rysowanie pierwszej polowy powierzchni bocznej jajka
			if (x != N-1) {
				glColor3f(matrixOfColors[x][y][0], matrixOfColors[x][y][1], matrixOfColors[x][y][2]);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - 5, matrixOfPoints[x][y][2]);
				glColor3f(matrixOfColors[x + 1][y][0], matrixOfColors[x + 1][y][1], matrixOfColors[x + 1][y][2]);
				glVertex3f(matrixOfPoints[x + 1][y][0], matrixOfPoints[x + 1][y][1] - 5, matrixOfPoints[x + 1][y][2]);
				glColor3f(matrixOfColors[x + 1][y + 1][0], matrixOfColors[x + 1][y + 1][1], matrixOfColors[x + 1][y + 1][2]);
				glVertex3f(matrixOfPoints[x + 1][y + 1][0], matrixOfPoints[x + 1][y + 1][1] - 5, matrixOfPoints[x + 1][y + 1][2]);

				glColor3f(matrixOfColors[x][y][0], matrixOfColors[x][y][1], matrixOfColors[x][y][2]);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - 5, matrixOfPoints[x][y][2]);
				glColor3f(matrixOfColors[x][y + 1][0], matrixOfColors[x][y + 1][1], matrixOfColors[x][y + 1][2]);
				glVertex3f(matrixOfPoints[x][y + 1][0], matrixOfPoints[x][y + 1][1] - 5, matrixOfPoints[x][y + 1][2]);
				glColor3f(matrixOfColors[x + 1][y + 1][0], matrixOfColors[x + 1][y + 1][1], matrixOfColors[x + 1][y + 1][2]);
				glVertex3f(matrixOfPoints[x + 1][y + 1][0], matrixOfPoints[x + 1][y + 1][1] - 5, matrixOfPoints[x + 1][y + 1][2]);
			}
			// Przypadek odpowiadajacy za rysowanie pierwszej polowy dolnej czesci jajka
			else {
				glColor3f(matrixOfColors[x][y][0], matrixOfColors[x][y][1], matrixOfColors[x][y][2]);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - 5, matrixOfPoints[x][y][2]);
				glColor3f(matrixOfColors[x][y + 1][0], matrixOfColors[x][y + 1][1], matrixOfColors[x][y + 1][2]);
				glVertex3f(matrixOfPoints[x][y + 1][0], matrixOfPoints[x][y + 1][1] - 5, matrixOfPoints[x][y + 1][2]);
				glColor3f(matrixOfColors[0][0][0], matrixOfColors[0][0][1], matrixOfColors[0][0][2]);
				glVertex3f(matrixOfPoints[0][0][0], matrixOfPoints[0][0][1] - 5, matrixOfPoints[0][0][2]);
			}
			// Przypadek odpowiadajacy za rysowanie pierwszej czesci laczacej dwie polowy jajka
			if (x != 0) {
				glColor3f(matrixOfColors[x][0][0], matrixOfColors[x][0][1], matrixOfColors[x][0][2]);
				glVertex3f(matrixOfPoints[x][0][0], matrixOfPoints[x][0][1] - 5, matrixOfPoints[x][0][2]);
				glColor3f(matrixOfColors[N - x][N - 1][0], matrixOfColors[N - x][N - 1][1], matrixOfColors[N - x][N - 1][2]);
				glVertex3f(matrixOfPoints[N - x][N - 1][0], matrixOfPoints[N - x][N - 1][1] - 5, matrixOfPoints[N - x][N - 1][2]);
				glColor3f(matrixOfColors[N - x + 1][N - 1][0], matrixOfColors[N - x + 1][N - 1][1], matrixOfColors[N - x + 1][N - 1][2]);
				glVertex3f(matrixOfPoints[N - x + 1][N - 1][0], matrixOfPoints[N - x + 1][N - 1][1] - 5, matrixOfPoints[N - x + 1][N - 1][2]);

				if (x != N / 2) {
					glColor3f(matrixOfColors[x][0][0], matrixOfColors[x][0][1], matrixOfColors[x][0][2]);
					glVertex3f(matrixOfPoints[x][0][0], matrixOfPoints[x][0][1] - 5, matrixOfPoints[x][0][2]);
					glColor3f(matrixOfColors[x - 1][0][0], matrixOfColors[x - 1][0][1], matrixOfColors[x - 1][0][2]);
					glVertex3f(matrixOfPoints[x - 1][0][0], matrixOfPoints[x - 1][0][1] - 5, matrixOfPoints[x - 1][0][2]);
					glColor3f(matrixOfColors[N - x + 1][N - 1][0], matrixOfColors[N - x + 1][N - 1][1], matrixOfColors[N - x + 1][N - 1][2]);
					glVertex3f(matrixOfPoints[N - x + 1][N - 1][0], matrixOfPoints[N - x + 1][N - 1][1] - 5, matrixOfPoints[N - x + 1][N - 1][2]);
				}
			}
		}
	}

	// Petla odpowiadajaca za rysowanie drugiej czesci jajka
	for (int x = 0; x < N / 2 + 1; x++) {
		for (int y = 0; y < N - 1; y++) {
			// Przypadek odpowiadajacy za rysowanie drugiej polowy dolnej czesci jajka
			if (x == 1) 
			{
				glColor3f(matrixOfColors[x][y][0], matrixOfColors[x][y][1], matrixOfColors[x][y][2]);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - 5, matrixOfPoints[x][y][2]);
				glColor3f(matrixOfColors[x][y + 1][0], matrixOfColors[x][y + 1][1], matrixOfColors[x][y + 1][2]);
				glVertex3f(matrixOfPoints[x][y + 1][0], matrixOfPoints[x][y + 1][1] - 5, matrixOfPoints[x][y + 1][2]);
				glColor3f(matrixOfColors[0][0][0], matrixOfColors[0][0][1], matrixOfColors[0][0][2]);
				glVertex3f(matrixOfPoints[0][0][0], matrixOfPoints[0][0][1] - 5, matrixOfPoints[0][0][2]);
			}
			// Przypadek odpowiadajacy za rysowanie drugiej polowy powierzchni bocznej jajka
			else 
			{
				if (x != 0) {
					glColor3f(matrixOfColors[x][y][0], matrixOfColors[x][y][1], matrixOfColors[x][y][2]);
					glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - 5, matrixOfPoints[x][y][2]);
					glColor3f(matrixOfColors[x - 1][y][0], matrixOfColors[x - 1][y][1], matrixOfColors[x - 1][y][2]);
					glVertex3f(matrixOfPoints[x - 1][y][0], matrixOfPoints[x - 1][y][1] - 5, matrixOfPoints[x - 1][y][2]);
					glColor3f(matrixOfColors[x - 1][y + 1][0], matrixOfColors[x - 1][y + 1][1], matrixOfColors[x - 1][y + 1][2]);
					glVertex3f(matrixOfPoints[x - 1][y + 1][0], matrixOfPoints[x - 1][y + 1][1] - 5, matrixOfPoints[x - 1][y + 1][2]);
				}

				glColor3f(matrixOfColors[x][y][0], matrixOfColors[x][y][1], matrixOfColors[x][y][2]);
				glVertex3f(matrixOfPoints[x][y][0], matrixOfPoints[x][y][1] - 5, matrixOfPoints[x][y][2]);
				glColor3f(matrixOfColors[x][y + 1][0], matrixOfColors[x][y + 1][1], matrixOfColors[x][y + 1][2]);
				glVertex3f(matrixOfPoints[x][y + 1][0], matrixOfPoints[x][y + 1][1] - 5, matrixOfPoints[x][y + 1][2]);
				glColor3f(matrixOfColors[x - 1][y + 1][0], matrixOfColors[x - 1][y + 1][1], matrixOfColors[x - 1][y + 1][2]);
				glVertex3f(matrixOfPoints[x - 1][y + 1][0], matrixOfPoints[x - 1][y + 1][1] - 5, matrixOfPoints[x - 1][y + 1][2]);
			}
			// Przypadek odpowiadajacy za rysowanie drugiej czesci laczacej dwie polowy jajka
			if (x != 0) {
				glColor3f(matrixOfColors[x][0][0], matrixOfColors[x][0][1], matrixOfColors[x][0][2]);
				glVertex3f(matrixOfPoints[x][0][0], matrixOfPoints[x][0][1] - 5, matrixOfPoints[x][0][2]);
				glColor3f(matrixOfColors[N - x][N - 1][0], matrixOfColors[N - x][N - 1][1], matrixOfColors[N - x][N - 1][2]);
				glVertex3f(matrixOfPoints[N - x][N - 1][0], matrixOfPoints[N - x][N - 1][1] - 5, matrixOfPoints[N - x][N - 1][2]);
				glColor3f(matrixOfColors[N - x - 1][N - 1][0], matrixOfColors[N - x - 1][N - 1][1], matrixOfColors[N - x - 1][N - 1][2]);
				glVertex3f(matrixOfPoints[N - x - 1][N - 1][0], matrixOfPoints[N - x - 1][N - 1][1] - 5, matrixOfPoints[N - x - 1][N - 1][2]);

				glColor3f(matrixOfColors[x][0][0], matrixOfColors[x][0][1], matrixOfColors[x][0][2]);
				glVertex3f(matrixOfPoints[x][0][0], matrixOfPoints[x][0][1] - 5, matrixOfPoints[x][0][2]);
				glColor3f(matrixOfColors[x + 1][0][0], matrixOfColors[x + 1][0][1], matrixOfColors[x + 1][0][2]);
				glVertex3f(matrixOfPoints[x + 1][0][0], matrixOfPoints[x + 1][0][1] - 5, matrixOfPoints[x + 1][0][2]);
				glColor3f(matrixOfColors[N - x - 1][N - 1][0], matrixOfColors[N - x - 1][N - 1][1], matrixOfColors[N - x - 1][N - 1][2]);
				glVertex3f(matrixOfPoints[N - x - 1][N - 1][0], matrixOfPoints[N - x - 1][N - 1][1] - 5, matrixOfPoints[N - x - 1][N - 1][2]);
			}
		}
	}

	// Rysowanie 2 brakujacych trojkota na dole jajka
	glColor3f(matrixOfColors[0][0][0], matrixOfColors[0][0][1], matrixOfColors[0][0][2]);
	glVertex3f(matrixOfPoints[0][0][0], matrixOfPoints[0][0][1] - 5, matrixOfPoints[0][0][2]);
	glColor3f(matrixOfColors[N - 1][0][0], matrixOfColors[N - 1][0][1], matrixOfColors[N - 1][0][2]);
	glVertex3f(matrixOfPoints[N - 1][0][0], matrixOfPoints[N - 1][0][1] - 5, matrixOfPoints[N - 1][0][2]);
	glColor3f(matrixOfColors[1][N - 1][0], matrixOfColors[1][N - 1][1], matrixOfColors[1][N - 1][2]);
	glVertex3f(matrixOfPoints[1][N - 1][0], matrixOfPoints[1][N - 1][1] - 5, matrixOfPoints[1][N - 1][2]);

	glColor3f(matrixOfColors[0][0][0], matrixOfColors[0][0][1], matrixOfColors[0][0][2]);
	glVertex3f(matrixOfPoints[0][0][0], matrixOfPoints[0][0][1] - 5, matrixOfPoints[0][0][2]);
	glColor3f(matrixOfColors[1][0][0], matrixOfColors[1][0][1], matrixOfColors[1][0][2]);
	glVertex3f(matrixOfPoints[1][0][0], matrixOfPoints[1][0][1] - 5, matrixOfPoints[1][0][2]);
	glColor3f(matrixOfColors[N - 1][N - 1][0], matrixOfColors[N - 1][N - 1][1], matrixOfColors[N - 1][N - 1][2]);
	glVertex3f(matrixOfPoints[N - 1][N - 1][0], matrixOfPoints[N - 1][N - 1][1] - 5, matrixOfPoints[N - 1][N - 1][2]);

	glEnd();
}

// Funkcja oblicza losowe kolory potrzebne do pokolorowania jajka i zapisuje je w talicy 
void CalculateColours()
{
	float u = 0, v = 0;
	for (int x = 0; x < N; x++)
	{
		for (int y = 0; y < N; y++)
		{
			matrixOfColors[x][y][0] = GetRandomNumber();
			matrixOfColors[x][y][1] = GetRandomNumber();
			matrixOfColors[x][y][2] = GetRandomNumber();
		}
	}
}

// Funkcja okreslajaca, ktory typ jajka bedzie rysowany
void Egg(void)
{
	if (mode == 1) glutWireTeapot(4.0);
	else if (mode == 2) DotsEgg();
	else if (mode == 3) LinesEgg();
	else if (mode == 4) ColorfulEgg();
}

// Funkcja rysująca osie x, y, z
void Axes(void)
{
	// rysowanie osi x 
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	// rysowanie osi y
	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	// rysowanie osi z
	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

// Glowna funkcja rysujaca scene
void RenderScene(void)
{
	// Czyszczenie okna aktualnym kolorem czyszczącym
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Zastepuj biezaca macierz, macierza jednotkowa
	glLoadIdentity();

	// Wywołanie funkcji odpowiadających za obracanie obiektu względem osi x, y, z na podstawie wyznaczonego kat theta 
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);
	
	// Wywolanie funkcji odpowiedzialnej za rysowanie osi
	Axes();
	// Wywolanie funkcji odpowiedzialnej za rysowane jajka w wybranym wariancie
	Egg();
	// Czyszczenie macierzy bieżącej
	glFlush();
	// Przekazanie poleceń rysujących do wykonania
	glutSwapBuffers();
}

// Funkcja odpowiadająca za zmiane stanu zmiennej mode, na podstawie ktorej program wie co ma rysowac
void KeyManager(unsigned char key, int x, int y)
{
	if (key == '1') mode = 1;
	else if (key == '2') mode = 2;
	else if (key == '3') mode = 3;
	else if (key == '4') mode = 4;
	RenderScene();
}

// Funkcja ustalająca stan renderowania
void MyInit(void)
{
	// Kolor czyszcący (wypełnienia okna) ustawiono na czarny
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Wywolanie funkcji, ktora na poczatku programu wyznacza kolory jajka
	CalculateColours();
	// Wywolanie funkcji, ktora na poczatku programu wyznacza punkty na powierzchni jajka
	CalculatePoints();
}

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  określającej proporcję
	// wymiarów okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkościokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji 
	glLoadIdentity();
	// Czyszcznie macierzy bieżącej            
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie współczynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej
	// przestrzeni ograniczającej pozwalającej zachować właściwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczjącej służy funkcja
	// glOrtho(...)            
	if (horizontal <= vertical)

		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else

		glOrtho(-7.5*AspectRatio, 7.5*AspectRatio, -7.5, 7.5, 10.0, -10.0);
	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu                                   

	glLoadIdentity();
	// Czyszcenie macierzy bieżącej
}

// Główny punkt wejścia programu. Program działa w trybie konsoli
void main(void)
{
	// Funkcji konfiguruje tryb wyswietlania
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// Funkcja okresla poczatkowy rozmiar okna
	glutInitWindowSize(600, 600);
	// Funkcja okresla nazwe okna
	glutCreateWindow("Układ współrzędnych 3-D");
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback function).  Bedzie ona wywoływana za każdym razem gdy zajdzie potrzba przeryswania okna 
	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja ChangeSize będzie funkcją zwrotną (callback function).  Bedzie ona wywoływana za każdym razem gdy zajdzie potrzba zmiany rozmiaru okna 
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną zazmiany rozmiaru okna   
	glutIdleFunc(spinEgg);
	// Określenie, że funkcja KeyManager będzie funkcją zwrotną (callback function).  Bedzie ona wywoływana za każdym razem gdy zajdzie potrzba obslugi klawiatury
	glutKeyboardFunc(KeyManager);
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie inicjalizacje konieczne  przed przystąpieniem do renderowania
	MyInit();
	// Włączenie mechanizmu usuwania powierzchni niewidocznych
	glEnable(GL_DEPTH_TEST);
	// Funkcja uruchamia szkielet biblioteki GLUT
	glutMainLoop();	
}