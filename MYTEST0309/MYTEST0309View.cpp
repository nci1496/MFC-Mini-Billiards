
// MYTEST0309View.cpp: CMYTEST0309View 类的实现
//

#include <algorithm>
#include <random>

std::random_device rd;
std::mt19937 rng(rd());

std::shuffle(remain.begin(), remain.end(), rng);

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MYTEST0309.h"
#endif

#include "MYTEST0309Doc.h"
#include "MYTEST0309View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


COLORREF ballColors[16] =
{
	RGB(255,255,255), //0 白球
	RGB(255,215,0),   //1 黄
	RGB(0,0,255),     //2 蓝
	RGB(255,0,0),     //3 红
	RGB(128,0,128),   //4 紫
	RGB(255,165,0),   //5 橙
	RGB(0,128,0),     //6 绿
	RGB(128,0,0),     //7 棕
	RGB(0,0,0),       //8 黑
	RGB(255,215,0),   //9
	RGB(0,0,255),     //10
	RGB(255,0,0),     //11
	RGB(128,0,128),   //12
	RGB(255,165,0),   //13
	RGB(0,128,0),     //14
	RGB(128,0,0)      //15
};


// CMYTEST0309View

IMPLEMENT_DYNCREATE(CMYTEST0309View, CView)

BEGIN_MESSAGE_MAP(CMYTEST0309View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CMYTEST0309View 构造/析构

CMYTEST0309View::CMYTEST0309View() noexcept
{
	// TODO: 在此处添加构造代码
	
}

void CMYTEST0309View::UpdatePhysics()
{
	const float friction = 0.995f;
	const float stopThreshold = 0.03f;

	for (auto& b : balls)
	{
		if (!b.alive)
			continue;

		b.x += b.vx * 0.25f;
		b.y += b.vy * 0.25f;

		b.vx *= friction;
		b.vy *= friction;

		if (fabs(b.vx) < stopThreshold) b.vx = 0;
		if (fabs(b.vy) < stopThreshold) b.vy = 0;

		if (b.x - b.r < tableRect.left)
		{
			b.x = tableRect.left + b.r;
			b.vx = -b.vx;
		}

		if (b.x + b.r > tableRect.right)
		{
			b.x = tableRect.right - b.r;
			b.vx = -b.vx;
		}

		if (b.y - b.r < tableRect.top)
		{
			b.y = tableRect.top + b.r;
			b.vy = -b.vy;
		}

		if (b.y + b.r > tableRect.bottom)
		{
			b.y = tableRect.bottom - b.r;
			b.vy = -b.vy;
		}
	}
}

void CMYTEST0309View::CheckCollision()
{
	for (size_t i = 0; i < balls.size(); i++)
	{
		for (size_t j = i + 1; j < balls.size(); j++)
		{
			Ball& a = balls[i];
			Ball& b = balls[j];

			if (!a.alive || !b.alive)
				continue;

			float dx = b.x - a.x;
			float dy = b.y - a.y;

			float dist2 = dx * dx + dy * dy;
			float minDist = a.r + b.r;

			if (dist2 >= minDist * minDist)
				continue;

			float dist = sqrt(dist2);

			if (dist == 0)
				dist = 0.01f;

			// 法线
			float nx = dx / dist;
			float ny = dy / dist;

			// 相对速度
			float rvx = b.vx - a.vx;
			float rvy = b.vy - a.vy;

			float velAlongNormal = rvx * nx + rvy * ny;

			// 如果正在远离，不处理
			if (velAlongNormal > 0)
				continue;

			// 弹性碰撞（质量相同）
			float impulse = -velAlongNormal;

			a.vx -= impulse * nx;
			a.vy -= impulse * ny;

			b.vx += impulse * nx;
			b.vy += impulse * ny;

			// 位置修正（防止黏住）
			float overlap = minDist - dist;

			float correction = overlap * 0.5f;

			a.x -= nx * correction;
			a.y -= ny * correction;

			b.x += nx * correction;
			b.y += ny * correction;

			CreateFirework((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
		}
	}
}
void CMYTEST0309View::CheckHole()
{
	for (auto& b : balls)
	{
		if (!b.alive) continue;

		for (auto& h : holes)
		{
			float dx = b.x - h.x;
			float dy = b.y - h.y;



			if (sqrt(dx * dx + dy * dy) < h.r)
			{
				b.alive = false;

				b.vx = 0;
				b.vy = 0;

				b.r -= 1;

				if (b.isWhite)
					//AfxMessageBox(_T("Game Over"));
				{
					if (MessageBox(
						_T("白球落袋！游戏结束\n是否重新开局？"),
						_T("MFC迷你台球"),
						MB_YESNO | MB_ICONQUESTION
					) == IDYES)
					{
						InitTable();
					}
				}

				CreateFirework(b.x, b.y);
			}
		}
	}
}

void CMYTEST0309View::CreateFirework(float x, float y)
{
	for (int i = 0; i < 30; i++)
	{
		Particle p;

		p.x = x;
		p.y = y;

		p.vx = (rand() % 200 - 100) / 40.0f;
		p.vy = (rand() % 200 - 100) / 40.0f;

		p.life = 30;

		p.color = RGB(rand() % 255, rand() % 255, rand() % 255);

		particles.push_back(p);
	}
}

CMYTEST0309View::~CMYTEST0309View()
{
}

BOOL CMYTEST0309View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMYTEST0309View 绘图

void CMYTEST0309View::OnDraw(CDC* pDC)
{
	CMYTEST0309Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	CRect rect;
	GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

	CBitmap* oldBmp = memDC.SelectObject(&bmp);


	memDC.FillSolidRect(rect, RGB(0, 100, 0));

	memDC.FillSolidRect(tableRect, RGB(0,120,0));

	for (auto& h : holes)
	{
		memDC.Ellipse(h.x - h.r, h.y - h.r, h.x + h.r, h.y + h.r);
	}
	for (auto& b : balls)
	{
		if (!b.alive) continue;

		CBrush brush(b.color);
		CBrush* old = memDC.SelectObject(&brush);

		memDC.Ellipse(
			b.x - b.r,
			b.y - b.r,
			b.x + b.r,
			b.y + b.r
		);

		memDC.SelectObject(old);


		// 条纹球
		if (b.stripe)
		{
			CBrush stripeBrush(RGB(255, 255, 255));
			CBrush* old2 = memDC.SelectObject(&stripeBrush);

			memDC.Rectangle(
				b.x - b.r,
				b.y - b.r / 2,
				b.x + b.r,
				b.y + b.r / 2
			);

			memDC.SelectObject(old2);
		}


		// 数字背景
		if (!b.isWhite)
		{
			CBrush whiteBrush(RGB(255, 255, 255));
			CBrush* old3 = memDC.SelectObject(&whiteBrush);

			int cr = b.r * 0.5f;

			memDC.Ellipse(
				b.x - cr,
				b.y - cr,
				b.x + cr,
				b.y + cr
			);

			memDC.SelectObject(old3);


			CString text;
			text.Format(_T("%d"), b.number);

			memDC.SetBkMode(TRANSPARENT);
			memDC.SetTextColor(RGB(0, 0, 0));

			memDC.TextOut(
				b.x - 5,
				b.y - 7,
				text
			);
		}
	}

	

	for (auto& p : particles)
	{
		CBrush brush(p.color);
		CBrush* old = memDC.SelectObject(&brush);

		memDC.Ellipse(p.x, p.y, p.x + 4, p.y + 4);

		memDC.SelectObject(old);
	}

	if (aiming)
	{


		Ball& white = balls[0];
		/*TRACE(_T("Drawing cue...\n"));
		TRACE(_T("mousePos: (%d, %d)\n"), mousePos.x, mousePos.y);
		TRACE(_T("white ball: (%f, %f)\n"), white.x, white.y);*/
		float dx = mousePos.x - white.x;
		float dy = mousePos.y - white.y;

		float len = sqrt(dx * dx + dy * dy);

		if (len < 5) return;

		float dirx = dx / len;
		float diry = dy / len;

		// 球杆长度 = 鼠标距离
		float cueLen = len;

		// 球杆头
		float headX = white.x + dirx * (white.r + 2);
		float headY = white.y + diry * (white.r + 2);

		// 球杆尾部（在鼠标反方向）
		float tailX = headX + dirx * cueLen;
		float tailY = headY + diry * cueLen;

		CPen pen(PS_SOLID, 6, RGB(180, 140, 90));
		CPen* old = memDC.SelectObject(&pen);

		memDC.MoveTo((int)tailX, (int)tailY);
		memDC.LineTo((int)headX, (int)headY);

		memDC.SelectObject(old);
	}

	pDC->BitBlt(
		0,
		0,
		rect.Width(),
		rect.Height(),
		&memDC,
		0,
		0,
		SRCCOPY
	);

	memDC.SelectObject(oldBmp);
}


// CMYTEST0309View 打印

BOOL CMYTEST0309View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMYTEST0309View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMYTEST0309View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMYTEST0309View 诊断

#ifdef _DEBUG
void CMYTEST0309View::AssertValid() const
{
	CView::AssertValid();
}

void CMYTEST0309View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMYTEST0309Doc* CMYTEST0309View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMYTEST0309Doc)));
	return (CMYTEST0309Doc*)m_pDocument;
}
#endif //_DEBUG


