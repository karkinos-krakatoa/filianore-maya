#include <memory>

#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MFnLight.h>
#include <maya/MFnPointLight.h>
#include <maya/MColor.h>

#include "illuminantsexporter.h"
#include "util.h"
#include "dagUtils.h"

#include "filianore/color/rgb.h"

#include "filianore/illuminants/point.h"

using namespace filianore;

std::vector<std::shared_ptr<Illuminant>> IlluminantExporter::ExportIlluminants()
{
    std::vector<std::shared_ptr<Illuminant>> illums;

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
            MFnPointLight pointIllum(dagPath, &status);
            if (!status)
            {
                status.perror("MFnLight constructor");
                continue;
            }

            TransformVectors transformData = GetDagObjectTransformData(dagPath);
            RGBSpectrum color = RGBSpectrum(pointIllum.color().r, pointIllum.color().g, pointIllum.color().b);
            RGBSpectrum shadowColor = RGBSpectrum(pointIllum.shadowColor().r, pointIllum.shadowColor().g, pointIllum.shadowColor().b);
            float intensity = pointIllum.intensity();

            std::shared_ptr<Illuminant> actualPointIllum = std::make_shared<PointIlluminant>(transformData.Translate, color, intensity, pointIllum.decayRate(), shadowColor);
            illums.emplace_back(actualPointIllum);
        }
    }

    return illums;
}
