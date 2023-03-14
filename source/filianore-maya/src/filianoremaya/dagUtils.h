#ifndef _MAYA_DAG_UTILS_H
#define _MAYA_DAG_UTILS_H

#include "filianore/core/elemental.h"
#include "filianore/maths/vec.h"
#include <maya/MDagPath.h>

struct TransformVectors {
    filianore::Vector3f Translate;
    filianore::Vector3f Rotate;
    filianore::Vector3f Scale;
};

TransformVectors GetDagObjectTransformData(const MDagPath &dagPath);

#endif