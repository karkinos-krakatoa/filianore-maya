#include "matteshader.h"

MHWRender::MPxSurfaceShadingNodeOverride *MatteShader::creator(const MObject &obj)
{
    return (new MatteShader(obj));
}

MatteShader::MatteShader(const MObject &obj)
    : MPxSurfaceShadingNodeOverride(obj)
{
}

MatteShader::~MatteShader()
{
}

MHWRender::DrawAPI MatteShader::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

MString MatteShader::fragmentName() const
{
    return ("mayaLambertSurface");
}

void MatteShader::getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings)
{
    MHWRender::MAttributeParameterMapping diffuseMapping(
        "color", "outColor", true, true);
    mappings.append(diffuseMapping);
}

MString MatteShader::primaryColorParameter() const
{
    return ("outColor");
}

MString MatteShader::bumpAttribute() const
{
    return ("normalCamera");
}