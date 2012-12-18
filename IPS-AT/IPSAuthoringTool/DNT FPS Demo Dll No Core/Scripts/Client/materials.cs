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
        [Torque_Decorations.TorqueCallBack("", "", "materials_Init", "", 0, 42000, true)]
        public void materials_Init()
            {
            TorqueSingleton ts = new TorqueSingleton("Material", "WarningMaterial");
            ts.PropsAddString("diffuseMap[0]", "core/art/warnMat");
            ts.Props.Add("emissive[0]", "false");
            ts.Props.Add("translucent", "false");
            ts.Create(m_ts);

            ts = new TorqueSingleton("CubemapData", "WarnMatCubeMap");
            ts.PropsAddString("cubeFace[0]", "core/art/warnMat");
            ts.PropsAddString("cubeFace[1]", "core/art/warnMat");
            ts.PropsAddString("cubeFace[2]", "core/art/warnMat");
            ts.PropsAddString("cubeFace[3]", "core/art/warnMat");
            ts.PropsAddString("cubeFace[4]", "core/art/warnMat");
            ts.PropsAddString("cubeFace[5]", "core/art/warnMat");
            ts.Create(m_ts);

            }
        }
    }
