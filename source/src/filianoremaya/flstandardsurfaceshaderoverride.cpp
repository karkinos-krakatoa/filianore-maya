#include "flstandardsurfaceshaderoverride.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MShaderManager.h>

MHWRender::MPxSurfaceShadingNodeOverride *FlStandardSurfaceShaderOverride::creator(const MObject &obj)
{
    return new FlStandardSurfaceShaderOverride(obj);
}

FlStandardSurfaceShaderOverride::FlStandardSurfaceShaderOverride(const MObject &obj)
    : MPxSurfaceShadingNodeOverride(obj), fObject(obj), fResolvedSpecularColorName("")
{
    fSpecularColor[0] = 0.5f;
    fSpecularColor[1] = 0.5f;
    fSpecularColor[2] = 0.5f;
}

FlStandardSurfaceShaderOverride::~FlStandardSurfaceShaderOverride()
{
}

MHWRender::DrawAPI FlStandardSurfaceShaderOverride::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

MString FlStandardSurfaceShaderOverride::fragmentName() const
{
    fResolvedSpecularColorName = "";
    return "mayaPhongSurface";
}

void FlStandardSurfaceShaderOverride::getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings)
{
    MHWRender::MAttributeParameterMapping diffuseMapping(
        "diffuse", "diffuseBaseWeight", true, true);
    mappings.append(diffuseMapping);

    MHWRender::MAttributeParameterMapping translucenceMapping(
        "translucence", "transmissionWeight", true, true);
    mappings.append(translucenceMapping);

    MHWRender::MAttributeParameterMapping reflectivityMapping(
        "reflectivity", "specularWeight", true, true);
    mappings.append(reflectivityMapping);

    MHWRender::MAttributeParameterMapping specColMapping(
        "specularColor", "", true, true);
    mappings.append(specColMapping);
}

MString FlStandardSurfaceShaderOverride::primaryColorParameter() const
{
    return "color";
}

MString FlStandardSurfaceShaderOverride::bumpAttribute() const
{
    return "normalCamera";
}

void FlStandardSurfaceShaderOverride::updateDG()
{
    MStatus status;
    MFnDependencyNode node(fObject, &status);
    if (status)
    {
        float specularity = 0.5f;
        node.findPlug("specularWeight", true).getValue(specularity);

        // Expand specularity to a 3-float
        fSpecularColor[0] = specularity;
        fSpecularColor[1] = specularity;
        fSpecularColor[2] = specularity;
    }
}

void FlStandardSurfaceShaderOverride::updateShader(
    MHWRender::MShaderInstance &shader,
    const MHWRender::MAttributeParameterMappingList &mappings)
{
    // Cache resolved name if found to avoid lookup on every update
    if (fResolvedSpecularColorName.length() == 0)
    {
        const MHWRender::MAttributeParameterMapping *mapping =
            mappings.findByParameterName("specularColor");
        if (mapping)
        {
            fResolvedSpecularColorName = mapping->resolvedParameterName();
        }
    }

    // Set parameter
    if (fResolvedSpecularColorName.length() > 0)
    {
        shader.setParameter(fResolvedSpecularColorName, fSpecularColor);
    }
}