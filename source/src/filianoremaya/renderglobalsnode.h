#ifndef _RENDER_GLOBALS_NODE_H
#define _RENDER_GLOBALS_NODE_H

#include <maya/MPxNode.h>

#include "filianore/core/renderparams.h"

class RenderGlobalsNode : public MPxNode {
public:
    static const MString name;
    static const MTypeId id;

    static void *creator();
    static MStatus initialize();

    MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

    static const filianore::RenderParams &fetchContext();
    static const filianore::RenderParams &getContext();

    static void clean();

private:
    static filianore::RenderParams context;

    static MObject samples;
    static MObject diffuseRayDepth;
    static MObject reflectionRayDepth;
    static MObject transmissionRayDepth;
};

#endif