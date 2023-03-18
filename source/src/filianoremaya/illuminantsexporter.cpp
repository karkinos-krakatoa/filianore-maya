#include <memory>

#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnAreaLight.h>
#include <maya/MFnDirectionalLight.h>
#include <maya/MFnLight.h>
#include <maya/MFnPointLight.h>
#include <maya/MFnSpotLight.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDag.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>

#include "dagUtils.h"
#include "illuminantsexporter.h"
#include "util.h"

#include "filianore/illuminants/diffusearea.h"
#include "filianore/illuminants/directional.h"
#include "filianore/illuminants/point.h"
#include "filianore/illuminants/spot.h"

#include "filianore/core/primitive.h"
#include "filianore/creators/shapecreator.h"
#include "filianore/shapes/triangle.h"

#include "filianore/materials/lambert.h"
#include "filianore/textures/constant.h"

#include "filianore/color/spectruminits.h"
#include "filianore/color/spectrumoperations.h"

IlluminantExporterResponse IlluminantExporter::ExportIlluminants(const std::vector<std::shared_ptr<filianore::Primitive>> &prims) {
    IlluminantExporterResponse illuminantExporterResponse;

    std::vector<std::shared_ptr<filianore::Primitive>> newPrims;
    std::vector<std::shared_ptr<filianore::Illuminant>> illums;

    filianore::PrincipalSpectrum col(1.f);
    std::shared_ptr<filianore::Texture<filianore::PrincipalSpectrum>> tex = std::make_shared<filianore::ConstantTexture<filianore::PrincipalSpectrum>>(col);
    std::shared_ptr<filianore::Material> defaultMaterial = std::make_shared<filianore::LambertMaterial>(tex);

    MStatus status;
    MItDag dagIt(MItDag::kBreadthFirst, MFn::kLight, &status);
    for (; !dagIt.isDone(); dagIt.next()) {
        MDagPath dagPath;
        status = dagIt.getPath(dagPath);
        if (!status) {
            status.perror("MItDag::getPath");
            continue;
        }

        TransformVectors transformData = GetDagObjectTransformData(dagPath);
        filianore::Transform transform = filianore::translate(transformData.Translate.x, transformData.Translate.y, transformData.Translate.z);

        if (dagPath.hasFn(MFn::kPointLight)) {
            MFnPointLight pointIllum(dagPath, &status);
            if (!status) {
                status.perror("MFnPointLight constructor");
                continue;
            }

            filianore::PrincipalSpectrum color = filianore::from_illuminance_rgb(filianore::Vector3f(pointIllum.color().r, pointIllum.color().g, pointIllum.color().b));
            filianore::PrincipalSpectrum shadowColor = filianore::from_reflectance_rgb(filianore::Vector3f(pointIllum.shadowColor().r, pointIllum.shadowColor().g, pointIllum.shadowColor().b));
            float intensity = pointIllum.intensity();

            std::shared_ptr<filianore::Illuminant> actualIllum = std::make_shared<filianore::PointIlluminant>(transform, color, intensity, pointIllum.decayRate(), shadowColor);
            illums.emplace_back(actualIllum);
        }

        if (dagPath.hasFn(MFn::kSpotLight)) {
            MFnSpotLight spotLight(dagPath, &status);
            if (!status) {
                status.perror("MFnSpotLight constructor");
                continue;
            }

            filianore::PrincipalSpectrum color = filianore::from_illuminance_rgb(filianore::Vector3f(spotLight.color().r, spotLight.color().g, spotLight.color().b));
            filianore::PrincipalSpectrum shadowColor = filianore::from_reflectance_rgb(filianore::Vector3f(spotLight.shadowColor().r, spotLight.shadowColor().g, spotLight.shadowColor().b));
            float intensity = spotLight.intensity();
            MFloatVector direction = spotLight.lightDirection(0, MSpace::kWorld);

            std::shared_ptr<filianore::Illuminant> actualIllum = std::make_shared<filianore::SpotIlluminant>(transform, filianore::Vector3f(direction.x, direction.y, direction.z),
                                                                                                             (float)spotLight.coneAngle(), (float)spotLight.penumbraAngle(),
                                                                                                             true, true,
                                                                                                             color, intensity, spotLight.decayRate(), shadowColor);
            illums.emplace_back(actualIllum);
        }

        if (dagPath.hasFn(MFn::kDirectionalLight)) {
            MFnDirectionalLight dirLight(dagPath, &status);
            if (!status) {
                status.perror("MFnDirectionalLight constructor");
                continue;
            }

            filianore::PrincipalSpectrum color = filianore::from_illuminance_rgb(filianore::Vector3f(dirLight.color().r, dirLight.color().g, dirLight.color().b));
            filianore::PrincipalSpectrum shadowColor = filianore::from_reflectance_rgb(filianore::Vector3f(dirLight.shadowColor().r, dirLight.shadowColor().g, dirLight.shadowColor().b));
            float intensity = dirLight.intensity();
            MFloatVector direction = dirLight.lightDirection(0, MSpace::kWorld);

            std::shared_ptr<filianore::Illuminant> actualIllum = std::make_shared<filianore::DirectionalIlluminant>(transform, filianore::Vector3f(direction.x, direction.y, direction.z),
                                                                                                                    color, intensity, shadowColor);
            illums.emplace_back(actualIllum);
        }

        if (dagPath.hasFn(MFn::kAreaLight)) {
            MFnAreaLight areaLight(dagPath, &status);
            if (!status) {
                status.perror("MFnAreaLight constructor");
                continue;
            }

            filianore::Transform _transform = filianore::translate(transformData.Translate.x, transformData.Translate.y, transformData.Translate.z) *
                                              (filianore::rotate_z(transformData.Rotate.z, true) * filianore::rotate_y(transformData.Rotate.y, true) * filianore::rotate_x(transformData.Rotate.x, true)) *
                                              filianore::scale(transformData.Scale.x, transformData.Scale.y, transformData.Scale.z);

            filianore::ShapeCreator shapeCreator;
            std::vector<std::shared_ptr<filianore::Shape>> quad = shapeCreator.create_quad(_transform);

            filianore::PrincipalSpectrum color = filianore::from_illuminance_rgb(filianore::Vector3f(areaLight.color().r, areaLight.color().g, areaLight.color().b));
            filianore::PrincipalSpectrum shadowColor = filianore::from_reflectance_rgb(filianore::Vector3f(areaLight.shadowColor().r, areaLight.shadowColor().g, areaLight.shadowColor().b));

            float intensity = areaLight.intensity();

            std::shared_ptr<filianore::AreaIlluminant> areaIllum1 = std::make_shared<filianore::DiffuseAreaIlluminant>(_transform, color, intensity, areaLight.decayRate(), shadowColor, quad.at(0));
            std::shared_ptr<filianore::GeometricPrimitive> prim1 = std::make_shared<filianore::GeometricPrimitive>(quad.at(0), defaultMaterial, areaIllum1);
            newPrims.emplace_back(prim1);
            illums.emplace_back(areaIllum1);

            std::shared_ptr<filianore::AreaIlluminant> areaIllum2 = std::make_shared<filianore::DiffuseAreaIlluminant>(_transform, color, intensity, areaLight.decayRate(), shadowColor, quad.at(1));
            std::shared_ptr<filianore::GeometricPrimitive> prim2 = std::make_shared<filianore::GeometricPrimitive>(quad.at(1), defaultMaterial, areaIllum2);
            newPrims.emplace_back(prim2);
            illums.emplace_back(areaIllum2);
        }
    }

    illuminantExporterResponse.illuminants = illums;
    illuminantExporterResponse.prims = newPrims;

    return illuminantExporterResponse;
}
