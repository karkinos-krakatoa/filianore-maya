#include "matteshadernode.h"

#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MFnCompoundAttribute.h>

const MString MatteShaderNode::name("flMatteShader");
const MTypeId MatteShaderNode::id(0x00132b42);

MObject MatteShaderNode::nameData;
MObject MatteShaderNode::nameAttr;

MObject MatteShaderNode::aColorR;
MObject MatteShaderNode::aColorG;
MObject MatteShaderNode::aColorB;
MObject MatteShaderNode::aColor;
MObject MatteShaderNode::aWeight;
MObject MatteShaderNode::aRoughness;

MObject MatteShaderNode::aOutColorR;
MObject MatteShaderNode::aOutColorG;
MObject MatteShaderNode::aOutColorB;
MObject MatteShaderNode::aOutColor;

MObject MatteShaderNode::aNormalCameraX;
MObject MatteShaderNode::aNormalCameraY;
MObject MatteShaderNode::aNormalCameraZ;
MObject MatteShaderNode::aNormalCamera;

MObject MatteShaderNode::aLightDirectionX;
MObject MatteShaderNode::aLightDirectionY;
MObject MatteShaderNode::aLightDirectionZ;
MObject MatteShaderNode::aLightDirection;
MObject MatteShaderNode::aLightIntensityR;
MObject MatteShaderNode::aLightIntensityG;
MObject MatteShaderNode::aLightIntensityB;
MObject MatteShaderNode::aLightIntensity;
MObject MatteShaderNode::aLightAmbient;
MObject MatteShaderNode::aLightDiffuse;
MObject MatteShaderNode::aLightSpecular;
MObject MatteShaderNode::aLightShadowFraction;
MObject MatteShaderNode::aPreShadowIntensity;
MObject MatteShaderNode::aLightBlindData;
MObject MatteShaderNode::aLightData;

MatteShaderNode::MatteShaderNode()
{
}

MatteShaderNode::~MatteShaderNode()
{
}

void *MatteShaderNode::creator()
{
    return new MatteShaderNode();
}

