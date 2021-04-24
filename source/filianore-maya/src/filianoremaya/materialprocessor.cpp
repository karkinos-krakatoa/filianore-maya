#include "materialprocessor.h"
#include "util.h"

#include "matteshadernode.h"

#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include "filianore/textures/constant.h"

#include "filianore/materials/lambert.h"
#include "filianore/materials/matte.h"

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

    // if (materialName.find("flMirrorShader") != std::string::npos)
    // {
    //     MPlug mColorPlug(mShaderObject, MirrorMaterialNode::aColor);
    //     MFloatVector mColor = mColorPlug.asMDataHandle().asFloatVector();

    //     PrincipalSpectrum specularColor = FromReflectanceRGB(StaticArray<float, 3>(mColor.x, mColor.y, mColor.z));
    //     std::shared_ptr<Texture<PrincipalSpectrum>> specularTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(specularColor);
    //     std::shared_ptr<Material> mirrorMaterial = std::make_shared<MirrorMaterial>(specularTex);

    //     return mirrorMaterial;
    // }

    return defaultMaterial;
}
