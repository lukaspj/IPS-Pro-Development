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
        [Torque_Decorations.TorqueCallBack("", "", "terrainBlock_Init", "", 0, 43000, true)]
        public void terrainBlock_Init()
            {

            TorqueSingleton ts = new TorqueSingleton("ShaderData", "TerrainBlendShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/terrain/blendV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/terrain/blendP.hlsl");

            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/terrain/gl/blendV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/terrain/gl/blendP.glsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            }
        }
    }
