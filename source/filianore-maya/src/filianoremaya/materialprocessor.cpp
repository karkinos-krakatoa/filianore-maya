#include "materialprocessor.h"

#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>

#include "util.h"

#include "flstandardsurfaceshader.h"
#include "flplasticshader.h"

#include "filianore/textures/constant.h"
#include "filianore/textures/imagemap.h"

#include "filianore/materials/lambert.h"
#include "filianore/materials/standardsurface.h"

#include "filianore/color/spectrumoperations.h"
#include "filianore/color/spectruminits.h"

std::shared_ptr<Texture<PrincipalSpectrum>> GetColorNode(MPlug &mShaderObjectPlug)
{
    MStatus status;

    // Default Debug Texture
    PrincipalSpectrum defaultColor = FromReflectanceRGB(StaticArray<float, 3>(0.f, 1.f, 1.f));
    auto defaultTexture = std::make_shared<ConstantTexture<PrincipalSpectrum>>(defaultColor);

    MPlugArray connectedPlugs;
    mShaderObjectPlug.connectedTo(connectedPlugs, true, false);
    if (connectedPlugs.length() > 0)
    {
        MFnDependencyNode fnDN(connectedPlugs[0].node());

        MItDependencyGraph dgIt(mShaderObjectPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream,
                                MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &status);
        if (status == MS::kFailure)
        {
            FILIANORE_MAYA_LOG_INFO("Can't load graph textures");
            return defaultTexture;
        }

        dgIt.disablePruningOnFilter();
        if (dgIt.isDone())
        {
            return defaultTexture;
        }

        MObject textureNode = dgIt.thisNode();
        MPlug filenamePlug = MFnDependencyNode(textureNode).findPlug("fileTextureName", &status);
        MString textureName; // name texture + path
        filenamePlug.getValue(textureName);

        return std::make_shared<ImageMapTexture>(textureName.asChar());
    }
    else
    {
        // Default Color Node
        MFloatVector mShaderColor = mShaderObjectPlug.asMDataHandle().asFloatVector();
        PrincipalSpectrum shaderColor = FromReflectanceRGB(StaticArray<float, 3>(mShaderColor.x, mShaderColor.y, mShaderColor.z));
        return std::make_shared<ConstantTexture<PrincipalSpectrum>>(shaderColor);
    }

    return defaultTexture;
}

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

        MPlug mDiffuseWeightPlug(mShaderObject, FlStandardSurfaceShader::diffuseBaseWeight);
        float mDiffuseWeight = mDiffuseWeightPlug.asMDataHandle().asFloat();

        MPlug mDiffuseRoughnessPlug(mShaderObject, FlStandardSurfaceShader::diffuseBaseRoughness);
        float mDiffuseRoughness = mDiffuseRoughnessPlug.asMDataHandle().asFloat();

        std::shared_ptr<Texture<PrincipalSpectrum>> diffuseColorTex = GetColorNode(mDiffuseColorPlug);
        std::shared_ptr<Texture<float>> diffuseRoughnessTex = std::make_shared<ConstantTexture<float>>(mDiffuseRoughness);

        // Specular
        MPlug mSpecColorPlug(mShaderObject, FlStandardSurfaceShader::specularColor);
        MFloatVector mSpecColor = mSpecColorPlug.asMDataHandle().asFloatVector();

        MPlug mSpecWeightPlug(mShaderObject, FlStandardSurfaceShader::specularWeight);
        float mSpecWeight = mSpecWeightPlug.asMDataHandle().asFloat();

        MPlug mSpecRoughnessPlug(mShaderObject, FlStandardSurfaceShader::specularRoughness);
        float mSpecRoughness = mSpecRoughnessPlug.asMDataHandle().asFloat();

        MPlug mSpecAnisotropicPlug(mShaderObject, FlStandardSurfaceShader::specularAnisotropic);
        float mSpecAnisotropic = mSpecAnisotropicPlug.asMDataHandle().asFloat();

        MPlug mSpecTypePlug(mShaderObject, FlStandardSurfaceShader::specularType);
        int mSpecType = mSpecTypePlug.asMDataHandle().asInt();

        MPlug mSpecIORPlug(mShaderObject, FlStandardSurfaceShader::specularIOR);
        float mSpecIOR = mSpecIORPlug.asMDataHandle().asFloat();

        MPlug mMetallicPresetTypePlug(mShaderObject, FlStandardSurfaceShader::metallicPreset);
        int mMetallicPreset = mMetallicPresetTypePlug.asMDataHandle().asInt();

        PrincipalSpectrum specColor = FromReflectanceRGB(StaticArray<float, 3>(mSpecColor.x, mSpecColor.y, mSpecColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> specColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(specColor);
        std::shared_ptr<Texture<float>> specRoughnessTex = std::make_shared<ConstantTexture<float>>(mSpecRoughness);
        std::shared_ptr<Texture<float>> specAnisotropicTex = std::make_shared<ConstantTexture<float>>(mSpecAnisotropic);

        // Sheen
        MPlug mSheenColorPlug(mShaderObject, FlStandardSurfaceShader::sheenColor);
        MFloatVector mSheenColor = mSheenColorPlug.asMDataHandle().asFloatVector();

        MPlug mSheenWeightPlug(mShaderObject, FlStandardSurfaceShader::sheenWeight);
        float mSheenWeight = mSheenWeightPlug.asMDataHandle().asFloat();

        MPlug mSheenRoughnessPlug(mShaderObject, FlStandardSurfaceShader::sheenRoughness);
        float mSheenRoughness = mSheenRoughnessPlug.asMDataHandle().asFloat();

        PrincipalSpectrum sheenColor = FromReflectanceRGB(StaticArray<float, 3>(mSheenColor.x, mSheenColor.y, mSheenColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> sheenColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(sheenColor);
        std::shared_ptr<Texture<float>> sheenRoughnessTex = std::make_shared<ConstantTexture<float>>(mSheenRoughness);

        // Transmission
        MPlug mTransmissionColorPlug(mShaderObject, FlStandardSurfaceShader::transmissionColor);
        MFloatVector mTransmissionColor = mTransmissionColorPlug.asMDataHandle().asFloatVector();

        MPlug mTransmissionWeightPlug(mShaderObject, FlStandardSurfaceShader::transmissionWeight);
        float mTransmissionWeight = mTransmissionWeightPlug.asMDataHandle().asFloat();

        PrincipalSpectrum transmissionColor = FromReflectanceRGB(StaticArray<float, 3>(mTransmissionColor.x, mTransmissionColor.y, mTransmissionColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> transmissionColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(transmissionColor);

        // Clearcoat
        MPlug mClearcoatColorPlug(mShaderObject, FlStandardSurfaceShader::clearcoatColor);
        MFloatVector mClearcoatColor = mClearcoatColorPlug.asMDataHandle().asFloatVector();

        MPlug mClearcoatWeightPlug(mShaderObject, FlStandardSurfaceShader::clearcoatWeight);
        float mClearcoatWeight = mClearcoatWeightPlug.asMDataHandle().asFloat();

        MPlug mClearcoatGlossPlug(mShaderObject, FlStandardSurfaceShader::clearcoatGloss);
        float mClearcoatGloss = mClearcoatGlossPlug.asMDataHandle().asFloat();

        MPlug mClearcoatIORPlug(mShaderObject, FlStandardSurfaceShader::clearcoatIOR);
        float mClearcoatIOR = mClearcoatIORPlug.asMDataHandle().asFloat();

        PrincipalSpectrum clearcoatColor = FromReflectanceRGB(StaticArray<float, 3>(mClearcoatColor.x, mClearcoatColor.y, mClearcoatColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> clearcoatColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(clearcoatColor);

        // Thin Film
        MPlug mThinFilmThicknessPlug(mShaderObject, FlStandardSurfaceShader::thinFilmThickness);
        float mThinFilmThickness = mThinFilmThicknessPlug.asMDataHandle().asFloat();

        MPlug mThinFilmIORPlug(mShaderObject, FlStandardSurfaceShader::thinFilmIOR);
        float mThinFilmIOR = mThinFilmIORPlug.asMDataHandle().asFloat();

        // Actual Material
        std::shared_ptr<Material> standardSurfaceMaterial = std::make_shared<StandardSurfaceMaterial>(
            mDiffuseWeight, diffuseColorTex, diffuseRoughnessTex,
            mSpecWeight, specColorTex, specRoughnessTex, specAnisotropicTex, mSpecType, mSpecIOR, mMetallicPreset,
            mSheenWeight, sheenColorTex, sheenRoughnessTex,
            mTransmissionWeight, transmissionColorTex,
            mClearcoatWeight, clearcoatColorTex, mClearcoatIOR, mClearcoatGloss,
            mThinFilmThickness, mThinFilmIOR);

        return standardSurfaceMaterial;
    }

    return defaultMaterial;
}
