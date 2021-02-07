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
#include <maya/MFnSpotLight.h>
#include <maya/MColor.h>

#include "illuminantsexporter.h"
#include "util.h"
#include "dagUtils.h"

#include "filianore/color/rgb.h"

#include "filianore/illuminants/point.h"
#include "filianore/illuminants/spot.h"

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

        TransformVectors transformData = GetDagObjectTransformData(dagPath);
        Transform transform = Translate(transformData.Translate);
        //transform = transform * (RotateX(transformData.Rotate.x(), true) * RotateY(transformData.Rotate.y(), true) * RotateZ(transformData.Rotate.z(), true));

        if (dagPath.hasFn(MFn::kPointLight))
        {
            MFnPointLight pointIllum(dagPath, &status);
            if (!status)
            {
                status.perror("MFnLight constructor");
                continue;
            }

            RGBSpectrum color = RGBSpectrum(pointIllum.color().r, pointIllum.color().g, pointIllum.color().b);
            RGBSpectrum shadowColor = RGBSpectrum(pointIllum.shadowColor().r, pointIllum.shadowColor().g, pointIllum.shadowColor().b);
            float intensity = pointIllum.intensity();

            std::shared_ptr<Illuminant> actualIllum = std::make_shared<PointIlluminant>(transform, color, intensity, pointIllum.decayRate(), shadowColor);
            illums.emplace_back(actualIllum);
        }

        if (dagPath.hasFn(MFn::kSpotLight))
        {
            MFnSpotLight spotLight(dagPath, &status);
            if (!status)
            {
                status.perror("MFnLight constructor");
                continue;
            }

            RGBSpectrum color = RGBSpectrum(spotLight.color().r, spotLight.color().g, spotLight.color().b);
            RGBSpectrum shadowColor = RGBSpectrum(spotLight.shadowColor().r, spotLight.shadowColor().g, spotLight.shadowColor().b);
            float intensity = spotLight.intensity();
            MFloatVector direction = spotLight.lightDirection(0, MSpace::kWorld);

            std::shared_ptr<Illuminant> actualIllum = std::make_shared<SpotIlluminant>(transform, StaticArray<float, 3>(direction.x, direction.y, direction.z),
                                                                                       (float)spotLight.coneAngle(), (float)spotLight.penumbraAngle(),
                                                                                       true, true,
                                                                                       color, intensity, spotLight.decayRate(), shadowColor);
            illums.emplace_back(actualIllum);
        }
    }

    return illums;
}
