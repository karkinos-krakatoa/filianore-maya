import maya.mel as mel


def createRenderMelProcedure():
    mel.eval('''
        global proc filianoreRendererRenderProcedureProxy(int $width, int $height, int $doShadows, int $doGlowPass, string $camera, string $option)
        {
            python("import globalsNode");
            python("globalsNode.createGlobalsNode()");
            filianoreRendererRenderProcedure -w $width -h $height -c $camera;
        }''')

    mel.eval('''
        global proc filianoreRendererRenderRegionProcedureProxy(int $width, int $height, int $doShadows, int $doGlowPass, string $camera, string $option)
        {
            python("import globalsNode");
            python("globalsNode.createGlobalsNode()");
            filianoreRendererRenderRegionProcedure -w $width -h $height -c $camera;
        }''')
