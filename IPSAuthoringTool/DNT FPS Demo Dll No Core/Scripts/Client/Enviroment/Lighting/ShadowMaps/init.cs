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
        [Torque_Decorations.TorqueCallBack("", "", "initializeShadowMaps", "", 0, 67000, true)]
        public void initializeShadowMaps()
            {
            Torque_Class_Helper tch = new Torque_Class_Helper("ShaderData", "BlurDepthShader");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/lighting/shadowMap/boxFilterV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/lighting/shadowMap/boxFilterP.hlsl");

            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/lighting/shadowMap/gl/boxFilterV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/lighting/shadowMap/gl/boxFilterP.glsl");
            tch.Props.Add("pixVersion", "2.0");
            tch.Create(m_ts);
            }
        }
    }
