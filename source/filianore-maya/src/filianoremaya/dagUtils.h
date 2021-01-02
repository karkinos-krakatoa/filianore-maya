#ifndef _MAYA_DAG_UTILS_H
#define _MAYA_DAG_UTILS_H

#include <maya/MDagPath.h>
#include "filianore/core/elemental.h"
#include "filianore/maths/static_array.h"

struct TransformVectors
{
    filianore::StaticArray<float, 3> Translate;
    filianore::StaticArray<float, 3> Rotate;
    filianore::StaticArray<float, 3> Scale;
};

TransformVectors GetDagObjectTransformData(const MDagPath &dagPath);

#endif