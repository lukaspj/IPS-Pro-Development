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
        [Torque_Decorations.TorqueCallBack("", "", "flash_Init", "", 0, 94000, true)]
        public void flash_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("ShaderData", "PFX_FlashShader");
            ts.PropsAddString("DXVertexShaderFile", "shaders/common/postFx/postFxV.hlsl");
            ts.PropsAddString("DXPixelShaderFile", "shaders/common/postFx/flashP.hlsl");
            ts.PropsAddString("defines", "WHITE_COLOR=float4(1.0,1.0,1.0,0.0);MUL_COLOR=float4(1.0,0.25,0.25,0.0)");
            ts.Props.Add("pixVersion", "2.0");
            ts.Create(m_ts);

            ts = new TorqueSingleton("PostEffect", "FlashFx");
            ts.Props.Add("isEnabled", "false");
            ts.Props.Add("allowReflectPass", "false");

            ts.PropsAddString("renderTime", "PFXAfterDiffuse");

            ts.Props.Add("shader", "PFX_FlashShader");
            ts.PropsAddString("texture[0]", "$backBuffer");
            ts.Props.Add("renderPriority", "10");
            ts.Props.Add("stateBlock", "PFX_DefaultStateBlock");
            ts.Create(m_ts);
            }

        //private bool flashskip = true;

        [Torque_Decorations.TorqueCallBack("", "FlashFx", "setShaderConsts", "", 1, 94100, false)]
        public void FlashFxsetShaderConsts(string thisobj)
            {
            //PostEffect.setShaderConst(thisobj, "$damageFlash", "0");
            //PostEffect.setShaderConst(thisobj, "$whiteOut", "0");
            //return;
            //if (flashskip)
                {
                if (console.isObject(thisobj))
                    {
                    PostEffect.setShaderConst(thisobj, "$damageFlash", GameConnection.getDamageFlash("ServerConnection").AsString());
                    PostEffect.setShaderConst(thisobj, "$whiteOut", GameConnection.getWhiteOut("ServerConnection").AsString());
                    }
                else
                    {
                    PostEffect.setShaderConst(thisobj, "$damageFlash", "0");
                    PostEffect.setShaderConst(thisobj, "$whiteOut", "0");
                    }
                }
            //flashskip = !flashskip;
            //Thread.Sleep(5);
            }
        }
    }
