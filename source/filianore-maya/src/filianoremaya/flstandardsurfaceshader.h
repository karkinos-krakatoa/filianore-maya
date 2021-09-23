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

    // Specular
    static MObject specularColor;
    static MObject specularWeight;
    static MObject specularRoughness;
    static MObject specularAnisotropic;
    static MObject specularType;
    static MObject metallicPreset;
    static MObject specularIOR;

    // Sheen
    static MObject sheenColor;
    static MObject sheenWeight;
    static MObject sheenRoughness;

    // Transmission
    static MObject transmissionColor;
    static MObject transmissionWeight;

    // Clearcoat
    static MObject clearcoatColor;
    static MObject clearcoatWeight;
    static MObject clearcoatGloss;
    static MObject clearcoatIOR;

    // Thin Film
    static MObject thinFilmThickness;
    static MObject thinFilmIOR;

    // Outputs
    static MObject aOutColor;

    // camera
    static MObject aPointCamera;
    static MObject aNormalCamera;

    // Light Params
    static MObject aLightDirection;
    static MObject aLightIntensity;
    static MObject aLightAmbient;
    static MObject aLightDiffuse;
    static MObject aLightSpecular;
    static MObject aLightShadowFraction;
    static MObject aPreShadowIntensity;
    static MObject aLightBlindData;
    static MObject aLightData;

    static MObject aRayOrigin;
    static MObject aRayDirection;

    static MObject aObjectId;
    static MObject aRaySampler;
    static MObject aRayDepth;
    static MObject aTriangleNormalCamera;
};

#endif