#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MRenderUtil.h>
#include <maya/MRenderView.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>

#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>

#include "filianore/bvhx/bvhx.h"
#include "filianore/core/interaction.h"

#include "cameraexporter.h"
#include "meshexporter.h"

#include "finalrendercommand.h"
#include "renderglobalsnode.h"
#include "util.h"

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
        FILIANORE_MAYA_LOG_ERROR("Render View does not exist.");
        return MS::kFailure;
    }

    // Common Maya Render Params
    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);
    if (!MRenderView::startRender(renderSettings.width, renderSettings.height))
    {
        FILIANORE_MAYA_LOG_ERROR("Error occured in startRender.");
        return MS::kFailure;
    }

    // Filianore Maya Render Params
    const RenderContext &context = RenderGlobalsNode::fetchContext();

    // Camera setup
    CameraExporter cameraExporter;
    std::unique_ptr<filianore::Camera> camera;
    try
    {
        cameraExporter = CameraExporter(renderSettings.width, renderSettings.height);
        camera = cameraExporter.ExportCamera();
    }
    catch (const std::exception &e)
    {
        FILIANORE_MAYA_LOG_ERROR("Error in creating the Render Camera.");
        std::cerr << e.what() << '\n';
    }

    // Meshes setup
    MeshExporter meshExporter;
    std::vector<std::shared_ptr<filianore::Primitive>> scenePrimitives;
    try
    {
        scenePrimitives = meshExporter.ExportPrimitives();
        if (scenePrimitives.size() <= 0)
        {
            FILIANORE_MAYA_LOG_INFO("No Primitives in Scene. Render cannot start.");
            return MS::kFailure;
        }

        auto ss = std::to_string(scenePrimitives.size());
        MString primCount = ss.c_str();
        FILIANORE_MAYA_LOG_INFO("Prim Count - " + primCount);
    }
    catch (const std::exception &e)
    {
        FILIANORE_MAYA_LOG_ERROR("Error in getting/initializing the Scene Primitives.");
        std::cerr << e.what() << '\n';
    }

    // Accel Setup
    filianore::BVH bvhx(scenePrimitives);

    // Main Render Loop
    FILIANORE_MAYA_LOG_INFO("Final Render started...");
    RV_PIXEL *pixels = new RV_PIXEL[renderSettings.width * renderSettings.height];

    tbb::task_scheduler_init init(11);

    tbb::parallel_for(tbb::blocked_range<int>(0, renderSettings.height),
                      [renderSettings, &camera, &bvhx, pixels](const tbb::blocked_range<int> &range) {
                          for (unsigned int y = range.begin(); y != (unsigned int)range.end(); y++)
                          {
                              for (unsigned int x = 0; x < renderSettings.width; x++)
                              {
                                  int pixelIndex = (renderSettings.height - y - 1) * renderSettings.width + x;

                                  float u = (static_cast<float>(x) + 0.125f) / float(renderSettings.width);
                                  float v = (static_cast<float>(y) + 0.8775f) / float(renderSettings.height);

                                  filianore::Ray ray = camera->AwakenRay(filianore::StaticArray<float, 2>(u, v), filianore::StaticArray<float, 2>(0.332f, 0.55012f));

                                  filianore::StaticArray<float, 3> currPixel;

                                  filianore::SurfaceInteraction isect;
                                  if (bvhx.Intersect(ray, &isect))
                                  {
                                      currPixel = filianore::StaticArray<float, 3>(isect.n.x(), isect.n.y(), isect.n.z());
                                      pixels[pixelIndex].a = 255.f;
                                  }
                                  else
                                  {
                                      currPixel = filianore::StaticArray<float, 3>(0.f, 0.f, 0.f);
                                      pixels[pixelIndex].a = 0.f;
                                  }

                                  pixels[pixelIndex].r = 255.f * filianore::Clamp<float>(currPixel.x(), 0.f, 1.f);
                                  pixels[pixelIndex].g = 255.f * filianore::Clamp<float>(currPixel.y(), 0.f, 1.f);
                                  pixels[pixelIndex].b = 255.f * filianore::Clamp<float>(currPixel.z(), 0.f, 1.f);
                              }
                          }
                      });

    MRenderView::updatePixels(0, renderSettings.width - 1, 0, renderSettings.height - 1, pixels);
    MRenderView::refresh(0, renderSettings.width - 1, 0, renderSettings.height - 1);

    delete[] pixels;

    if (!MRenderView::endRender())
    {
        FILIANORE_MAYA_LOG_ERROR("Error occured in endRender.");
        return MS::kFailure;
    }

    FILIANORE_MAYA_LOG_INFO("Final Render completed.");

    return status;
}
