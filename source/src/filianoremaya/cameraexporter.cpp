#include <maya/MDagPath.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFnCamera.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MSelectionList.h>

#include "cameraexporter.h"

std::unique_ptr<filianore::Camera> CameraExporter::ExportCamera() {
    MDagPath mDag;
    MSelectionList mList;
    mList.add("persp");
    mList.getDagPath(0, mDag);

    MFnCamera mCamera(mDag);

    MPoint mOrigin = mCamera.eyePoint(MSpace::kWorld);
    MPoint mLookAt = mCamera.centerOfInterestPoint(MSpace::kWorld);
    MVector mUp = mCamera.upDirection(MSpace::kWorld);

    double vFov = mCamera.verticalFieldOfView();

    return std::make_unique<filianore::PhysicalCamera>(filianore::Vector3f((float)mOrigin.x, (float)mOrigin.y, (float)mOrigin.z),
                                                       filianore::Vector3f((float)mLookAt.x, (float)mLookAt.y, (float)mLookAt.z),
                                                       xRes, yRes,
                                                       (float)vFov * 45.f, 0.f, 0.f, 1.f,
                                                       filianore::Vector3f(0.f, -1.f, 0.f), true);
}