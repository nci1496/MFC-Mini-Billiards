
// MYTEST0309View.h: CMYTEST0309View 类的接口
//

#pragma once


#include<vector>
struct Particle {
	float x;
	float y;

	float vx;
	float vy;

	int life;
	COLORREF color;

};

struct Ball {
	float x;
	float y;

	float vx;
	float vy;

	int r;
	COLORREF color;
	
	
	bool alive;
	bool isWhite;
	int number;
	bool stripe;//花色球条纹
};

struct Hole
{
	int x;
	int y;
	int r;
};

class CMYTEST0309View : public CView
{
protected: // 仅从序列化创建
	CMYTEST0309View() noexcept;
	DECLARE_DYNCREATE(CMYTEST0309View)

public:
	std::vector<Particle>particles;
	std::vector<Hole> holes;
	std::vector<Ball> balls;

	bool aiming = false;
	CPoint mousePos;
	CPoint aimStart;
	CPoint aimEnd;

	int	tableMargin = 80;
	CRect tableRect;



public:
	void UpdatePhysics();
	void CheckCollision();
	void CheckHole();
	void CreateFirework(float x, float y);
	void InitTable();


// 特性
public:
	CMYTEST0309Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMYTEST0309View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // MYTEST0309View.cpp 中的调试版本
inline CMYTEST0309Doc* CMYTEST0309View::GetDocument() const
   { return reinterpret_cast<CMYTEST0309Doc*>(m_pDocument); }
#endif

