import os

import maya.cmds as mc
import maya.mel as mel

from logger import logger


def showAbout():
    if mc.window('filianoreAboutDialog', query=True, exists=True):
        mc.deleteUI('filianoreAboutDialog')

    window = mc.window('filianoreAboutDialog', title='About')

    mc.columnLayout(rs=20, columnOffset=['both', 22], width=400)

    mc.text(label='')

    mc.text(
        label='Filianore for Maya',
        font='boldLabelFont',
        align='center')

    mc.text(
        label='Copyright (c) 2020 Frederick Loh',
        font='boldLabelFont',
        align='center')

    # Add some empty space. Is there a better way to do this?
    mc.text(label='')

    mc.setParent('..')
    mc.showWindow(window)


__g_filianoreMenu = None


def createMenu():
    logger.debug("Creating Filianore menu.")

    global __g_filianoreMenu
    deleteMenu()

    gMainWindow = mel.eval('$temp1=$gMainWindow')
    __g_filianoreMenu = mc.menu(
        'filianoreMenu', 
        parent=gMainWindow, 
        label='Filianore', 
        tearOff=True)

    mc.menuItem(
        label='About',
        parent='filianoreMenu',
        command='import menu; menu.showAbout()')


def deleteMenu():
    global __g_filianoreMenu

    try:
        mc.deleteUI(__g_filianoreMenu)
        logger.debug("deleted Filianore menu.")
    except:
        pass
