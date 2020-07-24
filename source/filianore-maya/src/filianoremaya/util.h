#pragma once

#include <string>

#include <maya/MGlobal.h>
#include <maya/MStatus.h>


constexpr auto FILIANORE_MAYA_NAME = "FilianoreMaya";

constexpr auto FILIANORE_MAYA_VERSION = "1.0";


#define FILIANORE_MAYA_LOG_INFO(message)\
{\
	MGlobal::displayInfo(MString(message));\
}

#define FILIANORE_MAYA_LOG_ERROR(message)\
{\
	MGlobal::displayError(MString(message));\
}

#define FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, message)\
{\
	MStatus stat = (status);\
	if(stat != MStatus::kSuccess)\
	{\
		MGlobal::displayError(MString(message));\
	}\
}

MStatus GetDependencyNodeByName(const MString& name, MObject& node);

