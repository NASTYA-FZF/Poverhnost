#define _USE_MATH_DEFINES
#include "math3d.h"
#include <afxwin.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

double FuncZ(double x, double y)
{
	return cos(x * x + y * y) / (x * x + y * y + 1);
}

struct my_polygon
{
	double l;
	math3d::vec4 v1, v2, v3, v4;

	my_polygon(double x, double y, double stepX, double stepY)
	{
		v1 = math3d::vec4(x, y, FuncZ(x, y));
		v2 = math3d::vec4(x + stepX, y, FuncZ(x + stepX, y));
		v3 = math3d::vec4(x + stepX, y + stepY, FuncZ(x + stepX, y + stepY));
		v4 = math3d::vec4(x, y + stepY, FuncZ(x, y + stepY));
	}

	void RotatePolygon(double aroundY, double aroundZ, double x0)
	{
		math3d::mat4 my_mat = math3d::mat4();
		my_mat.rotateY(aroundY).rotateZ(aroundZ).perspective(10);

		v1 = v1 * my_mat;
		v2 = v2 * my_mat;
		v3 = v3 * my_mat;
		v4 = v4 * my_mat;

		double centerX = (v1.x() + v2.x() + v3.x() + v4.x()) / 4;
		double centerY = (v1.y() + v2.y() + v3.y() + v4.y()) / 4;
		double centerZ = (v1.z() + v2.z() + v3.z() + v4.z()) / 4;

		l = sqrt((centerX - x0) * (centerX - x0) + centerY * centerY + centerZ * centerZ);
	}

	PointF* GetPointF()
	{
		PointF* points = new PointF[4];

		points[0] = PointF(v1.y(), v1.z());
		points[1] = PointF(v2.y(), v2.z());
		points[2] = PointF(v3.y(), v3.z());
		points[3] = PointF(v4.y(), v4.z());
		return points;
	}
};

std::vector<my_polygon> RotateAllPolygons(std::vector<my_polygon> my_pl, double aroundY, double aroundZ, double maxX)
{
	for (auto& pl : my_pl) pl.RotatePolygon(aroundY, aroundZ, maxX);
	return my_pl;
}

void SorterPolygons(std::vector<my_polygon>& polygons)
{
	int num;

	for (int i = 0; i < polygons.size() - 1; i++)
	{
		num = i;
		for (int j = i; j < polygons.size(); j++)
		{
			if (polygons[j].l > polygons[num].l) num = j;
		}
		std::swap(polygons[i], polygons[num]);
	}
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
	double aroundZ, aroundY, minX, maxX, minY, maxY;
	std::vector<my_polygon> polygons, cur_polygons;
public:
	Win() 
	{ 
		Create(NULL, L"Поверхность"); 
		GdiplusStartupInput input;  
		Status s; 
		s = GdiplusStartup(&token, &input, NULL); 
		if (s != Ok) MessageBox(L"s != Ok", L"Error", NULL);

		maxX = 6.;
		minX = -6.;
		maxY = 4.;
		minY = -4.;
		int num_polygon = 30;
		double stepX = (maxX - minX) / num_polygon, stepY = (maxY - minY) / num_polygon;
		for (double x = -2.; x < 2.; x += stepX)
			for (double y = -2.; y < 2.; y += stepY)
				polygons.push_back(my_polygon(x, y, stepX, stepY));
	}

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
	buf_gr.Clear({ 255, 0, 0, 0 });

	Pen my_pen({ 255, 255, 0, 0 }, 0.01);
	SolidBrush* my_brush = new SolidBrush(Color::Coral);;

	cur_polygons = RotateAllPolygons(polygons, aroundY, aroundZ, maxX);

	SorterPolygons(cur_polygons);

	for (auto pl : cur_polygons)
	{
		buf_gr.DrawPolygon(&my_pen, pl.GetPointF(), 4);
		buf_gr.FillPolygon(my_brush, pl.GetPointF(), 4);
	}

	gr.DrawImage(&buffer, 0, 0, 0, 0, my_rect_wnd.right, my_rect_wnd.bottom, UnitPixel);
}


void Win::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	double change_ugol = 1;
	if (nChar == VK_LEFT) aroundZ -= change_ugol;
	if (nChar == VK_RIGHT) aroundZ += change_ugol;
	if (nChar == VK_UP) aroundY -= change_ugol;
	if (nChar == VK_DOWN) aroundY += change_ugol;
	Invalidate(FALSE);
}
