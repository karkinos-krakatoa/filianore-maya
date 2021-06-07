#include "renderglobalsnode.h"

#include <maya/MDGModifier.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>

#include "util.h"

const MString RenderGlobalsNode::name("filianoreRendererGlobalsNode");
const MTypeId RenderGlobalsNode::id(0x00132b41);

RenderParams RenderGlobalsNode::context;

MObject RenderGlobalsNode::samples;
MObject RenderGlobalsNode::diffuseRayDepth;
MObject RenderGlobalsNode::specularRayDepth;
MObject RenderGlobalsNode::glossyRayDepth;
MObject RenderGlobalsNode::gammaCorrect;
MObject RenderGlobalsNode::tonemap;

void *RenderGlobalsNode::creator()
{
    return (new RenderGlobalsNode);
}

MStatus RenderGlobalsNode::initialize()
{
    MStatus status;
    MFnNumericAttribute numAttr;
    MFnEnumAttribute enumAttrFn;

    samples = numAttr.create("samples", "samples", MFnNumericData::kInt, 1, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Samples] Attribute.");
    numAttr.setMin(1);
    numAttr.setMax(12000);
    addAttribute(samples);

    diffuseRayDepth = numAttr.create("diffuseRayDepth", "diffuseRayDepth", MFnNumericData::kInt, 0, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Diffuse Ray Depth] Attribute.");
    numAttr.setMin(0);
    numAttr.setMax(100);
    addAttribute(diffuseRayDepth);

    specularRayDepth = numAttr.create("specularRayDepth", "specularRayDepth", MFnNumericData::kInt, 0, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Specular Ray Depth] Attribute.");
    numAttr.setMin(0);
    numAttr.setMax(100);
    addAttribute(specularRayDepth);

    glossyRayDepth = numAttr.create("glossyRayDepth", "glossyRayDepth", MFnNumericData::kInt, 0, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Glossy Ray Depth] Attribute.");
    numAttr.setMin(0);
    numAttr.setMax(100);
    addAttribute(glossyRayDepth);

    gammaCorrect = enumAttrFn.create("clrGammaCorrect", "clrGammaCorrect", 2, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Gamma Correct] Attribute");
    enumAttrFn.addField("None", 1);
    enumAttrFn.addField("Rec 709", 2);
    enumAttrFn.addField("Gamma 1.8", 3);
    enumAttrFn.addField("Gamma 2.2", 4);
    enumAttrFn.addField("Gamma 4.0", 5);
    addAttribute(gammaCorrect);

    tonemap = enumAttrFn.create("clrTonemap", "clrTonemap", 1, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Tonemapping] Attribute");
    enumAttrFn.addField("None", 1);
    enumAttrFn.addField("ACES Basic", 2);
    enumAttrFn.addField("Reinhard", 3);
    enumAttrFn.addField("Filmic", 4);
    addAttribute(tonemap);

    return (MS::kSuccess);
}

MStatus RenderGlobalsNode::compute(const MPlug &plug, MDataBlock &data)
{
    return (MS::kSuccess);
}

const RenderParams &RenderGlobalsNode::fetchContext()
{
    MStatus status;
    MObject mObj;
    if (GetDependencyNodeByName(RenderGlobalsNode::name, mObj) != MS::kSuccess)
    {
        return (context);
    }

    MPlug mSamplesPlug(mObj, samples);
    status = mSamplesPlug.getValue(context.samples);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Samples] Attribute value.");

    MPlug mDiffuseRayDepthPlug(mObj, diffuseRayDepth);
    status = mDiffuseRayDepthPlug.getValue(context.diffuseRayDepth);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Diffuse Ray Depth] Attribute value.");

    MPlug mSpecularRayDepthPlug(mObj, specularRayDepth);
    status = mSpecularRayDepthPlug.getValue(context.specularRayDepth);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Specular Ray Depth] Attribute value.");

    MPlug mGlossyRayDepthPlug(mObj, glossyRayDepth);
    status = mGlossyRayDepthPlug.getValue(context.glossyRayDepth);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Glossy Ray Depth] Attribute value.");

    MPlug mGammaCorrectPlug(mObj, gammaCorrect);
    status = mGammaCorrectPlug.getValue(context.gammaCorrectType);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Gamma Correct] Attribute value.");

    MPlug mTonemappingPlug(mObj, tonemap);
    status = mTonemappingPlug.getValue(context.tonemapType);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Tonemapping] Attribute value.");

    return (context);
}

const RenderParams &RenderGlobalsNode::getContext()
{
    return (context);
}

void RenderGlobalsNode::clean()
{
    MObject mObj;
    GetDependencyNodeByName(RenderGlobalsNode::name, mObj);

    MDGModifier modifier;

    modifier.deleteNode(samples);
    modifier.deleteNode(diffuseRayDepth);
    modifier.deleteNode(specularRayDepth);
    modifier.deleteNode(glossyRayDepth);

    modifier.deleteNode(gammaCorrect);
    modifier.deleteNode(tonemap);

    modifier.deleteNode(mObj);

    modifier.doIt();
}