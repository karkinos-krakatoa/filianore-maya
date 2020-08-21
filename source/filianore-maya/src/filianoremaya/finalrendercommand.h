#ifndef _FINAL_RENDER_COMMAND_H
#define _FINAL_RENDER_COMMAND_H

#include <maya/MPxCommand.h>

class FinalRenderCommand : public MPxCommand
{
public:
    static MString commandName;

    static MSyntax syntaxCreator();
    static void *creator();

    MStatus doIt(const MArgList &args) override;
};

#endif
