#include "wx/wx.h"
#include "cMainClient.h"
#pragma once


class cClient : public wxApp
{
public:
	cClient();
	~cClient();

private:
	cMainClient* window = nullptr;

public:
	virtual bool OnInit();
};

