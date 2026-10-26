#pragma once
#include "KAABB.h"
#include "KCollideTriangle.h"


//originally wanted to make a child class extra for end nodes but thats a memory management mess. It doesn't actually matter since we just store pointers to the next node anyway, so it doesn't cost anything to do this
struct KBvhNode
{
	KAABB bb;
	KBvhNode* children[2];

	uint8_t numTris = 0;
	//indices to the tris
	KCollideTriangle *tris[8];
};


