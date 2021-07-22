#ifndef _FL_PLASTIC_SHADER_H
#define _FL_PLASTIC_SHADER_H

#include <maya/MPxNode.h>

class FlPlasticShader : public MPxNode
{
public:
    FlPlasticShader();
    ~FlPlasticShader() override;

    static const MString name;
    static const MTypeId id;

    virtual MStatus compute(const MPlug &plug, MDataBlock &block) override;

    static void *creator();
    static MStatus initialize();

    SchedulingType schedulingType() const override { return MPxNode::kParallel; }

    static MObject nameData;
    static MObject nameAttr;

    // Diffuse
    static MObject diffuseBaseColor;
    static MObject diffuseBaseWeight;

    // Specular
    static MObject specularColor;
    static MObject specularWeight;
    static MObject specularRoughness;
    static MObject specularAnisotropic;
    static MObject specularIOR;

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