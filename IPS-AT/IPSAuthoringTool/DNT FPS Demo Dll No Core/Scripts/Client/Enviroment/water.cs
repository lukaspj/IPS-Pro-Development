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
        //-----------------------------------------------------------------------------
        // Water
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "water_Init", "", 0, 44000, true)]
        public void water_init()
            {
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "WaterShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/water/waterV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/water/waterP.hlsl");
            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/water/gl/waterV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/water/gl/waterP.glsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            Torque_Class_Helper tch = new Torque_Class_Helper("GFXSamplerStateData", "WaterSampler");

            tch.Props.Add("textureColorOp", "GFXTOPModulate");
            tch.Props.Add("addressModeU", "GFXAddressWrap");
            tch.Props.Add("addressModeV", "GFXAddressWrap");
            tch.Props.Add("addressModeW", "GFXAddressWrap");
            tch.Props.Add("magFilter", "GFXTextureFilterLinear");
            tch.Props.Add("minFilter", "GFXTextureFilterAnisotropic");
            tch.Props.Add("mipFilter", "GFXTextureFilterLinear");
            tch.Props.Add("maxAnisotropy", "4");
            tch.Create(m_ts);


            ts = new TorqueSingleton("GFXStateBlockData", "WaterStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "WaterSampler");  // noise
            ts.Props.Add("samplerStates[1]", "SamplerClampPoint");  // #prepass
            ts.Props.Add("samplerStates[2]", "SamplerClampLinear"); // $reflectbuff
            ts.Props.Add("samplerStates[3]", "SamplerClampPoint");  // $backbuff
            ts.Props.Add("samplerStates[4]", "SamplerWrapLinear");  // $cubemap   
            ts.Props.Add("samplerStates[5]", "SamplerWrapLinear");  // foam     
            ts.Props.Add("samplerStates[6]", "SamplerClampLinear"); // depthMap ( color gradient ) 
            ts.Props.Add("cullDefined", "true");
            ts.PropsAddString("cullMode", "GFXCullCCW");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "UnderWaterStateBlock : WaterStateBlock");
            ts.PropsAddString("cullMode", "GFXCullCW");
            ts.Create(m_ts);

            ts = new TorqueSingleton("CustomMaterial", "WaterMat");
            ts.PropsAddString(@"sampler[""prepassTex""]", "#prepass");
            ts.PropsAddString(@"sampler[""reflectMap""]", "$reflectbuff");
            ts.PropsAddString(@"sampler[""refractBuff""]", "$backbuff");

            ts.Props.Add("shader", "WaterShader");
            ts.Props.Add("stateBlock", "WaterStateBlock");
            ts.Props.Add("version", "3.0");

            ts.Props.Add("useAnisotropic[0]", "true");
            ts.Create(m_ts);


            //-----------------------------------------------------------------------------
            // Underwater
            //-----------------------------------------------------------------------------

            ts = new TorqueSingleton("ShaderData", "UnderWaterShader");
            ts.PropsAddString(@"DXVertexShaderFile", "shaders/common/water/waterV.hlsl");
            ts.PropsAddString(@"DXPixelShaderFile", "shaders/common/water/waterP.hlsl");

            ts.PropsAddString(@"OGLVertexShaderFile", "shaders/common/water/gl/waterV.glsl");
            ts.PropsAddString(@"OGLPixelShaderFile", "shaders/common/water/gl/waterP.glsl");

            ts.PropsAddString(@"defines", "UNDERWATER");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("CustomMaterial", "UnderwaterMat");
            // These samplers are set in code not here.
            // This is to allow different WaterObject instances
            // to use this same material but override these textures
            // per instance.   
            //sampler["bumpMap"] = "core/art/water/noise02";
            //sampler["foamMap"] = "core/art/water/foam";
            ts.PropsAddString(@"sampler[""prepassTex""]", "#prepass");
            ts.PropsAddString(@"sampler[""refractBuff""]", "$backbuff");

            ts.PropsAddString(@"shader", "UnderWaterShader");
            ts.PropsAddString(@"stateBlock", "UnderWaterStateBlock");
            ts.PropsAddString(@"specular", "0.75 0.75 0.75 1.0");
            ts.Props.Add(@"specularPower", "48.0");
            ts.Props.Add(@"version", "3.0");
            ts.Create(m_ts);


            //-----------------------------------------------------------------------------
            // Basic Water
            //-----------------------------------------------------------------------------

            ts = new TorqueSingleton("ShaderData", "WaterBasicShader");

            ts.PropsAddString("DXVertexShaderFile", "shaders/common/water/waterBasicV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/water/waterBasicP.hlsl");

            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/water/gl/waterBasicV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/water/gl/waterBasicP.glsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "WaterBasicStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "WaterSampler");  // noise
            ts.Props.Add("samplerStates[2]", "SamplerClampLinear");  // $reflectbuff
            ts.Props.Add("samplerStates[3]", "SamplerClampPoint");  // $backbuff
            ts.Props.Add("samplerStates[4]", "SamplerWrapLinear");  // $cubemap
            ts.Props.Add("cullDefined", "true");
            ts.PropsAddString("cullMode", "GFXCullCCW");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "UnderWaterBasicStateBlock : WaterBasicStateBlock");
            ts.PropsAddString("cullMode", "GFXCullCW");
            ts.Create(m_ts);

            ts = new TorqueSingleton("CustomMaterial", "WaterBasicMat");
            // These samplers are set in code not here.
            // This is to allow different WaterObject instances
            // to use this same material but override these textures
            // per instance.     
            //sampler["bumpMap"] = "core/art/water/noise02";
            //sampler["skyMap"] = "$cubemap";   

            //sampler["prepassTex"] = "#prepass";
            ts.PropsAddString(@"sampler[""reflectMap""]", "$reflectbuff");
            ts.PropsAddString(@"sampler[""refractBuff""]", "$backbuff");

            ts.Props.Add("cubemap", "NewLevelSkyCubemap");
            ts.Props.Add("shader", "WaterBasicShader");
            ts.Props.Add("stateBlockZ", "WaterBasicStateBlock");
            ts.Props.Add("version", "2.0");
            ts.Create(m_ts);

            //-----------------------------------------------------------------------------
            // Basic UnderWater
            //-----------------------------------------------------------------------------

            ts = new TorqueSingleton("ShaderData", "UnderWaterBasicShader");
            ts.PropsAddString(@"DXVertexShaderFile", "shaders/common/water/waterBasicV.hlsl");
            ts.PropsAddString(@"DXPixelShaderFile", "shaders/common/water/waterBasicP.hlsl");

            ts.PropsAddString(@"OGLVertexShaderFile", "shaders/common/water/gl/waterBasicV.glsl");
            ts.PropsAddString(@"OGLPixelShaderFile", "shaders/common/water/gl/waterBasicP.glsl");

            ts.PropsAddString(@"defines", "UNDERWATER");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("CustomMaterial", "UnderwaterBasicMat");
            // These samplers are set in code not here.
            // This is to allow different WaterObject instances
            // to use this same material but override these textures
            // per instance.  
            //sampler["bumpMap"] = "core/art/water/noise02";
            //samplers["skyMap"] = "$cubemap";  

            //sampler["prepassTex"] = "#prepass";
            ts.PropsAddString(@"sampler[""refractBuff""]", "$backbuff");

            ts.Props.Add("shader", "UnderWaterBasicShader");
            ts.Props.Add("stateBlock", "UnderWaterBasicStateBlock");
            ts.Props.Add("version", "2.0");
            ts.Create(m_ts);


            }
        }
    }
