#ifndef RENDERCRAFT_OBJECT_H
#define RENDERCRAFT_OBJECT_H
#include <utility>

#include "types.h"
#include "Intersection.h"
#include "Ray.h"
#include "../../XMath/ext/Graphics/MathUtils.h"
#include "Surface.h"
#include "../../XMath/ext/Matrix.h"
#include "Transform.h"

class Object : public Surface
{
	public:
		Object() = default;
		virtual ~Object() = default;
	protected:
        Material* mat = nullptr;
        TextureGroup* tex = nullptr;
};
#endif