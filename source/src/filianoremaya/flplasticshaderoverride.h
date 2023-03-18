#ifndef _FL_PLASTIC_SHADER_OVERRIDE_H
#define _FL_PLASTIC_SHADER_OVERRIDE_H

#include <maya/MPxSurfaceShadingNodeOverride.h>

class FlPlasticShaderOverride : public MHWRender::MPxSurfaceShadingNodeOverride
{
public:
    static MHWRender::MPxSurfaceShadingNodeOverride *creator(const MObject &obj);

    ~FlPlasticShaderOverride() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MString fragmentName() const override;
    void getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings) override;

    MString primaryColorParameter() const override;
    MString bumpAttribute() const override;

private:
    FlPlasticShaderOverride(const MObject &obj);
};

#endif