#include "flplasticshaderoverride.h"

MHWRender::MPxSurfaceShadingNodeOverride *FlPlasticShaderOverride::creator(const MObject &obj)
{
    return new FlPlasticShaderOverride(obj);
}

FlPlasticShaderOverride::FlPlasticShaderOverride(const MObject &obj)
    : MPxSurfaceShadingNodeOverride(obj)
{
}

FlPlasticShaderOverride::~FlPlasticShaderOverride()
{
}

MHWRender::DrawAPI FlPlasticShaderOverride::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

MString FlPlasticShaderOverride::fragmentName() const
{
    return "mayaPhongSurface";
}

void FlPlasticShaderOverride::getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings)
{
    MHWRender::MAttributeParameterMapping diffuseMapping("color", "outColor", true, true);
    mappings.append(diffuseMapping);
}

MString FlPlasticShaderOverride::primaryColorParameter() const
{
    return ("outColor");
}

MString FlPlasticShaderOverride::bumpAttribute() const
{
    return ("normalCamera");
}