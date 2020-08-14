#pragma once

#include <maya/MPxCommand.h>

class FinalRenderCommand : public MPxCommand
{
public:
    static MString commandName;

    static MSyntax syntaxCreator();
    static void *creator();

    MStatus doIt(const MArgList &args) override;
};
