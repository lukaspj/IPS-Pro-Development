using System.Globalization;
using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "enableManualDetonation", "(%obj)",  1, 2200, false)]
        public void EnableManualDetonation(string thisobj)
            {
            console.SetVar(string.Format("{0}.detonadeEnabled", thisobj), true);
            }

        [Torque_Decorations.TorqueCallBack("", "", "doManualDetonation", "(%obj)",  1, 2200, false)]
        public void DoManualDetonation(string obj)
            {
            Torque_Class_Helper tch = new Torque_Class_Helper("Item", "");
            tch.Props.Add("dataBlock", "Detonade");
            string nade = tch.Create(m_ts).ToString(CultureInfo.InvariantCulture);
            SimSet.pushToBack("MissionCleanUp", nade);
            SceneObject.setTransform(nade, SceneObject.getTransform(obj));
            console.SetVar("sourceObject", console.GetVarString(string.Format("{0}.sourceObject", obj)));
            SimObject.schedule(nade, "50", "setDamageState", "Destroyed");
            console.deleteVariables(obj);
            }

        [Torque_Decorations.TorqueCallBack("", "Detonade", "onDestroyed", "(%this, %object, %lastState)",  3, 2200, false)]
        public void DetonadeOnDestroyed(string thisobj, string obj, string laststate)
            {
            RadiusDamage(obj, SceneObject.getTransform(obj).AsString(), "10", "25", "DetonadeDamage", "2000");
            }

        [Torque_Decorations.TorqueCallBack("", "GrenadeLauncherImage", "onMount", "(%this, %obj, %slot,nameSpaceDepth)",  4, 2200, false)]
        public void GrenadeLauncherImageOnMount(string thisobj, string obj, string slot, string nameSpaceDepth)
            {
            // Make it ready
            console.SetVar(string.Format("{0}.detonadeEnabled", obj), true);
            int nsd = (nameSpaceDepth.AsInt() + 1);
            console.ParentExecute(thisobj, "onMount", nsd, new[] {thisobj, obj, slot});
            }

        [Torque_Decorations.TorqueCallBack("", "GrenadeLauncherImage", "onAltFire", "(%this, %obj, %slot)",  3, 2200, false)]
        public void GrenadeLauncherImageOnAltFire(string thisobj, string obj, string slot)
            {
            }
        }
    }