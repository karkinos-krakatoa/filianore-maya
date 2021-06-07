#ifndef _MIRROR_SHADER_H
#define _MIRROR_SHADER_H

#include <maya/MPxSurfaceShadingNodeOverride.h>

class PlasticShader : public MHWRender::MPxSurfaceShadingNodeOverride
{
public:
    static MHWRender::MPxSurfaceShadingNodeOverride *creator(const MObject &obj);

    ~PlasticShader() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MString fragmentName() const override;
    void getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings) override;

    MString primaryColorParameter() const override;
    MString bumpAttribute() const override;

private:
    PlasticShader(const MObject &obj);
};

#endif