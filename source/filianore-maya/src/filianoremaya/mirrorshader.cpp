#include "mirrorshader.h"

MHWRender::MPxSurfaceShadingNodeOverride *MirrorShader::creator(const MObject &obj)
{
    return (new MirrorShader(obj));
}

MirrorShader::MirrorShader(const MObject &obj)
    : MPxSurfaceShadingNodeOverride(obj)
{
}

MirrorShader::~MirrorShader()
{
}

MHWRender::DrawAPI MirrorShader::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

MString MirrorShader::fragmentName() const
{
    return ("mayaPhongSurface");
}

void MirrorShader::getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings)
{
    MHWRender::MAttributeParameterMapping diffuseMapping(
        "color", "outColor", true, true);
    mappings.append(diffuseMapping);
}

MString MirrorShader::primaryColorParameter() const
{
    return ("outColor");
}

MString MirrorShader::bumpAttribute() const
{
    return ("normalCamera");
}