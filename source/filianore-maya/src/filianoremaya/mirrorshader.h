#ifndef _MIRROR_SHADER
#define _MIRROR_SHADER

#include <maya/MPxSurfaceShadingNodeOverride.h>

class MirrorShader : public MHWRender::MPxSurfaceShadingNodeOverride
{
public:
    static MHWRender::MPxSurfaceShadingNodeOverride *creator(const MObject &obj);

    ~MirrorShader() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MString fragmentName() const override;
    void getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings) override;

    MString primaryColorParameter() const override;
    MString bumpAttribute() const override;

private:
    MirrorShader(const MObject &obj);
};

#endif