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
        [Torque_Decorations.TorqueCallBack("", "", "initbaselighting", "", 0, 65000, true)]
        public void initbaselighting()
            {
            /*
             * client/lighting/advanced/shaders.cs  60000
             * client/lighting/advanced/lightViz.cs         61000
             * client/lighting/advanced/shadowViz.cs        62000
             * client/lighting/advanced/shadowViz.gui       63000
             * client/lighting/advanced/init.cs             64000
             * client/lighting/basic/init.cs                65000
             * client/lighting/basic/shadowFilter           66000
             * client/lighting/shadowMaps/init              67000
             */
            dnt.DoScriptInjection(ScriptType.Client, 66000, 66999);

            #region singleton GFXStateBlockData( BL_ProjectedShadowSBData )
            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "BL_ProjectedShadowSBData");
            ts.Props.Add("blendDefined", "true");
            ts.Props.Add("blendEnable", "true");
            ts.Props.Add("blendSrc", "GFXBlendDestColor");
            ts.Props.Add("blendDest", "GFXBlendZero");

            ts.Props.Add("zDefined", "true");
            ts.Props.Add("zEnable", "true");
            ts.Props.Add("zWriteEnable", "false");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("vertexColorEnable", "true");
            ts.Create(m_ts);
            #endregion

            #region singleton ShaderData( BL_ProjectedShadowShaderData )
            ts = new TorqueSingleton("ShaderData", "BL_ProjectedShadowShaderData");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/projectedShadowV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/projectedShadowP.hlsl");

            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/projectedShadowV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/projectedShadowP.glsl");

            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);
            #endregion

            #region singleton CustomMaterial( BL_ProjectedShadowMaterial )
            ts = new TorqueSingleton("CustomMaterial", "BL_ProjectedShadowMaterial");
            ts.PropsAddString(@"sampler[""inputTex""]", "$miscbuff");

            ts.Props.Add("shader", "BL_ProjectedShadowShaderData");
            ts.Props.Add("stateBlock", "BL_ProjectedShadowSBData");
            ts.Props.Add("version", "2.0");
            ts.Props.Add("forwardLit", "true");
            ts.Create(m_ts);
            #endregion
            }
        [Torque_Decorations.TorqueCallBack("", "", "onActivateBasicLM", "", 0, 65100, false)]
        public void onActivateBasicLM()
            {
            // If HDR is enabled... enable the special format token.
            if ((console.GetVarString("$platform") == "macos") || console.Call("HDRPostFx", "isEnabled").AsBool())
                console.Call("AL_FormatToken", "enable");
            // Create render pass for projected shadow.

            new Torque_Class_Helper("RenderPassManager", "BL_ProjectedShadowRPM").Create(m_ts);
            // Create the mesh bin and add it to the manager.

            string meshbin = new Torque_Class_Helper("RenderMeshMgr").Create(m_ts).AsString();
            RenderPassManager.addManager("BL_ProjectedShadowRPM", meshbin);
            // Add both to the root group so that it doesn't
            // end up in the MissionCleanup instant group.

            SimSet.pushToBack("RootGroup", "BL_ProjectedShadowRPM");
            SimSet.pushToBack("RootGroup", meshbin);
            }
        [Torque_Decorations.TorqueCallBack("", "", "onDeactivateBasicLM", "", 0, 65200, false)]
        public void onDeactivateBasicLM()
            {
            // Delete the pass manager which also deletes the bin.
            console.Call("BL_ProjectedShadowRPM", "delete");
            }
        [Torque_Decorations.TorqueCallBack("", "", "setBasicLighting", "", 0, 65300, false)]
        public void setBasicLighting()
            {
            Util.setLightManager("Basic Lighting");
            }
        }
    }