// CMYTEST0309View 消息处理程序

void CMYTEST0309View::OnTimer(UINT_PTR nIDEvent)
{
	for (int i = 0; i < 4; i++)
	{
		UpdatePhysics();
		CheckCollision();
	}
	CheckHole();

	for (auto& p : particles)
	{
		p.x += p.vx;
		p.y += p.vy;
		p.vy += 0.1;
		p.life--;
	}

	particles.erase(
		std::remove_if(particles.begin(), particles.end(),
			[](Particle& p) {return p.life <= 0; }), particles.end());

	Invalidate(FALSE);

	CView::OnTimer(nIDEvent);
}

void CMYTEST0309View::InitTable()
{
	AfxGetMainWnd()->SetWindowText(_T("MFC迷你台球"));
	
	balls.clear();
	holes.clear();

	CRect rect;
	GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	tableRect = CRect(
		rect.left + tableMargin,
		rect.top + tableMargin,
		rect.right - tableMargin,
		rect.bottom - tableMargin
	);

	int r = 30;

	// 白球
	Ball white;

	white.x = tableRect.left + tableRect.Width() * 0.25f;
	white.y = tableRect.top + tableRect.Height() * 0.5f;

	white.vx = 0;
	white.vy = 0;

	white.r = r;

	white.number = 0;
	white.color = RGB(255, 255, 255);

	white.alive = true;
	white.isWhite = true;
	white.stripe = false;
	
	balls.push_back(white);


	// 三角阵起点
	float startX = tableRect.left + tableRect.Width() * 0.70f;
	float startY = tableRect.top + tableRect.Height() * 0.50f;

	/*int number = 1;

	float spacing = r * 2.02f;

	for (int row = 0; row < 5; row++)
	{
		for (int col = 0; col <= row; col++)
		{
			Ball b;

			b.x = startX + row * spacing;
			b.y = startY + (col - row * 0.5f) * spacing;

			b.vx = 0;
			b.vy = 0;

			b.r = r;

			b.number = number;
			b.color = ballColors[number];

			b.alive = true;
			b.isWhite = false;

			b.stripe = (number >= 9);

			balls.push_back(b);

			number++;
		}
	}*/


	int layout[15];

	// 先放默认值
	for (int i = 0; i < 15; i++)
		layout[i] = i + 1;


	// 1️⃣ 八号球放中间
	layout[4] = 8;


	// 2️⃣ 底边两端一实一花
	layout[10] = 1 + rand() % 7;       // 实心球
	layout[14] = 9 + rand() % 7;       // 花球


	// 3️⃣ 剩余球
	std::vector<int> remain;

	for (int i = 1; i <= 15; i++)
	{
		if (i == 8) continue;
		if (i == layout[10]) continue;
		if (i == layout[14]) continue;

		remain.push_back(i);
	}

	int idx = 0;

	for (int i = 0; i < 15; i++)
	{
		if (i == 4 || i == 10 || i == 14)
			continue;

		layout[i] = remain[idx++];
	}

	int index = 0;

	float spacing = r * 2.2f;

	for (int row = 0; row < 5; row++)
	{
		for (int col = 0; col <= row; col++)
		{
			Ball b;

			b.x = startX + row * spacing;
			b.y = startY + (col - row * 0.5f) * spacing;

			b.vx = 0;
			b.vy = 0;

			b.r = r;

			int number = layout[index++];

			b.number = number;
			b.color = ballColors[number];

			b.alive = true;
			b.isWhite = false;

			b.stripe = (number >= 9);

			balls.push_back(b);
		}
	}


	int holeR = 60;

	holes.push_back({ tableRect.left,tableRect.top,holeR });
	holes.push_back({ tableRect.CenterPoint().x,tableRect.top,holeR });
	holes.push_back({ tableRect.right,tableRect.top,holeR });

	holes.push_back({ tableRect.left,tableRect.bottom,holeR });
	holes.push_back({ tableRect.CenterPoint().x,tableRect.bottom,holeR });
	holes.push_back({ tableRect.right,tableRect.bottom,holeR });
}

void CMYTEST0309View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	InitTable();

	SetTimer(1, 16, NULL);
}

void CMYTEST0309View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (aiming)
	{
		mousePos = point;
		Invalidate(FALSE);
	}

	CView::OnMouseMove(nFlags, point);
}


void CMYTEST0309View::OnLButtonDown(UINT nFlags, CPoint point)
{

	SetCapture();

	aiming = true;
	aimStart = point;
	aimEnd = point;


	CView::OnLButtonDown(nFlags, point);
}


void CMYTEST0309View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ReleaseCapture();

	if (!aiming) { return; }

	aiming = false;

	Ball& white = balls[0];

	float dx = white.x - point.x;
	float dy = white.y - point.y;

	float power = 0.08f;

	white.vx = dx * power;
	white.vy = dy * power;

	CView::OnLButtonUp(nFlags, point);
}
