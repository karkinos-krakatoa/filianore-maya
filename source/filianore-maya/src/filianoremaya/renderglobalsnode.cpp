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
MObject RenderGlobalsNode::reflectionRayDepth;

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

    reflectionRayDepth = numAttr.create("reflectionRayDepth", "reflectionRayDepth", MFnNumericData::kInt, 0, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Reflection Ray Depth] Attribute.");
    numAttr.setMin(0);
    numAttr.setMax(100);
    addAttribute(reflectionRayDepth);

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

    MPlug mReflectionRayDepthPlug(mObj, reflectionRayDepth);
    status = mReflectionRayDepthPlug.getValue(context.reflectionRayDepth);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Reflection Ray Depth] Attribute value.");

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
    modifier.deleteNode(reflectionRayDepth);

    modifier.deleteNode(mObj);

    modifier.doIt();
}