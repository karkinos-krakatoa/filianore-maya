#include "flplasticshader.h"

#include <maya/MFloatVector.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnLightDataAttribute.h>

const MString FlPlasticShader::name("flPlastic");
const MTypeId FlPlasticShader::id(0x00132b43);

MObject FlPlasticShader::nameData;
MObject FlPlasticShader::nameAttr;

MObject FlPlasticShader::diffuseBaseColor;
MObject FlPlasticShader::diffuseBaseWeight;

MObject FlPlasticShader::specularColor;
MObject FlPlasticShader::specularWeight;
MObject FlPlasticShader::specularRoughness;
MObject FlPlasticShader::specularAnisotropic;
MObject FlPlasticShader::specularIOR;

MObject FlPlasticShader::aOutColor;
MObject FlPlasticShader::aOutColorR;
MObject FlPlasticShader::aOutColorG;
MObject FlPlasticShader::aOutColorB;

MObject FlPlasticShader::aNormalCamera;
MObject FlPlasticShader::aNormalCameraX;
MObject FlPlasticShader::aNormalCameraY;
MObject FlPlasticShader::aNormalCameraZ;
MObject FlPlasticShader::aLightData;
MObject FlPlasticShader::aLightDirection;
MObject FlPlasticShader::aLightDirectionX;
MObject FlPlasticShader::aLightDirectionY;
MObject FlPlasticShader::aLightDirectionZ;
MObject FlPlasticShader::aLightIntensity;
MObject FlPlasticShader::aLightIntensityR;
MObject FlPlasticShader::aLightIntensityG;
MObject FlPlasticShader::aLightIntensityB;
MObject FlPlasticShader::aLightAmbient;
MObject FlPlasticShader::aLightDiffuse;
MObject FlPlasticShader::aLightSpecular;
MObject FlPlasticShader::aLightShadowFraction;
MObject FlPlasticShader::aPreShadowIntensity;
MObject FlPlasticShader::aLightBlindData;

#define MAKE_INPUT(attr)                   \
    CHECK_MSTATUS(attr.setKeyable(true));  \
    CHECK_MSTATUS(attr.setStorable(true)); \
    CHECK_MSTATUS(attr.setReadable(true)); \
    CHECK_MSTATUS(attr.setWritable(true));

#define MAKE_OUTPUT(attr)                   \
    CHECK_MSTATUS(attr.setKeyable(false));  \
    CHECK_MSTATUS(attr.setStorable(false)); \
    CHECK_MSTATUS(attr.setReadable(true));  \
    CHECK_MSTATUS(attr.setWritable(false));

FlPlasticShader::FlPlasticShader()
{
}

FlPlasticShader::~FlPlasticShader()
{
}

void *FlPlasticShader::creator()
{
    return new FlPlasticShader();
}

MStatus FlPlasticShader::initialize()
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

    /** DIFFUSE **/
    // Color
    diffuseBaseColor = numAttr.createColor("color", "dfcolor", &status);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0.6f, 0.6f, 0.6f));

    // Weight
    diffuseBaseWeight = numAttr.create("diffuseWeight", "dfweight", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0.85));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    /** SPECULAR **/
    // Color
    specularColor = numAttr.createColor("specularColor", "speccolor", &status);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.f, 1.f, 1.f));
    // Weight
    specularWeight = numAttr.create("specularWeight", "specweight", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0.95));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));
    // Roughness
    specularRoughness = numAttr.create("specularRoughness", "specroughness", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));
    // Anisotropic
    specularAnisotropic = numAttr.create("specularAnisotropic", "specanisotropic", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(-1));
    CHECK_MSTATUS(numAttr.setMax(1));
    // IOR
    specularIOR = numAttr.create("specularIOR", "specanior", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.52));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(3));

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

    CHECK_MSTATUS(addAttribute(specularColor));
    CHECK_MSTATUS(addAttribute(specularWeight));
    CHECK_MSTATUS(addAttribute(specularRoughness));
    CHECK_MSTATUS(addAttribute(specularAnisotropic));
    CHECK_MSTATUS(addAttribute(specularIOR));

    CHECK_MSTATUS(addAttribute(aOutColor));

    CHECK_MSTATUS(addAttribute(aNormalCamera));

    CHECK_MSTATUS(addAttribute(aLightData));

    //
    CHECK_MSTATUS(attributeAffects(diffuseBaseColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseWeight, aOutColor));

    CHECK_MSTATUS(attributeAffects(specularColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularRoughness, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularWeight, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularAnisotropic, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularIOR, aOutColor));

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

MStatus FlPlasticShader::compute(const MPlug &plug, MDataBlock &block)
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