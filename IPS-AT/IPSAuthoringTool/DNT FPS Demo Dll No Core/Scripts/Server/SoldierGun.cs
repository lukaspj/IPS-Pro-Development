using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "Soldier_gunImage", "onMount", "(%this, %obj, %slot,nameSpaceDepth)",  4, 2200, false)]
        public void Soldier_gunImageOnMount(string thisobj, string obj, string slot, string nameSpaceDepth)
            {
            // Make it ready
            //Parent::onMount(%this, %obj, %slot); See Decoration
            int nsd = (nameSpaceDepth.AsInt() + 1);
            console.ParentExecute(thisobj, "onMount", nsd, new[] {thisobj, obj, slot});
            }

        [Torque_Decorations.TorqueCallBack("", "Soldier_gunImage", "onAltFire", "(%this, %obj, %slot)",  3, 2200, false)]
        public void Soldier_gunImageOnAltFire(string thisobj, string obj, string slot)
            {
            console.print("Fire Grenade!");
            }
        }
    }