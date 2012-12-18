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
        // Anim flag settings - must match material.h
        // These cannot be enumed through script becuase it cannot
        // handle the "|" operation for combining them together
        // ie. Scroll | Wave does not work.
        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "", "commonMaterialData_Init", "", 0, 40000, true)]
        public void commonMaterialData_Init()
            {
            console.SetVar("$scroll", 1);
            console.SetVar("$rotate", 2);
            console.SetVar("$wave", 4);
            console.SetVar("$scale", 8);
            console.SetVar("$sequence", 16);
            // Common stateblock definitions

            Torque_Class_Helper tch = new Torque_Class_Helper("GFXSamplerStateData", "SamplerClampLinear");
            tch.Props.Add("textureColorOp", "GFXTOPModulate");
            tch.Props.Add("addressModeU", "GFXAddressClamp");
            tch.Props.Add("addressModeV", "GFXAddressClamp");
            tch.Props.Add("addressModeW", "GFXAddressClamp");
            tch.Props.Add("magFilter", "GFXTextureFilterLinear");
            tch.Props.Add("minFilter", "GFXTextureFilterLinear");
            tch.Props.Add("mipFilter", "GFXTextureFilterLinear");
            tch.Create(m_ts);

            tch = new Torque_Class_Helper("GFXSamplerStateData", "SamplerClampPoint");
            tch.Props.Add("textureColorOp", "GFXTOPModulate");
            tch.Props.Add("addressModeU", "GFXAddressClamp");
            tch.Props.Add("addressModeV", "GFXAddressClamp");
            tch.Props.Add("addressModeW", "GFXAddressClamp");
            tch.Props.Add("magFilter", "GFXTextureFilterPoint");
            tch.Props.Add("minFilter", "GFXTextureFilterPoint");
            tch.Props.Add("mipFilter", "GFXTextureFilterPoint");
            tch.Create(m_ts);

            tch = new Torque_Class_Helper("GFXSamplerStateData", "SamplerWrapLinear");
            tch.Props.Add("textureColorOp", "GFXTOPModulate");
            tch.Props.Add("addressModeU", "GFXTextureAddressWrap");
            tch.Props.Add("addressModeV", "GFXTextureAddressWrap");
            tch.Props.Add("addressModeW", "GFXTextureAddressWrap");
            tch.Props.Add("magFilter", "GFXTextureFilterLinear");
            tch.Props.Add("minFilter", "GFXTextureFilterLinear");
            tch.Props.Add("mipFilter", "GFXTextureFilterLinear");
            tch.Create(m_ts);

            tch = new Torque_Class_Helper("GFXSamplerStateData", "SamplerWrapPoint");
            tch.Props.Add("textureColorOp", "GFXTOPModulate");
            tch.Props.Add("addressModeU", "GFXTextureAddressWrap");
            tch.Props.Add("addressModeV", "GFXTextureAddressWrap");
            tch.Props.Add("addressModeW", "GFXTextureAddressWrap");
            tch.Props.Add("magFilter", "GFXTextureFilterPoint");
            tch.Props.Add("minFilter", "GFXTextureFilterPoint");
            tch.Props.Add("mipFilter", "GFXTextureFilterPoint");
            tch.Create(m_ts);


            }

        }
    }
