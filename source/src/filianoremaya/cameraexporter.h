#ifndef _CAMERA_EXPORTER_H
#define _CAMERA_EXPORTER_H

#include "filianore/cameras/physical.h"

class CameraExporter
{
public:
    CameraExporter()
        : xRes(0), yRes(0) {}
    CameraExporter(int _xRes, int _yRes)
        : xRes(_xRes), yRes(_yRes)
    {
    }

    std::unique_ptr<filianore::Camera> ExportCamera();

private:
    int xRes, yRes;
};

#endif