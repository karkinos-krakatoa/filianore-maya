#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

#include "renderglobalsnode.h"
#include "finalrendercommand.h"

#include "util.h"

MStatus initializePlugin(MObject plugin)
{
	MStatus status;
	MFnPlugin fnPlugin(plugin, FILIANORE_MAYA_NAME, FILIANORE_MAYA_VERSION);

	// Rendering Nodes
	status = fnPlugin.registerNode(RenderGlobalsNode::name,
								   RenderGlobalsNode::id, RenderGlobalsNode::creator, RenderGlobalsNode::initialize);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Error registering RenderGlobalNode.");

	// Commands
	status = fnPlugin.registerCommand(FinalRenderCommand::commandName, FinalRenderCommand::creator);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Error registering Final Render command.");

	// Scripts
	status = MGlobal::executePythonCommand("import register; register.register()", false, false);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Error initializing rendering engine.");

	return status;
}

MStatus uninitializePlugin(MObject plugin)
{
	MStatus status;
	MFnPlugin fnPlugin(plugin);

	// Rendering Nodes
	RenderGlobalsNode::clean();
	status = fnPlugin.deregisterNode(RenderGlobalsNode::id);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Error deregistering RenderGlobalNode.");

	// Commands
	status = fnPlugin.deregisterCommand(FinalRenderCommand::commandName);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Error deregistering Final Render command.");

	// Scripts
	status = MGlobal::executePythonCommand("import register; register.unregister()", false, false);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Error uninitializing rendering engine.");

	return status;
}