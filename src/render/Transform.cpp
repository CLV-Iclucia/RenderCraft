//
// Created by creeper on 23-1-6.
//

#include "Transform.h"
#include "../../XMath/ext/Matrix.h"

Transform Transform::inv() const
{
    return { rot.transpose(), -rot.transpose() * translate };
}
