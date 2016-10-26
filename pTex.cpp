#include "pTex.h"

PTex::PTex(int size) : size(size) {
	pages.clear();
}

PTex::~PTex() {}

void PTex::addPage(Page page) {
	pages.push_back(page);
}

Page PTex::getPage(int index) const {
	if (index < pages.size())
		return Page(pages[index]);
	else
		return Page();
}
