#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>

#include "illuminantsexporter.h"
#include "util.h"

#include "filianore/core/illuminant.h"

std::vector<std::shared_ptr<filianore::Illuminant>> IlluminantExporter::ExportIlluminants()
{
    std::vector<std::shared_ptr<filianore::Illuminant>> illums;

    int lightCount = 0;

    for (MItDag it; !it.isDone(); it.next())
    {
        MObject obj = it.currentItem();
        if (obj.apiType() == MFn::kLight)
        {
            lightCount++;
        }
    }

    auto ss = std::to_string(lightCount);
    MString primCount = ss.c_str();
    FILIANORE_MAYA_LOG_INFO("Light Count - " + primCount);

    return illums;
}
