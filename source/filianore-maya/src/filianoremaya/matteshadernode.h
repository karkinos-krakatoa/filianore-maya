#ifndef _MATTE_SHADER_NODE
#define _MATTE_SHADER_NODE

#include <maya/MPxNode.h>

class MatteShaderNode : public MPxNode
{
public:
    MatteShaderNode();
    ~MatteShaderNode() override;

    static const MString name;
    static const MTypeId id;

    virtual MStatus compute(const MPlug &plug, MDataBlock &block) override;

    static void *creator();
    static MStatus initialize();

    SchedulingType schedulingType() const override { return MPxNode::kParallel; }

    static MObject aColorR;
    static MObject aColorG;
    static MObject aColorB;
    static MObject aColor;
    static MObject aWeight;
    static MObject aRoughness;

    static MObject aOutColorR;
    static MObject aOutColorG;
    static MObject aOutColorB;
    static MObject aOutColor;

    static MObject aNormalCameraX;
    static MObject aNormalCameraY;
    static MObject aNormalCameraZ;
    static MObject aNormalCamera;

    static MObject aLightDirectionX;
    static MObject aLightDirectionY;
    static MObject aLightDirectionZ;
    static MObject aLightDirection;
    static MObject aLightIntensityR;
    static MObject aLightIntensityG;
    static MObject aLightIntensityB;
    static MObject aLightIntensity;
    static MObject aLightAmbient;
    static MObject aLightDiffuse;
    static MObject aLightSpecular;
    static MObject aLightShadowFraction;
    static MObject aPreShadowIntensity;
    static MObject aLightBlindData;
    static MObject aLightData;
};

#endif