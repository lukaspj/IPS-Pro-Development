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
        [Torque_Decorations.TorqueCallBack("", "", "Glow_Init", "", 0, 98000, true)]
        public void Glow_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "PFX_GlowBlurVertShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/glowBlurV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/glowBlurP.hlsl");
            //   OGLVertexShaderFile  = "shaders/common/postFx/glowBlurV.glsl";
            //   OGLPixelShaderFile   = "shaders/common/postFx/glowBlurP.glsl";
            ts.PropsAddString("defines", "BLUR_DIR=float2(0.0,1.0)");
            ts.PropsAddString("samplerNames[0]", "$diffuseMap");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "PFX_GlowBlurHorzShader : PFX_GlowBlurVertShader");
            ts.PropsAddString("defines", "BLUR_DIR=float2(1.0,0.0)");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "PFX_GlowCombineStateBlock : PFX_DefaultStateBlock");
            // Use alpha test to save some fillrate 
            // on the non-glowing areas of the scene.
            ts.Props.Add("alphaDefined", "true");
            ts.Props.Add("alphaTestEnable", "true");
            ts.Props.Add("alphaTestRef", "1");
            ts.Props.Add("alphaTestFunc", "GFXCmpGreaterEqual");

            // Do a one to one blend.
            ts.Props.Add("blendDefined", "true");
            ts.Props.Add("blendEnable", "true");
            ts.Props.Add("blendSrc", "GFXBlendOne");
            ts.Props.Add("blendDest", "GFXBlendOne");

            ts.Create(m_ts);

            //until I come up for a solution for nested objects I'm just going to run this code inline.
            console.Eval(@"

singleton PostEffect( GlowPostFx )
{  
   // Do not allow the glow effect to work in reflection 
   // passes by default so we don't do the extra drawing.
   allowReflectPass = false;
                  
   renderTime = ""PFXAfterBin"";
   renderBin = ""GlowBin"";
   renderPriority = 1;
                    
   // First we down sample the glow buffer.   
   shader = PFX_PassthruShader;
   stateBlock = PFX_DefaultStateBlock;
   texture[0] = ""#glowbuffer"";
   target = ""$outTex"";
   targetScale = ""0.5 0.5"";
   
   isEnabled = true;
               
   // Blur vertically
   new PostEffect()
   {
      shader = PFX_GlowBlurVertShader;
      stateBlock = PFX_DefaultStateBlock;
      texture[0] = ""$inTex"";
      target = ""$outTex"";
   };
   
   // Blur horizontally
   new PostEffect()
   {
      shader = PFX_GlowBlurHorzShader;
      stateBlock = PFX_DefaultStateBlock;
      texture[0] = ""$inTex"";
      target = ""$outTex"";
   };
            
   // Upsample and combine with the back buffer.
   new PostEffect()
   {      
      shader = PFX_PassthruShader;
      stateBlock = PFX_GlowCombineStateBlock;
      texture[0] = ""$inTex"";
      target = ""$backBuffer"";
   };
};
");



            }
        }
    }
