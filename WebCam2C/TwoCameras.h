#pragma once
#ifndef __TWOCAMERAS
#define __TWOCAMERAS

#include <windows.h>
#include <windef.h>
#include <opencv2\opencv.hpp>
#include <dshow.h>

using namespace std;
using namespace cv;

class TwoCameras
{
public:
	enum Errores
	{
		Ok = 0,
		NoImagenIzq = 1,
		NoImagenDch = 2,
		NoMismoFormato = 3,
		NoProcesado = 99
	};

	class CamValues
	{
	public:
		string resolution;
		long width;
		long height;
		unsigned int bitrate;
		unsigned int bitcount;
		int FPS;
	};

	class CamInfo
	{
	public:
		string name;
		vector<CamValues> values;
	};

	TwoCameras();

	vector<CamInfo> GetListCameras();

	bool Init(int c1, int c2, long width, long height, int FPS);
	void ShowImages();
	void Release();
	static void showHelp();

	Errores JoinImageFiles(string fchIzq, string fchDch, string fchRes, int modo);

	~TwoCameras();

private:
	const int MAX_ANGLE = 45;

	const string m_frame0 = "frame0";
	const string m_frame1 = "frame1";
	const string m_frame2 = "frame2";
	const string SAVEPATH = ".\\Images\\";
	const float FPS = 30.0;
	static const int MAX_POSCOLOR = 12;

	static const int RED = 2;
	static const int GREEN = 1;
	static const int BLUE = 0;

	int m_xpos = 0;
	int m_ypos = 0;
	int m_realWidth = 0;
	int m_realHeight = 0;

	int m_angulo = 0;
	Point2f m_centro;

	VideoCapture m_videoCapture0;
	VideoCapture m_videoCapture1;

	VideoWriter m_videoWriter;

	Scalar m_colorIzq[MAX_POSCOLOR][3];
	Scalar m_colorDch[MAX_POSCOLOR][3];
	string m_strColor[MAX_POSCOLOR];

	 //http://acodigo.blogspot.com/2013/06/acceso-la-webcam.html

	Mat m_oriImage0;
	Mat m_oriImage1;
	Mat m_imgCompuesta;
	Mat m_dstImage0;
	Mat m_dstImage1;
	Mat m_dstImage1b;
	Mat m_dstImage1c;
	Mat m_imgAnaglifo;

	Mat m_rotacion;

	void CamSettings();

	void CargaVectoresColor();

	// https://stackoverflow.com/questions/4286223/how-to-get-a-list-of-video-capture-devices-web-cameras-on-windows-c
	HRESULT GetDevices(REFGUID CLSID_categoria, IEnumMoniker **ppEnum);
	vector<CamInfo> GetCams(IEnumMoniker *pEnum);
	vector<CamValues> GetCamValues(IBaseFilter *pBaseFilter);

	// https://stackoverflow.com/questions/6284524/bstr-to-stdstring-stdwstring-and-vice-versa
	std::string ConvertBSTRToMBS(BSTR bstr);
	std::string ConvertWCHARToMBS(WCHAR wc[], int lon);
	std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
};

#endif // !__TWOCAMERAS
