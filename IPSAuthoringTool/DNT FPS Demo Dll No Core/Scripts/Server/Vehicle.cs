using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    //-----------------------------------------------------------------------------
    // Torque
    // Copyright GarageGames, LLC 2011
    //-----------------------------------------------------------------------------

    // Parenting is in place for WheeledVehicleData to VehicleData.  This should
    // make it easier for people to simply drop in new (generic) vehicles.  All that
    // the user needs to create is a set of datablocks for the new wheeled vehicle
    // to use.  This means that no (or little) scripting should be necessary.

    // Special, or unique vehicles however will still require some scripting.  They
    // may need to override the onAdd() function in order to mount weapons,
    // differing tires/springs, etc., almost everything else is taken care of in the
    // WheeledVehicleData and VehicleData methods.  This helps us by not having to
    // duplicate the same code for every new vehicle.

    // In theory this would work for HoverVehicles and FlyingVehicles also, but
    // hasn't been tested or fully implemented for those classes -- yet.
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "VehicleData", "onAdd", "(%this, %obj)",  2, 2600, false)]
        public void VehicleDataOnAdd(string thisobj, string obj)
            {
            ShapeBase.setRechargeRate(obj, console.GetVarFloat(string.Format("{0}.rechargeRate", thisobj)));
            ShapeBase.setEnergyLevel(obj, console.GetVarFloat(string.Format("{0}.MaxEnergy", thisobj)));
            ShapeBase.setRepairRate(obj, 0);

            if ((console.GetVarBool(string.Format("{0}.mountable", obj)) || console.GetVarString(string.Format("{0}.mountable", obj)) == ""))
                console.Call(thisobj, "isMountable", new[] {obj, "true"});
            else
                console.Call(thisobj, "isMountable", new[] {obj, "false"});

            string nametag = console.GetVarString(string.Format("{0}.nameTag", thisobj));


            if (nametag.Trim() != "")
                ShapeBase.setShapeName(obj, nametag);
            }

        [Torque_Decorations.TorqueCallBack("", "VehicleData", "onRemove", "(%this, %obj)",  2, 2600, false)]
        public void VehicleDataOnRemove(string thisobj, string obj)
            {
            // if there are passengers/driver, kick them out
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numMountPoints", console.getDatablock(obj))); i++)
                {
                string passenger = SceneObject.getMountNodeObject(obj, i).ToString(CultureInfo.InvariantCulture);
                if (passenger != "0")
                    console.Call(console.getDatablock(passenger).AsString(), "doDismount", new[] { passenger, "true" });
                }
            }

        // ----------------------------------------------------------------------------
        // Vehicle player mounting and dismounting
        // ----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "VehicleData", "isMountable", "(%this, %obj, %val)",  3, 2600, false)]
        public void VehicleDataIsMountable(string thisobj, string obj, string val)
            {
            console.SetVar(string.Format("{0}.mountable", obj), val);
            }

        [Torque_Decorations.TorqueCallBack("", "VehicleData", "mountPlayer", "(%this, %vehicle, %player)",  3, 2600, false)]
        public void VehicleDataMountPlayer(string thisobj, string vehicle, string player)
            {
            if (!console.isObject(vehicle) || ShapeBase.getDamageState(vehicle) == "Destroyed") return;
            ShapeBase.startFade(player, 1000, 0, true);
            SimObject.schedule(thisobj, "1000", "setMountVehicle", vehicle, player);
            SimObject.schedule(player, "1500", "startFade", "1000", "0", "false");
            }

        [Torque_Decorations.TorqueCallBack("", "VehicleData", "setMountVehicle", "(%this, %vehicle, %player)",  3, 2600, false)]
        public void VehicleDataSetMountVehicle(string thisobj, string vehicle, string player)
            {
            if (!console.isObject(vehicle) || ShapeBase.getDamageState(vehicle) == "Destroyed") return;
            string node = console.Call(thisobj, "findEmptySeat", new[] {vehicle, player});
            if (node == "-1") return;
            SceneObject.mountObject(vehicle, player, int.Parse(node),new TransformF( true));
            console.SetVar(string.Format("{0}.mVehicle", player), vehicle);
            }

        [Torque_Decorations.TorqueCallBack("", "VehicleData", "findEmptySeat", "(%this, %vehicle, %player)",  3, 2600, false)]
        public string VehicleDataFindEmptySeat(string thisobj, string vehicle, string player)
            {
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numMountPoints", thisobj)); i++)
                {
                string node = SceneObject.getMountNodeObject(vehicle, i).ToString(CultureInfo.InvariantCulture);
                if (node != "0")
                    return i.ToString(CultureInfo.InvariantCulture);
                }
            return "-1";
            }

        [Torque_Decorations.TorqueCallBack("", "VehicleData", "switchSeats", "(%this, %vehicle, %player)",  3, 2600, false)]
        public string VehicleDataSwitchSeats(string thisobj, string vehicle, string player)
            {
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numMountPoints", thisobj)); i++)
                {
                string node = SceneObject.getMountNodeObject(vehicle, i).ToString(CultureInfo.InvariantCulture);
                if (node == player || int.Parse(node) > 0)
                    continue;
                if (node == "0")
                    return i.ToString(CultureInfo.InvariantCulture);
                }
            return "-1";
            }
        }
    }