#include "mirrormaterialnode.h"

#include <maya/MFloatVector.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnLightDataAttribute.h>

const MString MirrorMaterialNode::name("flMirrorShader");
const MTypeId MirrorMaterialNode::id(0x00132b43);

MObject MirrorMaterialNode::nameData;
MObject MirrorMaterialNode::nameAttr;

MObject MirrorMaterialNode::aColor;
MObject MirrorMaterialNode::aColorR;
MObject MirrorMaterialNode::aColorG;
MObject MirrorMaterialNode::aColorB;
MObject MirrorMaterialNode::aOutColor;
MObject MirrorMaterialNode::aOutColorR;
MObject MirrorMaterialNode::aOutColorG;
MObject MirrorMaterialNode::aOutColorB;

MObject MirrorMaterialNode::aNormalCamera;
MObject MirrorMaterialNode::aNormalCameraX;
MObject MirrorMaterialNode::aNormalCameraY;
MObject MirrorMaterialNode::aNormalCameraZ;
MObject MirrorMaterialNode::aLightData;
MObject MirrorMaterialNode::aLightDirection;
MObject MirrorMaterialNode::aLightDirectionX;
MObject MirrorMaterialNode::aLightDirectionY;
MObject MirrorMaterialNode::aLightDirectionZ;
MObject MirrorMaterialNode::aLightIntensity;
MObject MirrorMaterialNode::aLightIntensityR;
MObject MirrorMaterialNode::aLightIntensityG;
MObject MirrorMaterialNode::aLightIntensityB;
MObject MirrorMaterialNode::aLightAmbient;
MObject MirrorMaterialNode::aLightDiffuse;
MObject MirrorMaterialNode::aLightSpecular;
MObject MirrorMaterialNode::aLightShadowFraction;
MObject MirrorMaterialNode::aPreShadowIntensity;
MObject MirrorMaterialNode::aLightBlindData;

void *MirrorMaterialNode::creator()
{
    return (new MirrorMaterialNode);
}

MStatus MirrorMaterialNode::initialize()
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

    // Main Specular Color
    aColorR = numAttr.create("aColorR", "rcr", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setStorable(true));
    CHECK_MSTATUS(numAttr.setDefault(1.f));

    aColorG = numAttr.create("aColorG", "rcg", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setStorable(true));
    CHECK_MSTATUS(numAttr.setDefault(1.f));

    aColorB = numAttr.create("aColorB", "rcb", MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setStorable(true));
    CHECK_MSTATUS(numAttr.setDefault(1.f));

    aColor = numAttr.create("aColor", "rc", aColorR, aColorG, aColorB, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setKeyable(true));
    CHECK_MSTATUS(numAttr.setStorable(true));
    CHECK_MSTATUS(numAttr.setDefault(1.f, 1.f, 1.f));
    CHECK_MSTATUS(numAttr.setUsedAsColor(true));

    // Out color
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

    aLightData = lAttr.create("lightDataArray", "ltd", aLightDirection,
                              aLightIntensity, aLightAmbient, aLightDiffuse, aLightSpecular,
                              aLightShadowFraction, aPreShadowIntensity, aLightBlindData,
                              &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(lAttr.setArray(true));
    CHECK_MSTATUS(lAttr.setStorable(false));
    CHECK_MSTATUS(lAttr.setHidden(true));
    CHECK_MSTATUS(lAttr.setDefault(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                   true, true, false, 1.0f, 1.0f, NULL));

    CHECK_MSTATUS(addAttribute(nameAttr));
    CHECK_MSTATUS(addAttribute(aColor));
    CHECK_MSTATUS(addAttribute(aOutColor));
    CHECK_MSTATUS(addAttribute(aNormalCamera));
    CHECK_MSTATUS(addAttribute(aLightData));

    CHECK_MSTATUS(attributeAffects(aColorR, aOutColor));
    CHECK_MSTATUS(attributeAffects(aColorG, aOutColor));
    CHECK_MSTATUS(attributeAffects(aColorB, aOutColor));
    CHECK_MSTATUS(attributeAffects(aColor, aOutColor));
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

MStatus MirrorMaterialNode::compute(const MPlug &plug, MDataBlock &block)
{
    MStatus status;
    MFloatVector resultColor(0.0, 0.0, 0.0);

    MFloatVector &surfaceColor = block.inputValue(aColor, &status).asFloatVector();
    CHECK_MSTATUS(status);

    MFloatVector &surfaceNormal = block.inputValue(aNormalCamera, &status).asFloatVector();
    CHECK_MSTATUS(status);

    CHECK_MSTATUS(status);

    resultColor = surfaceColor;

    // start
    MArrayDataHandle lightData = block.inputArrayValue(aLightData, &status);
    CHECK_MSTATUS(status);

    int numLights = lightData.elementCount(&status);
    CHECK_MSTATUS(status);

    // Calculate the effect of the lights in the scene on the color
    //

    // Iterate through light list and get ambient/diffuse values
    //
    for (int count = 1; count <= numLights; count++)
    {
        // Get the current light out of the array
        //
        MDataHandle currentLight = lightData.inputValue(&status);
        CHECK_MSTATUS(status);

        // Get the intensity of that light
        //
        MFloatVector &lightIntensity = currentLight.child(aLightIntensity).asFloatVector();

        // Find ambient component
        //
        if (currentLight.child(aLightAmbient).asBool())
        {
            resultColor += lightIntensity;
        }

        // Find diffuse component
        //
        if (currentLight.child(aLightDiffuse).asBool())
        {
            MFloatVector &lightDirection = currentLight.child(aLightDirection).asFloatVector();
            float cosln = lightDirection * surfaceNormal;

            if (cosln > 0.0f)
            {
                resultColor += lightIntensity * (cosln * 60.f);
            }
        }

        // Advance to the next light.
        //
        if (count < numLights)
        {
            status = lightData.next();
            CHECK_MSTATUS(status);
        }
    }

    // Factor incident light with surface color and add incandescence
    //
    resultColor[0] = resultColor[0] * surfaceColor[0];
    resultColor[1] = resultColor[1] * surfaceColor[1];
    resultColor[2] = resultColor[2] * surfaceColor[2];

    if (plug == aOutColor || plug == aOutColorR || plug == aOutColorG || plug == aOutColorB)
    {
        MDataHandle outColorHandle = block.outputValue(aOutColor, &status);
        CHECK_MSTATUS(status);
        MFloatVector &outColor = outColorHandle.asFloatVector();

        outColor = resultColor;
        outColorHandle.setClean();
    }
    return (MS::kSuccess);
}
