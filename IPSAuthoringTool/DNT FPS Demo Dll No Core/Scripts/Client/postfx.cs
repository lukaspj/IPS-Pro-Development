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
        //------------------------------------------------------------------------------
        // CloudLayer
        //------------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "initialize_postFx", "", 0, 48000, true)]
        public void initialize_postFx()
            {

            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "PFX_DefaultStateBlock");


            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_PassthruShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/passthruP.hlsl");
            //   OGLVertexShaderFile  = "shaders/common/postFx/gl//postFxV.glsl";
            //   OGLPixelShaderFile   = "shaders/common/postFx/gl/passthruP.glsl";
            ts.PropsAddString("samplerNames[0]", "$inputTex");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);
            }

        //I should use a script inject here, versus reading the 
        //the torque scripts.  I'll need to make a second pass.
        [Torque_Decorations.TorqueCallBack("", "", "initPostEffects", "", 0, 48001, false)]
        public void initPostEffects()
            {
            // First exec the scripts for the different light managers
            // in the lighting folder.

            dnt.DoScriptInjection(ScriptType.Client, 90000, 90999);//chromaticLens
            dnt.DoScriptInjection(ScriptType.Client, 91000, 91999);//default.postfxpreset
            dnt.DoScriptInjection(ScriptType.Client, 92000, 92999);//dof
            dnt.DoScriptInjection(ScriptType.Client, 93000, 93999);//edgeAA
            dnt.DoScriptInjection(ScriptType.Client, 94000, 94999);//flash
            dnt.DoScriptInjection(ScriptType.Client, 95000, 95999);//Fog

            dnt.DoScriptInjection(ScriptType.Client, 96000, 96999);//fxaa
            dnt.DoScriptInjection(ScriptType.Client, 97000, 97999);//GammaPostFX
            dnt.DoScriptInjection(ScriptType.Client, 98000, 98999);//Glow
            dnt.DoScriptInjection(ScriptType.Client, 99000, 99999);//HDR


            dnt.DoScriptInjection(ScriptType.Client, 100000, 100999);//lightRay
            dnt.DoScriptInjection(ScriptType.Client, 101000, 101999);//MLAA
            dnt.DoScriptInjection(ScriptType.Client, 106000, 106999);//ssao

            dnt.DoScriptInjection(ScriptType.Client, 102000, 102999);
            dnt.DoScriptInjection(ScriptType.Client, 103000, 103999);
            dnt.DoScriptInjection(ScriptType.Client, 104000, 104999);
            dnt.DoScriptInjection(ScriptType.Client, 105000, 105999);


            }

        [Torque_Decorations.TorqueCallBack("", "PostEffect", "inspectVars", "this", 1, 48002, false)]
        public void PostEffectinspectVars(string thisobj)
            {
            string name = console.getName(thisobj);
            string globals = "$" + name + "::*";
            console.Call("inspectVars", new string[] { globals });
            }
        [Torque_Decorations.TorqueCallBack("", "PostEffect", "viewDisassembly", "this", 1, 48003, false)]
        public void PostEffectviewDisassembly(string thisobj)
            {
            string file = console.Call(thisobj, "dumpShaderDisassembly");
            if (file != "")
                {
                console.print("PostEffect::viewDisassembly - no shader disassembly found.");
                }
            else
                {
                console.print("PostEffect::viewDisassembly - shader disassembly file dumped ( " + file + " ).");
                Util.openFile(file);
                }

            }
        // Return true if we really want the effect enabled.
        // By default this is the case.
        [Torque_Decorations.TorqueCallBack("", "PostEffect", "onEnabled", "this", 1, 48004, false)]
        public bool PostEffectonEnabled(string thisobj)
            {
            return true;
            }
        }
    }
