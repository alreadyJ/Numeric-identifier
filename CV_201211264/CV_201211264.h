
// CV_201211264.h : CV_201211264 ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h" // �� ��ȣ�Դϴ�.


// CCV_201211264App:
// �� Ŭ������ ������ ���ؼ��� CV_201211264.cpp�� �����Ͻʽÿ�.
//

class CCV_201211264App : public CWinAppEx
{
public:
	CCV_201211264App();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCV_201211264App theApp;
