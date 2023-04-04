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

#include "filianore/bvh/flbvh.h"

#include "filianore/core/interaction.h"
#include "filianore/core/scene.h"
#include "filianore/integrators/normalintegrator.h"
#include "filianore/integrators/pathintegrator.h"
#include "filianore/sampling/samplers/whitenoise.h"

#include "filianore/color/spectruminits.h"
#include "filianore/color/spectrumoperations.h"

#include "cameraexporter.h"
#include "illuminantsexporter.h"
#include "meshexporter.h"

#include "finalrendercommand.h"
#include "renderglobalsnode.h"
#include "util.h"

MString FinalRenderCommand::commandName("filianoreRendererRenderProcedure");

void *FinalRenderCommand::creator() {
    return new FinalRenderCommand();
}

MSyntax FinalRenderCommand::syntaxCreator() {
    MSyntax syntax;
    syntax.addFlag("-c", "-camera", MSyntax::kString);
    syntax.addFlag("-w", "-width", MSyntax::kLong);
    syntax.addFlag("-h", "-height", MSyntax::kLong);
    syntax.addFlag("-b", "-batch", MSyntax::kString);
    return syntax;
}

MStatus FinalRenderCommand::doIt(const MArgList &args) {
    MStatus status = MS::kSuccess;

    if (!MRenderView::doesRenderEditorExist()) {
        FILIANORE_MAYA_LOG_ERROR("Render View does not exist.");
        return MS::kFailure;
    }

    // Common Maya Render Params
    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);
    if (!MRenderView::startRender(renderSettings.width, renderSettings.height)) {
        FILIANORE_MAYA_LOG_ERROR("Error occured in startRender.");
        return MS::kFailure;
    }

    // Filianore Maya Render Params
    const filianore::RenderParams &renderParams = RenderGlobalsNode::fetchContext();

    // filianore::Camera setup
    CameraExporter cameraExporter;
    std::unique_ptr<filianore::Camera> camera;
    try {
        cameraExporter = CameraExporter(renderSettings.width, renderSettings.height);
        camera = cameraExporter.ExportCamera();
    } catch (const std::exception &e) {
        FILIANORE_MAYA_LOG_ERROR("Error in creating the Render filianore::Camera.");
        std::cerr << e.what() << '\n';
    }

    // Meshes setup
    MeshExporter meshExporter;
    std::vector<std::shared_ptr<filianore::Primitive>> scenePrimitives;
    try {
        scenePrimitives = meshExporter.ExportPrimitives();
        if (scenePrimitives.size() <= 0) {
            FILIANORE_MAYA_LOG_INFO("No Primitives in filianore::Scene. Render cannot start.");
            return MS::kFailure;
        }

        auto ss = std::to_string(scenePrimitives.size());
        MString primCount = ss.c_str();
        FILIANORE_MAYA_LOG_INFO("Prim Count - " + primCount);
    } catch (const std::exception &e) {
        FILIANORE_MAYA_LOG_ERROR("Error in getting/initializing the filianore::Scene Primitives.");
        std::cerr << e.what() << '\n';
    }

    // Illum setup
    IlluminantExporter illumExporter;
    IlluminantExporterResponse illuminantExporterResponse;
    std::vector<std::shared_ptr<filianore::Illuminant>> illums;
    try {
        illuminantExporterResponse = illumExporter.ExportIlluminants(scenePrimitives);
        illums = illuminantExporterResponse.illuminants;
        scenePrimitives.insert(scenePrimitives.end(), illuminantExporterResponse.prims.begin(), illuminantExporterResponse.prims.end());

        auto illumsSize = std::to_string(illums.size());
        MString illumsCount = illumsSize.c_str();
        FILIANORE_MAYA_LOG_INFO("Illums Count - " + illumsCount);
    } catch (const std::exception &e) {
        FILIANORE_MAYA_LOG_ERROR("Error in getting/initializing the filianore::Scene Illuminants.");
        std::cerr << e.what() << '\n';
    }

    // Accel Setup
    std::shared_ptr<filianore::Primitive> bvh = std::make_shared<filianore::BVH>(scenePrimitives);

    // Render components setup
    filianore::Scene scene(bvh, illums);
    std::shared_ptr<filianore::Sampler> sampler = std::make_shared<filianore::Whitenoise>();
    std::unique_ptr<filianore::Integrator> integrator = std::make_unique<filianore::NormalIntegrator>(renderParams);
    integrator->prepare_the_renderer(scene, *sampler);

    // Main Render Loop
    FILIANORE_MAYA_LOG_INFO("Final Render started...");
    RV_PIXEL *pixels = new RV_PIXEL[renderSettings.width * renderSettings.height];

    tbb::task_scheduler_init init(11);

    for (float s = 0; s < renderParams.samples; s++) {
        tbb::parallel_for(tbb::blocked_range<int>(0, renderSettings.height),
                          [renderSettings, &renderParams, &s, &camera, &scene, &sampler, &integrator, pixels](const tbb::blocked_range<int> &range) {
                              for (unsigned int y = range.begin(); y != (unsigned int)range.end(); y++) {
                                  for (unsigned int x = 0; x < renderSettings.width; x++) {
                                      int pixelIndex = (renderSettings.height - y - 1) * renderSettings.width + x;

                                      filianore::Vector2f uCamera = sampler->get_2d();
                                      float u = (static_cast<float>(x) + uCamera.x) / float(renderSettings.width);
                                      float v = (static_cast<float>(y) + uCamera.y) / float(renderSettings.height);

                                      filianore::Vector2f uLens = sampler->get_2d();
                                      filianore::Ray ray = camera->awaken_ray(filianore::Vector2f(u, v), filianore::Vector2f(0.332f, 0.55012f));

                                      filianore::PrincipalSpectrum currPixelSpec = integrator->Li(ray, scene, *sampler, 0);
                                      filianore::Vector3f currPixel = filianore::to_rgb(currPixelSpec);

                                      pixels[pixelIndex].r = (pixels[pixelIndex].r * s + (255.f * currPixel.x)) / (s + 1);
                                      pixels[pixelIndex].g = (pixels[pixelIndex].g * s + (255.f * currPixel.y)) / (s + 1);
                                      pixels[pixelIndex].b = (pixels[pixelIndex].b * s + (255.f * currPixel.z)) / (s + 1);
                                      pixels[pixelIndex].a = 255.f;
                                  }
                              }
                          });

        MRenderView::updatePixels(0, renderSettings.width - 1, 0, renderSettings.height - 1, pixels);
        MRenderView::refresh(0, renderSettings.width - 1, 0, renderSettings.height - 1);

        sampler->prepare_next_sample();
    }

    delete[] pixels;

    if (!MRenderView::endRender()) {
        FILIANORE_MAYA_LOG_ERROR("Error occured in endRender.");
        return MS::kFailure;
    }

    FILIANORE_MAYA_LOG_INFO("Final Render completed.");

    return status;
}
