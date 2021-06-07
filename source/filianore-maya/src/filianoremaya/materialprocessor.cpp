#include "materialprocessor.h"
#include "util.h"

#include "matteshadernode.h"
#include "plasticshadernode.h"

#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include "filianore/textures/constant.h"

#include "filianore/materials/lambert.h"
#include "filianore/materials/matte.h"
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
    if (materialName.find("flMatteShader") != std::string::npos)
    {
        MPlug mDiffuseColorPlug(mShaderObject, MatteShaderNode::aColor);
        MFloatVector mDiffuseColor = mDiffuseColorPlug.asMDataHandle().asFloatVector();

        MPlug mDiffuseWeightPlug(mShaderObject, MatteShaderNode::aWeight);
        float mDiffuseWeight = mDiffuseWeightPlug.asMDataHandle().asFloat();

        MPlug mDiffuseRoughnessPlug(mShaderObject, MatteShaderNode::aRoughness);
        float mDiffuseRoughness = mDiffuseRoughnessPlug.asMDataHandle().asFloat();

        PrincipalSpectrum diffuseColor = FromReflectanceRGB(StaticArray<float, 3>(mDiffuseColor.x, mDiffuseColor.y, mDiffuseColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> diffuseColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(diffuseColor);
        std::shared_ptr<Texture<float>> diffuseRoughnessTex = std::make_shared<ConstantTexture<float>>(mDiffuseRoughness);

        std::shared_ptr<Material> matteMaterial = std::make_shared<MatteMaterial>(diffuseColorTex, diffuseRoughnessTex, mDiffuseWeight);

        return matteMaterial;
    }

    if (materialName.find("flPlasticShader") != std::string::npos)
    {
        MPlug mDiffuseColorPlug(mShaderObject, PlasticShaderNode::aColor);
        MFloatVector mDiffuseColor = mDiffuseColorPlug.asMDataHandle().asFloatVector();

        PrincipalSpectrum diffuseColor = FromReflectanceRGB(StaticArray<float, 3>(mDiffuseColor.x, mDiffuseColor.y, mDiffuseColor.z));
        PrincipalSpectrum specColor = FromReflectanceRGB(StaticArray<float, 3>(1.f, 1.f, 1.f));

        std::shared_ptr<Texture<PrincipalSpectrum>> diffuseColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(diffuseColor);
        std::shared_ptr<Texture<PrincipalSpectrum>> specularColorTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(specColor);

        std::shared_ptr<Texture<float>> diffuseRoughnessTex = std::make_shared<ConstantTexture<float>>(0.1f);
        std::shared_ptr<Texture<float>> specRoughnessTex = std::make_shared<ConstantTexture<float>>(0.3f);

        std::shared_ptr<Material> plasticMaterial = std::make_shared<PlasticMaterial>(diffuseColorTex, diffuseRoughnessTex, 1.f,
                                                                                      specularColorTex, specRoughnessTex, 1.f);

        return plasticMaterial;
    }

    return defaultMaterial;
}
