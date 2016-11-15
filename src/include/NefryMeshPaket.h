/*

*/

#ifndef __NEFRYMESHPAKET_H__
#define __NEFRYMESHPAKET_H__

#include "../include/aJson/aJSON.h"
#include "../include/aJson/stringbuffer.h"

class NefryMeshPaket {

private:


public:
	bool createPaket(String message);
	aJsonObject getPaket(String paket);
};

#endif
