using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {

    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        //*****************************************************************************
        // Shaders  ( For Custom Materials )
        //*****************************************************************************

      

        [Torque_Decorations.TorqueCallBack("", "", "shaders_Init", "", 0, 41000, true)]
        public void shaders_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "_DebugInterior_");

            ts.PropsAddString("DXVertexShaderFile", "shaders/common/debugInteriorsV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/debugInteriorsP.hlsl");
            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/debugInteriorsV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/debugInteriorsP.glsl");
            ts.Props.Add("pixVersion", "1.1");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "ParticlesShaderData");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/particlesV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/particlesP.hlsl");

            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/particlesV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/particlesP.glsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "OffscreenParticleCompositeShaderData");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/particleCompositeV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/particleCompositeP.hlsl");

            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/particleCompositeV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/particleCompositeP.glsl");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            //-----------------------------------------------------------------------------
            // Planar Reflection
            //-----------------------------------------------------------------------------

            Torque_Class_Helper tch = new Torque_Class_Helper("ShaderData", "ReflectBump");

            tch.PropsAddString("DXVertexShaderFile", "shaders/common/planarReflectBumpV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/planarReflectBumpP.hlsl");

            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/planarReflectBumpV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/planarReflectBumpP.glsl");

            tch.PropsAddString("samplerNames[0]", "$diffuseMap");
            tch.PropsAddString("samplerNames[1]", "$refractMap");
            tch.PropsAddString("samplerNames[2]", "$bumpMap");
            tch.Props.Add("pixVersion", "2.0");
            tch.Create(m_ts);

            tch = new Torque_Class_Helper("ShaderData", "Reflect");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/planarReflectV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/planarReflectP.hlsl");

            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/planarReflectV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/planarReflectP.glsl");

            tch.PropsAddString("samplerNames[0]", "$diffuseMap");
            tch.PropsAddString("samplerNames[1]", "$refractMap");
            tch.Props.Add("pixVersion", "1.4");
            tch.Create(m_ts);

            tch = new Torque_Class_Helper("ShaderData", "fxFoliageReplicatorShader");
            tch.PropsAddString("DXVertexShaderFile", "shaders/common/fxFoliageReplicatorV.hlsl");
            tch.PropsAddString("DXPixelShaderFile", "shaders/common/fxFoliageReplicatorP.hlsl");

            tch.PropsAddString("OGLVertexShaderFile", "shaders/common/gl/fxFoliageReplicatorV.glsl");
            tch.PropsAddString("OGLPixelShaderFile", "shaders/common/gl/fxFoliageReplicatorP.glsl");

            tch.PropsAddString("samplerNames[0]", "$diffuseMap");
            tch.PropsAddString("samplerNames[1]", "$alphaMap");
            tch.Props.Add("pixVersion", "1.4");
            tch.Create(m_ts);
            }
        }
    }
