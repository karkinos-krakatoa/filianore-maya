#ifndef _RENDER_CONTEXT_H
#define _RENDER_CONTEXT_H

struct RenderContext
{
    int samples;
    int diffuseRayDepth;
    int specularRayDepth;
};

#endif