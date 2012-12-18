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
        [Torque_Decorations.TorqueCallBack("", "ActionMap", "copyBind", "( %this, %otherMap, %command )", 3, 53000, false)]
        public void ActionMapcopyBind(string thisobj, string otherMap, string command)
            {
            if (Util._isObject(otherMap))
                {
                Util._error("ActionMap::copyBind - \"" + otherMap + "\" is not an object!");
                return;
                }

            string bind = ActionMap.getBinding(otherMap, command);
            if (bind == "") return;
            string device = Util.getField(bind, 0);
            string action = Util.getField(bind, 1);
            string flags = ActionMap.isInverted(otherMap, device, action) ? "SDI" : "SD";
            string deadzone = ActionMap.getDeadZone(otherMap, device, action);
            float scale = ActionMap.getScale(otherMap, device, action);
            ActionMap.bind(thisobj, device, action, flags, deadzone, scale.AsString(), command);
            }
        [Torque_Decorations.TorqueCallBack("", "ActionMap", "blockBind", "( %this, %otherMap, %command )", 3, 53000, false)]
        public void ActionMapblockBind(string thisobj, string othermap, string command)
            {
            if (Util._isObject(othermap))
                {
                Util._error("ActionMap::blockBind - \"" + othermap + "\" is not an object!");
                return;
                }
            string bind = ActionMap.getBinding(othermap, command);
            if (bind != "")
                ActionMap.bind(thisobj, Util.getField(bind, 0), Util.getField(bind, 1), "");
            }
        }
    }
