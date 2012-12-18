using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {

    public partial class Main : TorqueScriptTemplate
        {
        private dnTorque dnt;
        public Main(ref dnTorque c)
            : base(ref c)
            {
            dnt = c;
            }
        /*
         * 
         * Defaults.cs          1000
         * init.cs              2000
         * PlayerList           3000
         * ChatHud              4000
         * playgui.cs           4900
         * MessageHud           5000
         * Client               6000
         * Game                 7000
         * MissionDownload      8000
         * ServerConnection     9000
         * Shaders              10000
         * default.bind         11000
         * AudioData            12000
         * 
         * core                 20000
         * audio                21000
         * canvas               22000
         * cursor               23000
         * profiles             24000
         * GuiCursors           25000
         * audioEnviroments     26000
         * audioDescriptions    27000
         * audioStates          28000
         * audioAmbiences       29000   
         * 
         * help                 30000
         * screenshot           31000
         * scriptDoc            32000
         * helperfuncs          33000
         * commands             34000
         * devHelpers           35000
         * metrics              36000
         * recordings           37000
         * centerPrint          38000
         * 
         * commonMaterialData   40000
         * shaders              41000
         * materials            42000
         * terrainBlock         43000
         * water                44000
         * imposter             45000
         * scatterSky           46000
         * clouds               47000
         * postfx               48000
         * 
         * message              50000
         * mission              51000
         * missionDownload      52000
         * actionMap            53000
         * rendermanager        54000
         * lighting             55000
         * 
         * 
         * client/lighting/advanced/shaders.cs  60000
         * client/lighting/advanced/lightViz.cs         61000
         * client/lighting/advanced/shadowViz.cs        62000
         * client/lighting/advanced/shadowViz.gui       63000
         * client/lighting/advanced/init.cs             64000
         * client/lighting/basic/init.cs                65000
         * client/lighting/basic/shadowFilter           66000
         * client/lighting/shadowMaps/init              67000
         * 
         * client   70000
         * default  80000
         * 
         * postFX/chromaticLens                         90000
         * postFX/default.postfxpreset                  91000
         * postFX/dof                                   92000
         * postFX/edgeAA                                93000
         * postFX/flash                                 94000
         * postFX/Fog                                   95000
         * postFX/fxaa                                  96000
         * postFX/GammaPostFX                           97000
         * postFX/Glow                                  98000
         * postFX/HDR                                   99000
         * postFX/lightRay                              100000
         * postFX/MLAA                                  101000
         * postFX/postFXManager_Gui_cs                  102000
         * postFX/postFXManager_gui_cs_cs               103000
         * postfx/postfxmanagerguid_settings            104000
         * postFX/postFXManager.persistance             105000
         * postFX/ssao                                  106000
         */

        public const string default_color_correction = "art/postFX/null_color_ramp.png";

        public const string areamap33_dds = "art/postFX/AreaMap33.dds";

        public const string noise_png = "art/postFX/noise.png";


        public void Load_Defaults()
            {
            dnt.DoScriptInjection(ScriptType.Client, 1000, 1000);
            }
        public void Load_ClientInit()
            {
            //Calls the Init.cs file.
            dnt.DoScriptInjection(ScriptType.Client, 2000, 2000);
            }
        }

    }
