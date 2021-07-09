#include "flstandardsurfaceshader.h"

#include <maya/MFloatVector.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnLightDataAttribute.h>

const MString FlStandardSurfaceShader::name("flStandardSurface");
const MTypeId FlStandardSurfaceShader::id(0x00132b44);

MObject FlStandardSurfaceShader::nameData;
MObject FlStandardSurfaceShader::nameAttr;

MObject FlStandardSurfaceShader::diffuseBaseColor;
MObject FlStandardSurfaceShader::diffuseBaseColorR;
MObject FlStandardSurfaceShader::diffuseBaseColorG;
MObject FlStandardSurfaceShader::diffuseBaseColorB;
MObject FlStandardSurfaceShader::diffuseBaseWeight;
MObject FlStandardSurfaceShader::diffuseBaseRoughness;

MObject FlStandardSurfaceShader::aOutColor;
MObject FlStandardSurfaceShader::aOutColorR;
MObject FlStandardSurfaceShader::aOutColorG;
MObject FlStandardSurfaceShader::aOutColorB;

MObject FlStandardSurfaceShader::aNormalCamera;
MObject FlStandardSurfaceShader::aNormalCameraX;
MObject FlStandardSurfaceShader::aNormalCameraY;
MObject FlStandardSurfaceShader::aNormalCameraZ;
MObject FlStandardSurfaceShader::aLightData;
MObject FlStandardSurfaceShader::aLightDirection;
MObject FlStandardSurfaceShader::aLightDirectionX;
MObject FlStandardSurfaceShader::aLightDirectionY;
MObject FlStandardSurfaceShader::aLightDirectionZ;
MObject FlStandardSurfaceShader::aLightIntensity;
MObject FlStandardSurfaceShader::aLightIntensityR;
MObject FlStandardSurfaceShader::aLightIntensityG;
MObject FlStandardSurfaceShader::aLightIntensityB;
MObject FlStandardSurfaceShader::aLightAmbient;
MObject FlStandardSurfaceShader::aLightDiffuse;
MObject FlStandardSurfaceShader::aLightSpecular;
MObject FlStandardSurfaceShader::aLightShadowFraction;
MObject FlStandardSurfaceShader::aPreShadowIntensity;
MObject FlStandardSurfaceShader::aLightBlindData;

FlStandardSurfaceShader::FlStandardSurfaceShader()
{
}

FlStandardSurfaceShader::~FlStandardSurfaceShader()
{
}

void *FlStandardSurfaceShader::creator()
{
    return new FlStandardSurfaceShader();
}

MStatus FlStandardSurfaceShader::initialize()
{
    MStatus status;
    MFnNumericAttribute numAttr;
    MFnLightDataAttribute lAttr;
    MFnTypedAttribute typedAttr;

    MFnStringData strData;
    MObject nameData = strData.create();
    strData.set(name);
    nameAttr = typedAttr.create("materialName", "mn", MFnData::kString, nameData, &status);
    CHECK_MSTATUS(typedAttr.setWritable(false));

    /** BASE DIFFUSE **/
    // Color
    diffuseBaseColorR = numAttr.create("dfcolorR", "dfcolorR", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5));
    diffuseBaseColorG = numAttr.create("dfcolorG", "dfcolorG", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5));
    diffuseBaseColorB = numAttr.create("dfcolorB", "dfcolorB", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5));
    diffuseBaseColor = numAttr.create("Color", "dfcolor", diffuseBaseColorR, diffuseBaseColorG, diffuseBaseColorB, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.5, 0.5, 0.5));
    CHECK_MSTATUS(numAttr.setUsedAsColor(true));
    // Weight
    diffuseBaseWeight = numAttr.create("Weight", "dfweight", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0.85));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));
    // Roughness
    diffuseBaseRoughness = numAttr.create("Roughness", "dfroughness", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    /** OUT COLOR **/
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

    /** CAMERA NORMALS **/
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

    /** LIGHT DIRECTION **/
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

    /** LIGHT INTENSITY **/
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

    /** LIGHT **/
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

    aLightData = lAttr.create("lightDataArray", "ltd", aLightDirection,
                              aLightIntensity, aLightAmbient, aLightDiffuse, aLightSpecular,
                              aLightShadowFraction, aPreShadowIntensity, aLightBlindData,
                              &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(lAttr.setArray(true));
    CHECK_MSTATUS(lAttr.setStorable(false));
    CHECK_MSTATUS(lAttr.setHidden(true));
    CHECK_MSTATUS(lAttr.setDefault(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true, true, false, 1.0f, 1.0f, NULL));

    CHECK_MSTATUS(addAttribute(nameAttr));
    CHECK_MSTATUS(addAttribute(diffuseBaseColor));
    CHECK_MSTATUS(addAttribute(diffuseBaseWeight));
    CHECK_MSTATUS(addAttribute(diffuseBaseRoughness));
    CHECK_MSTATUS(addAttribute(aOutColor));

    CHECK_MSTATUS(addAttribute(aNormalCamera));

    CHECK_MSTATUS(addAttribute(aLightData));

    CHECK_MSTATUS(attributeAffects(diffuseBaseColorR, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseColorG, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseColorB, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseRoughness, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseWeight, aOutColor));
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

MStatus FlStandardSurfaceShader::compute(const MPlug &plug, MDataBlock &block)
{
    if (plug == aOutColorR || plug == aOutColorG || plug == aOutColorB || plug == aOutColor)
    {
        MStatus status;
        MFloatVector resultColor(0.0, 0.0, 0.0);

        // Get Shading Params from input block
        MFloatVector &surfaceNormal = block.inputValue(aNormalCamera, &status).asFloatVector();
        CHECK_MSTATUS(status);
        MFloatVector &surfaceColor = block.inputValue(diffuseBaseColor, &status).asFloatVector();
        CHECK_MSTATUS(status);
        float &surfaceColorWeight = block.inputValue(diffuseBaseWeight, &status).asFloat();
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