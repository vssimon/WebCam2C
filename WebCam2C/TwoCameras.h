#pragma once
#ifndef __TWOCAMERAS
#define __TWOCAMERAS

#include <windows.h>
#include <windef.h>
#include <list>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

class TwoCameras
{
private:
	const float WIDTH = 800.0;
	const float HEIGHT = 600.0;
	const int MAX_ANGLE = 45;

	const string m_frame0 = "frame0";
	const string m_frame1 = "frame1";
	const string m_frame2 = "frame2";
	const string SAVEPATH = ".\\Images\\";
	const float FPS = 30.0;
	static const int MAX_POSCOLOR = 10;

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

	// https://stackoverflow.com/questions/4286223/how-to-get-a-list-of-video-capture-devices-web-cameras-on-windows-c
	HRESULT GetDevices(REFGUID CLSID_categoria, IEnumMoniker **ppEnum);
	list<string> DisplayDeviceInformation(IEnumMoniker *pEnum);

	// https://stackoverflow.com/questions/6284524/bstr-to-stdstring-stdwstring-and-vice-versa
	std::string ConvertBSTRToMBS(BSTR bstr);
	std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
public:
	TwoCameras();

	list<string> GetListCameras();
	bool Init(int c1, int c2);
	void ShowImages();
	void Release();
	static void showHelp();

	~TwoCameras();
};

#endif // !__TWOCAMERAS
