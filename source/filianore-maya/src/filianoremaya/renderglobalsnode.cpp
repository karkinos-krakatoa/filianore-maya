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

void *RenderGlobalsNode::creator()
{
    return (new RenderGlobalsNode);
}

MStatus RenderGlobalsNode::initialize()
{
    MStatus status;
    MFnNumericAttribute numAttr;

    samples = numAttr.create("samples", "samples", MFnNumericData::kInt, 8, &status);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Failed to add [Samples] Attribute.");
    numAttr.setMin(1);
    numAttr.setMax(1024);
    addAttribute(samples);

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
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Failed to read [Samples] Attribute value.");

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
    modifier.doIt();

    modifier.deleteNode(mObj);
    modifier.doIt();
}