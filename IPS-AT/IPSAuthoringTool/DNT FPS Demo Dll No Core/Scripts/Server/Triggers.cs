using System.Collections.Generic;
using WinterLeaf.Classes;
using WinterLeaf.Enums;
using WinterLeaf.Containers;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    //-----------------------------------------------------------------------------
    // Torque
    // Copyright GarageGames, LLC 2011
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // DefaultTrigger is used by the mission editor.  This is also an example
    // of trigger methods and callbacks.

    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "DefaultTrigger", "onEnterTrigger", "(%this,%trigger,%obj)",  3, 1100, false)]
        public void DefaultTriggerOnEnterTrigger(string thisobj,string trigger,string obj)
            {
            // This method is called whenever an object enters the %trigger
            // area, the object is passed as %obj.
            }
        [Torque_Decorations.TorqueCallBack("", "DefaultTrigger", "onLeaveTrigger", "(%this,%trigger,%obj)",  3, 1100, false)]
        public void DefaultTriggerOnLeaveTrigger(string thisobj, string trigger, string obj)
            {
            // This method is called whenever an object leaves the %trigger
            // area, the object is passed as %obj.
            }
        [Torque_Decorations.TorqueCallBack("", "DefaultTrigger", "onTickTrigger", "(%this,%trigger)",  2, 1100, false)]
        public void DefaultTriggerOnTickTrigger(string thisobj,string trigger)
            {
            // This method is called every tickPerioMS, as long as any
            // objects intersect the trigger.

            // You can iterate through the objects in the list by using these
            // methods:
            //    %trigger.getNumObjects();
            //    %trigger.getObject(n);
            }
        }
    }
