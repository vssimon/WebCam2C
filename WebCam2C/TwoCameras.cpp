#include "TwoCameras.h"
#include <conio.h>
#include <direct.h>
#include <dshow.h>
#include <time.h>

#pragma comment(lib, "strmiids")

TwoCameras::TwoCameras()
{
}

TwoCameras::~TwoCameras()
{
}

list<string> TwoCameras::GetListCameras()
{
	list<string> res;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		IEnumMoniker *pEnum;

		hr = GetDevices(CLSID_VideoInputDeviceCategory, &pEnum);
		if (SUCCEEDED(hr))
		{
			res = DisplayDeviceInformation(pEnum);
			pEnum->Release();
		}
		CoUninitialize();
	}

	return res;
};

HRESULT TwoCameras::GetDevices(REFGUID CLSID_categoria, IEnumMoniker **ppEnum)
{
	ICreateDevEnum *pDevEnum;

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_categoria, ppEnum, 0);
		if (hr == S_FALSE )
		{
			hr = VFW_E_NOT_FOUND;
		}
		pDevEnum->Release();
	}

	return hr;
}

list<string> TwoCameras::DisplayDeviceInformation(IEnumMoniker *pEnum)
{
	list<string> res;

	IMoniker *pMoniker = NULL;

	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		IPropertyBag *pPropBag;

		HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));

		if (SUCCEEDED(hr))
		{
			VARIANT var;
			VariantInit(&var);

			hr = pPropBag->Read(L"FriendlyName", &var, 0);
			if (SUCCEEDED(hr))
			{
				res.push_back(ConvertBSTRToMBS(var.bstrVal));
				VariantClear(&var);
			}
		}

		pPropBag->Release();
		pMoniker->Release();
	}

	return res;
}

TwoCameras::Errores TwoCameras::JoinImageFiles(string fchIzq, string fchDch, string fchRes, int posColor)
{
	Errores res = Errores::Ok;

	m_oriImage0 = cv::imread(fchIzq, cv::IMREAD_COLOR);
	m_oriImage1 = cv::imread(fchDch, cv::IMREAD_COLOR);


	if (m_oriImage0.data == NULL)
	{
		res = Errores::NoImagenIzq;
	}
	else if( m_oriImage1.data==NULL )
	{
		res = Errores::NoImagenDch;
	}
	else
	{
		posColor = posColor % MAX_POSCOLOR;

		Mat BGR0[3];
		Mat BGR1[3];
		Mat BGR2[3];

		cv::split(m_oriImage0, BGR0);
		cv::split(m_oriImage1, BGR1);
		cv::split(m_oriImage1, BGR2);

		CargaVectoresColor();

		BGR0[RED] = (BGR0[RED] * m_colorIzq[posColor][RED][RED]) + (BGR0[GREEN] * m_colorIzq[posColor][RED][GREEN]) + (BGR0[BLUE] * m_colorIzq[posColor][RED][BLUE]);
		//BGR0[GREEN] = (BGR0[RED] * m_colorIzq[posColor][GREEN][RED]) + (BGR0[GREEN] * m_colorIzq[posColor][GREEN][GREEN]) + (BGR0[BLUE] * m_colorIzq[posColor][GREEN][BLUE]);
		//BGR0[BLUE] = (BGR0[RED] * m_colorIzq[posColor][BLUE][RED]) + (BGR0[GREEN] * m_colorIzq[posColor][BLUE][GREEN]) + (BGR0[BLUE] * m_colorIzq[posColor][BLUE][BLUE]);

		//BGR1[RED] = (BGR1[RED] * m_colorDch[posColor][RED][RED]) + (BGR1[GREEN] * m_colorDch[posColor][RED][GREEN]) + (BGR1[BLUE] * m_colorDch[posColor][RED][BLUE]);
		BGR1[GREEN] = (BGR1[RED] * m_colorDch[posColor][GREEN][RED]) + (BGR1[GREEN] * m_colorDch[posColor][GREEN][GREEN]) + (BGR1[BLUE] * m_colorDch[posColor][GREEN][BLUE]);
		BGR1[BLUE] = (BGR1[RED] * m_colorDch[posColor][BLUE][RED]) + (BGR1[GREEN] * m_colorDch[posColor][BLUE][GREEN]) + (BGR1[BLUE] * m_colorDch[posColor][BLUE][BLUE]);

		BGR2[RED] = BGR0[RED];
		BGR2[GREEN] = BGR1[GREEN];
		BGR2[BLUE] = BGR1[BLUE];

		cv::merge(BGR2, 3, m_imgAnaglifo);

		//std::vector<int> qualityType;
		//qualityType.push_back(cv::IMWRITE_JPEG_QUALITY);
		//qualityType.push_back(100);

		//vector<int> compression_params;
		//compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		//compression_params.push_back(9);

		imwrite(fchRes, m_imgAnaglifo);

		waitKey(30);

		res = Errores::Ok;
	}


	return res;
};

