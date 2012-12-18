using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
using System.Threading;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "lightRay_Init", "", 0, 100000, true)]
        public void lightRay_Init()
            {
            console.SetVar("$LightRayPostFX::brightScalar", 0.75);
            console.SetVar("$LightRayPostFX::numSamples", 40);
            console.SetVar("$LightRayPostFX::density", 0.94);
            console.SetVar("$LightRayPostFX::weight", 5.65);
            console.SetVar("$LightRayPostFX::decay", 1.0);
            console.SetVar("$LightRayPostFX::exposure", 0.0005);
            console.SetVar("$LightRayPostFX::resolutionScale", 1.0);

            TorqueSingleton ts = new TorqueSingleton("ShaderData", "LightRayOccludeShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString(" DXPixelShaderFile", "shaders/common/postFx/lightRay/lightRayOccludeP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("ShaderData", "LightRayShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/lightRay/lightRayP.hlsl");
            ts.Props.Add("pixVersion", "3.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("GFXStateBlockData", "LightRayStateBlock : PFX_DefaultStateBlock");
            ts.Props.Add("samplersDefined", "true");
            ts.Props.Add("samplerStates[0]", "SamplerClampLinear");
            ts.Props.Add("samplerStates[1]", "SamplerClampLinear");
            ts.Create(m_ts);

            console.Eval(
                @"
singleton PostEffect( LightRayPostFX )
{
   isEnabled = false;
   allowReflectPass = false;
        
   renderTime = ""PFXAfterDiffuse"";
   renderPriority = 0.1;
      
   shader = LightRayOccludeShader;
   stateBlock = LightRayStateBlock;
   texture[0] = ""$backBuffer"";
   texture[1] = ""#prepass"";
   target = ""$outTex"";
   targetFormat = ""GFXFormatR16G16B16A16F"";
      
   new PostEffect()
   {
      shader = LightRayShader;
      stateBlock = LightRayStateBlock;
      internalName = ""final"";
      texture[0] = ""$inTex"";
      texture[1] = ""$backBuffer"";
      target = ""$backBuffer"";
   };
};
");
            }
        [Torque_Decorations.TorqueCallBack("", "LightRayPostFX", "preProcess", "%this", 1, 100100, false)]
        public void LightRayPostFXpreProcess(string thisobj)
            {
          //  if (either)
                console.SetVar(thisobj + ".targetScale", console.GetVarString("$LightRayPostFX::resolutionScale") + " " + console.GetVarString("$LightRayPostFX::resolutionScale"));
            }

      //  private bool either = true;

        [Torque_Decorations.TorqueCallBack("", "LightRayPostFX", "setShaderConsts", "%this", 1, 100100, false)]
        public void LightRayPostFXsetShaderConsts(string thisobj)
            {
           // if (either)
                {
                PostEffect.setShaderConst(thisobj, "$brightScalar", console.GetVarString("$LightRayPostFX::brightScalar"));
                string pfx = SimSet.findObjectByInternalName(thisobj, "final", true);
                PostEffect.setShaderConst(pfx, "$numSamples", console.GetVarString("$LightRayPostFX::numSamples"));
                PostEffect.setShaderConst(pfx, "$density", console.GetVarString("$LightRayPostFX::density"));
                PostEffect.setShaderConst(pfx, "$weight", console.GetVarString("$LightRayPostFX::weight"));
                PostEffect.setShaderConst(pfx, "$decay", console.GetVarString("$LightRayPostFX::decay"));
                PostEffect.setShaderConst(pfx, "$exposure", console.GetVarString("$LightRayPostFX::exposure"));

                }
           // either = !either;
            //Thread.Sleep(5);
            }
        }
    }
