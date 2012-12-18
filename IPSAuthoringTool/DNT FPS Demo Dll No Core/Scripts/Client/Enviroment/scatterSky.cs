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

        [Torque_Decorations.TorqueCallBack("", "", "initialize_scattersky", "", 0, 46000, true)]
        public void initialize_scattersky()
            {
            Torque_Class_Helper tch = new Torque_Class_Helper("GFXStateBlockData", "ScatterSkySBData");
            tch.Props.Add("cullDefined", "true");
            tch.PropsAddString("cullMode", "GFXCullNone");
            tch.Props.Add("zDefined", "true");
            tch.Props.Add("zEnable", "true");
            tch.Props.Add("zWriteEnable", "false");
            tch.Props.Add("zFunc", "GFXCmpLessEqual");

            tch.Props.Add("samplersDefined", "true");
            tch.Props.Add("samplerStates[0]", "SamplerClampLinear");
            tch.Props.Add("samplerStates[1]", "SamplerClampLinear");
            tch.Props.Add("vertexColorEnable", "true");
            tch.Create(m_ts);

            TorqueSingleton ts = new TorqueSingleton("ShaderData", "ScatterSkyShaderData");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/scatterSkyV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/scatterSkyP.hlsl");

            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/scatterSkyV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/scatterSkyP.glsl");

            ts.Props.Add("pixVersion", "2.0");

            ts.Create(m_ts);
            }
        }
    }
