#include <maya/MDrawRegistry.h>
#include <maya/MFnPlugin.h>

#include "flplasticshader.h"
#include "flplasticshaderoverride.h"
#include "flstandardsurfaceshader.h"
#include "flstandardsurfaceshaderoverride.h"

#include "finalrendercommand.h"
#include "renderglobalsnode.h"

#include "filianore/color/metallurgy.h"
#include "filianore/color/spectruminits.h"

#include "util.h"

MStatus initializePlugin(MObject plugin) {
    // Filianore Inits
    filianore::InitializeSpectrals();
    filianore::InitializeForge();

    MStatus status;
    MFnPlugin fnPlugin(plugin, FILIANORE_MAYA_NAME, FILIANORE_MAYA_VERSION);

    // Rendering Nodes
    status = fnPlugin.registerNode(RenderGlobalsNode::name,
                                   RenderGlobalsNode::id, RenderGlobalsNode::creator, RenderGlobalsNode::initialize);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error registering RenderGlobalNode.");

    // Shading Nodes
    MString sDrawFlStandardSurfaceDBClassification("drawdb/shader/surface/" + FlStandardSurfaceShader::name);
    MString sFlStandardSurfaceFullClassification("shader/surface:" + sDrawFlStandardSurfaceDBClassification);
    fnPlugin.registerNode(FlStandardSurfaceShader::name, FlStandardSurfaceShader::id, FlStandardSurfaceShader::creator, FlStandardSurfaceShader::initialize, MPxNode::kDependNode, &sFlStandardSurfaceFullClassification);
    MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(sDrawFlStandardSurfaceDBClassification, FlStandardSurfaceShader::name, FlStandardSurfaceShaderOverride::creator);

    MString sDrawPlasticShaderDBClassification("drawdb/shader/surface/" + FlPlasticShader::name);
    MString sPlasticShaderFullClassification("shader/surface:" + sDrawPlasticShaderDBClassification);
    fnPlugin.registerNode(FlPlasticShader::name, FlPlasticShader::id, FlPlasticShader::creator, FlPlasticShader::initialize, MPxNode::kDependNode, &sPlasticShaderFullClassification);
    MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(sDrawPlasticShaderDBClassification, FlPlasticShader::name, FlPlasticShaderOverride::creator);

    // Commands
    status = fnPlugin.registerCommand(FinalRenderCommand::commandName, FinalRenderCommand::creator);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error registering Final Render command.");

    // Scripts
    status = MGlobal::executePythonCommand("import register; register.register()", false, false);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error initializing rendering engine.");

    FILIANORE_MAYA_LOG_INFO("Renderer Plugin registered successfully.");

    return status;
}

MStatus uninitializePlugin(MObject plugin) {
    MStatus status;
    MFnPlugin fnPlugin(plugin);

    // Rendering Nodes
    RenderGlobalsNode::clean();
    status = fnPlugin.deregisterNode(RenderGlobalsNode::id);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error deregistering RenderGlobalNode.");

    // Shading Nodes
    MString sDrawFlStandardSurfaceDBClassification("drawdb/shader/surface/" + FlStandardSurfaceShader::name);
    fnPlugin.deregisterNode(FlStandardSurfaceShader::id);
    MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(sDrawFlStandardSurfaceDBClassification, FlStandardSurfaceShader::name);

    MString sDrawPlasticShaderDBClassification("drawdb/shader/surface/" + FlPlasticShader::name);
    fnPlugin.deregisterNode(FlPlasticShader::id);
    MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(sDrawPlasticShaderDBClassification, FlPlasticShader::name);

    // Commands
    status = fnPlugin.deregisterCommand(FinalRenderCommand::commandName);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error deregistering Final Render command.");

    // Scripts
    status = MGlobal::executePythonCommand("import register; register.unregister()", false, false);
    FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error uninitializing rendering engine.");

    FILIANORE_MAYA_LOG_INFO("Renderer Plugin deregistered successfully.");

    return status;
}