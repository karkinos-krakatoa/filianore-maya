#ifndef _MATERIAL_PROCESSOR_UTIL_H
#define _MATERIAL_PROCESSOR_UTIL_H

#include <maya/MObject.h>
#include <maya/MPlug.h>

#include "filianore/core/elemental.h"

using namespace filianore;

std::shared_ptr<Texture<PrincipalSpectrum>> EvaluateColorFromPlugNetwork(MPlug &mShaderObjectPlug);

std::shared_ptr<Texture<float>> EvaluateBumpShadingNetwork(MObject &mShaderObject);

#endif