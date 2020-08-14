#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MRenderUtil.h>
#include <maya/MRenderView.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>

#include "finalrendercommand.h"

MString FinalRenderCommand::commandName("filianoreRendererRenderProcedure");

void *FinalRenderCommand::creator()
{
    return new FinalRenderCommand();
}

MSyntax FinalRenderCommand::syntaxCreator()
{
    MSyntax syntax;
    syntax.addFlag("-c", "-camera", MSyntax::kString);
    syntax.addFlag("-w", "-width", MSyntax::kLong);
    syntax.addFlag("-h", "-height", MSyntax::kLong);
    syntax.addFlag("-b", "-batch", MSyntax::kString);
    return syntax;
}

MStatus FinalRenderCommand::doIt(const MArgList &args)
{
    MStatus status = MS::kSuccess;

    if (!MRenderView::doesRenderEditorExist())
    {
        setResult("FilianoreMaya : Render View does not exist");
        return MS::kFailure;
    }

    // Get Flags

    // Init Render Params
    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);

    if (!MRenderView::startRender(renderSettings.width, renderSettings.height))
    {
        setResult("FilianoreMaya : Error occured in startRender.");
        return MS::kFailure;
    }

    MGlobal::displayInfo("FilianoreMaya : Final Render started.");

    int num_side_tiles = 16;
    int average_tiles_width = renderSettings.width / num_side_tiles;
    int average_tiles_height = renderSettings.height / num_side_tiles;

    int resDimens = renderSettings.width * renderSettings.height;
    RV_PIXEL *pixels = new RV_PIXEL[resDimens];

    for (unsigned int y = 0; y < renderSettings.height; y++)
    {
        for (unsigned int x = 0; x < renderSettings.width; x++)
        {
            int pixelIndex = (renderSettings.height - y - 1) * renderSettings.width + x;

            RV_PIXEL pixel;

            pixel.r = 255.f * 0.3f;
            pixel.g = 255.f * 0.f;
            pixel.b = 255.f * 0.f;
            pixel.a = 255.f;

            pixels[pixelIndex] = pixel;
        }
    }

    MRenderView::updatePixels(0, renderSettings.width - 1, 0, renderSettings.height - 1, pixels);
    MRenderView::refresh(0, renderSettings.width - 1, 0, renderSettings.height - 1);

    delete[] pixels;

    if (!MRenderView::endRender())
    {
        setResult("FilianoreMaya : Error occured in endRender.");
        return MS::kFailure;
    }

    MGlobal::displayInfo("FilianoreMaya : Final Render completed.");

    return status;
}
