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
        [Torque_Decorations.TorqueCallBack("", "", "Initialize_AdvanceLight_Shaders", "", 0, 60000, true)]
        public void Initialize_AdvanceLight_Shaders()
            {
            #region new GFXStateBlockData( AL_VectorLightState )
            Torque_Class_Helper tch = new Torque_Class_Helper("GFXStateBlockData", "AL_VectorLightState");
            tch.Props.Add("blendDefined", "true");
            tch.Props.Add("blendEnable", "true");
            tch.Props.Add("blendSrc", "GFXBlendOne");
            tch.Props.Add("blendDest", "GFXBlendOne");
            tch.Props.Add("blendOp", "GFXBlendOpAdd");

            tch.Props.Add("zDefined", "true");
            tch.Props.Add("zEnable", "false");
            tch.Props.Add("zWriteEnable", "false");

            tch.Props.Add("samplersDefined", "true");
            tch.Props.Add("samplerStates[0]", "SamplerClampPoint");  // G-buffer
            tch.Props.Add("samplerStates[1]", "SamplerClampPoint");  // Shadow Map (Do not change this to linear, as all cards can not filter equally.)
            tch.Props.Add("samplerStates[2]", "SamplerClampLinear");  // SSAO Mask
            tch.Props.Add("samplerStates[3]", "SamplerWrapPoint");   // Random Direction Map

            tch.Props.Add("cullDefined", "true");
            tch.Props.Add("cullMode", "GFXCullNone");

            tch.Props.Add("stencilDefined", "true");
            tch.Props.Add("stencilEnable", "true");
            tch.Props.Add("stencilFailOp", "GFXStencilOpKeep");
            tch.Props.Add("stencilZFailOp", "GFXStencilOpKeep");
            tch.Props.Add("stencilPassOp", "GFXStencilOpKeep");
            tch.Props.Add("stencilFunc", "GFXCmpLess");
            tch.Props.Add("stencilRef", "0");
            tch.Create(m_ts);
            #endregion

            #region new ShaderData( AL_VectorLightShader )
            tch = new Torque_Class_Helper("ShaderData", "AL_VectorLightShader");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/lighting/advanced/farFrustumQuadV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/lighting/advanced/vectorLightP.hlsl");

            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/lighting/advanced/gl/farFrustumQuadV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/lighting/advanced/gl/vectorLightP.glsl");
            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion

            #region new CustomMaterial( AL_VectorLightMaterial )
            tch = new Torque_Class_Helper("CustomMaterial", "AL_VectorLightMaterial");
            tch.Props.Add("shader", "AL_VectorLightShader");
            tch.Props.Add("stateBlock", "AL_VectorLightState");
            tch.PropsAddString(@"sampler[""prePassBuffer""]", "#prepass");
            tch.PropsAddString(@"sampler[""ShadowMap""]", "$dynamiclight");
            tch.PropsAddString(@"sampler[""ssaoMask""]", "#ssaoMask");

            tch.PropsAddString("target", "lightinfo");

            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion

            #region new GFXStateBlockData( AL_ConvexLightState )

            tch = new Torque_Class_Helper("GFXStateBlockData", "AL_ConvexLightState");

            tch.Props.Add("blendDefined", "true");
            tch.Props.Add("blendEnable", "true");
            tch.Props.Add("blendSrc", "GFXBlendOne");
            tch.Props.Add("blendDest", "GFXBlendOne");
            tch.Props.Add("blendOp", "GFXBlendOpAdd");

            tch.Props.Add("zDefined", "true");
            tch.Props.Add("zEnable", "true");
            tch.Props.Add("zWriteEnable", "false");
            tch.Props.Add("zFunc", "GFXCmpGreaterEqual");

            tch.Props.Add("samplersDefined", "true");
            tch.Props.Add("samplerStates[0]", "SamplerClampPoint");  // G-buffer
            tch.Props.Add("samplerStates[1]", "SamplerClampPoint");  // Shadow Map (Do not use linear, these are perspective projections)
            tch.Props.Add("samplerStates[2]", "SamplerClampLinear"); // Cookie Map   
            tch.Props.Add("samplerStates[3]", "SamplerWrapPoint");   // Random Direction Map

            tch.Props.Add("cullDefined", "true");
            tch.Props.Add("cullMode", "GFXCullCW");

            tch.Props.Add("stencilDefined", "true");
            tch.Props.Add("stencilEnable", "true");
            tch.Props.Add("stencilFailOp", "GFXStencilOpKeep");
            tch.Props.Add("stencilZFailOp", "GFXStencilOpKeep");
            tch.Props.Add("stencilPassOp", "GFXStencilOpKeep");
            tch.Props.Add("stencilFunc", "GFXCmpLess");
            tch.Props.Add("stencilRef", "0");
            tch.Create(m_ts);
            #endregion

            #region new ShaderData( AL_PointLightShader )
            tch = new Torque_Class_Helper("ShaderData", "AL_PointLightShader");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/lighting/advanced/convexGeometryV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/lighting/advanced/pointLightP.hlsl");
            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/lighting/advanced/gl/convexGeometryV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/lighting/advanced/gl/pointLightP.glsl");
            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion

            #region new CustomMaterial( AL_PointLightMaterial )
            tch = new Torque_Class_Helper("CustomMaterial", "AL_PointLightMaterial");
            tch.Props.Add("shader", "AL_PointLightShader");
            tch.Props.Add("stateBlock", "AL_ConvexLightState");
            tch.PropsAddString(@"sampler[""prePassBuffer""]", "#prepass");
            tch.PropsAddString(@"sampler[""shadowMap""]", "$dynamiclight");
            tch.PropsAddString(@"sampler[""cookieTex""]", "$dynamiclightmask");
            tch.PropsAddString(@"target", "lightinfo");
            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion

            #region new ShaderData( AL_SpotLightShader )
            tch = new Torque_Class_Helper("ShaderData", "AL_SpotLightShader");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/lighting/advanced/convexGeometryV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/lighting/advanced/spotLightP.hlsl");
            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/lighting/advanced/gl/convexGeometryV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/lighting/advanced/gl/spotLightP.glsl");
            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion

            #region new CustomMaterial( AL_SpotLightMaterial )
            tch = new Torque_Class_Helper("CustomMaterial", "AL_SpotLightMaterial");
            tch.Props.Add("shader", "AL_SpotLightShader");
            tch.Props.Add("stateBlock", "AL_ConvexLightState");

            tch.PropsAddString(@"sampler[""prePassBuffer""]", "#prepass");
            tch.PropsAddString(@"sampler[""shadowMap""]", "$dynamiclight");
            tch.PropsAddString(@"sampler[""cookieTex""]", "$dynamiclightmask");

            tch.PropsAddString("target", "lightinfo");
            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion

            #region new Material( AL_DefaultPrePassMaterial )
            tch = new Torque_Class_Helper("Material", "AL_DefaultPrePassMaterial");
            // We need something in the first pass else it 
            // won't create a proper material instance.  
            //
            // We use color here because some objects may not
            // have texture coords in their vertex format... 
            // for example like terrain.
            //

            tch.PropsAddString("diffuseColor[0]", "1 1 1 1");
            tch.Create(m_ts);
            #endregion

            // This material is used for generating shadow 
            // materials for objects that do not have materials.

            #region new Material( AL_DefaultShadowMaterial )
            tch = new Torque_Class_Helper("Material", "AL_DefaultShadowMaterial");
            // We need something in the first pass else it 
            // won't create a proper material instance.  
            //
            // We use color here because some objects may not
            // have texture coords in their vertex format... 
            // for example like terrain.
            //

            tch.PropsAddString("diffuseColor[0]", "1 1 1 1");
            // This is here mostly for terrain which uses
            // this material to create its shadow material.
            //
            // At sunset/sunrise the sun is looking thru 
            // backsides of the terrain which often are not
            // closed.  By changing the material to be double
            // sided we avoid holes in the shadowed geometry.
            //
            tch.Props.Add("doubleSided", "true");
            tch.Create(m_ts);
            #endregion

            #region new ShaderData( AL_ParticlePointLightShader )
            tch = new Torque_Class_Helper("ShaderData", "AL_ParticlePointLightShader");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/lighting/advanced/particlePointLightV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/lighting/advanced/particlePointLightP.hlsl");
            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/lighting/advanced/gl/convexGeometryV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/lighting/advanced/gl/pointLightP.glsl");
            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion

            #region new CustomMaterial( AL_ParticlePointLightMaterial )
            tch = new Torque_Class_Helper("CustomMaterial", "AL_ParticlePointLightMaterial");
            tch.Props.Add("shader", "AL_ParticlePointLightShader");
            tch.Props.Add("stateBlock", "AL_ConvexLightState");

            tch.PropsAddString(@"sampler[""prePassBuffer""]", "#prepass");
            tch.PropsAddString(@"target", "lightinfo");

            tch.Props.Add("pixVersion", "3.0");
            tch.Create(m_ts);
            #endregion
            }
        }
    }
