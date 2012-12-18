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
        [Torque_Decorations.TorqueCallBack("", "", "initializeShadowFilter", "", 0, 66000, true)]
        public void initializeShadowFilter()
            {
            #region singleton ShaderData( BL_ShadowFilterShaderV )
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "BL_ShadowFilterShaderV");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/lighting/basic/shadowFilterV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/lighting/basic/shadowFilterP.hlsl");
            ts.PropsAddString("OGLVertexShaderFile", "shaders/common/lighting/basic/gl/shadowFilterV.glsl");
            ts.PropsAddString("OGLPixelShaderFile", "shaders/common/lighting/basic/gl/shadowFilterP.glsl");
            ts.PropsAddString("samplerNames[0]", "$diffuseMap");
            ts.PropsAddString("defines", "BLUR_DIR=float2(1.0,0.0)");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);
            #endregion

            #region singleton ShaderData( BL_ShadowFilterShaderH : BL_ShadowFilterShaderV )
            ts = new TorqueSingleton("ShaderData", "BL_ShadowFilterShaderH : BL_ShadowFilterShaderV");
            ts.PropsAddString("defines", "BLUR_DIR=float2(0.0,1.0)");
            ts.Create(m_ts);
            #endregion

            #region singleton GFXStateBlockData( BL_ShadowFilterSB : PFX_DefaultStateBlock )
            ts = new TorqueSingleton("GFXStateBlockData", " BL_ShadowFilterSB : PFX_DefaultStateBlock");
            ts.Props.Add("colorWriteDefined", "true");
            ts.Props.Add("colorWriteRed", "false");
            ts.Props.Add("colorWriteGreen", "false");
            ts.Props.Add("colorWriteBlue", "false");
            ts.Props.Add("blendDefined", "true");
            ts.Props.Add("blendEnable", "true");
            ts.Create(m_ts);
            #endregion

            // NOTE: This is ONLY used in Basic Lighting, and 
            // only directly by the ProjectedShadow.  It is not 
            // meant to be manually enabled like other PostEffects.
            #region singleton PostEffect( BL_ShadowFilterPostFx )
            console.Eval(
                @"singleton PostEffect( BL_ShadowFilterPostFx )
{
    // Blur vertically
   shader = BL_ShadowFilterShaderV;
   stateBlock = PFX_DefaultStateBlock;
   targetClear = ""PFXTargetClear_OnDraw"";
   targetClearColor = ""0 0 0 0"";
   texture[0] = ""$inTex"";
   target = ""$outTex"";   

   // Blur horizontal
   new PostEffect()
   {
      shader = BL_ShadowFilterShaderH;
      stateBlock = PFX_DefaultStateBlock;
      texture[0] = ""$inTex"";
      target = ""$outTex"";
   };
};
");
            #endregion
            }
        }
    }
