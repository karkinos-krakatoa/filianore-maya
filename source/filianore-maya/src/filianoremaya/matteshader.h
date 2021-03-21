#ifndef _MATTE_SHADER
#define _MATTE_SHADER

#include <maya/MPxSurfaceShadingNodeOverride.h>

class MatteShader : public MHWRender::MPxSurfaceShadingNodeOverride
{
public:
    static MHWRender::MPxSurfaceShadingNodeOverride *creator(const MObject &obj);

    ~MatteShader() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MString fragmentName() const override;
    void getCustomMappings(MHWRender::MAttributeParameterMappingList &mappings) override;

    MString primaryColorParameter() const override;
    MString bumpAttribute() const override;

private:
    MatteShader(const MObject &obj);
};

#endif