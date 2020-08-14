import os

import pymel.core as pm
import maya.cmds as mc
import maya.OpenMaya as om

from logger import logger
from menu import createMenu, deleteMenu
from renderer import (createRenderMelProcedure)
import globalsNode
from globalsNode import (
    createRenderTabsMelProcedure,
    renderBuildSettingsCallback,
    addRenderGlobalsScriptJob)


def register():
    logger.info("Registering Filianore Renderer.")

    pm.renderer("filianoreRenderer", rendererUIName="Filianore Renderer")

    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        createMenu()

    createRenderMelProcedure()
    pm.renderer("filianoreRenderer", edit=True,
                renderProcedure="filianoreRendererRenderProcedureProxy",
                renderRegionProcedure="mayaRenderRegion")

    createRenderTabsMelProcedure()
    renderBuildSettingsCallback("filianoreRenderer")
    pm.renderer("filianoreRenderer", edit=True,
                addGlobalsNode="defaultRenderGlobals")
    pm.renderer("filianoreRenderer", edit=True,
                addGlobalsNode="filianoreRendererGlobalsNode")

    pm.callbacks(addCallback=renderBuildSettingsCallback,
                 hook="renderSettingsBuilt",
                 owner="filianoreRenderer")

    addRenderGlobalsScriptJob()


def unregister():
    logger.info("Unregistering Filianore Renderer.")

    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        deleteMenu()

    mc.flushUndo()
    pm.callbacks(clearCallbacks=True, owner="filianoreRenderer")

    pm.renderer('filianoreRenderer', unregisterRenderer=True)
