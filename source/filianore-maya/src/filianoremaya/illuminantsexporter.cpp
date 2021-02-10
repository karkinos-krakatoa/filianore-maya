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
#include <maya/MFnDirectionalLight.h>
#include <maya/MFnAreaLight.h>
#include <maya/MColor.h>

#include "illuminantsexporter.h"
#include "util.h"
#include "dagUtils.h"

#include "filianore/color/rgb.h"

#include "filianore/illuminants/point.h"
#include "filianore/illuminants/spot.h"
#include "filianore/illuminants/directional.h"
#include "filianore/illuminants/diffusearea.h"

#include "filianore/creators/shapecreator.h"
#include "filianore/shapes/triangle.h"
#include "filianore/core/primitive.h"

using namespace filianore;

using Vec3 = StaticArray<float, 3>;
using Vec2 = StaticArray<float, 2>;
using TriEntity = TriangleEntity;

std::vector<std::shared_ptr<Illuminant>> IlluminantExporter::ExportIlluminants(std::vector<std::shared_ptr<Primitive>> *prims)
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
                status.perror("MFnPointLight constructor");
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
                status.perror("MFnSpotLight constructor");
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

        if (dagPath.hasFn(MFn::kDirectionalLight))
        {
            MFnDirectionalLight dirLight(dagPath, &status);
            if (!status)
            {
                status.perror("MFnDirectionalLight constructor");
                continue;
            }

            RGBSpectrum color = RGBSpectrum(dirLight.color().r, dirLight.color().g, dirLight.color().b);
            RGBSpectrum shadowColor = RGBSpectrum(dirLight.shadowColor().r, dirLight.shadowColor().g, dirLight.shadowColor().b);
            float intensity = dirLight.intensity();
            MFloatVector direction = dirLight.lightDirection(0, MSpace::kWorld);

            std::shared_ptr<Illuminant> actualIllum = std::make_shared<DirectionalIlluminant>(transform, StaticArray<float, 3>(direction.x, direction.y, direction.z),
                                                                                              color, intensity, shadowColor);
            illums.emplace_back(actualIllum);
        }

        if (dagPath.hasFn(MFn::kAreaLight))
        {
            MFnAreaLight areaLight(dagPath, &status);
            if (!status)
            {
                status.perror("MFnAreaLight constructor");
                continue;
            }

            ShapeCreator shapeCreator;
            std::vector<std::shared_ptr<Shape>> quad = shapeCreator.CreateQuad(transform);

            RGBSpectrum color = RGBSpectrum(areaLight.color().r, areaLight.color().g, areaLight.color().b);
            RGBSpectrum shadowColor = RGBSpectrum(areaLight.shadowColor().r, areaLight.shadowColor().g, areaLight.shadowColor().b);
            float intensity = areaLight.intensity();

            for (auto shape : quad)
            {
                const std::shared_ptr<AreaIlluminant> areaIllum = std::make_shared<DiffuseAreaIlluminant>(transform, color, intensity, areaLight.decayRate(), shadowColor, shape);
                //prims->emplace_back(std::make_shared<GeometricPrimitive>(shape, nullptr, areaIllum));
                illums.emplace_back(areaIllum);
            }
        }
    }

    return illums;
}
