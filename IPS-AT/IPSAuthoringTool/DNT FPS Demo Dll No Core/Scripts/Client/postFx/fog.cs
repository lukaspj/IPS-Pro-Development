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
        [Torque_Decorations.TorqueCallBack("", "", "fog_Init", "", 0, 95000, true)]
        public void fog_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "FogPassShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/fogP.hlsl");
            //   OGLVertexShaderFile  = "shaders/common/postFx/gl//postFxV.glsl";
            //   OGLPixelShaderFile   = "shaders/common/postFx/gl/fogP.glsl";
            ts.PropsAddString("samplerNames[0]", "$prepassTex");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "FogPassStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("blendDefined", "true");
            ts.Props.Add("blendEnable", "true");
            ts.Props.Add("blendSrc", "GFXBlendSrcAlpha");
            ts.Props.Add("blendDest", "GFXBlendInvSrcAlpha");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "FogPostFx");
            // We forward render the reflection pass
            // so it does its own fogging.
            ts.Props.Add("allowReflectPass", "false");
            ts.PropsAddString("renderTime", "PFXBeforeBin");
            ts.PropsAddString("renderBin", "ObjTranslucentBin");
            ts.Props.Add("shader", "FogPassShader");
            ts.Props.Add("stateBlock", "FogPassStateBlock");
            ts.PropsAddString("texture[0]", "#prepass");
            ts.Props.Add("renderPriority", "5");
            ts.Props.Add("isEnabled", "true");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "UnderwaterFogPassShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/underwaterFogP.hlsl");
            //   OGLVertexShaderFile  = "shaders/common/postFx/gl/postFxV.glsl";
            //   OGLPixelShaderFile   = "shaders/common/postFx/gl/fogP.glsl";
            ts.PropsAddString("samplerNames[0]", "$prepassTex");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "UnderwaterFogPassStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[1]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[2]", "SamplerClampLinear");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "UnderwaterFogPostFx");
            ts.Props.Add("oneFrameOnly", "true");
            ts.Props.Add("onThisFrame", "false");

            // Let the fog effect render during the 
            // reflection pass.
            ts.Props.Add("allowReflectPass", "true");

            ts.PropsAddString("renderTime", "PFXBeforeBin");
            ts.PropsAddString("renderBin", "ObjTranslucentBin");

            ts.Props.Add("shader", "UnderwaterFogPassShader");
            ts.Props.Add("stateBlock", "UnderwaterFogPassStateBlock");
            ts.PropsAddString("texture[0]", "#prepass");
            ts.PropsAddString("texture[1]", "$backBuffer");
            ts.PropsAddString("texture[2]", "#waterDepthGradMap");

            // Needs to happen after the FogPostFx
            ts.Props.Add("renderPriority", "4");

            ts.Props.Add("isEnabled", "true");
            ts.Create(m_ts);





            }
        }
    }
