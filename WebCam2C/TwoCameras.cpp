#include "TwoCameras.h"
#include <direct.h>
#include <dshow.h>

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

bool TwoCameras::Init(int c1, int c2)
{
	bool res = false;
	m_videoCapture0.open(c1);
	m_videoCapture0.set(CAP_PROP_FRAME_WIDTH, WIDTH); //Set the frame width
	m_videoCapture0.set(CAP_PROP_FRAME_HEIGHT, HEIGHT); //Set the frame height

	m_videoCapture1.open(c2);
	m_videoCapture1.set(CAP_PROP_FRAME_WIDTH, WIDTH); //Set the frame width
	m_videoCapture1.set(CAP_PROP_FRAME_HEIGHT, HEIGHT); //Set the frame height

	if (m_videoCapture0.isOpened() && m_videoCapture1.isOpened())
	{
		res = true;
		m_videoCapture0.read(m_oriImage0);
		m_mskImage0 = m_oriImage0.clone();
		m_mskImage0.setTo(m_colorIzquierdo);

		m_videoCapture1.read(m_oriImage1);
		m_mskImage1 = m_oriImage1.clone();
		m_mskImage1.setTo(m_colorDerecho);

		m_dstImage1b = m_oriImage1.clone();
		m_dstImage1b.setTo(Scalar(0, 0, 0));

		m_dstImage1c = m_oriImage1.clone();
		m_dstImage1c.setTo(Scalar(0, 0, 0));

		namedWindow(m_frame0, WINDOW_AUTOSIZE);
		namedWindow(m_frame1, WINDOW_AUTOSIZE);
		namedWindow(m_frame2, WINDOW_AUTOSIZE);

		imshow(m_frame0, m_oriImage0);
		imshow(m_frame1, m_oriImage1);
		imshow(m_frame2, m_mskImage0);

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
	bool primario = true;
	bool natural = true;

	vector<int> compression_params;
	compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

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

		// Convertir
		cv::bitwise_and(m_mskImage0, m_oriImage0, m_dstImage0);
		cv::bitwise_and(m_mskImage1, m_oriImage1, m_dstImage1);

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

		// Show
		if (showCam0)
		{
			if (cv::getWindowProperty(m_frame0, WINDOW_AUTOSIZE)==-1)
			{
				namedWindow(m_frame0, WINDOW_AUTOSIZE);
			}
			imshow(m_frame0, m_dstImage0);
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
			imshow(m_frame1, m_dstImage1);
		}
		else
		{
			if (cv::getWindowProperty(m_frame1, WINDOW_AUTOSIZE) != -1)
			{
				destroyWindow(m_frame1);
			}
		}

		// Transform
		m_dstImage1b.setTo(0x000000);
		m_dstImage1.colRange(oinix, ofinx).rowRange(oiniy, ofiny).copyTo(m_dstImage1b.colRange(dinix, dfinx).rowRange(diniy,dfiny));

		// Rotation
		m_rotacion = getRotationMatrix2D(m_centro, m_angulo, 1.0);
		warpAffine(m_dstImage1b, m_dstImage1c, m_rotacion, m_dstImage1b.size());

		// Anaglifo
		cv::bitwise_or(m_dstImage0,
			m_dstImage1c,
			m_imgAnaglifo);

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

		int key = waitKeyEx(1);
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
			char nomfichero[MAX_PATH];
			snprintf(nomfichero, MAX_PATH, "%sAnaglifo%d.png", SAVEPATH.c_str(), m_idImage++);
			imwrite(nomfichero, m_imgAnaglifo, compression_params);
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
			if( key==(int)'p' || key == (int)'P')
				primario = !primario;
			if (key == (int)'k' || key == (int)'K')
				natural = !natural;

			if (primario)
			{
				if (natural)
				{
					m_colorIzquierdo = Scalar(0, 0, 255);
					m_colorDerecho = Scalar(255, 255, 0);
				}
				else
				{
					m_colorIzquierdo = Scalar(0, 255, 0);
					m_colorDerecho = Scalar(255, 0, 255);
				}
			}
			else
			{
				if (natural)
				{
					m_colorIzquierdo = Scalar(255, 255, 0);
					m_colorDerecho = Scalar(0, 0, 255); 
				}
				else
				{
					m_colorIzquierdo = Scalar(255, 0, 255);
					m_colorDerecho = Scalar(0, 255, 0); 
				}
			}
			m_mskImage0.setTo(m_colorIzquierdo);
			m_mskImage1.setTo(m_colorDerecho);
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
};

void TwoCameras::Release()
{
	m_oriImage0.release();
	m_oriImage1.release();
	m_imgCompuesta.release();
	m_mskImage0.release();
	m_mskImage1.release();
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