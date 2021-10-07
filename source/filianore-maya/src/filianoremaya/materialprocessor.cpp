#include "materialprocessor.h"

#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>

#include "util.h"
#include "materialprocessorutil.h"

#include "flstandardsurfaceshader.h"
#include "flplasticshader.h"

#include "filianore/textures/constant.h"
#include "filianore/textures/imagemap.h"

#include "filianore/materials/lambert.h"
#include "filianore/materials/standardsurface.h"

#include "filianore/color/spectrumoperations.h"
#include "filianore/color/spectruminits.h"

std::shared_ptr<Material> ProcessMeshMaterials(MFnMesh &mMesh)
{
    PrincipalSpectrum col = FromReflectanceRGB(StaticArray<float, 3>(0.5f));
    std::shared_ptr<Texture<PrincipalSpectrum>> tex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(col);
    std::shared_ptr<Material> defaultMaterial = std::make_shared<LambertMaterial>(tex);

    MObjectArray mShaderArray;
    MIntArray mShaderIndices;
    mMesh.getConnectedShaders(0, mShaderArray, mShaderIndices);

    if (mShaderArray.length() < 1)
    {
        return defaultMaterial;
    }

    MFnDependencyNode mShaderWrapperNode(mShaderArray[0]);
    MString mShaderName = mShaderWrapperNode.name().substring(0, mShaderWrapperNode.name().length() - 3);

    MObject mShaderObject;
    if (GetDependencyNodeByName(mShaderName, mShaderObject) != MS::kSuccess)
    {
        return defaultMaterial;
    }

    std::string materialName = mShaderName.asChar();
    if (materialName.find("flStandardSurface") != std::string::npos)
    {
        // Foundation
        MPlug mDiffuseColorPlug(mShaderObject, FlStandardSurfaceShader::diffuseBaseColor);
        std::shared_ptr<Texture<PrincipalSpectrum>> diffuseColorTex = EvaluateColorFromPlugNetwork(mDiffuseColorPlug);

        MPlug mDiffuseWeightPlug(mShaderObject, FlStandardSurfaceShader::diffuseBaseWeight);
        float mDiffuseWeight = mDiffuseWeightPlug.asMDataHandle().asFloat();

        MPlug mDiffuseRoughnessPlug(mShaderObject, FlStandardSurfaceShader::diffuseBaseRoughness);
        float mDiffuseRoughness = mDiffuseRoughnessPlug.asMDataHandle().asFloat();
        std::shared_ptr<Texture<float>> diffuseRoughnessTex = std::make_shared<ConstantTexture<float>>(mDiffuseRoughness);

        // Specular
        MPlug mSpecColorPlug(mShaderObject, FlStandardSurfaceShader::specularColor);
        std::shared_ptr<Texture<PrincipalSpectrum>> specColorTex = EvaluateColorFromPlugNetwork(mSpecColorPlug);

        MPlug mSpecWeightPlug(mShaderObject, FlStandardSurfaceShader::specularWeight);
        float mSpecWeight = mSpecWeightPlug.asMDataHandle().asFloat();

        MPlug mSpecRoughnessPlug(mShaderObject, FlStandardSurfaceShader::specularRoughness);
        float mSpecRoughness = mSpecRoughnessPlug.asMDataHandle().asFloat();
        std::shared_ptr<Texture<float>> specRoughnessTex = std::make_shared<ConstantTexture<float>>(mSpecRoughness);

        MPlug mSpecAnisotropicPlug(mShaderObject, FlStandardSurfaceShader::specularAnisotropic);
        float mSpecAnisotropic = mSpecAnisotropicPlug.asMDataHandle().asFloat();
        std::shared_ptr<Texture<float>> specAnisotropicTex = std::make_shared<ConstantTexture<float>>(mSpecAnisotropic);

        MPlug mSpecTypePlug(mShaderObject, FlStandardSurfaceShader::specularType);
        int mSpecType = mSpecTypePlug.asMDataHandle().asInt();

        MPlug mSpecIORPlug(mShaderObject, FlStandardSurfaceShader::specularIOR);
        float mSpecIOR = mSpecIORPlug.asMDataHandle().asFloat();

        MPlug mMetallicPresetTypePlug(mShaderObject, FlStandardSurfaceShader::metallicPreset);
        int mMetallicPreset = mMetallicPresetTypePlug.asMDataHandle().asInt();

        // Sheen
        MPlug mSheenColorPlug(mShaderObject, FlStandardSurfaceShader::sheenColor);
        std::shared_ptr<Texture<PrincipalSpectrum>> sheenColorTex = EvaluateColorFromPlugNetwork(mSheenColorPlug);

        MPlug mSheenWeightPlug(mShaderObject, FlStandardSurfaceShader::sheenWeight);
        float mSheenWeight = mSheenWeightPlug.asMDataHandle().asFloat();

        MPlug mSheenRoughnessPlug(mShaderObject, FlStandardSurfaceShader::sheenRoughness);
        float mSheenRoughness = mSheenRoughnessPlug.asMDataHandle().asFloat();
        std::shared_ptr<Texture<float>> sheenRoughnessTex = std::make_shared<ConstantTexture<float>>(mSheenRoughness);

        // Transmission
        MPlug mTransmissionColorPlug(mShaderObject, FlStandardSurfaceShader::transmissionColor);
        std::shared_ptr<Texture<PrincipalSpectrum>> transmissionColorTex = EvaluateColorFromPlugNetwork(mTransmissionColorPlug);

        MPlug mTransmissionWeightPlug(mShaderObject, FlStandardSurfaceShader::transmissionWeight);
        float mTransmissionWeight = mTransmissionWeightPlug.asMDataHandle().asFloat();

        // Clearcoat
        MPlug mClearcoatColorPlug(mShaderObject, FlStandardSurfaceShader::clearcoatColor);
        std::shared_ptr<Texture<PrincipalSpectrum>> clearcoatColorTex = EvaluateColorFromPlugNetwork(mClearcoatColorPlug);

        MPlug mClearcoatWeightPlug(mShaderObject, FlStandardSurfaceShader::clearcoatWeight);
        float mClearcoatWeight = mClearcoatWeightPlug.asMDataHandle().asFloat();

        MPlug mClearcoatGlossPlug(mShaderObject, FlStandardSurfaceShader::clearcoatGloss);
        float mClearcoatGloss = mClearcoatGlossPlug.asMDataHandle().asFloat();

        MPlug mClearcoatIORPlug(mShaderObject, FlStandardSurfaceShader::clearcoatIOR);
        float mClearcoatIOR = mClearcoatIORPlug.asMDataHandle().asFloat();

        // Thin Film
        MPlug mThinFilmThicknessPlug(mShaderObject, FlStandardSurfaceShader::thinFilmThickness);
        float mThinFilmThickness = mThinFilmThicknessPlug.asMDataHandle().asFloat();

        MPlug mThinFilmIORPlug(mShaderObject, FlStandardSurfaceShader::thinFilmIOR);
        float mThinFilmIOR = mThinFilmIORPlug.asMDataHandle().asFloat();

        // Bump Map
        std::shared_ptr<Texture<float>> bumpMap = EvaluateBumpShadingNetwork(mShaderObject);

        // Actual Material
        std::shared_ptr<Material>
            standardSurfaceMaterial = std::make_shared<StandardSurfaceMaterial>(
                mDiffuseWeight, diffuseColorTex, diffuseRoughnessTex,
                mSpecWeight, specColorTex, specRoughnessTex, specAnisotropicTex, mSpecType, mSpecIOR, mMetallicPreset,
                mSheenWeight, sheenColorTex, sheenRoughnessTex,
                mTransmissionWeight, transmissionColorTex,
                mClearcoatWeight, clearcoatColorTex, mClearcoatIOR, mClearcoatGloss,
                mThinFilmThickness, mThinFilmIOR,
                bumpMap);

        return standardSurfaceMaterial;
    }

    return defaultMaterial;
}
