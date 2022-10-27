#ifndef RENDERCRAFT_TEXTURE_H
#define RENDERCRAFT_TEXTURE_H
#include "global.h"
template<typename T>
struct Texture
{
	virtual const T eval(Real u, Real v) const = 0;
};
#endif

