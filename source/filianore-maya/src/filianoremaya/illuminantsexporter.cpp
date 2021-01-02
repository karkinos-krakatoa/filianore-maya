#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MFnLight.h>
#include <maya/MFnPointLight.h>

#include "illuminantsexporter.h"
#include "util.h"

#include "filianore/illuminants/point.h"

std::vector<std::shared_ptr<filianore::Illuminant>> IlluminantExporter::ExportIlluminants()
{
    std::vector<std::shared_ptr<filianore::Illuminant>> illums;

    MStatus status;
    MItDag dagIt(MItDag::kBreadthFirst, MFn::kLight, &status);
    for (; !dagIt.isDone(); dagIt.next())
    {
        MDagPath dagPath;
        status = dagIt.getPath(dagPath);
        if (!status)
        {
            status.perror("MItDag::getPath");
            continue;
        }

        if (dagPath.hasFn(MFn::kPointLight))
        {
            MFnPointLight pointIllum;
            //pointIllum
        }
    }

    return illums;
}
