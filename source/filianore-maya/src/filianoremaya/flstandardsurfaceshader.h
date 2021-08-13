#ifndef _FL_STANDARD_SURFACE_SHADER_H
#define _FL_STANDARD_SURFACE_SHADER_H

#include <maya/MPxNode.h>

class FlStandardSurfaceShader : public MPxNode
{
public:
    FlStandardSurfaceShader();
    ~FlStandardSurfaceShader() override;

    static const MString name;
    static const MTypeId id;

    virtual MStatus compute(const MPlug &plug, MDataBlock &block) override;

    static void *creator();
    static MStatus initialize();

    SchedulingType schedulingType() const override { return MPxNode::kParallel; }

    static MObject nameData;
    static MObject nameAttr;

    // Foundation
    static MObject diffuseBaseColor;
    static MObject diffuseBaseWeight;
    static MObject diffuseBaseRoughness;

    // Metallurgy
    static MObject metallicPreset;
    static MObject metallicWeight;

    // Specular
    static MObject specularColor;
    static MObject specularWeight;
    static MObject specularRoughness;
    static MObject specularAnisotropic;
    static MObject specularIOR;

    // Sheen
    static MObject sheenColor;
    static MObject sheenWeight;
    static MObject sheenRoughness;

    // Transmission
    static MObject transmissionColor;
    static MObject transmissionWeight;

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