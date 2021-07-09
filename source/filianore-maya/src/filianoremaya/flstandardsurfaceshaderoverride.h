#ifndef _FL_STANDARD_SURFACE_SHADER_OVERRIDE_H
#define _FL_STANDARD_SURFACE_SHADER_OVERRIDE_H

#include <maya/MPxSurfaceShadingNodeOverride.h>

class FlStandardSurfaceShaderOverride : public MHWRender::MPxSurfaceShadingNodeOverride
{
public:
    static MHWRender::MPxSurfaceShadingNodeOverride *creator(const MObject &obj);

    ~FlStandardSurfaceShaderOverride() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MString fragmentName() const override;
    void getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings) override;

    MString primaryColorParameter() const override;
    MString bumpAttribute() const override;

private:
    FlStandardSurfaceShaderOverride(const MObject &obj);
};

#endif