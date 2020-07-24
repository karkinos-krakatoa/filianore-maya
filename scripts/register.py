import os

import pymel.core as pm
import maya.cmds as mc
import maya.OpenMaya as om

from logger import logger
from menu import createMenu, deleteMenu


def register():
    logger.info("Registering Filianore for Maya renderer.")
    
    # Menu
    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        createMenu()

def unregister():
    logger.info("Unregistering Filianore for Maya renderer.")

    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        deleteMenu()