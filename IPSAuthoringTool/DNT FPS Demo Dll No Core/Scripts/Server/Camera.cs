using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        // Global movement speed that affects all cameras.  This should be moved
        // into the camera datablock.


        [Torque_Decorations.TorqueCallBack("", "", "InitializeCamera", "()", 0, 19000, true)]
        public void InitializeCamera()
            {
            console.SetVar("$Camera::movementSpeed", 30);
            }


        //-----------------------------------------------------------------------------
        // Camera methods
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "Camera", "onAdd", "(%this,%obj)", 2, 19000, false)]
        public void CameraOnAdd(string thisobj, string obj)
            {
            console.Call(thisobj, "setMode", new[] { console.GetVarString(thisobj + ".mode") });
            }

        [Torque_Decorations.TorqueCallBack("", "Camera", "setMode", "(%this,%mode,%arg1,%arg2,%arg3)", 5, 19000, false)]
        public void CameraSetMode(string thisobj, string mode, string arg1, string arg2, string arg3)
            {
            // Punt this one over to our datablock
            console.Call(console.getDatablock(thisobj).AsString(), "setMode", new[] { thisobj, mode, arg1, arg2, arg3 });
            }




        [Torque_Decorations.TorqueCallBack("", "Observer", "onTrigger", "(%this,%obj,%trigger,%state)", 4, 19000, false)]
        public void ObserverObserverOnTrigger(string thisobj, string obj, string trigger, string state)
            {
            // state = 0 means that a trigger key was released
            if (state.AsBool() == false)
                return;

            // Default player triggers: 0=fire 1=altFire 2=jump
            string client = ShapeBase.getControllingClient(obj).ToString(CultureInfo.InvariantCulture);
            // Evaluate(obj + ".getControllingClient();");

            string mode = console.GetVarString(string.Format("{0}.mode", obj));
            switch (mode)
                {
                case "Observer":
                    // Do something interesting.
                    break;
                case "Corpse":
                    //Here is a winner... if you send the prepare too soon, it will crash out the client.  The GameConnectionOnControlObjectChange needs
                    //time to run, and since it's still processing the first change (Switch to camera) this second change (Camera->player) will crash
                    //out the client.
                    GameConnectiOnPreparePlayer(client);
                    //if (!console.GetVarBool(client + ".isrespawning"))
                    //    {
                    //    console.SetVar(client + ".isrespawning", true);
                    //    SimObject.schedule(client, "2000", "preparePlayer", client); //   GameConnectiOnPreparePlayer(client);
                    //    }
                    break;
                }
            }

        [Torque_Decorations.TorqueCallBack("", "Observer", "setMode", "(%this,%obj,%mode,%arg1,%arg2,%arg3)", 6, 19000, false)]
        public void ObserverObserverSetMode(string thisobj, string obj, string mode, string arg1, string arg2, string arg3)
            {
            switch (mode)
                {
                case "Observer":
                    console.Call(obj, "setFlyMode");
                    break;
                case "Corpse":
                    string transform = SceneObject.getTransform(arg1).AsString();
                    Camera.setOrbitMode(obj, arg1, new TransformF(transform), (float)0.5, (float)4.5, (float)4.5, false, new Point3F(), false);
                    console.SetVar(obj + ".orbitObj", arg1);
                    break;
                }
            console.SetVar(string.Format("{0}.mode", obj), mode);
            }
        }
    }