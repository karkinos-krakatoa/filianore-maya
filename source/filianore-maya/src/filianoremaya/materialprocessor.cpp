#include "materialprocessor.h"
#include "util.h"

#include "flstandardsurfaceshader.h"
#include "flplasticshader.h"

#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include "filianore/textures/constant.h"

#include "filianore/materials/lambert.h"
#include "filianore/materials/standardsurface.h"
#include "filianore/materials/plastic.h"

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
        MFloatVector mDiffuseColor = mDiffuseColorPlug.asMDataHandle().asFloatVector();

        MPlug mDiffuseWeightPlug(mShaderObject, FlStandardSurfaceShader::diffuseBaseWeight);
        float mDiffuseWeight = mDiffuseWeightPlug.asMDataHandle().asFloat();

        MPlug mDiffuseRoughnessPlug(mShaderObject, FlStandardSurfaceShader::diffuseBaseRoughness);
        float mDiffuseRoughness = mDiffuseRoughnessPlug.asMDataHandle().asFloat();

        PrincipalSpectrum diffuseColor = FromReflectanceRGB(StaticArray<float, 3>(mDiffuseColor.x, mDiffuseColor.y, mDiffuseColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> diffuseColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(diffuseColor);
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

        MPlug mSpecIORPlug(mShaderObject, FlStandardSurfaceShader::specularIOR);
        float mSpecIOR = mSpecIORPlug.asMDataHandle().asFloat();

        PrincipalSpectrum specColor = FromReflectanceRGB(StaticArray<float, 3>(mSpecColor.x, mSpecColor.y, mSpecColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> specColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(specColor);
        std::shared_ptr<Texture<float>> specRoughnessTex = std::make_shared<ConstantTexture<float>>(mSpecRoughness);

        std::shared_ptr<Material> standardSurfaceMaterial = std::make_shared<StandardSurfaceMaterial>(
            mDiffuseWeight, diffuseColorTex, diffuseRoughnessTex,
            mSpecWeight, specColorTex, specRoughnessTex, mSpecAnisotropic, mSpecIOR);

        return standardSurfaceMaterial;
    }

    if (materialName.find("flPlastic") != std::string::npos)
    {
        // Diffuse
        MPlug mDiffuseColorPlug(mShaderObject, FlPlasticShader::diffuseBaseColor);
        MFloatVector mDiffuseColor = mDiffuseColorPlug.asMDataHandle().asFloatVector();

        MPlug mDiffuseWeightPlug(mShaderObject, FlPlasticShader::diffuseBaseWeight);
        float mDiffuseWeight = mDiffuseWeightPlug.asMDataHandle().asFloat();

        PrincipalSpectrum diffuseColor = FromReflectanceRGB(StaticArray<float, 3>(mDiffuseColor.x, mDiffuseColor.y, mDiffuseColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> diffuseColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(diffuseColor);

        // Specular
        MPlug mSpecColorPlug(mShaderObject, FlPlasticShader::specularColor);
        MFloatVector mSpecColor = mSpecColorPlug.asMDataHandle().asFloatVector();

        MPlug mSpecWeightPlug(mShaderObject, FlPlasticShader::specularWeight);
        float mSpecWeight = mSpecWeightPlug.asMDataHandle().asFloat();

        MPlug mSpecRoughnessPlug(mShaderObject, FlPlasticShader::specularRoughness);
        float mSpecRoughness = mSpecRoughnessPlug.asMDataHandle().asFloat();

        MPlug mSpecAnisotropicPlug(mShaderObject, FlPlasticShader::specularAnisotropic);
        float mSpecAnisotropic = mSpecAnisotropicPlug.asMDataHandle().asFloat();

        MPlug mSpecIORPlug(mShaderObject, FlPlasticShader::specularIOR);
        float mSpecIOR = mSpecIORPlug.asMDataHandle().asFloat();

        PrincipalSpectrum specColor = FromReflectanceRGB(StaticArray<float, 3>(mSpecColor.x, mSpecColor.y, mSpecColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> specColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(specColor);
        std::shared_ptr<Texture<float>> specRoughnessTex = std::make_shared<ConstantTexture<float>>(mSpecRoughness);

        std::shared_ptr<Material> plasticMaterial = std::make_shared<PlasticMaterial>(
            mDiffuseWeight, diffuseColorTex,
            mSpecWeight, specColorTex, specRoughnessTex, mSpecAnisotropic, mSpecIOR);

        return plasticMaterial;
    }

    return defaultMaterial;
}
