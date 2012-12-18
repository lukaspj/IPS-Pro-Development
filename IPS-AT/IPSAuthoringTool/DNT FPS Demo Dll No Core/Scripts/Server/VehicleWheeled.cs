using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        // This file contains script methods unique to the WheeledVehicle class.  All
        // other necessary methods are contained in "../server/scripts/vehicle.cs" in
        // which the "generic" Vehicle class methods that are shared by all vehicles,
        // (flying, hover, and wheeled) can be found.

        // Parenting is in place for WheeledVehicleData to VehicleData.  This should
        // make it easier for people to simply drop in new (generic) vehicles.  All that
        // the user needs to create is a set of datablocks for the new wheeled vehicle
        // to use.  This means that no (or little) scripting should be necessary.

        [Torque_Decorations.TorqueCallBack("", "WheeledVehicleData", "onAdd", "(%this, %obj,nameSpaceDepth)",  3, 2700, false)]
        public void WheeledVehicleDataOnAdd(string thisobj, string obj, string nameSpaceDepth)
            {
            int nsd = (nameSpaceDepth.AsInt() + 1);
            console.ParentExecute(thisobj, "onAdd", nsd, new[] {thisobj, obj});
            // Setup the car with some tires & springs
            for (int i = WheeledVehicle.getWheelCount(obj) - 1; i >= 0; i--)
                {
                WheeledVehicle.setWheelTire(obj, i, "CheetahCarTire");
                WheeledVehicle.setWheelSpring(obj, i, "CheetahCarSpring");
                WheeledVehicle.setWheelPowered(obj, i, false);
                }
            // Steer with the front tires
            WheeledVehicle.setWheelSteering(obj, 0, 1);
            WheeledVehicle.setWheelSteering(obj, 1, 1);

            // Only power the two rear wheels... assuming there are only 4 wheels.
            WheeledVehicle.setWheelPowered(obj, 2, true);
            WheeledVehicle.setWheelPowered(obj, 3, true);
            }

        [Torque_Decorations.TorqueCallBack("", "WheeledVehicleData", "onCollision", "(%this, %obj, %col, %vec, %speed)",  5, 2700, false)]
        public void WheeledVehicleDataOnCollision(string thisobj, string obj, string col, string vec, string speed)
            {
            // Collision with other objects, including items
            }

        // Used to kick the players out of the car that your crosshair is over
        [Torque_Decorations.TorqueCallBack("", "", "serverCmdcarUnmountObj", "(%client, %obj)",  2, 2700, false)]
        public void ServerCmdcarUnmountObj(string client, string obj)
            {
            SceneObject.unmount(obj);
            Player.setControlObject(obj, obj);

            TransformF ejectpos = SceneObject.getTransform(obj);
            ejectpos += new TransformF(0, 0, 5);
            SceneObject.setTransform(obj, ejectpos);

            string mvehicle = console.GetVarString(string.Format("{0}.mVehicle", obj));

            Point3F ejectvel = ShapeBase.getVelocity(mvehicle);
            ejectvel += new Point3F(0, 0, 10);

            ejectvel = ejectvel.vectorScale(float.Parse(console.GetVarString(string.Format("{0}.mass", console.getDatablock(obj)))));
            ShapeBase.applyImpulse(obj, ejectpos.MPosition, ejectvel);
            }

        // Used to flip the car over if it manages to get stuck upside down
        [Torque_Decorations.TorqueCallBack("", "", "serverCmdflipCar", "(%client)",  1, 2700, false)]
        public void ServerCmdflipCar(string client)
            {
            string player = console.GetVarString(string.Format("{0}.player", client));

            string car = Player.getControlObject(player).ToString(CultureInfo.InvariantCulture);
            if (console.GetClassName(car) != "WheeledVehicle") return;
            var carpos = SceneObject.getTransform(car);
            carpos += new TransformF(0, 0, 3);
            SceneObject.setTransform(car, carpos);
            }

        [Torque_Decorations.TorqueCallBack("", "", "serverCmdsetPlayerControl", "(%client)",  1, 2700, false)]
        public void ServerCmdsetPlayerControl(string client)
            {
            string player = console.GetVarString(string.Format("{0}.player", client));
            GameConnection.setControlObject(client, player);
            }

        [Torque_Decorations.TorqueCallBack("", "", "serverCmddismountVehicle", "(%client)",  1, 2700, false)]
        public void ServerCmddismountVehicle(string client)
            {
            string player = console.GetVarString(string.Format("{0}.player", client));
            string car = Player.getControlObject(player).ToString(CultureInfo.InvariantCulture);
            string passenger = SceneObject.getMountNodeObject(car, 0).ToString(CultureInfo.InvariantCulture);
            console.Call(console.getDatablock(passenger).AsString(), "doDismount", new[] {passenger, "true"});
            GameConnection.setControlObject(client, player);
            }
        }
    }