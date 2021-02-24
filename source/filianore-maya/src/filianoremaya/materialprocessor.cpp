#include "materialprocessor.h"
#include "util.h"

#include "lambertnode.h"
#include "mirrormaterialnode.h"

#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include "filianore/textures/constant.h"
#include "filianore/materials/lambert.h"
#include "filianore/materials/mirror.h"

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
    if (materialName.find("flLambertShader") != std::string::npos)
    {
        MPlug mDiffuseColorPlug(mShaderObject, LambertNode::aDiffuseColor);
        MFloatVector mDiffuseColor = mDiffuseColorPlug.asMDataHandle().asFloatVector();

        PrincipalSpectrum diffuseColor = FromReflectanceRGB(StaticArray<float, 3>(mDiffuseColor.x, mDiffuseColor.y, mDiffuseColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> diffuseTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(diffuseColor);
        std::shared_ptr<Material> lambertMaterial = std::make_shared<LambertMaterial>(diffuseTex);

        return lambertMaterial;
    }

    if (materialName.find("flMirrorShader") != std::string::npos)
    {
        MPlug mColorPlug(mShaderObject, MirrorMaterialNode::aColor);
        MFloatVector mColor = mColorPlug.asMDataHandle().asFloatVector();

        PrincipalSpectrum specularColor = FromReflectanceRGB(StaticArray<float, 3>(mColor.x, mColor.y, mColor.z));
        std::shared_ptr<Texture<PrincipalSpectrum>> specularTex = std::make_shared<ConstantTexture<PrincipalSpectrum>>(specularColor);
        std::shared_ptr<Material> mirrorMaterial = std::make_shared<MirrorMaterial>(specularTex);

        return mirrorMaterial;
    }

    return defaultMaterial;
}
