#include "dagUtils.h"

#include <maya/MFnDagNode.h>
#include <maya/MObject.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MStatus.h>
#include <maya/MVector.h>

TransformVectors GetDagObjectTransformData(const MDagPath &dagPath) {
    TransformVectors transformVecs;

    MStatus status;
    MObject transformNode = dagPath.transform(&status);

    if (!status && status.statusCode() == MStatus::kInvalidParameter)
        return transformVecs;

    MFnDagNode transform(transformNode, &status);

    MTransformationMatrix matrix(transform.transformationMatrix());

    MVector translation = matrix.getTranslation(MSpace::kWorld);
    transformVecs.Translate = filianore::Vector3f((float)translation.x, (float)translation.y, (float)translation.z);

    double rotationValues[3];
    MTransformationMatrix::RotationOrder rOrder;
    matrix.getRotation(rotationValues, rOrder);
    transformVecs.Rotate = filianore::Vector3f((float)rotationValues[0], (float)rotationValues[1], (float)rotationValues[2]);

    double scaleValues[3];
    matrix.getScale(scaleValues, MSpace::kWorld);
    transformVecs.Scale = filianore::Vector3f((float)scaleValues[0], (float)scaleValues[1], (float)scaleValues[2]);

    return transformVecs;
}