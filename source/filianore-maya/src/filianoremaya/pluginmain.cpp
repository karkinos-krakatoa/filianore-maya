#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "util.h"


MStatus initializePlugin(MObject plugin)
{
	MStatus status;
	MFnPlugin fnPlugin(plugin, FILIANORE_MAYA_NAME, FILIANORE_MAYA_VERSION);

	// Rendering Nodes

	// Commands

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

	// Commands

	// Scripts
	status = MGlobal::executePythonCommand("import register; register.unregister()", false, false);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "FilianoreMaya : Error uninitializing rendering engine.");

	return status;
}