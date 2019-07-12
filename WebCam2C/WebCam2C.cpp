#include <conio.h>
#include <locale.h>
#include <vector>
#include "TwoCameras.h"
#include "WebCam2C.h"

// http://shailendra.me/tutorial/add-opencv-to-visual-studio-c++-project/

void showHelp()
{
	printf("WebCam2C\n");
	printf("----------------------------------------\n");
	
}

int main()
{
	TwoCameras twocameras;
	list<string> listaCam;
	int cam1 = -1, cam2 = -1;
	bool salir = false;

	setlocale(LC_ALL, "");

	listaCam = twocameras.GetListCameras();

	printf("\n");
	printf("\tWebCam2C\n");
	printf("\t----------------------------------------\n");
	if (listaCam.size() >= 2)
	{
		vector<string> vectorCam(listaCam.begin(), listaCam.end());
		do
		{
			printf("\n");
			int total = (int)vectorCam.size();
			for( int i=0; i<total; i++ )
			{
				printf("\t %2d Cámara = '%s'\n", i, vectorCam[i].c_str());
			}
			printf("\t[ESC] Salir\n");
			printf("\n");
			printf("\tSeleccione las cámaras:\n");

			int opc = _getch();
			if (opc == 27)
			{
				salir = true;
			}
			else
			{
				int opcNum = opc - '0';
				if (opcNum >= 0 && opcNum <= total)
				{
					if (cam1 < 0)
					{
						cam1 = opcNum;
					}
					else
					{
						cam2 = opcNum;
					}
				}
			}

			printf("\tCamara Izquierda='%s'   Derecha='%s'\n",
				cam1 < 0 ? "" : vectorCam[cam1].c_str(),
				cam2 < 0 ? "" : vectorCam[cam2].c_str());
		} while ((cam1 < 0 || cam2 < 0 || cam1 == cam2) && !salir);
	}
	else
	{
		salir = true;
		printf("\tNo hay suficientes camaras, son necesarias 2.\n");
	}

	if (!salir)
	{
		printf("\n");
		printf("\tWebCam2C (Ayuda)\n");
		printf("\t----------------------------------------\n");
		printf("\t----------------------------------------\n");
		printf("\t- Esc: Cierra el programa.\n");
		printf("\t- F1, F2 y F3 : Muestran u ocultan las pantallas izquierda, derecha y de anaglifo, respectivamente.\n");
		printf("\t- I,i : Intercambia las cámaras.\n");
		printf("\t- F,f : Toma una foto y la guarda en un fichero.\n");
		printf("\t- C,c : Centra la imágenes, a la posición inicial.\n");
		printf("\t- P,p : Intercambia el color de filtro de cada ojo. No equivale a I, porque I influye en la perspectiva.\n");
		printf("\t- K,k : Cambia los colores primarios de Rxx-xGB a xGx-RxB y viceversa.\n");
		printf("\t- Flechas izquierda, derecha, arriba y abajo : Mueven la imagen de la cámara derecha a izquierda, derecha, arriba y abajo; respectivamente.\n");
		printf("\t- AvPag y RePag : Rotan la imagen de la cámara de la derecha.\n");
		printf("\tEstas funcionalidades sirven para calibrar por software la alineación de las cámaras.\n");

		twocameras.Init(cam1, cam2);
		twocameras.ShowImages();
		twocameras.Release();
	}
}
