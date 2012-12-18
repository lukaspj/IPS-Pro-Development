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
        [Torque_Decorations.TorqueCallBack("", "", "GammaPostFX_Init", "", 0, 97000, true)]
        public void GammaPostFX_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "GammaShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/gammaP.hlsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "GammaStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "GammaPostFX");
            ts.Props.Add("isEnabled", "true");
            ts.Props.Add("allowReflectPass", "false");

            ts.PropsAddString("renderTime", "PFXBeforeBin");
            ts.PropsAddString("renderBin", "EditorBin");
            ts.Props.Add("renderPriority", "9999");

            ts.Props.Add("shader", "GammaShader");
            ts.Props.Add("stateBlock", "GammaStateBlock");

            ts.PropsAddString("texture[0]", "$backBuffer");
            ts.Props.Add("texture[1]", "$HDRPostFX::colorCorrectionRamp");
            ts.Create(m_ts);
            }

        [Torque_Decorations.TorqueCallBack("", "GammaPostFX", "preProcess", "%this", 1, 97100, false)]
        public void GammaPostFXpreProcess(string thisobj)
            {
            if (console.GetVarString(thisobj + ".texture[1]") != console.GetVarString("$HDRPostFX::colorCorrectionRamp"))
                PostEffect.setTexture(thisobj, 1, console.GetVarString("$HDRPostFX::colorCorrectionRamp"));
            }
        [Torque_Decorations.TorqueCallBack("", "GammaPostFX", "setShaderConsts", "%this", 1, 97200, false)]
        public void GammaPostFXsetShaderConsts(string thisobj)
            {
            float clampedGamma = Util.mClamp(console.GetVarFloat("$pref::Video::Gamma"), (float)0.001, (float)2.2);
            PostEffect.setShaderConst(thisobj, "$OneOverGamma", (1 / clampedGamma).AsString());
            }
        }
    }
