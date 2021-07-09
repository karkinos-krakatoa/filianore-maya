#include "flstandardsurfaceshaderoverride.h"

MHWRender::MPxSurfaceShadingNodeOverride *FlStandardSurfaceShaderOverride::creator(const MObject &obj)
{
    return new FlStandardSurfaceShaderOverride(obj);
}

FlStandardSurfaceShaderOverride::FlStandardSurfaceShaderOverride(const MObject &obj)
    : MPxSurfaceShadingNodeOverride(obj)
{
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
    return "mayaLambertSurface";
}

void FlStandardSurfaceShaderOverride::getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings)
{
    MHWRender::MAttributeParameterMapping diffuseMapping("color", "outColor", true, true);
    mappings.append(diffuseMapping);
}

MString FlStandardSurfaceShaderOverride::primaryColorParameter() const
{
    return ("outColor");
}

MString FlStandardSurfaceShaderOverride::bumpAttribute() const
{
    return ("normalCamera");
}