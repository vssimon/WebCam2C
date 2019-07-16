#include <conio.h>
#include <locale.h>
#include <vector>
#include "TwoCameras.h"
#include "WebCam2C.h"

// http://shailendra.me/tutorial/add-opencv-to-visual-studio-c++-project/

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
		TwoCameras::showHelp();

		twocameras.Init(cam1, cam2);
		twocameras.ShowImages();
		twocameras.Release();
	}
}


