#include "dagUtils.h"

#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MVector.h>

using namespace filianore;

TransformVectors GetDagObjectTransformData(const MDagPath &dagPath)
{
    TransformVectors transformVecs;

    MStatus status;
    MObject transformNode = dagPath.transform(&status);

    if (!status && status.statusCode() == MStatus::kInvalidParameter)
        return transformVecs;

    MFnDagNode transform(transformNode, &status);

    MTransformationMatrix matrix(transform.transformationMatrix());

    MVector translation = matrix.getTranslation(MSpace::kWorld);
    transformVecs.Translate = StaticArray<float, 3>((float)translation.x, (float)translation.y, (float)translation.z);

    double rotationValues[3];
    MTransformationMatrix::RotationOrder rOrder;
    matrix.getRotation(rotationValues, rOrder);
    transformVecs.Rotate = StaticArray<float, 3>((float)rotationValues[0], (float)rotationValues[1], (float)rotationValues[2]);

    double scaleValues[3];
    matrix.getScale(scaleValues, MSpace::kWorld);
    transformVecs.Scale = StaticArray<float, 3>((float)scaleValues[0], (float)scaleValues[1], (float)scaleValues[2]);

    return transformVecs;
}