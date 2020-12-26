#ifndef _MAYA_FILIANORE_UTIL_H
#define _MAYA_FILIANORE_UTIL_H

#include <string>

#include <maya/MGlobal.h>
#include <maya/MStatus.h>

constexpr auto FILIANORE_MAYA_NAME = "FilianoreForMaya";

constexpr auto FILIANORE_MAYA_NAME_FOR_LOGGING = "FilianoreForMaya : ";

constexpr auto FILIANORE_MAYA_VERSION = "1.0";

#define FILIANORE_MAYA_LOG_INFO(message)                                                   \
	{                                                                                      \
		MGlobal::displayInfo(MString(FILIANORE_MAYA_NAME_FOR_LOGGING) + MString(message)); \
	}

#define FILIANORE_MAYA_LOG_ERROR(message)                                          \
	{                                                                              \
		MGlobal::displayError(FILIANORE_MAYA_NAME_FOR_LOGGING + MString(message)); \
	}

#define FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, message)                              \
	{                                                                                  \
		MStatus stat = (status);                                                       \
		if (stat != MStatus::kSuccess)                                                 \
		{                                                                              \
			MGlobal::displayError(FILIANORE_MAYA_NAME_FOR_LOGGING + MString(message)); \
		}                                                                              \
	}

MStatus GetDependencyNodeByName(const MString &name, MObject &node);

#endif