MStatus MatteShaderNode::initialize()
{
    MStatus status;

    MFnNumericAttribute numAttr;
    MFnTypedAttribute typedAttr;
    MFnLightDataAttribute lightAttr;

    MFnStringData strData;
    nameData = strData.create();
    strData.set(name);
    nameAttr = typedAttr.create("materialName", "mn", MFnData::kString, nameData, &status);
    CHECK_MSTATUS(typedAttr.setWritable(false));

    // Diffuse Color
    aColorR = numAttr.create("dfcolorR", "mcr", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5));
    aColorG = numAttr.create("dfcolorG", "mcg", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5));
    aColorB = numAttr.create("dfcolorB", "mcb", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5));
    aColor = numAttr.create("dfcolor", "mc", aColorR, aColorG, aColorB, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5, 0.5, 0.5));
    CHECK_MSTATUS(numAttr.setUsedAsColor(true));

    // Weight
    aWeight = numAttr.create("weight", "mw", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.85));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    // Roughness
    aRoughness = numAttr.create("roughness", "mrr", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    // Final Output Color
    aOutColorR = numAttr.create("outColorR", "ocr", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    aOutColorG = numAttr.create("outColorG", "ocg", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    aOutColorB = numAttr.create("outColorB", "ocb", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    aOutColor = numAttr.create("outColor", "oc", aOutColorR, aOutColorG, aOutColorB, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setHidden(false));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));

    // Camera Normals
    aNormalCameraX = numAttr.create("normalCameraX", "nx", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aNormalCameraY = numAttr.create("normalCameraY", "ny", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aNormalCameraZ = numAttr.create("normalCameraZ", "nz", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aNormalCamera = numAttr.create("normalCamera", "n", aNormalCameraX, aNormalCameraY, aNormalCameraZ, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f, 1.0f, 1.0f));
    CHECK_MSTATUS(numAttr.setHidden(true));

    // Light Direction
    aLightDirectionX = numAttr.create("lightDirectionX", "ldx", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aLightDirectionY = numAttr.create("lightDirectionY", "ldy", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aLightDirectionZ = numAttr.create("lightDirectionZ", "ldz", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aLightDirection = numAttr.create("lightDirection", "ld", aLightDirectionX, aLightDirectionY, aLightDirectionZ, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f, 1.0f, 1.0f));

    // Light Intensity
    aLightIntensityR = numAttr.create("lightIntensityR", "lir", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aLightIntensityG = numAttr.create("lightIntensityG", "lig", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aLightIntensityB = numAttr.create("lightIntensityB", "lib", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aLightIntensity = numAttr.create("lightIntensity", "li", aLightIntensityR, aLightIntensityG, aLightIntensityB, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f, 1.0f, 1.0f));

    // Light
    aLightAmbient = numAttr.create("lightAmbient", "la", MFnNumericData::kBoolean, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(true));
    aLightDiffuse = numAttr.create("lightDiffuse", "ldf", MFnNumericData::kBoolean, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(true));
    aLightSpecular = numAttr.create("lightSpecular", "ls", MFnNumericData::kBoolean, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(false));
    aLightShadowFraction = numAttr.create("lightShadowFraction", "lsf", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aPreShadowIntensity = numAttr.create("preShadowIntensity", "psi", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f));
    aLightBlindData = numAttr.createAddr("lightBlindData", "lbld", &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    aLightData = lightAttr.create("lightDataArray", "ltd", aLightDirection, aLightIntensity, aLightAmbient, aLightDiffuse, aLightSpecular,
                                  aLightShadowFraction, aPreShadowIntensity, aLightBlindData, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(lightAttr.setArray(true));
    CHECK_MSTATUS(lightAttr.setStorable(false));
    CHECK_MSTATUS(lightAttr.setHidden(true));
    CHECK_MSTATUS(lightAttr.setDefault(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true, true, false, 1.0f, 1.0f, NULL));

    // Set Attributes
    CHECK_MSTATUS(addAttribute(nameAttr));
    CHECK_MSTATUS(addAttribute(aColor));
    CHECK_MSTATUS(addAttribute(aWeight));
    CHECK_MSTATUS(addAttribute(aRoughness));
    CHECK_MSTATUS(addAttribute(aOutColor));

    CHECK_MSTATUS(addAttribute(aNormalCamera));

    CHECK_MSTATUS(addAttribute(aLightData));

    // Attribute Affects
    CHECK_MSTATUS(attributeAffects(aColorR, aOutColor));
    CHECK_MSTATUS(attributeAffects(aColorG, aOutColor));
    CHECK_MSTATUS(attributeAffects(aColorB, aOutColor));
    CHECK_MSTATUS(attributeAffects(aColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(aRoughness, aOutColor));
    CHECK_MSTATUS(attributeAffects(aWeight, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightIntensityR, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightIntensityB, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightIntensityG, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightIntensity, aOutColor));
    CHECK_MSTATUS(attributeAffects(aNormalCameraX, aOutColor));
    CHECK_MSTATUS(attributeAffects(aNormalCameraY, aOutColor));
    CHECK_MSTATUS(attributeAffects(aNormalCameraZ, aOutColor));
    CHECK_MSTATUS(attributeAffects(aNormalCamera, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightDirectionX, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightDirectionY, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightDirectionZ, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightDirection, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightAmbient, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightSpecular, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightDiffuse, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightShadowFraction, aOutColor));
    CHECK_MSTATUS(attributeAffects(aPreShadowIntensity, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightBlindData, aOutColor));
    CHECK_MSTATUS(attributeAffects(aLightData, aOutColor));

    return (MS::kSuccess);
}

MStatus MatteShaderNode::compute(const MPlug &plug, MDataBlock &block)
{
    if (plug == aOutColorR || plug == aOutColorG || plug == aOutColorB || plug == aOutColor)
    {
        MStatus status;
        MFloatVector resultColor(0.0, 0.0, 0.0);

        // Get Shading Params from input block
        MFloatVector &surfaceNormal = block.inputValue(aNormalCamera, &status).asFloatVector();
        CHECK_MSTATUS(status);
        MFloatVector &surfaceColor = block.inputValue(aColor, &status).asFloatVector();
        CHECK_MSTATUS(status);
        float &surfaceColorWeight = block.inputValue(aWeight, &status).asFloat();
        CHECK_MSTATUS(status);

        resultColor = surfaceColor;

        // Get light list
        MArrayDataHandle lightData = block.inputArrayValue(aLightData, &status);
        CHECK_MSTATUS(status);
        int numLights = lightData.elementCount(&status);
        CHECK_MSTATUS(status);

        for (int count = 1; count <= numLights; count++)
        {
            MDataHandle currentLight = lightData.inputValue(&status);
            CHECK_MSTATUS(status);

            MFloatVector &lightIntensity = currentLight.child(aLightIntensity).asFloatVector();

            // Find ambient component
            if (currentLight.child(aLightAmbient).asBool())
            {
                resultColor += lightIntensity;
            }

            // Find diffuse component
            if (currentLight.child(aLightDiffuse).asBool())
            {
                MFloatVector &lightDirection = currentLight.child(aLightDirection).asFloatVector();
                float cosln = lightDirection * surfaceNormal;

                if (cosln > 0.0f)
                {
                    resultColor += lightIntensity * cosln;
                }
            }

            // Advance to the next light.
            if (count < numLights)
            {
                status = lightData.next();
                CHECK_MSTATUS(status);
            }
        }

        // Factor incident light with surface color
        resultColor[0] = resultColor[0] * surfaceColor[0] * surfaceColorWeight;
        resultColor[1] = resultColor[1] * surfaceColor[1] * surfaceColorWeight;
        resultColor[2] = resultColor[2] * surfaceColor[2] * surfaceColorWeight;

        // Set ouput color attribute
        if (plug == aOutColorR || plug == aOutColorG || plug == aOutColorB || plug == aOutColor)
        {
            // Get the handle to the attribute
            MDataHandle outColorHandle = block.outputValue(aOutColor, &status);
            CHECK_MSTATUS(status);
            MFloatVector &outColor = outColorHandle.asFloatVector();

            outColor = resultColor;
            outColorHandle.setClean();
        }
    }
    else
    {
        return (MS::kUnknownParameter); // We got an unexpected plug
    }

    return (MS::kSuccess);
}