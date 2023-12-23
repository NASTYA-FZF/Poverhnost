#define _USE_MATH_DEFINES
#include "math3d.h"
#include <afxwin.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

double FuncZ(double x, double y)
{
	return cos(x * x + y * y) / (x * x + y * y + 1);
}

class App : public CWinApp
{

public:
	virtual BOOL InitInstance();
};

App my_app;

class Win : public CFrameWnd
{
	ULONG_PTR token;
	double aroundZ;
	double aroundY;
public:
	Win() { Create(NULL, L"Поверхность"); GdiplusStartupInput input;  Status s; s = GdiplusStartup(&token, &input, NULL); if (s != Ok) MessageBox(L"s != Ok", L"Error", NULL); }
	~Win() { GdiplusShutdown(token); };
	DECLARE_MESSAGE_MAP()
		afx_msg void OnPaint();
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

BOOL App::InitInstance()
{
	// TODO: добавьте специализированный код или вызов базового класса
	m_pMainWnd = new Win;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return CWinApp::InitInstance();
}
BEGIN_MESSAGE_MAP(Win, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void Win::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: добавьте свой код обработчика сообщений
	// Не вызывать CFrameWnd::OnPaint() для сообщений рисования
	
	double maxX = 3., minX = -3., maxY = 2., minY = -2.;
	double stepX = (maxX - minX) / 20, stepY = (maxY - minY) / 20;

	RECT my_rect_wnd;
	GetClientRect(&my_rect_wnd);

	Graphics gr(dc);
	Bitmap buffer(my_rect_wnd.right, my_rect_wnd.bottom, &gr);
	Graphics buf_gr(&buffer);
	buf_gr.SetSmoothingMode(SmoothingModeAntiAlias);

	Matrix matr;
	matr.Translate(my_rect_wnd.right / 2, my_rect_wnd.bottom / 2);
	matr.Scale(my_rect_wnd.right / (maxX - minX), my_rect_wnd.bottom / (minY - maxY));
	buf_gr.SetTransform(&matr);
	buf_gr.Clear({ 255, 171, 205, 239 });

	Pen my_pen({ 255, 255, 0, 0 }, 0.01);
	SolidBrush* my_brush = new SolidBrush({ 255, 200, 5, 0 });

	for (double x = -2.; x < 2.; x += stepX)
	{
		for (double y = -2.; y < 2.; y += stepY)
		{
			math3d::vec4 v1(x, y, FuncZ(x, y));
			math3d::vec4 v2(x + stepX, y, FuncZ(x + stepX, y));
			math3d::vec4 v3(x + stepX, y + stepY, FuncZ(x + stepX, y + stepY));
			math3d::vec4 v4(x, y + stepY, FuncZ(x, y + stepY));

			math3d::mat4 my_mat = math3d::mat4();
			my_mat.rotateY(aroundY).rotateZ(aroundZ);
			v1 = v1 * my_mat;
			v2 = v2 * my_mat;
			v3 = v3 * my_mat;
			v4 = v4 * my_mat;

			PointF p1(v1.y(), v1.z());
			PointF p2(v2.y(), v2.z());
			PointF p3(v3.y(), v3.z());
			PointF p4(v4.y(), v4.z());

			PointF points[] = { p1, p2, p3, p4 };
			buf_gr.DrawPolygon(&my_pen, points, 4);
			buf_gr.FillPolygon(my_brush, points, 4);
		}
	}

	gr.DrawImage(&buffer, 0, 0, 0, 0, my_rect_wnd.right, my_rect_wnd.bottom, UnitPixel);
}


void Win::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_LEFT) aroundZ--;
	if (nChar == VK_RIGHT) aroundZ++;
	if (nChar == VK_UP) aroundY--;
	if (nChar == VK_DOWN) aroundY++;
	Invalidate(FALSE);
}
