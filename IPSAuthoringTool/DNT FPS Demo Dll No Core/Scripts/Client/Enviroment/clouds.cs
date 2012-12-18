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
        //------------------------------------------------------------------------------
        // CloudLayer
        //------------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "initialize_clouds", "", 0, 47000, true)]
        public void initialize_clouds()
            {
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "CloudLayerShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/cloudLayerV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/cloudLayerP.hlsl");

            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/cloudLayerV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/cloudLayerP.glsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);


            //------------------------------------------------------------------------------
            // BasicClouds
            //------------------------------------------------------------------------------

            ts = new TorqueSingleton("ShaderData", "BasicCloudsShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/basicCloudsV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/basicCloudsP.hlsl");

            //OGLVertexShaderFile = "shaders/common/gl/basicCloudsV.glsl";
            //OGLPixelShaderFile = "shaders/common/gl/basicCloudsP.glsl";

            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);
            }
        }
    }
