#include "flstandardsurfaceshader.h"

#include <maya/MFloatVector.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MRenderUtil.h>

const MString FlStandardSurfaceShader::name("flStandardSurface");
const MTypeId FlStandardSurfaceShader::id(0x00132b44);

MObject FlStandardSurfaceShader::nameData;
MObject FlStandardSurfaceShader::nameAttr;

MObject FlStandardSurfaceShader::diffuseBaseColor;
MObject FlStandardSurfaceShader::diffuseBaseWeight;
MObject FlStandardSurfaceShader::diffuseBaseRoughness;

MObject FlStandardSurfaceShader::specularColor;
MObject FlStandardSurfaceShader::specularWeight;
MObject FlStandardSurfaceShader::specularRoughness;
MObject FlStandardSurfaceShader::specularAnisotropic;
MObject FlStandardSurfaceShader::specularType;
MObject FlStandardSurfaceShader::specularIOR;
MObject FlStandardSurfaceShader::metallicPreset;

MObject FlStandardSurfaceShader::transmissionColor;
MObject FlStandardSurfaceShader::transmissionWeight;

MObject FlStandardSurfaceShader::sheenColor;
MObject FlStandardSurfaceShader::sheenWeight;
MObject FlStandardSurfaceShader::sheenRoughness;

MObject FlStandardSurfaceShader::clearcoatColor;
MObject FlStandardSurfaceShader::clearcoatWeight;
MObject FlStandardSurfaceShader::clearcoatGloss;
MObject FlStandardSurfaceShader::clearcoatIOR;

MObject FlStandardSurfaceShader::thinFilmThickness;
MObject FlStandardSurfaceShader::thinFilmIOR;

MObject FlStandardSurfaceShader::aOutColor;

MObject FlStandardSurfaceShader::aPointCamera;
MObject FlStandardSurfaceShader::aNormalCamera;

MObject FlStandardSurfaceShader::aLightData;
MObject FlStandardSurfaceShader::aLightDirection;
MObject FlStandardSurfaceShader::aLightIntensity;
MObject FlStandardSurfaceShader::aLightAmbient;
MObject FlStandardSurfaceShader::aLightDiffuse;
MObject FlStandardSurfaceShader::aLightSpecular;
MObject FlStandardSurfaceShader::aLightShadowFraction;
MObject FlStandardSurfaceShader::aPreShadowIntensity;
MObject FlStandardSurfaceShader::aLightBlindData;

MObject FlStandardSurfaceShader::aRayDirection;
MObject FlStandardSurfaceShader::aRayOrigin;

