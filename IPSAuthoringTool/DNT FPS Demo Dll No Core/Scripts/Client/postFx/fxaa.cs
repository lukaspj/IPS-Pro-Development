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
        [Torque_Decorations.TorqueCallBack("", "", "fxaa_Init", "", 0, 96000, true)]
        public void fxaa_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "FXAA_StateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "FXAA_ShaderData");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/fxaa/fxaaV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/fxaa/fxaaP.hlsl");
            ts.PropsAddString("samplerNames[0]", "$colorTex");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "FXAA_PostEffect");
            ts.Props.Add("isEnabled", "false");

            ts.Props.Add("allowReflectPass", "false");
            ts.PropsAddString("renderTime", "PFXAfterDiffuse");

            ts.PropsAddString("texture[0]", "$backBuffer");

            ts.PropsAddString("target", "$backBuffer");

            ts.Props.Add("stateBlock", "FXAA_StateBlock");
            ts.Props.Add("shader", "FXAA_ShaderData");
            ts.Create(m_ts);

            }
        }
    }
