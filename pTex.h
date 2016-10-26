#pragma once

#include "global.h"
#include "page.h"
using namespace std;

class PTex {
private:
	vector<Page> pages;
	int size;
public:
	PTex(int size);
	~PTex();
	void addPage(Page page);
	Page getPage(int index) const;
};

