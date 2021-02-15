#ifndef _LAMBERT_SHADER
#define _LAMBERT_SHADER

#include <maya/MPxSurfaceShadingNodeOverride.h>

class LambertShader : public MHWRender::MPxSurfaceShadingNodeOverride
{
public:
    static MHWRender::MPxSurfaceShadingNodeOverride *creator(const MObject &obj);

    ~LambertShader() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MString fragmentName() const override;
    void getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings) override;

    MString primaryColorParameter() const override;
    MString bumpAttribute() const override;

private:
    LambertShader(const MObject &obj);
};

#endif