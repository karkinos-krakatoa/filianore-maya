#include "materialprocessorutil.h"

#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlugArray.h>
#include <maya/MDataHandle.h>

#include "util.h"

#include "filianore/textures/constant.h"
#include "filianore/textures/imagemap.h"

#include "filianore/color/spectrumoperations.h"
#include "filianore/color/spectruminits.h"

std::shared_ptr<Texture<PrincipalSpectrum>> EvaluateColorFromPlugNetwork(MPlug &mShaderObjectPlug)
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

        MObject textureNode = dgIt.currentItem();
        MPlug filenamePlug = MFnDependencyNode(textureNode).findPlug("fileTextureName", true, &status);
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

std::shared_ptr<Texture<float>> EvaluateBumpShadingNetwork(MObject &mShaderObject)
{
    MStatus status;

    MFnDependencyNode fn(mShaderObject);

    MPlug bumpPlug = fn.findPlug("normalCamera", true, &status);
    if (status == MS::kSuccess)
    {
        MPlugArray connections;
        bumpPlug.connectedTo(connections, true, false);
        if (connections.length() > 0)
        {
            MFnDependencyNode fnBump(connections[0].node());

            float bumpDepth;

            // Get Bump Depth
            MPlug bumpDepthPlug = fnBump.findPlug("bumpDepth", true, &status);
            bumpDepthPlug.getValue(bumpDepth);

            // Get Bump Value and Connections
            MPlug bumpValuePlug = fnBump.findPlug("bumpValue", true, &status);
            MPlugArray bumpValueConnections;
            bumpValuePlug.connectedTo(bumpValueConnections, true, false);
            for (unsigned int j = 0; j < bumpValueConnections.length(); ++j)
            {
                if (bumpValueConnections[j].node().apiType() == MFn::kFileTexture)
                {
                    // We have the texture we need
                    MFnDependencyNode fnTex(bumpValueConnections[j].node());

                    MPlug filenamePlug = fnTex.findPlug("fileTextureName", true, &status);
                    MString textureName; // name texture + path
                    filenamePlug.getValue(textureName);

                    FILIANORE_MAYA_LOG_INFO("Bump Map : ");
                    FILIANORE_MAYA_LOG_INFO(textureName);

                    // TODO
                    return nullptr;
                }
            }
        }
    }

    return nullptr;
}