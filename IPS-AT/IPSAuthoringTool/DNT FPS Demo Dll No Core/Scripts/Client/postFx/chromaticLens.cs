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
        [Torque_Decorations.TorqueCallBack("", "", "chromaticLens_init", "", 0, 90000, true)]
        public void chromaticLens_init()
            {
            console.SetVar("$CAPostFx::enabled", false);
            // The lens distortion coefficient.
            console.SetVar("$CAPostFx::distCoeffecient", -0.05);
            // The cubic distortion value.
            console.SetVar("$CAPostFx::cubeDistortionFactor", -0.1);
            // The amount and direction of the maxium shift for
            // the red, green, and blue channels.
            console.SetVar("$CAPostFx::colorDistortionFactor", "0.005 -0.005 0.01");


            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "PFX_DefaultChromaticLensStateBlock");

            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_ChromaticLensShader");

            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/chromaticLens.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);


            ts = new TorqueSingleton("PostEffect", "ChromaticLensPostFX");
            ts.PropsAddString("renderTime", "PFXAfterDiffuse");
            ts.Props.Add("renderPriority", "0.2");
            ts.Props.Add("isEnabled", "false");
            ts.Props.Add("allowReflectPass", "false");

            ts.Props.Add("shader", "PFX_ChromaticLensShader");
            ts.Props.Add("stateBlock", "PFX_DefaultChromaticLensStateBlock");
            ts.PropsAddString("texture[0]", "$backBuffer");
            ts.PropsAddString("target", "backBuffer");
            ts.Create(m_ts);
            }

        [Torque_Decorations.TorqueCallBack("", "ChromaticLensPostFX", "setShaderConsts", "%this", 1, 90100, false)]
        public void ChromaticLensPostFXsetShaderConsts(string thisobj)
            {
            PostEffect.setShaderConst(thisobj, "$distCoeff", console.GetVarString("$CAPostFx::distCoeffecient"));
            PostEffect.setShaderConst(thisobj, "$cubeDistort", console.GetVarString("$CAPostFx::cubeDistortionFactor"));
            PostEffect.setShaderConst(thisobj, "$colorDistort", console.GetVarString("$CAPostFx::colorDistortionFactor"));
            }
        }
    }