void TwoCameras::CargaVectoresColor()
{
	// Cargar colores en los vectores de ojo
	// http://zaguan.unizar.es/record/64288/files/TAZ-PFC-2017-047.pdf
	// http://3dtv.at/Knowhow/AnaglyphComparison_en.aspx
	m_strColor[0] = "Rojo - Cian (Color)";
	m_colorIzq[0][RED] = Scalar(0.000, 0.000, 1.000); m_colorIzq[0][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[0][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[0][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[0][GREEN] = Scalar(0.000, 1.000, 0.000); m_colorDch[0][BLUE] = Scalar(1.000, 0.000, 0.000);

	m_strColor[1] = "Rojo - Cian (Verdadero)";
	m_colorIzq[1][RED] = Scalar(0.144, 0.587, 0.299); m_colorIzq[1][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[1][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[1][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[1][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorDch[1][BLUE] = Scalar(0.144, 0.587, 0.299);

	m_strColor[2] = "Rojo - Cian (Mono)";
	m_colorIzq[2][RED] = Scalar(0.144, 0.587, 0.299); m_colorIzq[2][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[2][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[2][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[2][GREEN] = Scalar(0.144, 0.587, 0.299); m_colorDch[2][BLUE] = Scalar(0.144, 0.587, 0.299);

	m_strColor[3] = "Rojo - Cian (Colores medios)";
	m_colorIzq[3][RED] = Scalar(0.144, 0.587, 0.299); m_colorIzq[3][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[3][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[3][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[3][GREEN] = Scalar(0.000, 1.000, 0.000); m_colorDch[3][BLUE] = Scalar(1.000, 0.000, 0.000);

	m_strColor[4] = "Rojo - Cian (Optimizado)";
	m_colorIzq[4][RED] = Scalar(0.300, 0.700, 0.000); m_colorIzq[4][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[4][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[4][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[4][GREEN] = Scalar(0.000, 1.000, 0.000); m_colorDch[4][BLUE] = Scalar(1.000, 0.000, 0.000);

	m_strColor[5] = "Verde - Magenta";
	m_colorIzq[5][RED] = Scalar(0.000, 0.000, 0.000); m_colorIzq[5][GREEN] = Scalar(0.000, 1.000, 0.000); m_colorIzq[5][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[5][RED] = Scalar(0.000, 0.000, 1.000); m_colorDch[5][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorDch[5][BLUE] = Scalar(1.000, 0.000, 0.000);

	m_strColor[6] = "Azul - Amarillo";
	m_colorIzq[6][RED] = Scalar(0.000, 0.000, 0.000); m_colorIzq[6][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[6][BLUE] = Scalar(1.000, 0.000, 0.000);
	m_colorDch[6][RED] = Scalar(0.000, 0.000, 1.000); m_colorDch[6][GREEN] = Scalar(0.000, 1.000, 0.000); m_colorDch[6][BLUE] = Scalar(0.000, 0.000, 0.000);

	m_strColor[7] = "Rojo - Verde";
	m_colorIzq[7][RED] = Scalar(0.000, 0.000, 1.000); m_colorIzq[7][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[7][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[7][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[7][GREEN] = Scalar(0.000, 1.000, 0.000); m_colorDch[7][BLUE] = Scalar(0.000, 0.000, 0.000);

	m_strColor[8] = "Rojo - Azul";
	m_colorIzq[8][RED] = Scalar(0.000, 0.000, 1.000); m_colorIzq[8][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[8][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[8][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[8][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorDch[8][BLUE] = Scalar(1.000, 0.000, 0.000);

	m_strColor[9] = "Verde - Azul";
	m_colorIzq[9][RED] = Scalar(0.000, 0.000, 0.000); m_colorIzq[9][GREEN] = Scalar(0.000, 1.000, 0.000); m_colorIzq[9][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[9][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[9][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorDch[9][BLUE] = Scalar(1.000, 0.000, 0.000);

	m_strColor[10] = "Rojo - Cian (Verdadero clareado)";
	m_colorIzq[10][RED] = Scalar(0.144, 0.587, 0.299); m_colorIzq[10][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[10][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[10][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[10][GREEN] = Scalar(0.100, 0.100, 0.100); m_colorDch[10][BLUE] = Scalar(0.144, 0.587, 0.299);

	m_strColor[11] = "Rojo - Cian (Verdadero muy clareado)";
	m_colorIzq[11][RED] = Scalar(0.144, 0.587, 0.299); m_colorIzq[11][GREEN] = Scalar(0.000, 0.000, 0.000); m_colorIzq[11][BLUE] = Scalar(0.000, 0.000, 0.000);
	m_colorDch[11][RED] = Scalar(0.000, 0.000, 0.000); m_colorDch[11][GREEN] = Scalar(0.300, 0.300, 0.300); m_colorDch[11][BLUE] = Scalar(0.144, 0.587, 0.299);

	/*
	Matrices = {
	'color': [ [ 1, 0, 0, 0, 0, 0, 0, 0, 0 ], [ 0, 0, 0, 0, 1, 0, 0, 0, 1 ] ],
	'true': [ [ 0.299, 0.587, 0.114, 0, 0, 0, 0, 0, 0 ], [ 0, 0, 0, 0, 0, 0, 0.299, 0.587, 0.114 ] ],
	'mono': [ [ 0.299, 0.587, 0.114, 0, 0, 0, 0, 0, 0 ], [ 0, 0, 0, 0.299, 0.587, 0.114, 0.299, 0.587, 0.114 ] ],
	'halfcolor': [ [ 0.299, 0.587, 0.114, 0, 0, 0, 0, 0, 0 ], [ 0, 0, 0, 0, 1, 0, 0, 0, 1 ] ],
	'optimized': [ [ 0, 0.7, 0.3, 0, 0, 0, 0, 0, 0 ], [ 0, 0, 0, 0, 1, 0, 0, 0, 1 ] ],
	}
	*/
}

bool TwoCameras::Init(int c1, int c2)
{
	bool res = false;
	m_videoCapture0.open(c1);
	m_videoCapture0.set(CAP_PROP_FRAME_WIDTH, WIDTH); //Set the frame width
	m_videoCapture0.set(CAP_PROP_FRAME_HEIGHT, HEIGHT); //Set the frame height

	m_videoCapture1.open(c2);
	m_videoCapture1.set(CAP_PROP_FRAME_WIDTH, WIDTH); //Set the frame width
	m_videoCapture1.set(CAP_PROP_FRAME_HEIGHT, HEIGHT); //Set the frame height

	CargaVectoresColor();

	if (m_videoCapture0.isOpened() && m_videoCapture1.isOpened())
	{
		res = true;
		m_videoCapture0.read(m_oriImage0);

		m_videoCapture1.read(m_oriImage1);

		m_dstImage1b = m_oriImage1.clone();
		m_dstImage1b.setTo(Scalar(0, 0, 0));

		m_dstImage1c = m_oriImage1.clone();
		m_dstImage1c.setTo(Scalar(0, 0, 0));

		namedWindow(m_frame0, WINDOW_AUTOSIZE);
		namedWindow(m_frame1, WINDOW_AUTOSIZE);
		namedWindow(m_frame2, WINDOW_AUTOSIZE);

		DWORD dwAttr = GetFileAttributes(SAVEPATH.c_str());

		if (dwAttr == INVALID_FILE_ATTRIBUTES) {
			_mkdir(SAVEPATH.c_str());
		}

		m_realWidth = m_oriImage0.cols;
		m_realHeight = m_oriImage0.rows;

		m_centro = Point2f(m_realWidth / 2.0F, m_realHeight / 2.0F);

		CamSettings();
	}

	return res;
};

void TwoCameras::CamSettings()
{
// https://stackoverflow.com/questions/9191597/disable-auto-focus-in-video-input-library-or-opencv
	// Camara 0
	m_videoCapture0.set(cv::CAP_PROP_AUTOFOCUS, 1);

	// Camara 1
	m_videoCapture1.set(cv::CAP_PROP_AUTOFOCUS, 1);
};

void TwoCameras::ShowImages()
{
	bool ordenImagen = true;
	bool continuar = true;
	// Coordenadas de origen
	int oinix = 0;
	int oiniy = 0;
	int ofinx = 0;
	int ofiny = 0;
	// Coordenadas de destino
	int dinix = 0;
	int diniy = 0;
	int dfinx = 0;
	int dfiny = 0;
	// Mostrar imágenes
	bool showCam0 = true;
	bool showCam1 = true;
	bool showRes = true;
	// Colores de filtro
	bool natural = true;
	int posColor = 0;

	vector<int> compression_params;
	compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	// Para revisar
	// https://walchko.github.io/blog/Vision/anaglyphs/anaglyphs.html
	// http://3dtv.at/Knowhow/AnaglyphComparison_en.aspx
	// http://zaguan.unizar.es/record/64288/files/TAZ-PFC-2017-047.pdf

	Mat BGR0[3];
	Mat BGR1[3];
	Mat BGR2[3];

	cv::split(m_oriImage0, BGR0);
	cv::split(m_oriImage0, BGR1);
	cv::split(m_oriImage0, BGR2);

	while (continuar)
	{
		// Capture
		if (ordenImagen)
		{
			m_videoCapture0.read(m_oriImage0);
			m_videoCapture1.read(m_oriImage1);
		}
		else
		{
			m_videoCapture0.read(m_oriImage1);
			m_videoCapture1.read(m_oriImage0);
		}

		if (m_xpos < 0)
		{
			oinix = -m_xpos;
			ofinx = (int)m_realWidth;
			dinix = 0;
			dfinx = (int)m_realWidth + m_xpos;
		}
		else
		{
			oinix = 0;
			ofinx = (int)m_realWidth - m_xpos;
			dinix = m_xpos;
			dfinx = (int)m_realWidth;
		}

		if (m_ypos < 0)
		{
			oiniy = -m_ypos;
			ofiny = (int)m_realHeight;
			diniy = 0;
			dfiny = (int)m_realHeight + m_ypos;
		}
		else
		{
			oiniy = 0;
			ofiny = (int)m_realHeight - m_ypos;
			diniy = m_ypos;
			dfiny = (int)m_realHeight;
		}

		// Transform
		m_dstImage1b.setTo(0x000000);
		m_oriImage1.colRange(oinix, ofinx).rowRange(oiniy, ofiny).copyTo(m_dstImage1b.colRange(dinix, dfinx).rowRange(diniy, dfiny));

		// Rotation
		m_rotacion = getRotationMatrix2D(m_centro, m_angulo, 1.0);
		warpAffine(m_dstImage1b, m_dstImage1c, m_rotacion, m_dstImage1b.size());

		// Separar las imágenes
		if (natural)
		{
			cv::split(m_oriImage0, BGR0);
			cv::split(m_dstImage1c, BGR1);
		}
		else
		{
			cv::split(m_oriImage0, BGR1);
			cv::split(m_dstImage1c, BGR0);
		}

		BGR0[RED] = (BGR0[RED] * m_colorIzq[posColor][RED][RED]) + (BGR0[GREEN] * m_colorIzq[posColor][RED][GREEN]) + (BGR0[BLUE] * m_colorIzq[posColor][RED][BLUE]);
		BGR0[GREEN] = (BGR0[RED] * m_colorIzq[posColor][GREEN][RED]) + (BGR0[GREEN] * m_colorIzq[posColor][GREEN][GREEN]) + (BGR0[BLUE] * m_colorIzq[posColor][GREEN][BLUE]);
		BGR0[BLUE] = (BGR0[RED] * m_colorIzq[posColor][BLUE][RED]) + (BGR0[GREEN] * m_colorIzq[posColor][BLUE][GREEN]) + (BGR0[BLUE] * m_colorIzq[posColor][BLUE][BLUE]);

		BGR1[RED] = (BGR1[RED] * m_colorDch[posColor][RED][RED]) + (BGR1[GREEN] * m_colorDch[posColor][RED][GREEN]) + (BGR1[BLUE] * m_colorDch[posColor][RED][BLUE]);
		BGR1[GREEN] = (BGR1[RED] * m_colorDch[posColor][GREEN][RED]) + (BGR1[GREEN] * m_colorDch[posColor][GREEN][GREEN]) + (BGR1[BLUE] * m_colorDch[posColor][GREEN][BLUE]);
		BGR1[BLUE] = (BGR1[RED] * m_colorDch[posColor][BLUE][RED]) + (BGR1[GREEN] * m_colorDch[posColor][BLUE][GREEN]) + (BGR1[BLUE] * m_colorDch[posColor][BLUE][BLUE]);

		BGR2[RED] = BGR0[RED];
		BGR2[GREEN] = BGR1[GREEN];
		BGR2[BLUE] = BGR1[BLUE];

		if (natural)
		{
			cv::merge(BGR0, 3, m_oriImage0);
			cv::merge(BGR1, 3, m_dstImage1c);
		}
		else
		{
			cv::merge(BGR1, 3, m_oriImage0);
			cv::merge(BGR0, 3, m_dstImage1c);
		}
		cv::merge(BGR2, 3, m_imgAnaglifo);

		// Show
		if (showCam0)
		{
			if (cv::getWindowProperty(m_frame0, WINDOW_AUTOSIZE)==-1)
			{
				namedWindow(m_frame0, WINDOW_AUTOSIZE);
			}
			imshow(m_frame0, m_oriImage0);
		}
		else
		{
			if (cv::getWindowProperty(m_frame0, WINDOW_AUTOSIZE) != -1)
			{
				destroyWindow(m_frame0);
			}
		}
		
		if (showCam1)
		{
			if (cv::getWindowProperty(m_frame1, WINDOW_AUTOSIZE) == -1)
			{
				namedWindow(m_frame1, WINDOW_AUTOSIZE);
			}
			imshow(m_frame1, m_dstImage1c);
		}
		else
		{
			if (cv::getWindowProperty(m_frame1, WINDOW_AUTOSIZE) != -1)
			{
				destroyWindow(m_frame1);
			}
		}


		// Video result
		if (m_videoWriter.isOpened())
		{
			m_videoWriter.write(m_imgAnaglifo);
		}

		// Result
		if (showRes)
		{
			if (cv::getWindowProperty(m_frame2, WINDOW_AUTOSIZE) == -1)
			{
				namedWindow(m_frame2, WINDOW_AUTOSIZE);
			}
			imshow(m_frame2, m_imgAnaglifo);
		}
		else
		{
			if (cv::getWindowProperty(m_frame2, WINDOW_AUTOSIZE) != -1)
			{
				destroyWindow(m_frame2);
			}
		}

		int key = waitKeyEx((int)(1000/FPS));
		switch (key)
		{
			// Esc
		case 0x1B:
			continuar = false;
			break;
			// I
		case (int)'i':
		case (int)'I':
			ordenImagen = !ordenImagen;
			break;
			// F
		case (int)'f':
		case (int)'F':
			{
				char nomfichero[MAX_PATH];
				// Fecha y hora actual
				time_t now = time(0);
				struct tm newtime;
				localtime_s(&newtime, &now);
				snprintf(nomfichero, MAX_PATH, "%sImgAnaglifo_%04d%02d%02d_%02d%02d%02d.png",
					SAVEPATH.c_str(),
					newtime.tm_year+1900, newtime.tm_mon+1, newtime.tm_mday,
					newtime.tm_hour, newtime.tm_min, newtime.tm_sec);
				imwrite(nomfichero, m_imgAnaglifo, compression_params);
			}
			break;
			// V
		case (int)'v':
		case (int)'V':
		{
			if (!m_videoWriter.isOpened())
			{
				char nomfichero[MAX_PATH];
				// Fecha y hora actual
				time_t now = time(0);
				struct tm newtime;
				localtime_s(&newtime, &now);
				snprintf(nomfichero, MAX_PATH, "%sVidAnaglifo_%04d%02d%02d_%02d%02d%02d.avi",
					SAVEPATH.c_str(),
					newtime.tm_year + 1900, newtime.tm_mon + 1, newtime.tm_mday,
					newtime.tm_hour, newtime.tm_min, newtime.tm_sec);
				Size size = Size((int)m_realWidth, (int)m_realHeight);
				m_videoWriter.open(nomfichero,
					VideoWriter::fourcc('M', 'J', 'P', 'G'),
					FPS, size, true);
				if (!m_videoWriter.isOpened())
				{
					printf("No se abre el vídeo\n");
				}
			}
			else
			{
				m_videoWriter.release();
			}
		}
		break;
		// C
		case (int)'c':
		case (int)'C':
			m_xpos = 0;
			m_ypos = 0;
			m_angulo = 0;
			break;
			// P, K
		case (int)'p':
		case (int)'P':
		case (int)'k':
		case (int)'K':
			if (key == (int)'k' || key == (int)'K')
			{
				posColor = (++posColor) % MAX_POSCOLOR;
			}
			if (key == (int)'p' || key == (int)'P')
				natural = !natural;

			if (natural)
			{
				printf("\tColor: Natural %s\n",
					m_strColor[posColor].c_str());
			}
			else
			{
				printf("\tColor: Invertido %s\n",
					m_strColor[posColor].c_str());
			}
			break;
			// Muestra/Oculta cámara 0
		case (int)0x700000:
			showCam0 = !showCam0;
			break;
			// Muestra/Oculta cámara 1
		case (int)0x710000:
			showCam1 = !showCam1;
			break;
			// Muestra/Oculta resultado
		case (int)0x720000:
			showRes = !showRes;
			break;
			// Retroceso de página
		case (int)0x210000:
			if (m_angulo > -MAX_ANGLE)
				m_angulo--;
			break;
			// Avance de página
		case (int)0x220000:
			if (m_angulo < MAX_ANGLE)
				m_angulo++;
			break;
			// Flecha Izquierda
		case (int)0x250000:
			if (m_xpos > (int)-(m_realWidth / 2))
				m_xpos -= 1;
			break;
			// Flecha Derecha
		case (int)0x270000:
			if (m_xpos < (int)(m_realWidth / 2))
				m_xpos += 1;
			break;
			// Flecha Arriba
		case (int)0x260000:
			if (m_ypos > (int)-(m_realHeight / 2))
				m_ypos -= 1;
			break;
			// Flecha Abajo
		case (int)0x280000:
			if (m_ypos < (int)(m_realHeight / 2))
				m_ypos += 1;
			break;
		default:
			if( key!=-1 )
				printf("La tecla pulsada es %x", key);
			break;
		}
	}
	if (m_videoWriter.isOpened())
	{
		m_videoWriter.release();
	}
};

void TwoCameras::Release()
{
	m_oriImage0.release();
	m_oriImage1.release();
	m_imgCompuesta.release();
	m_dstImage0.release();
	m_dstImage1.release();
	m_dstImage1b.release();
	m_imgAnaglifo.release();

	m_videoCapture0.release();
	m_videoCapture1.release();
	destroyAllWindows();
};

// Utilidades
std::string TwoCameras::ConvertBSTRToMBS(BSTR bstr)
{
	int wslen = ::SysStringLen(bstr);
	return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

std::string TwoCameras::ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
	int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

	std::string dblstr(len, '\0');
	len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
		pstr, wslen /* not necessary NULL-terminated */,
		&dblstr[0], len,
		NULL, NULL /* no default char */);

	return dblstr;
}

void TwoCameras::showHelp()
{
	printf("\n");
	printf("\tWebCam2C (Ayuda)\n");
	printf("\t----------------------------------------\n");
	printf("\t----------------------------------------\n");
	printf("\t- Esc: Cierra el programa.\n");
	printf("\t- F1, F2 y F3 : Muestran u ocultan las pantallas izquierda,\n\t\tderecha y de anaglifo, respectivamente.\n");
	printf("\t- I,i : Intercambia las cámaras.\n");
	printf("\t- F,f : Toma una foto y la guarda en un fichero.\n");
	printf("\t- V,v : Inicia y finaliza la grabación de vídeo.\n");
	printf("\t- C,c : Centra la imágenes, a la posición inicial.\n");
	printf("\t- P,p : Intercambia el color de filtro de cada ojo. No equivale\n\t\ta I, porque I influye en la perspectiva.\n");
	printf("\t- K,k : Cambia los colores de cada filtro.\n");
	printf("\t- Flechas izquierda, derecha, arriba y abajo : Mueven la imagen de\n\t\tla cámara derecha a izquierda, derecha, arriba y abajo; respectivamente.\n");
	printf("\t- AvPag y RePag : Rotan la imagen de la cámara de la derecha.\n");
	printf("\tEstas funcionalidades sirven para calibrar por software la alineación\n\t\tde las cámaras.\n");
}