MObject FlStandardSurfaceShader::aObjectId;
MObject FlStandardSurfaceShader::aRaySampler;
MObject FlStandardSurfaceShader::aRayDepth;
MObject FlStandardSurfaceShader::aTriangleNormalCamera;

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
    MFnEnumAttribute enumAttr;

    MFnStringData strData;
    MObject nameData = strData.create();
    strData.set(name);
    nameAttr = typedAttr.create("materialName", "mn", MFnData::kString, nameData, &status);
    CHECK_MSTATUS(typedAttr.setWritable(false));

    /** DIFFUSE **/
    diffuseBaseColor = numAttr.createColor("color", "dfcolor", &status);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0.6f, 0.6f, 0.6f));

    diffuseBaseWeight = numAttr.create("diffuseWeight", "dfweight", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0.85));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    diffuseBaseRoughness = numAttr.create("diffuseRoughness", "dfroughness", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    /** SPECULAR **/
    specularColor = numAttr.createColor("specColor", "speccolor", &status);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.f, 1.f, 1.f));

    specularWeight = numAttr.create("specularWeight", "specweight", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0.95));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    specularRoughness = numAttr.create("specularRoughness", "specroughness", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    specularAnisotropic = numAttr.create("specularAnisotropic", "specanisotropic", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(-1));
    CHECK_MSTATUS(numAttr.setMax(1));

    specularType = enumAttr.create("specularType", "specType", 0, &status);
    MAKE_INPUT(enumAttr);
    CHECK_MSTATUS(enumAttr.addField("Dielectric", 0));
    CHECK_MSTATUS(enumAttr.addField("Metallic", 1));
    CHECK_MSTATUS(enumAttr.addField("Wavelength-Dependent Metallic", 2));

    specularIOR = numAttr.create("specularIOR", "specior", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.50));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(10));

    metallicPreset = enumAttr.create("metallicPreset", "mmPreset", 0, &status);
    MAKE_INPUT(enumAttr);
    CHECK_MSTATUS(enumAttr.addField("Gold", 0));
    CHECK_MSTATUS(enumAttr.addField("Copper", 1));

    /** SHEEN **/
    sheenColor = numAttr.createColor("sheenColor", "shcolor", &status);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.f, 1.f, 1.f));

    sheenWeight = numAttr.create("sheenWeight", "shweight", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    sheenRoughness = numAttr.create("sheenRoughness", "shroughness", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    /** TRANSMISSION **/
    transmissionColor = numAttr.createColor("transmissionColor", "ttcolor", &status);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.f, 1.f, 1.f));

    transmissionWeight = numAttr.create("transmissionWeight", "ttweight", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    /** CLEARCOAT **/
    clearcoatColor = numAttr.createColor("clearcoatColor", "clrcolor", &status);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.f, 1.f, 1.f));

    clearcoatWeight = numAttr.create("clearcoatWeight", "clrweight", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    clearcoatGloss = numAttr.create("clearcoatGloss", "clrgloss", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(1));

    clearcoatIOR = numAttr.create("clearcoatIOR", "clrior", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.50));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(10));

    /** THIN FILM **/
    thinFilmThickness = numAttr.create("thinFilmThickness", "tfThickness", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(0));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(3000));

    thinFilmIOR = numAttr.create("thinFilmIOR", "tfIOR", MFnNumericData::kFloat);
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.50));
    CHECK_MSTATUS(numAttr.setMin(0));
    CHECK_MSTATUS(numAttr.setMax(10));

    /** OUT COLOR **/
    aOutColor = numAttr.createColor("outColor", "oc");
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setHidden(false));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));

    /** CAMERA NORMALS **/
    aPointCamera = numAttr.createPoint("pointCamera", "pc");
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f, 1.0f, 1.0f));
    CHECK_MSTATUS(numAttr.setHidden(true));

    aNormalCamera = numAttr.createPoint("normalCamera", "nc");
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f, 1.0f, 1.0f));
    CHECK_MSTATUS(numAttr.setHidden(true));

    aTriangleNormalCamera = numAttr.createPoint("triangleNormalCamera", "tn");
    MAKE_INPUT(numAttr);
    CHECK_MSTATUS(numAttr.setDefault(1.0f, 1.0f, 1.0f));
    CHECK_MSTATUS(numAttr.setHidden(true));

    /** RAY **/
    MObject RayX = numAttr.create("rayOx", "rxo", MFnNumericData::kFloat, 0.0);
    MObject RayY = numAttr.create("rayOy", "ryo", MFnNumericData::kFloat, 0.0);
    MObject RayZ = numAttr.create("rayOz", "rzo", MFnNumericData::kFloat, 0.0);
    aRayOrigin = numAttr.create("rayOrigin", "rog", RayX, RayY, RayZ);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(false));

    RayX = numAttr.create("rayDirectionX", "rdx", MFnNumericData::kFloat, 1.0);
    RayY = numAttr.create("rayDirectionY", "rdy", MFnNumericData::kFloat, 0.0);
    RayZ = numAttr.create("rayDirectionZ", "rdz", MFnNumericData::kFloat, 0.0);
    aRayDirection = numAttr.create("rayDirection", "rad", RayX, RayY, RayZ);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(false));

    // objectId
    aObjectId = numAttr.createAddr("objectId", "oi");
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(false));

    // raySampler
    aRaySampler = numAttr.createAddr("raySampler", "rtr");
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(false));

    // rayDepth
    aRayDepth = numAttr.create("rayDepth", "rd", MFnNumericData::kShort, 0.0);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(false));

    /** LIGHT PARAMS **/
    aLightDirection = numAttr.createPoint("lightDirection", "ld");
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numAttr.setStorable(false));
    CHECK_MSTATUS(numAttr.setHidden(true));
    CHECK_MSTATUS(numAttr.setReadable(true));
    CHECK_MSTATUS(numAttr.setWritable(false));
    CHECK_MSTATUS(numAttr.setDefault(1.0f, 1.0f, 1.0f));

    aLightIntensity = numAttr.createColor("lightIntensity", "li");
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
    CHECK_MSTATUS(lAttr.setDefault(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true, true, false, 0.0f, 1.0f, NULL));

    CHECK_MSTATUS(addAttribute(nameAttr));

    CHECK_MSTATUS(addAttribute(diffuseBaseColor));
    CHECK_MSTATUS(addAttribute(diffuseBaseWeight));
    CHECK_MSTATUS(addAttribute(diffuseBaseRoughness));

    CHECK_MSTATUS(addAttribute(specularColor));
    CHECK_MSTATUS(addAttribute(specularWeight));
    CHECK_MSTATUS(addAttribute(specularRoughness));
    CHECK_MSTATUS(addAttribute(specularAnisotropic));
    CHECK_MSTATUS(addAttribute(specularType));
    CHECK_MSTATUS(addAttribute(specularIOR));
    CHECK_MSTATUS(addAttribute(metallicPreset));

    CHECK_MSTATUS(addAttribute(sheenColor));
    CHECK_MSTATUS(addAttribute(sheenWeight));
    CHECK_MSTATUS(addAttribute(sheenRoughness));

    CHECK_MSTATUS(addAttribute(transmissionColor));
    CHECK_MSTATUS(addAttribute(transmissionWeight));

    CHECK_MSTATUS(addAttribute(clearcoatColor));
    CHECK_MSTATUS(addAttribute(clearcoatWeight));
    CHECK_MSTATUS(addAttribute(clearcoatGloss));
    CHECK_MSTATUS(addAttribute(clearcoatIOR));

    CHECK_MSTATUS(addAttribute(thinFilmThickness));
    CHECK_MSTATUS(addAttribute(thinFilmIOR));

    CHECK_MSTATUS(addAttribute(aOutColor));

    CHECK_MSTATUS(addAttribute(aPointCamera));
    CHECK_MSTATUS(addAttribute(aNormalCamera));
    CHECK_MSTATUS(addAttribute(aTriangleNormalCamera));

    CHECK_MSTATUS(addAttribute(aRayOrigin));
    CHECK_MSTATUS(addAttribute(aRayDirection));

    CHECK_MSTATUS(addAttribute(aObjectId));
    CHECK_MSTATUS(addAttribute(aRaySampler));
    CHECK_MSTATUS(addAttribute(aRayDepth));

    CHECK_MSTATUS(addAttribute(aLightData));

    //
    CHECK_MSTATUS(attributeAffects(diffuseBaseColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseRoughness, aOutColor));
    CHECK_MSTATUS(attributeAffects(diffuseBaseWeight, aOutColor));

    CHECK_MSTATUS(attributeAffects(specularColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularRoughness, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularWeight, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularAnisotropic, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularType, aOutColor));
    CHECK_MSTATUS(attributeAffects(specularIOR, aOutColor));
    CHECK_MSTATUS(attributeAffects(metallicPreset, aOutColor));

    CHECK_MSTATUS(attributeAffects(sheenColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(sheenWeight, aOutColor));
    CHECK_MSTATUS(attributeAffects(sheenRoughness, aOutColor));

    CHECK_MSTATUS(attributeAffects(transmissionColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(transmissionWeight, aOutColor));

    CHECK_MSTATUS(attributeAffects(clearcoatColor, aOutColor));
    CHECK_MSTATUS(attributeAffects(clearcoatWeight, aOutColor));
    CHECK_MSTATUS(attributeAffects(clearcoatGloss, aOutColor));
    CHECK_MSTATUS(attributeAffects(clearcoatIOR, aOutColor));

    CHECK_MSTATUS(attributeAffects(thinFilmThickness, aOutColor));
    CHECK_MSTATUS(attributeAffects(thinFilmIOR, aOutColor));

    CHECK_MSTATUS(attributeAffects(aPointCamera, aOutColor));
    CHECK_MSTATUS(attributeAffects(aNormalCamera, aOutColor));

    CHECK_MSTATUS(attributeAffects(aRayOrigin, aOutColor));
    CHECK_MSTATUS(attributeAffects(aRayDirection, aOutColor));

    CHECK_MSTATUS(attributeAffects(aLightIntensity, aOutColor));
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
    if ((plug != aOutColor) && (plug.parent() != aOutColor))
        return MS::kUnknownParameter;

    MFloatVector resultColor(0.0, 0.0, 0.0);

    // get sample surface shading parameters
    MFloatVector &surfaceNormal = block.inputValue(aNormalCamera).asFloatVector();
    MFloatVector &cameraPosition = block.inputValue(aPointCamera).asFloatVector();

    // use for raytracing api enhancement below
    MFloatVector point = cameraPosition;
    MFloatVector normal = surfaceNormal;

    MFloatVector &surfaceColor = block.inputValue(diffuseBaseColor).asFloatVector();
    float diffuseReflectivity = block.inputValue(diffuseBaseWeight).asFloat();

    // User-defined Reflection Color Gain
    float reflectGain = 0.5f;

    // Phong shading attributes
    float power = 20.f;
    float spec = block.inputValue(specularWeight).asFloat();

    float specularR, specularG, specularB;
    float diffuseR, diffuseG, diffuseB;
    diffuseR = diffuseG = diffuseB = specularR = specularG = specularB = 0.0;

    // get light list
    MArrayDataHandle lightData = block.inputArrayValue(aLightData);
    int numLights = lightData.elementCount();

    // iterate through light list and get ambient/diffuse values
    for (int count = 1; count <= numLights; count++)
    {
        MDataHandle currentLight = lightData.inputValue();
        MFloatVector &lightIntensity = currentLight.child(aLightIntensity).asFloatVector();

        // Find the blind data
        void *&blindData = currentLight.child(aLightBlindData).asAddr();

        // find ambient component
        if (currentLight.child(aLightAmbient).asBool())
        {
            diffuseR += lightIntensity[0];
            diffuseG += lightIntensity[1];
            diffuseB += lightIntensity[2];
        }

        MFloatVector &lightDirection = currentLight.child(aLightDirection).asFloatVector();

        if (blindData == NULL)
        {
            // find diffuse and specular component
            if (currentLight.child(aLightDiffuse).asBool())
            {
                float cosln = lightDirection * surfaceNormal;
                ;
                if (cosln > 0.0f) // calculate only if facing light
                {
                    diffuseR += lightIntensity[0] * (cosln * diffuseReflectivity);
                    diffuseG += lightIntensity[1] * (cosln * diffuseReflectivity);
                    diffuseB += lightIntensity[2] * (cosln * diffuseReflectivity);
                }

                CHECK_MSTATUS(cameraPosition.normalize());

                if (cosln > 0.0f) // calculate only if facing light
                {
                    float RV = (((2 * surfaceNormal) * cosln) - lightDirection) * cameraPosition;
                    if (RV > 0.0)
                        RV = 0.0;
                    if (RV < 0.0)
                        RV = -RV;

                    if (power < 0)
                        power = -power;

                    float s = spec * powf(RV, power);

                    specularR += lightIntensity[0] * s;
                    specularG += lightIntensity[1] * s;
                    specularB += lightIntensity[2] * s;
                }
            }
        }
        else
        {
            float cosln = MRenderUtil::diffuseReflectance(blindData, lightDirection, point, surfaceNormal, true);
            if (cosln > 0.0f) // calculate only if facing light
            {
                diffuseR += lightIntensity[0] * (cosln * diffuseReflectivity);
                diffuseG += lightIntensity[1] * (cosln * diffuseReflectivity);
                diffuseB += lightIntensity[2] * (cosln * diffuseReflectivity);
            }

            CHECK_MSTATUS(cameraPosition.normalize());

            if (currentLight.child(aLightSpecular).asBool())
            {
                MFloatVector specLightDirection = lightDirection;
                MDataHandle directionH = block.inputValue(aRayDirection);
                MFloatVector direction = directionH.asFloatVector();
                float lightAttenuation = 1.0;

                specLightDirection = MRenderUtil::maximumSpecularReflection(blindData,
                                                                            lightDirection, point, surfaceNormal, direction);
                lightAttenuation = MRenderUtil::lightAttenuation(blindData, point, surfaceNormal, false);

                // Are we facing the light
                if (specLightDirection * surfaceNormal > 0.0f)
                {
                    float power2 = 20.f;
                    MFloatVector rv = 2 * surfaceNormal * (surfaceNormal * direction) - direction;
                    float s = spec * powf(rv * specLightDirection, power2);

                    specularR += lightIntensity[0] * s * lightAttenuation;
                    specularG += lightIntensity[1] * s * lightAttenuation;
                    specularB += lightIntensity[2] * s * lightAttenuation;
                }
            }
        }
        if (!lightData.next())
            break;
    }

    float transWeight = block.inputValue(transmissionWeight).asFloat();
    resultColor[0] = (diffuseR * surfaceColor[0]) + specularR;
    resultColor[1] = (diffuseG * surfaceColor[1]) + specularG;
    resultColor[2] = (diffuseB * surfaceColor[2]) + specularB;

    // add the reflection color
    if (reflectGain > 0.0)
    {

        MStatus status;

        // required attributes for using raytracer
        // origin, direction, sampler, depth, and object id.
        //
        MDataHandle originH = block.inputValue(aRayOrigin, &status);
        MFloatVector origin = originH.asFloatVector();

        MDataHandle directionH = block.inputValue(aRayDirection, &status);
        MFloatVector direction = directionH.asFloatVector();

        MDataHandle samplerH = block.inputValue(aRaySampler, &status);
        void *&samplerPtr = samplerH.asAddr();

        MDataHandle depthH = block.inputValue(aRayDepth, &status);
        short depth = depthH.asShort();

        MDataHandle objH = block.inputValue(aObjectId, &status);
        void *&objId = objH.asAddr();

        MFloatVector reflectColor;
        MFloatVector reflectTransparency;

        MFloatVector &triangleNormal = block.inputValue(aTriangleNormalCamera).asFloatVector();

        // compute reflected ray
        MFloatVector l = -direction;
        float dot = l * normal;
        if (dot < 0.0)
            dot = -dot;
        MFloatVector refVector = 2 * normal * dot - l; // reflection ray
        float dotRef = refVector * triangleNormal;
        if (dotRef < 0.0)
        {
            const float s = 0.01f;
            MFloatVector mVec = refVector - dotRef * triangleNormal;
            mVec.normalize();
            refVector = mVec + s * triangleNormal;
        }
        CHECK_MSTATUS(refVector.normalize());

        status = MRenderUtil::raytrace(
            point,        //  origin
            refVector,    //  direction
            objId,        //  object id
            samplerPtr,   //  sampler info
            depth,        //  ray depth
            reflectColor, // output color and transp
            reflectTransparency);

        // add in the reflection color
        resultColor[0] += reflectGain * (reflectColor[0]);
        resultColor[1] += reflectGain * (reflectColor[1]);
        resultColor[2] += reflectGain * (reflectColor[2]);
    }

    // set ouput color attribute
    MDataHandle outColorHandle = block.outputValue(aOutColor);
    MFloatVector &outColor = outColorHandle.asFloatVector();
    outColor = resultColor;
    outColorHandle.setClean();

    return MS::kSuccess;
}