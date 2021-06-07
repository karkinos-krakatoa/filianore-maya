#include "plasticshader.h"

MHWRender::MPxSurfaceShadingNodeOverride *PlasticShader::creator(const MObject &obj)
{
    return (new PlasticShader(obj));
}

PlasticShader::PlasticShader(const MObject &obj)
    : MPxSurfaceShadingNodeOverride(obj)
{
}

PlasticShader::~PlasticShader()
{
}

MHWRender::DrawAPI PlasticShader::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

MString PlasticShader::fragmentName() const
{
    return ("mayaPhongSurface");
}

void PlasticShader::getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings)
{
    MHWRender::MAttributeParameterMapping diffuseMapping("color", "outColor", true, true);
    mappings.append(diffuseMapping);
}

MString PlasticShader::primaryColorParameter() const
{
    return ("outColor");
}

MString PlasticShader::bumpAttribute() const
{
    return ("normalCamera");
}