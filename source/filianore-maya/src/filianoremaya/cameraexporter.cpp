#include <maya/MDagPath.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFnCamera.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>

#include "cameraexporter.h"

using Vec3 = filianore::StaticArray<float, 3>;

std::unique_ptr<filianore::Camera> CameraExporter::ExportCamera()
{
    MDagPath mDag;
    MSelectionList mList;
    mList.add("persp");
    mList.getDagPath(0, mDag);

    MFnCamera mCamera(mDag);

    MPoint mOrigin = mCamera.eyePoint(MSpace::kWorld);
    MPoint mLookAt = mCamera.centerOfInterestPoint(MSpace::kWorld);
    MVector mUp = mCamera.upDirection(MSpace::kWorld);

    double vFov = mCamera.verticalFieldOfView();

    return std::make_unique<filianore::PhysicalCamera>(Vec3((float)mOrigin.x, (float)mOrigin.y, (float)mOrigin.z),
                                                       Vec3((float)mLookAt.x, (float)mLookAt.y, (float)mLookAt.z),
                                                       xRes, yRes,
                                                       (float)vFov * 57.29578f, 0.f, 0.f, 1.f,
                                                       Vec3(0.f, -1.f, 0.f), true);
}