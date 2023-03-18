#ifndef _MATERIAL_PROCESSOR_UTIL_H
#define _MATERIAL_PROCESSOR_UTIL_H

#include <maya/MObject.h>
#include <maya/MPlug.h>

#include "filianore/core/elemental.h"

std::shared_ptr<filianore::Texture<filianore::PrincipalSpectrum>> EvaluateColorFromPlugNetwork(MPlug &mShaderObjectPlug);

std::shared_ptr<filianore::Texture<float>> EvaluateBumpShadingNetwork(MObject &mShaderObject);

#endif