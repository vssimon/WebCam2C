#include <conio.h>
#include <locale.h>
#include <vector>
#include <io.h>
#include "WebCam2C.h"

// http://shailendra.me/tutorial/add-opencv-to-visual-studio-c++-project/

int main(int argc, char *argv[])
{
	TwoCameras twocameras;
	vector<TwoCameras::CamInfo> listaCam;
	int cam1 = -1, cam2 = -1;
	bool salir = false;
	long width = 0;
	long height = 0;
	int FPS = 0;

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
			do
			{
				int total = (int)listaCam.size();
				for (int i = 0; i < total; i++)
				{
					printf("\t %2d Cámara = '%s'\n", i, listaCam[i].name.c_str());
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
					cam1 < 0 ? "" : listaCam[cam1].name.c_str(),
					cam2 < 0 ? "" : listaCam[cam2].name.c_str());

				if (cam1 >= 0)
				{
					MostrarResolucion(listaCam[cam1]);
				}
				if (cam2 >= 0)
				{
					MostrarResolucion(listaCam[cam2]);
				}
			} while ((cam1 < 0 || cam2 < 0 || cam1 == cam2) && !salir);

			if (!salir)
			{
				// Muestra las opciones comunes y permite seleccionar una
				vector<TwoCameras::CamValues> interseccion = InterseccionResolucion(listaCam[cam1], listaCam[cam2]);
				if (interseccion.size() > 0)
				{
					do
					{
						int letra = 'a';
						printf("\tResoluciones comunes para '%s' y '%s'\n", listaCam[cam1].name.c_str(), listaCam[cam2].name.c_str());
						for (int i = 0; i < interseccion.size(); i++)
						{
							printf("\t\t%c .- '%-20s' - W=%5ld - H=%5ld - FPS=%3d - bitrate=%10u - bitcount=%3u\n", 
								letra++,
								interseccion[i].resolution.c_str(),
								interseccion[i].width,
								interseccion[i].height,
								interseccion[i].FPS,
								interseccion[i].bitrate,
								interseccion[i].bitcount);
						}

						int opc = _getch();
						if (opc == 27)
						{
							salir = true;
						}
						else
						{
							if (opc - 'a' >= 0 && opc - 'a' < interseccion.size())
							{
								int opcNum = opc - 'a';
								width = interseccion[opcNum].width;
								height = interseccion[opcNum].height;
								FPS = interseccion[opcNum].FPS;
							}
						}
					} while (!salir && !(width > 0 && height>0 && FPS>0));
				}
				else
				{
					printf("\tLas cámaras '%s' y '%s' no comparten resoluciones comunes\n", listaCam[cam1].name.c_str(), listaCam[cam2].name.c_str());
					salir = true;
				}
			}
		}
		else
		{
			salir = true;
			printf("\tNo hay suficientes camaras, son necesarias 2.\n");
		}
		if (!salir)
		{
			TwoCameras::showHelp();

			twocameras.Init(cam1, cam2, width, height, FPS);
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

void MostrarResolucion(TwoCameras::CamInfo cam)
{
	printf("\tResoluciones para Camara='%s'\n", cam.name.c_str());
	for (int i = 0; i < cam.values.size(); i++)
	{
		printf("\t\t'%-20s' - W=%5ld - H=%5ld - FPS=%3d - bitrate=%10u - bitcount=%3u\n",
			cam.values[i].resolution.c_str(),
			cam.values[i].width,
			cam.values[i].height,
			cam.values[i].FPS,
			cam.values[i].bitrate,
			cam.values[i].bitcount);
	}
};

vector<TwoCameras::CamValues> InterseccionResolucion(TwoCameras::CamInfo cam1, TwoCameras::CamInfo cam2)
{
	vector<TwoCameras::CamValues> res;

	for (int i = 0; i < cam1.values.size(); i++)
	{
		for (int j = 0; j < cam2.values.size(); j++)
		{
			if (cam1.values[i].width == cam2.values[j].width &&
				cam1.values[i].height == cam2.values[j].height &&
				cam1.values[i].FPS == cam2.values[j].FPS &&
				cam1.values[i].bitcount == cam2.values[j].bitcount &&
				cam1.values[i].bitrate == cam2.values[j].bitrate &&
				cam1.values[i].resolution == cam2.values[j].resolution)
			{
				TwoCameras::CamValues element;
				element.width = cam1.values[i].width;
				element.height = cam1.values[i].height;
				element.FPS = cam1.values[i].FPS;
				element.bitcount = cam1.values[i].bitcount;
				element.bitrate = cam1.values[i].bitrate;
				element.resolution = cam1.values[i].resolution;
				res.push_back(element);
			}
		}
	}

	return res;
};