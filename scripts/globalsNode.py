import maya.OpenMaya as om
import maya.cmds as mc
import maya.mel as mel
import pymel.core as pm


def createGlobalsNode():
    if mc.objExists("filianoreRendererGlobalsNode"):
        return

    mc.createNode(
        "filianoreRendererGlobalsNode",
        name="filianoreRendererGlobalsNode",
        shared=True,
        skipSelect=True)


def createRenderTabsMelProcedure():
    pm.mel.source("createMayaSoftwareCommonGlobalsTab.mel")

    mel.eval('''
    global proc filianoreRendererUpdateCommonTabProcedure()
    {
        updateMayaSoftwareCommonGlobalsTab();
    }
    ''')

    mel.eval('''
    global proc filianoreRendererCreateMainTabProcedure()
    {
        python("import globalsNode");
        python("globalsNode.g_filianoreRendererCommonTab.create()");
    }
    ''')

    mel.eval('''
    global proc filianoreRendererEmptyUpdateProcedure()
    {
    }
    ''')


def renderBuildSettingsCallback(renderer):
    pm.renderer("filianoreRenderer", edit=True,
                addGlobalsTab=(
                    "Common",
                    "createMayaSoftwareCommonGlobalsTab",
                    "filianoreRendererUpdateCommonTabProcedure"))

    pm.renderer("filianoreRenderer", edit=True,
                addGlobalsTab=(
                    "FilianoreRenderer",
                    "filianoreRendererCreateMainTabProcedure",
                    "filianoreRendererEmptyUpdateProcedure"))


def addRenderGlobalsScriptJob():
    mc.scriptJob(
        attributeChange=[
            "defaultRenderGlobals.currentRenderer",
            "import globalsNode; globalsNode.currentRendererChanged()"])


def currentRendererChanged():
    newRenderer = mel.eval("currentRenderer()")
    if newRenderer != "filianoreRenderer":
        return

    createGlobalsNode()


class FilianoreRendererGenericTab(object):

    def __init__(self):
        self._uis = {}

    def _addControl(self, ui, attrName, connectIndex=2):
        self._uis[attrName] = ui
        pm.connectControl(ui, "filianoreRendererGlobalsNode." +
                          attrName, index=connectIndex)

    def _getAttributeMenuItems(self, attrName):
        attr = pm.Attribute("filianoreRendererGlobalsNode." + attrName)
        menuItems = [
            (i, v) for i, v in enumerate(attr.getEnums().keys())
        ]
        return menuItems


class FilianoreRendererCommonTab(FilianoreRendererGenericTab):

    def create(self):
        createGlobalsNode()

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)

        with pm.scrollLayout("filianoreRendererScrollLayout", horizontalScrollBarThickness=0):
            with pm.columnLayout("filianoreRendererColumnLayout", adjustableColumn=True, width=400):

                with pm.frameLayout("generalFrameLayout", label="General", collapsable=True, collapse=False):
                    with pm.columnLayout("generalColumnLayout", adjustableColumn=True, width=380,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        pm.attrFieldSliderGrp(
                            label="Sample Count:",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=1,
                            maxValue=12000,
                            fieldMinValue=1,
                            fieldMaxValue=12000,
                            attribute="filianoreRendererGlobalsNode.samples")

                with pm.frameLayout("rayDepthFrameLayout", label="Ray Bounces", collapsable=True, collapse=False):
                    with pm.columnLayout("rayDepthColumnLayout", adjustableColumn=True, width=380,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        pm.attrFieldSliderGrp(
                            label="Diffuse:",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=0,
                            maxValue=100,
                            fieldMinValue=0,
                            fieldMaxValue=100,
                            attribute="filianoreRendererGlobalsNode.diffuseRayDepth")

                        pm.separator(height=2)

                        pm.attrFieldSliderGrp(
                            label="Specular:",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=0,
                            maxValue=100,
                            fieldMinValue=0,
                            fieldMaxValue=100,
                            attribute="filianoreRendererGlobalsNode.specularRayDepth")

        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(
            parentForm,
            edit=True,
            attachForm=[
                ("filianoreRendererScrollLayout", "top", 0),
                ("filianoreRendererScrollLayout", "bottom", 0),
                ("filianoreRendererScrollLayout", "left", 0),
                ("filianoreRendererScrollLayout", "right", 0)])

    def update(self):
        assert mc.objExists("filianoreRendererGlobalsNode")


g_filianoreRendererCommonTab = FilianoreRendererCommonTab()
