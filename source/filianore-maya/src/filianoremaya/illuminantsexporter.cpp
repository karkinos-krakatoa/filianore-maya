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

#include "filianore/illuminants/point.h"
#include "filianore/illuminants/spot.h"
#include "filianore/illuminants/directional.h"
#include "filianore/illuminants/diffusearea.h"

#include "filianore/creators/shapecreator.h"
#include "filianore/shapes/triangle.h"
#include "filianore/core/primitive.h"

#include "filianore/textures/constant.h"
#include "filianore/materials/lambert.h"

#include "filianore/color/spectrumoperations.h"
#include "filianore/color/spectruminits.h"

using namespace filianore;

using Vec3 = StaticArray<float, 3>;
using Vec2 = StaticArray<float, 2>;
using TriEntity = TriangleEntity;

IlluminantExporterResponse IlluminantExporter::ExportIlluminants(const std::vector<std::shared_ptr<Primitive>> &prims)
{
    IlluminantExporterResponse illuminantExporterResponse;

    std::vector<std::shared_ptr<Primitive>> newPrims;
    std::vector<std::shared_ptr<Illuminant>> illums;

    PrincipalSpectrum col(1.f);
    std::shared_ptr<Texture<PrincipalSpectrum>> tex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(col);
    std::shared_ptr<Material> defaultMaterial = std::make_shared<LambertMaterial>(tex);

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

        if (dagPath.hasFn(MFn::kPointLight))
        {
            MFnPointLight pointIllum(dagPath, &status);
            if (!status)
            {
                status.perror("MFnPointLight constructor");
                continue;
            }

            PrincipalSpectrum color = FromIlluminanceRGB(StaticArray<float, 3>(pointIllum.color().r, pointIllum.color().g, pointIllum.color().b));
            PrincipalSpectrum shadowColor = FromReflectanceRGB(StaticArray<float, 3>(pointIllum.shadowColor().r, pointIllum.shadowColor().g, pointIllum.shadowColor().b));
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

            PrincipalSpectrum color = FromIlluminanceRGB(StaticArray<float, 3>(spotLight.color().r, spotLight.color().g, spotLight.color().b));
            PrincipalSpectrum shadowColor = FromReflectanceRGB(StaticArray<float, 3>(spotLight.shadowColor().r, spotLight.shadowColor().g, spotLight.shadowColor().b));
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

            PrincipalSpectrum color = FromIlluminanceRGB(StaticArray<float, 3>(dirLight.color().r, dirLight.color().g, dirLight.color().b));
            PrincipalSpectrum shadowColor = FromReflectanceRGB(StaticArray<float, 3>(dirLight.shadowColor().r, dirLight.shadowColor().g, dirLight.shadowColor().b));
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

            Transform _transform = Translate(transformData.Translate) *
                                   (RotateX(transformData.Rotate.x(), true) * RotateY(transformData.Rotate.y(), true) * RotateZ(transformData.Rotate.z(), true)) *
                                   Scale(transformData.Scale.x(), transformData.Scale.y(), transformData.Scale.z());

            ShapeCreator shapeCreator;
            std::vector<std::shared_ptr<Shape>> quad = shapeCreator.CreateQuad(_transform);

            PrincipalSpectrum color = FromIlluminanceRGB(StaticArray<float, 3>(areaLight.color().r, areaLight.color().g, areaLight.color().b));
            PrincipalSpectrum shadowColor = FromReflectanceRGB(StaticArray<float, 3>(areaLight.shadowColor().r, areaLight.shadowColor().g, areaLight.shadowColor().b));

            float intensity = areaLight.intensity();

            std::shared_ptr<AreaIlluminant> areaIllum1 = std::make_shared<DiffuseAreaIlluminant>(_transform, color, intensity, areaLight.decayRate(), shadowColor, quad.at(0));
            std::shared_ptr<GeometricPrimitive> prim1 = std::make_shared<GeometricPrimitive>(quad.at(0), defaultMaterial, areaIllum1);
            newPrims.emplace_back(prim1);
            illums.emplace_back(areaIllum1);

            std::shared_ptr<AreaIlluminant> areaIllum2 = std::make_shared<DiffuseAreaIlluminant>(_transform, color, intensity, areaLight.decayRate(), shadowColor, quad.at(1));
            std::shared_ptr<GeometricPrimitive> prim2 = std::make_shared<GeometricPrimitive>(quad.at(1), defaultMaterial, areaIllum2);
            newPrims.emplace_back(prim2);
            illums.emplace_back(areaIllum2);
        }
    }

    illuminantExporterResponse.illuminants = illums;
    illuminantExporterResponse.prims = newPrims;

    return illuminantExporterResponse;
}
