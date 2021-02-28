#ifndef _RENDER_GLOBALS_NODE_H
#define _RENDER_GLOBALS_NODE_H

#include <maya/MPxNode.h>

#include "rendercontext.h"

class RenderGlobalsNode : public MPxNode
{
public:
    static const MString name;
    static const MTypeId id;

    static void *creator();
    static MStatus initialize();

    MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

    static const RenderContext &fetchContext();
    static const RenderContext &getContext();

    static void clean();

private:
    static RenderContext context;

    static MObject samples;
    static MObject diffuseRayDepth;
    static MObject specularRayDepth;
};

#endif