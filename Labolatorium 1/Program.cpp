#include "pch.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

using namespace std;

int displacement;
int depth;

// Funkcja generuje losowy numer w zakresie od 0 do 1
float GetRandomNumber()
{
	return ((float)(rand()%101)/100);
}
/*************************************************************************************/

// Funkcja generujaca losowa wartosc przesuniecia
float GetRandomDisplacement(float a)
{
	if (displacement == 0) return 0;
	int temp;
	do
	{
		temp = (rand() % ((2 * displacement) + 1)) - displacement;
	} while (temp == 0);
	return (((float)temp / 100) * a);
}
/*************************************************************************************/

// Funkcja rysujaca kwadrat na podstawie podanego wierzchołka startowego oraz dlugosci boku
void DrawRectangle(float x, float y, float a)
{
	glBegin(GL_POLYGON);
		glColor3f(GetRandomNumber(), GetRandomNumber(), GetRandomNumber());
		glVertex2f(x + GetRandomDisplacement(a), y + GetRandomDisplacement(a));
		glColor3f(GetRandomNumber(), GetRandomNumber(), GetRandomNumber());
		glVertex2f(x + a + GetRandomDisplacement(a), y + GetRandomDisplacement(a));
		glColor3f(GetRandomNumber(), GetRandomNumber(), GetRandomNumber());
		glVertex2f(x + a + GetRandomDisplacement(a), y - a + GetRandomDisplacement(a));
		glColor3f(GetRandomNumber(), GetRandomNumber(), GetRandomNumber());
		glVertex2f(x + GetRandomDisplacement(a), y - a + GetRandomDisplacement(a));
	glEnd();
}
/*************************************************************************************/

// Funkcja rysujaca czesc Dywanu Sierpinskiego
void DrawPartOfSierpinskiCarpet(float x, float y, float size)
{
	DrawRectangle(x - (size / 2), y + (size / 2), size / 3);
	DrawRectangle(x - (size / 6), y + (size / 2), size / 3);
	DrawRectangle(x + (size / 6), y + (size / 2), size / 3);

	DrawRectangle(x - (size / 2), y + (size / 6), size / 3);
	DrawRectangle(x + (size / 6), y + (size / 6), size / 3);

	DrawRectangle(x - (size / 2), y - (size / 6), size / 3);
	DrawRectangle(x - (size / 6), y - (size / 6), size / 3);
	DrawRectangle(x + (size / 6), y - (size / 6), size / 3);
}
/*************************************************************************************/

// Funkcja rysujaca Dywan Sierpinskiego
void DrawSierpinskiCarpet(float x, float y, float size, int level)
{
	if (level == 1)
	{
		DrawPartOfSierpinskiCarpet(x, y, size);
	}
	else
	{
		DrawSierpinskiCarpet(x - size / 3, y + size / 3, size / 3, (level - 1));
		DrawSierpinskiCarpet(x, y + size / 3, size / 3, (level - 1));
		DrawSierpinskiCarpet(x + size / 3, y + size / 3, size / 3, (level - 1));
		DrawSierpinskiCarpet(x - size / 3, y, size / 3, (level - 1));
		DrawSierpinskiCarpet(x + size / 3, y, size / 3, (level - 1));
		DrawSierpinskiCarpet(x - size / 3, y - size / 3, size / 3, (level - 1));
		DrawSierpinskiCarpet(x, y - size / 3, size / 3, (level - 1));
		DrawSierpinskiCarpet(x + size / 3, y - size / 3, size / 3, (level - 1));
	}
}
/*************************************************************************************/

// Funkcja renderująca scene
void RenderScene(void)

{
	glClear(GL_COLOR_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	DrawSierpinskiCarpet(0, 0, 190, depth);

	glFlush();
	// Przekazanie poleceń rysujących do wykonania
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Kolor okna wnętrza okna - ustawiono na szary

	srand(time(NULL));
	// Ustawienie punktu startowego dla mechanizmu losujacego
}
/*************************************************************************************/
// Funkcja służąca do kontroli zachowania proporcji rysowanych obiektów
// niezależnie od rozmiarów okna graficznego

void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna
{
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio określającej proporcję wymiarów okna

	if (vertical == 0) // Zabezpieczenie pzred dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkościokna okna urządzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION);
	// Określenie układu współrzędnych obserwatora

	glLoadIdentity();
	// Określenie przestrzeni ograniczającej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie współczynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// określenie okna obserwatora.
	// Pozwala to zachować właściwe proporcje rysowanego obiektu
	// Do określenia okna obserwatora służy funkcja glOrtho(...)

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
	else
		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);
	glMatrixMode(GL_MODELVIEW);

	// Określenie układu współrzędnych    
	glLoadIdentity();

}
/*************************************************************************************/

// Funkcja sluzaca do wprowadzania odchylenia i glebokosci
void ImputtingDisplacementAndDepth()
{
	do
	{
		cout << "Prosze podac procentowa wartosc odchylenia w zakresie od 0 do 100" << endl;
		cin >> displacement;
	} while (displacement < 0 || displacement > 100);

	do
	{
		cout << "Prosze podac liczbe poziomow dywanu w zakresie od 1 do 7" << endl;
		cin >> depth;
	} while (depth < 0 || depth > 7);
}
/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli
void main(void)
{
	ImputtingDisplacementAndDepth();

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wyświetlania
	// GLUT_SINGLE - pojedynczy bufor wyświetlania
	// GLUT_RGBA - model kolorów RGB

	glutCreateWindow("Dywane Sierpińskiego");
	// Utworzenie okna i określenie treści napisu w nagłówku okna

	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)
	// Biblioteka GLUT będzie wywoływała tą funkcję za każdym razem, gdy
	// trzeba będzie przerysować okno

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne przed przystąpieniem do renderowania

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}