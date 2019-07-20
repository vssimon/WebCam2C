#include <conio.h>
#include <locale.h>
#include <vector>
#include <io.h>
#include "TwoCameras.h"
#include "WebCam2C.h"

// http://shailendra.me/tutorial/add-opencv-to-visual-studio-c++-project/

int main(int argc, char *argv[])
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
	printf("\n");

	if (argc == 4)
	{
		string fchImagenIzq = argv[1];
		string fchImagenDch = argv[2];
		string fchImagenRes = argv[3];

		printf("\tFichero de origen ojo izquierdo .......: %s   %s\n", (_access(fchImagenIzq.c_str(), 0) == 0 ? "Ok" : "No"), fchImagenIzq.c_str());
		printf("\tFichero de origen ojo derecho .........: %s   %s\n", (_access(fchImagenDch.c_str(), 0) == 0 ? "Ok" : "No"), fchImagenDch.c_str());
		printf("\tFichero de destino ....................: %s   %s\n", (_access(fchImagenRes.c_str(), 0) == 0 ? "Ex" : "Ok"), fchImagenRes.c_str());
		printf("\n");

		int opc = 0;
		int modo = -1;
		do {
			printf("\tModo de salida\n");
			printf("\t---------------------------------------------------------\n");
			printf("\t0 .- Rojo - Cian (Color)\n");
			printf("\t1 .- Rojo - Cian (Verdadero)\n");
			printf("\t2 .- Rojo - Cian (Mono)\n");
			printf("\t3 .- Rojo - Cian (Colores medios)\n");
			printf("\t4 .- Rojo - Cian (Optimizado)\n");
			printf("\t5 .- Verde - Magenta\n");
			printf("\t6 .- Azul - Amarillo\n");
			printf("\t7 .- Rojo - Verde\n");
			printf("\t8 .- Rojo - Azul\n");
			printf("\t9 .- Verde - Azul\n");
			printf("\ta .- Rojo - Cian (Verdadero clareado)\n");
			printf("\tb .- Rojo - Cian (Verdadero muy clareado)\n");
			printf("\t[ESC] .- Salir\n");
			opc = _getch();
			if ((opc >= '0' && opc <= '9'))
				modo = opc - '0';
			else
			{
				if (opc == 'a' || opc == 'A')
					modo = 10;
				if (opc == 'b' || opc == 'B')
					modo = 12;
			}

			if (modo >= 0)
			{
				TwoCameras twoCameras;
				TwoCameras::Errores errores = twoCameras.JoinImageFiles(fchImagenIzq, fchImagenDch, fchImagenRes, modo);
				switch (errores)
				{
				case TwoCameras::Errores::Ok:
					printf("Ok\n");
					break;
				case TwoCameras::Errores::NoImagenIzq:
					printf("NoImagenIzq\n");
					break;
				case TwoCameras::Errores::NoImagenDch:
					printf("NoImagenDch\n");
					break;
				case TwoCameras::Errores::NoMismoFormato:
					printf("NoMismoFormato\n");
					break;
				case TwoCameras::Errores::NoProcesado:
					printf("NoProcesado\n");
					break;
				default:
					printf("Error desconocido\n");
					break;
				}
				opc = 27;
			}

		} while (opc != 27);
	}
	else if (argc == 1)
	{
		if (listaCam.size() >= 2)
		{
			vector<string> vectorCam(listaCam.begin(), listaCam.end());
			do
			{
				int total = (int)vectorCam.size();
				for (int i = 0; i < total; i++)
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
	else
	{
		printf("\tPara grabación directa\n");
		printf("\t%s\n", argv[0]);
		printf("\tPara tratamiento de imágenes\n");
		printf("\t%s fchImagenIzq fchImagenDch fchResultado\n", argv[0]);
		printf("\tPara tratamiento de imágenes\n");
		printf("\tEl programa preguntará por las opciones\n");
	}
}


