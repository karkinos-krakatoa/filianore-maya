#ifndef _PLASTIC_MAYA_SHADER_NODE_H
#define _PLASTIC_MAYA_SHADER_NODE_H

#include <maya/MPxNode.h>

class PlasticShaderNode : public MPxNode
{
public:
    static const MString name;
    static const MTypeId id;

    static void *creator();
    static MStatus initialize();

    MStatus compute(const MPlug &plug, MDataBlock &block) override;
    SchedulingType schedulingType() const override { return MPxNode::kParallel; }

    static MObject nameData;
    static MObject nameAttr;

    static MObject aColorR;
    static MObject aColorG;
    static MObject aColorB;
    static MObject aColor;

    static MObject aOutColorR;
    static MObject aOutColorG;
    static MObject aOutColorB;
    static MObject aOutColor;

    static MObject aOutTransparencyR;
    static MObject aOutTransparencyG;
    static MObject aOutTransparencyB;
    static MObject aOutTransparency;

    static MObject aNormalCamera;
    static MObject aNormalCameraX;
    static MObject aNormalCameraY;
    static MObject aNormalCameraZ;
    static MObject aLightData;
    static MObject aLightDirection;
    static MObject aLightDirectionX;
    static MObject aLightDirectionY;
    static MObject aLightDirectionZ;
    static MObject aLightIntensity;
    static MObject aLightIntensityR;
    static MObject aLightIntensityG;
    static MObject aLightIntensityB;
    static MObject aLightAmbient;
    static MObject aLightDiffuse;
    static MObject aLightSpecular;
    static MObject aLightShadowFraction;
    static MObject aPreShadowIntensity;
    static MObject aLightBlindData;
};

#endif