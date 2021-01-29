#include "renderglobalsnode.h"

#include <maya/MDGModifier.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>

#include "util.h"

const MString RenderGlobalsNode::name("filianoreRendererGlobalsNode");
const MTypeId RenderGlobalsNode::id(0x00132b41);

RenderContext RenderGlobalsNode::context;

MObject RenderGlobalsNode::samples;
MObject RenderGlobalsNode::rayDepth;

void *RenderGlobalsNode::creator()
{
    return (new RenderGlobalsNode);
}

MStatus RenderGlobalsNode::initialize()
{
    MStatus status;
    MFnNumericAttribute numAttr;

    samples = numAttr.create("samples", "samples", MFnNumericData::kInt, 1, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Samples] Attribute.");
    numAttr.setMin(1);
    numAttr.setMax(12000);
    addAttribute(samples);

    rayDepth = numAttr.create("rayDepth", "rayDepth", MFnNumericData::kInt, 1, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to add [Ray Depth] Attribute.");
    numAttr.setMin(0);
    numAttr.setMax(100);
    addAttribute(rayDepth);

    return (MS::kSuccess);
}

MStatus RenderGlobalsNode::compute(const MPlug &plug, MDataBlock &data)
{
    return (MS::kSuccess);
}

const RenderContext &RenderGlobalsNode::fetchContext()
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

    MPlug mRayDepthPlug(mObj, rayDepth);
    status = mRayDepthPlug.getValue(context.rayDepth);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Failed to read [Ray Depth] Attribute value.");

    return (context);
}

const RenderContext &RenderGlobalsNode::getContext()
{
    return (context);
}

void RenderGlobalsNode::clean()
{
    MObject mObj;
    GetDependencyNodeByName(RenderGlobalsNode::name, mObj);

    MDGModifier modifier;

    modifier.deleteNode(samples);
    modifier.deleteNode(rayDepth);

    modifier.deleteNode(mObj);

    modifier.doIt();
}