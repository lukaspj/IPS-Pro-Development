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
        [Torque_Decorations.TorqueCallBack("", "", "edgeAA_Init", "", 0, 93000, true)]
        public void edgeAA_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "PFX_DefaultEdgeAAStateBlock");

            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "false");
            ts.Props.Add("zWriteEnable", "false");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Create(m_ts);


            ts = new TorqueSingleton("ShaderData", "PFX_EdgeAADetectShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/edgeaa/edgeDetectP.hlsl");
            //OGLVertexShaderFile  = "shaders/common/postFx/gl//postFxV.glsl";
            //OGLPixelShaderFile   = "shaders/common/postFx/gl/passthruP.glsl";
            ts.PropsAddString("samplerNames[0]", "$inputTex");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_EdgeAAShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/edgeaa/edgeAAV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/edgeaa/edgeAAP.hlsl");
            //OGLVertexShaderFile  = "shaders/common/postFx/gl//postFxV.glsl";
            //OGLPixelShaderFile   = "shaders/common/postFx/gl/passthruP.glsl";
            ts.PropsAddString("samplerNames[0]", "$inputTex");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_EdgeAADebugShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/edgeaa/dbgEdgeDisplayP.hlsl");
            //OGLVertexShaderFile  = "shaders/common/postFx/gl//postFxV.glsl";
            //OGLPixelShaderFile   = "shaders/common/postFx/gl/passthruP.glsl";
            ts.PropsAddString("samplerNames[0]", "$inputTex");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "EdgeDetectPostEffect");
            ts.PropsAddString("renderTime", "PFXBeforeBin");
            ts.PropsAddString("renderBin", "ObjTranslucentBin");
            //renderPriority = 0.1;
            ts.PropsAddString("targetScale", "0.5 0.5");
            ts.Props.Add("shader", "PFX_EdgeAADetectShader");
            ts.Props.Add("stateBlock", "PFX_DefaultEdgeAAStateBlock");
            ts.PropsAddString("texture[0]", "#prepass");
            ts.PropsAddString("target", "#edge");
            ts.Props.Add("isEnabled", "false");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "EdgeAAPostEffect");
            ts.PropsAddString("renderTime", "PFXAfterDiffuse");
            //renderBin = "ObjTranslucentBin";      
            //renderPriority = 0.1;

            ts.Props.Add("shader", "PFX_EdgeAAShader");
            ts.Props.Add("stateBlock", "PFX_DefaultEdgeAAStateBlock");
            ts.PropsAddString("texture[0]", "#edge");
            ts.PropsAddString("texture[1]", "$backBuffer");
            ts.PropsAddString("target", "$backBuffer");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "Debug_EdgeAAPostEffect");
            ts.PropsAddString("renderTime", "PFXAfterDiffuse");
            //renderBin = "ObjTranslucentBin";      
            //renderPriority = 0.1;

            ts.Props.Add("shader", "PFX_EdgeAADebugShader");
            ts.Props.Add("stateBlock", "PFX_DefaultEdgeAAStateBlock");
            ts.PropsAddString("texture[0]", "#edge");
            ts.PropsAddString("target", "$backBuffer");
            ts.Create(m_ts);



            }
        }
    }
