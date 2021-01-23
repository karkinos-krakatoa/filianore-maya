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

#include "filianore/color/rgb.h"
#include "filianore/accel/bvh.h"
#include "filianore/core/interaction.h"
#include "filianore/core/scene.h"
#include "filianore/samplers/whitenoise.h"
#include "filianore/integrators/pathintegrator.h"

#include "cameraexporter.h"
#include "meshexporter.h"
#include "illuminantsexporter.h"

#include "finalrendercommand.h"
#include "renderglobalsnode.h"
#include "util.h"

using namespace filianore;

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
    const RenderContext &renderingContext = RenderGlobalsNode::fetchContext();

    // Camera setup
    CameraExporter cameraExporter;
    std::unique_ptr<Camera> camera;
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

    // Illum setup
    IlluminantExporter illumExporter;
    std::vector<std::shared_ptr<Illuminant>> illums;
    try
    {
        illums = illumExporter.ExportIlluminants();

        auto illumsSize = std::to_string(illums.size());
        MString illumsCount = illumsSize.c_str();
        FILIANORE_MAYA_LOG_INFO("Illums Count - " + illumsCount);
    }
    catch (const std::exception &e)
    {
        FILIANORE_MAYA_LOG_ERROR("Error in getting/initializing the Scene Illuminants.");
        std::cerr << e.what() << '\n';
    }

    // Meshes setup
    MeshExporter meshExporter;
    std::vector<std::shared_ptr<Primitive>> scenePrimitives;
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
    std::shared_ptr<Primitive> bvh = std::make_shared<BVH>(scenePrimitives);

    // Render components setup
    Scene scene(bvh, illums);
    std::shared_ptr<Sampler> sampler = std::make_shared<Whitenoise>();
    std::unique_ptr<Integrator> integrator = std::make_unique<PathIntegrator>(1);
    integrator->PrepareTheRenderer(scene, *sampler);

    // Main Render Loop
    FILIANORE_MAYA_LOG_INFO("Final Render started...");
    RV_PIXEL *pixels = new RV_PIXEL[renderSettings.width * renderSettings.height];

    tbb::task_scheduler_init init(11);

    for (float s = 0; s < renderingContext.samples; s++)
    {
        tbb::parallel_for(tbb::blocked_range<int>(0, renderSettings.height),
                          [renderSettings, &s, &camera, &scene, &sampler, &integrator, pixels](const tbb::blocked_range<int> &range) {
                              for (unsigned int y = range.begin(); y != (unsigned int)range.end(); y++)
                              {
                                  for (unsigned int x = 0; x < renderSettings.width; x++)
                                  {
                                      int pixelIndex = (renderSettings.height - y - 1) * renderSettings.width + x;

                                      StaticArray<float, 2> uRand = sampler->Get2D();
                                      float u = (static_cast<float>(x) + uRand.x()) / float(renderSettings.width);
                                      float v = (static_cast<float>(y) + uRand.y()) / float(renderSettings.height);

                                      Ray ray = camera->AwakenRay(StaticArray<float, 2>(u, v), StaticArray<float, 2>(0.332f, 0.55012f));

                                      RGBSpectrum currPixel(0.f);
                                      currPixel = integrator->Li(ray, scene, *sampler, 0);
                                      //currPixel = GammaCorrect(currPixel);

                                      pixels[pixelIndex].r = (pixels[pixelIndex].r * s + (255.f * currPixel.r)) / (s + 1);
                                      pixels[pixelIndex].g = (pixels[pixelIndex].g * s + (255.f * currPixel.g)) / (s + 1);
                                      pixels[pixelIndex].b = (pixels[pixelIndex].b * s + (255.f * currPixel.b)) / (s + 1);
                                      pixels[pixelIndex].a = 255.f;
                                  }
                              }
                          });

        MRenderView::updatePixels(0, renderSettings.width - 1, 0, renderSettings.height - 1, pixels);
        MRenderView::refresh(0, renderSettings.width - 1, 0, renderSettings.height - 1);

        sampler->PrepareNextSample();
    }

    delete[] pixels;

    if (!MRenderView::endRender())
    {
        FILIANORE_MAYA_LOG_ERROR("Error occured in endRender.");
        return MS::kFailure;
    }

    FILIANORE_MAYA_LOG_INFO("Final Render completed.");

    return status;
}
