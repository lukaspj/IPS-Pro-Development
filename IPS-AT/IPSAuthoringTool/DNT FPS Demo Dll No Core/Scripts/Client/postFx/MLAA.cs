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
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        // An implementation of "Practical Morphological Anti-Aliasing" from 
        // GPU Pro 2 by Jorge Jimenez, Belen Masia, Jose I. Echevarria, 
        // Fernando Navarro, and Diego Gutierrez.
        //
        // http://www.iryoku.com/mlaa/

        // NOTE: This is currently disabled in favor of FXAA.  See 
        // core\scripts\client\canvas.cs if you want to re-enable it.
        [Torque_Decorations.TorqueCallBack("", "", "MLAA_Init", "", 0, 101000, true)]
        public void MLAA_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("GFXStateBlockData", "MLAA_EdgeDetectStateBlock : PFX_DefaultStateBlock");
            // Mark the edge pixels in stencil.
            ts.Props.Add("stencilDefined", "true");
            ts.Props.Add("stencilEnable", "true");
            ts.Props.Add("stencilPassOp", "GFXStencilOpReplace");
            ts.Props.Add("stencilFunc", "GFXCmpAlways");
            ts.Props.Add("stencilRef", "1");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "MLAA_EdgeDetectionShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/mlaa/offsetV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/mlaa/edgeDetectionP.hlsl");
            ts.PropsAddString("samplerNames[0]", "$colorMapG");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "MLAA_BlendWeightCalculationStateBlock : PFX_DefaultStateBlock");
            // Here we want to process only marked pixels.
            ts.Props.Add("stencilDefined", "true");
            ts.Props.Add("stencilEnable", "true");
            ts.Props.Add("stencilPassOp", "GFXStencilOpKeep");
            ts.Props.Add("stencilFunc", "GFXCmpEqual");
            ts.Props.Add("stencilRef", "1");

            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[2]", "SamplerClampPoint");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "MLAA_BlendWeightCalculationShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/mlaa/passthruV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/mlaa/blendWeightCalculationP.hlsl");
            ts.PropsAddString("samplerNames[0]", "$edgesMap");
            ts.PropsAddString("samplerNames[1]", "$edgesMapL");
            ts.PropsAddString("samplerNames[2]", "$areaMap");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "MLAA_NeighborhoodBlendingStateBlock : PFX_DefaultStateBlock");

            // Here we want to process only marked pixels too.
            ts.Props.Add("stencilDefined", "true");
            ts.Props.Add("stencilEnable", "true");
            ts.Props.Add("stencilPassOp", "GFXStencilOpKeep");
            ts.Props.Add("stencilFunc", "GFXCmpEqual");
            ts.Props.Add("stencilRef", "1");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampPoint");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[2]", "SamplerClampPoint");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "MLAA_NeighborhoodBlendingShader");

            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/mlaa/offsetV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/mlaa/neighborhoodBlendingP.hlsl");
            ts.PropsAddString("samplerNames[0]", "$blendMap");
            ts.PropsAddString("samplerNames[1]", "$colorMapL");
            ts.PropsAddString("samplerNames[2]", "$colorMap");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            console.Eval(
                @"

singleton PostEffect( MLAAFx )
{
   isEnabled = false;
   
   allowReflectPass = false;
   renderTime = ""PFXAfterDiffuse"";

   texture[0] = ""$backBuffer""; //colorMapG      
   texture[1] = ""#prepass""; // Used for depth detection

   target = ""$outTex"";
   targetClear = PFXTargetClear_OnDraw;
   targetClearColor = ""0 0 0 0"";

   stateBlock = MLAA_EdgeDetectStateBlock;
   shader = MLAA_EdgeDetectionShader;
   lumaCoefficients = ""0.2126 0.7152 0.0722"";

   threshold = 0.1;

   depthThreshold = 0.01;

   new PostEffect()
   {
      internalName = ""blendingWeightsCalculation"";

      target = ""$outTex"";
      targetClear = PFXTargetClear_OnDraw;

      shader = MLAA_BlendWeightCalculationShader;
      stateBlock = MLAA_BlendWeightCalculationStateBlock;

      texture[0] = ""$inTex""; // Edges mask    
      texture[1] = ""$inTex""; // Edges mask 
      texture[2] = ""$AreaMap33_dds"";
   };

   new PostEffect()
   {
      internalName = ""neighborhoodBlending"";

      shader = MLAA_NeighborhoodBlendingShader;
      stateBlock = MLAA_NeighborhoodBlendingStateBlock;

      texture[0] = ""$inTex""; // Blend weights
      texture[1] = ""$backBuffer"";      
      texture[2] = ""$backBuffer"";      
   };
};

");
            }

        

        [Torque_Decorations.TorqueCallBack("", "MLAAFx", "setShaderConsts", "%this", 1, 101000, false)]
        public void MLAAFxsetShaderConsts(string thisobj)
            {
            PostEffect.setShaderConst(thisobj, "$lumaCoefficients", console.GetVarString(thisobj + ".lumaCoefficients"));
            PostEffect.setShaderConst(thisobj, "$threshold", console.GetVarString(thisobj + ".threshold"));
            PostEffect.setShaderConst(thisobj, "$depthThreshold", console.GetVarString(thisobj + ".depthThreshold"));
            }

        [Torque_Decorations.TorqueCallBack("", "", "rml1", "", 0, 101000, false)]
        public void rml1()
            {
            console.SetVar("$AreaMap33_dds", areamap33_dds);
            }
        }
    }
