using System;
using System.Globalization;
using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "ProximityMineData", "onThrow", "( %this, %user, %amount )", 3, 2300, false)]
        public string ProximityMineDataOnThrow(string datablock, string player, string amount)
            {
            ShapeBaseShapeBaseDecInventory(player, datablock, "1");

            Torque_Class_Helper tch = new Torque_Class_Helper("ProximityMine", "");
            tch.Props.Add("datablock", datablock);
            tch.Props.Add("sourceObject", player);
            tch.Props.Add("rotation", string.Format("\"0 0 1 {0} \"", new Random().NextDouble() * 360));
            tch.Props.Add("static", "false");
            tch.Props.Add("client", console.GetVarString(string.Format("{0}.client", player)));
            uint obj = tch.Create(m_ts);
            SimSet.pushToBack("MissionCleanup", obj.AsString());
            return obj.AsString();
            }

        [Torque_Decorations.TorqueCallBack("", "ProximityMineData", "onTriggered", "( %this, %obj, %target )", 3, 2300, false)]
        public void ProximityMineDataOnTriggered(string datablock, string obj, string target)
            {
            }

        [Torque_Decorations.TorqueCallBack("", "ProximityMineData", "onExplode", "( %this, %obj, %position )", 3, 2300, false)]
        public void ProximityMineDataOnExplode(string datablock, string shapebase, string position)
            {
            // Damage objects within the mine's damage radius
            if (console.GetVarFloat(datablock + ".damageRadius") > 0)
                RadiusDamage(shapebase, position, console.GetVarString(string.Format("{0}.damageRadius", datablock)), console.GetVarString(string.Format("{0}.radiusDamage", datablock)), console.GetVarString(string.Format("{0}.damageType", datablock)), console.GetVarString(string.Format("{0}.areaImpulse", datablock)));
            }

        [Torque_Decorations.TorqueCallBack("", "ProximityMineData", "damage", "( %this, %obj, %position, %source, %amount, %damageType )", 6, 2300, false)]
        public void ProximityMineDataDamage(string datablock, string shapebase, string position, string source, string amount, string damageType)
            {
            // Explode if any damage is applied to the mine
            int r = 50 + (new Random().Next(0, 50));
            SimObject.schedule(shapebase, r.AsString(), "explode");
            }

        [Torque_Decorations.TorqueCallBack("", "", "sendMsgClientKilled_MineDamage", "( %msgType, %client, %sourceClient, %damLoc )", 4, 2300, false)]
        public void SendMsgClientKilledMineDamage(string msgType, string client, string sourceClient, string damLoc)
            {
            // Customized kill message for deaths caused by proximity mines
            if (sourceClient == "")
                MessageAll(msgType, "%1 was blown up!", console.GetVarString(string.Format("{0}.playerName", client)));
            else if (sourceClient == client)
                MessageAll(msgType, "%1 stepped on his own mine!", console.GetVarString(string.Format("{0}.playerName", client)));
            else
                MessageAll(msgType, "%1 was blown up by %2!", console.GetVarString(string.Format("{0}.playerName", client)), console.GetVarString(string.Format("{0}.playerName", sourceClient)));
            }

        // ----------------------------------------------------------------------------
        // Player deployable proximity mine
        // ----------------------------------------------------------------------------

        // Cannot use the Weapon class for ProximityMineData datablocks as it is already tied
        // to ItemData.

        [Torque_Decorations.TorqueCallBack("", "ProxMine", "onUse", "(%this, %obj)", 2, 2300, false)]
        public void ProxMineOnUser(string thisobj, string obj)
            {
            // Act like a weapon on use
            console.Call_Classname("Weapon", "onUse", new string[] { thisobj, obj });
            }

        [Torque_Decorations.TorqueCallBack("", "ProxMine", "onPickup", "( %this, %obj, %shape, %amount )", 4, 2300, false)]
        public void ProxMineOnPickup(string thisobj, string obj, string shape, string amount)
            {
            // Act like a weapon on pickup
            console.Call_Classname("Weapon", "onPickup", new string[] { thisobj, obj, shape, amount });
            }

        [Torque_Decorations.TorqueCallBack("", "ProxMine", "onInventory", "( %this, %obj, %amount )", 3, 2300, false)]
        public void ProxMineOnInventory(string thisobj, string player, string amount)
            {
            if (console.isObject(player + ".client"))
                GameConnectionSetAmmoAmountHud(console.GetVarString(player + ".client"), "1", amount);
            if (amount == "0" && console.objectHasMethod(player, "getMountSlot"))
                ShapeBaseCycleWeapon(player, "prev");
            }

        [Torque_Decorations.TorqueCallBack("", "ProxMineImage", "onMount", "( %this, %obj, %slot )", 3, 2300, false)]
        public void ProxMineImageOnMount(string thisobj, string player, string slot)
            {
            ShapeBase.setImageAmmo(player, slot.AsInt(), true);
            int numofmines = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(thisobj + ".item"));

            string previewImage = console.GetVarString(string.Format("{0}.item.previewImage", thisobj));
            string reticle = console.GetVarString(string.Format("{0}.item.reticle", thisobj));
            string zoomReticle = console.GetVarString(string.Format("{0}.item.zoomReticle", thisobj));

            GameConnectionRefreshWeaponHud(console.GetVarString(player + ".client"), "1", previewImage, reticle, zoomReticle, numofmines.AsString());
            }

        [Torque_Decorations.TorqueCallBack("", "ProxMineImage", "onUnmount", "( %this, %obj, %slot )", 3, 2300, false)]
        public void ProxMineImageOnUnmount(string thisobj, string player, string slot)
            {
            GameConnectionRefreshWeaponHud(console.GetVarString(player + ".client"), "0", "", "", "", "");

            }

        [Torque_Decorations.TorqueCallBack("", "ProxMineImage", "onFire", "( %this, %obj, %slot )", 3, 2300, false)]
        public void ProxMineImageOnFire(string image, string player, string slot)
            {
            ShapeBaseShapeBaseThrow(player, console.GetVarString(image + ".item"));

            }
        }
    }