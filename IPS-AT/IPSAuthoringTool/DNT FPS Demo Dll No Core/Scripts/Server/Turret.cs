﻿using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "cs_init_TurretShapeData_vars", "",  0, 2900, true)]
        public void TurretShapeDataCsInitVars()
            {
            console.SetVar("$TurretShape::RespawnTime", (30*1000));
            console.SetVar("$TurretShape::DestroyedFadeDelay", 5*1000);
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onAdd", "(%this, %obj)",  2, 2900, false)]
        public void TurretShapeData_OnAdd(string thisobj, string obj)
            {
            ShapeBase.setRechargeRate(obj, console.GetVarFloat(thisobj + ".rechargeRate"));
            ShapeBase.setEnergyLevel(obj, console.GetVarFloat(thisobj + ".MaxEnergy"));
            ShapeBase.setRepairRate(obj, 0);
            if (console.GetVarString(string.Format("{0}.mountable", obj)) == "1" || console.GetVarString(string.Format("{0}.mountable", obj)) == "")
                console.Call(thisobj, "isMountable", new[] {obj, "true"});
            else
                console.Call(thisobj, "isMountable", new[] {obj, "false"});
            if (console.GetVarString(string.Format("{0}.nameTag", thisobj)) != "")
                ShapeBase.setShapeName(obj, console.GetVarString(string.Format("{0}.nameTag", thisobj)));
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numWeaponMountPoints", thisobj)); i++)
                {
                ShapeBaseShapeBaseIncInventory(obj, console.GetVarString(string.Format("{0}.weapon[{1}]", thisobj, i)), "1");
                string weaponAmmo = console.GetVarString(string.Format("{0}.weaponAmmo[{1}]", thisobj, i));


                string weaponAmmoAmount = console.GetVarString(string.Format("{0}.weaponAmmoAmount[{1}]", thisobj, i));
                ShapeBaseShapeBaseIncInventory(obj, weaponAmmo, weaponAmmoAmount);


                bool startLoaded = console.GetVarString(string.Format("{0}.startLoaded", thisobj)) == "1" ? true : false;

                ShapeBase.mountImage(obj, console.GetVarString(string.Format("{0}.weapon[{1}].Image", thisobj, i)), i, startLoaded, "");
                ShapeBase.setImageGenericTrigger(obj, i, 0, false);
                }
            if (console.GetVarString(string.Format("{0}.enterSequence", thisobj)) != "")
                {
                console.SetVar(string.Format("{0}.entranceThread", obj), 0);

                string et = console.GetVarString(obj + ".entranceThread");
                string es = console.GetVarString(thisobj + ".enterSequence");
                ShapeBase.playThread(obj, et.AsInt(), es);
                ShapeBase.pauseThread(obj, et.AsInt());
                }
            else
                {
                console.SetVar(string.Format("{0}.entranceThread", obj), "-1");
                }
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onRemove", "(%this, %obj)",  2, 2900, false)]
        public void TurretShapeData_OnRemove(string thisobj, string obj)
            {
            for (int i = 0; i < console.GetVarInt(thisobj + ".numMountPoints"); i++)
                {
                if (console.Call(obj, "getMountNodeObject", new[] {i.AsString()}) == "") continue;
                string passenger = console.Call(obj, "getMountNodeObject", new[] {i.AsString()});
                console.Call(console.getDatablock(passenger).AsString(), "doDismount", new[] {passenger, "true"});
                }
            }

        [Torque_Decorations.TorqueCallBack("", "MissionGroup", "respawnTurret", "(%this, %datablock, %className, %transform, %static, %respawn)",  6, 2900, false)]
        public string MissionGroupRespawnTurret(string thisobj, string datablock, string classname, string transform, string isstatic, string respawn)
            {
            var turret = new Torque_Class_Helper(classname, "");
            turret.Props.Add("datablock", datablock);
            turret.Props.Add("static", isstatic);
            turret.Props.Add("respawn", respawn);
            string turretid = turret.Create(m_ts).ToString(CultureInfo.InvariantCulture);

            SceneObject.setTransform(turretid, new TransformF(transform));
            SimSet.pushToBack("MissionGroup", turretid);
            return turretid;
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "damage", "(%this, %turret, %sourceObject, %position, %damage, %damageType)",  6, 2900, false)]
        private void TurretShapeData_damage(string thisobj, string turret, string sourceobject, string position, string damage, string datatype)
            {
            if (ShapeBase.getDamageState(turret) == "Dead")
                return;
            ShapeBase.applyDamage(turret, float.Parse(damage));

            // Update the numerical Health HUD
            string mountedobject = console.Call(turret, "getObjectMount");
            if (mountedobject != "")
                console.Call(mountedobject, "updateHealth");
            if (Player.getState(turret) != "Dead") return;
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numMountPoints", thisobj)); i++)
                {
                string player = console.Call(turret, "getMountNodeObject", new[] {i.AsString()});
                if (player != "0")
                    console.Call(player, "killWithSource", new[] {"sourceobject", "InsideTurret"});
                }
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onDamage", "%this, %obj, %delta",  3, 2900, false)]
        private void TurretShapeData_onDamage(string thisobj, string obj, string delta)
            {
            // This method is invoked by the ShapeBase code whenever the
            // object's damage level changes.
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onDestroyed", "(%this, %obj, %lastState)",  3, 2900, false)]
        public void TurretShapeDataOnDestroy(string thisobj, string obj, string laststate)
            {
            // This method is invoked by the ShapeBase code whenever the
            // object's damage state changes.

            // Fade out the destroyed object.  Then schedule a return.
            int destroydelay = console.GetVarInt("$TurretShape::DestroyedFadeDelay");
            console.Call(obj, "startFade", new[] {"1000", destroydelay.AsString(), "true"});
            SimObject.schedule(obj, (destroydelay + 1000).AsString(), "delete");
            if (console.Call(obj, "doRespawn") != "")
                //con.Eval(string.Format(@"MissionGroup.schedule($TurretShape::RespawnTime, ""respawnTurret"",{0},{1}.getClassName(), {1}.getTransform(), true, true);", thisobj, obj), false);
            SimObject.schedule("MissionGroup", console.GetVarString("$TurretShape::RespawnTime"), "respawnTurret", thisobj, console.GetClassName(obj),
                               SceneObject.getTransform(obj).AsString(), "true", "true");
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onDisabled", "(%this, %obj, %lastState)",  3, 2900, false)]
        public void TurrestShapeDataOnDisable(string thisobj, string obj, string laststate)
            {
            // This method is invoked by the ShapeBase code whenever the
            // object's damage state changes.
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onEnabled", "(%this, %obj, %lastState)",  3, 2900, false)]
        public void TurrestShapeDataOnEnable(string thisobj, string obj, string laststate)
            {
            // This method is invoked by the ShapeBase code whenever the
            // object's damage state changes.
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "isMountable", "(%this, %obj, %val)",  3, 2900, false)]
        public void TurretShapeDataIsMountable(string thisobj, string obj, string val)
            {
            console.SetVar(string.Format("{0}.mountable", obj), val);
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onMountObject", "(%this, %turret, %player, %node)",  4, 2900, false)]
        public void TurretShapeDataOnMountObject(string thisobj, string turret, string player, string node)
            {
            if (console.GetVarInt(turret + ".entranceThread") < 0) return;
            string et = console.GetVarString(turret + ".entranceThread");
            ShapeBase.setThreadDir(turret, et.AsInt(), false);
            ShapeBase.setThreadPosition(turret, et.AsInt(), 0);
            ShapeBase.playThread(turret, et.AsInt(), "");
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onUnmountObject", "(%this, %turret, %player)",  3, 2900, false)]
        public void TurretShapeDataOnUnmountObject(string thisobj, string turret, string player)
            {
            if (console.GetVarInt(string.Format("{0}.entranceThread", turret)) < 0) return;
            string et = console.GetVarString(string.Format("{0}.entranceThread", turret));
            // Play the entrance thread backwards for an exit

            ShapeBase.setThreadDir(turret, et.AsInt(), false);
            ShapeBase.setThreadPosition(turret, et.AsInt(), (float) 1.0);
            ShapeBase.playThread(turret, et.AsInt(), "");
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "mountPlayer", "(%this, %turret, %player)",  3, 2900, false)]
        public void TurretShapeDataMountPlayer(string thisobj, string turret, string player)
            {
            if (console.isObject(turret) && ShapeBase.getDamageState(turret) != "Destroyed")
                console.Call(thisobj, "setMountTurret", new[] {turret, player});
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "setMountTurret", "(%this, %turret, %player)",  3, 2900, false)]
        public void TurretShapeDataSetMountTurret(string thisobj, string turret, string player)
            {
            if (!console.isObject(turret) || ShapeBase.getDamageState(turret) == "Destroyed") return;
            string node = console.Call(thisobj, "findEmptySeat", new[] {turret, player});
            if (int.Parse(node) < 0) return;
            console.Call(turret, "mountObject", new[] {player, node});
            console.SetVar(string.Format("{0}.mVehicle", player), turret);
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "findEmptySeat", "(%this, %turret, %player)",  3, 2900, false)]
        public string TurrestshapeDataFindEmptySeat(string thisobj, string turret, string player)
            {
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numMountPoints", thisobj)); i++)
                {
                string node = console.Call(turret, "getMountNodeObject", new[] {i.AsString()});
                if (node == "0")
                    return i.ToString(CultureInfo.InvariantCulture);
                }
            return "-1";
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "switchSeats", "(%this, %turret, %player)",  3, 2900, false)]
        public string TurrestShapeDataSwitchSeats(string thisobj, string turret, string player)
            {
            for (int i = 0; i < console.GetVarInt(thisobj + ".numMountPoints"); i++)
                {
                string node = console.Call(turret, "getMountNodeObject", new[] {i.AsString()});
                if (node == player || node != "0" || node != "")
                    continue;
                if (node == "0")
                    return i.ToString(CultureInfo.InvariantCulture);
                }
            return "-1";
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onMount", "(%this, %turret, %player, %node)",  4, 2900, false)]
        public void TurretShapeDataOnMount(string thisobj, string turret, string player, string node)
            {
            console.Eval(string.Format("{0}.client.RefreshVehicleHud({1}, {2}.reticle,{2}.zoomReticle);", player, turret, thisobj), false);
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "onUnmount", "(%this, %turret, %player, %node)",  4, 2900, false)]
        public void TurretShapeDataOnUnmount(string thisobj, string turret, string player, string node)
            {
            console.Call(player, "RefreshVehicleHud", new[] {"0", "", ""});
            }

        [Torque_Decorations.TorqueCallBack("", "TurretShapeData", "damage", "(%this, %sourceObject, %position, %damage, %damageType)",  5, 2900, false)]
        public void TurretShapeDataDamage(string thisobj, string sourceObject, string position, string damage, string damagetype)
            {
            console.Call(console.getDatablock(thisobj).AsString(), "damage", new[] {thisobj, sourceObject, position, damage, damagetype});
            }

        [Torque_Decorations.TorqueCallBack("", "", "sendMsgClientKilled_TurretDamage", "( %msgType, %client, %sourceClient, %damLoc )",  4, 2900, false)]
        public void SendMsgClientKilledTurretDamage(string msgtype, string client, string sourceclient, string damloc)
            {
            if (sourceclient == "")
                MessageAll(msgtype, "%1 was shot down by a turret!", console.GetVarString(string.Format("{0}.playerName", client)) + ");");
            else if (sourceclient == client)
                MessageAll(msgtype, "%1 kill by his own turret!", console.GetVarString(string.Format("{0}.playerName", client)) + ");");
            else
                MessageAll(msgtype, "%1 was killed by a turret of %2!", console.GetVarString(string.Format("{0}.playerName", client)) + ");", console.GetVarString(sourceclient + ".playerName"));
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "onAdd", "(%this, %obj,nameSpaceDepth)",  3, 2900, false)]
        public void AiTurretShapeDataOnAdd(string thisobj, string obj, string nameSpaceDepth)
            {
            int nsd = (nameSpaceDepth.AsInt() + 1);
            console.ParentExecute(thisobj, "onAdd", nsd, new[] {thisobj, obj});
            console.SetVar(string.Format("{0}.mountable", obj), false);
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "onThrow", "(%this, %user, %amount)",  3, 2900, false)]
        public string AiTurretShapeDataOnThrow(string thisobj, string user, string amount)
            {
            if (amount == "")
                amount = "1";

            if (console.GetVarString(string.Format("{0}.maxInventory", thisobj)) != "")
                if (int.Parse(amount) > console.GetVarInt(string.Format("{0}.maxInventory", thisobj)))
                    amount = console.GetVarInt(string.Format("{0}.maxInventory", thisobj)).ToString(CultureInfo.InvariantCulture);

            if (amount == "0")
                return "0";

            console.Call(user, "decInventory", new[] {thisobj, amount});
            TransformF rot = console.getEulerRotation(user);

            var tc_obj = new Torque_Class_Helper("AITurretShape", "");
            tc_obj.Props.Add("datablock", thisobj);
            tc_obj.Props.Add("rotation", string.Format("{0}0 0 1 {1}{0}", '"', rot.MPosition.z));
            tc_obj.Props.Add("count", "1");
            tc_obj.Props.Add("sourceObject", user);
            tc_obj.Props.Add("client", console.GetVarString(string.Format("{0}.client", user)));
            console.print("AI CONTROL 4");
            tc_obj.Props.Add("isAiControlled", "1");

            string obj = tc_obj.Create(m_ts).ToString(CultureInfo.InvariantCulture);
            SimSet.pushToBack("MissionGroup", obj);
            console.Call(obj, "addToIgnoreList", new[] {user});
            string client = console.GetVarString(string.Format("{0}.client", user));
            if (client != "")
                {
                if (console.GetVarString(string.Format("{0}.ownedTurrets", client)) == "")
                    console.SetVar(client + ".ownedTurrets", new Torque_Class_Helper("SimSet", "").Create(m_ts));
                string SimSet_id = console.GetVarString(string.Format("{0}.ownedTurrets", client));
                int countofitems = SimSet.getCount(SimSet_id);
                for (uint i = 0; i < countofitems; i++)
                    {
                    string turret = SimSet.getObject(SimSet_id, i).AsString();
                    console.Call(turret, "addToIgnoreList", new[] {obj});
                    console.Call(obj, "addToIgnoreList", new[] {turret});
                    }
                SimSet.pushToBack(SimSet_id, obj);
                }
            return obj;
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "onDestroyed", "(%this, %turret, %lastState,nameSpaceDepth)",  4, 2900, false)]
        public void AiTurretShapeData(string thisobj, string turret, string lastState, string nameSpaceDepth)
            {
            console.Call(turret, "playAudio", new[] {"0", "TurretDestroyed"});
            console.Call(turret, "setAllGunsFiring", new[] {"false"});
            console.Call(turret, "resetTarget");
            console.Call(turret, "setTurretState", new[] {"Destroyed"});
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numWeaponMountPoints", this)); i++)
                ShapeBase.setImageGenericTrigger(turret, i, 0, true);

            int nsd = (nameSpaceDepth.AsInt() );
            console.ParentExecute(thisobj, "onDestroyed", nsd, new[] {thisobj, turret, lastState});
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "OnScanning", "(%this, %turret)",  2, 2900, false)]
        public void AiTurretShapeDataOnScanning(string thisobj, string turret)
            {
            console.Call(turret, "startScanForTargets");
            ShapeBase.playAudio(turret, 0, "TurretScanningSound");
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "OnTarget", "(%this, %turret)",  2, 2900, false)]
        public void AITurretShapeData_OnTarget(string thisobj, string turret)
            {
            console.Call(turret, "startTrackingTarget");
            ShapeBase.playAudio(turret, 0, "TargetAquiredSound");
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "OnNoTarget", "(%this, %turret)",  2, 2900, false)]
        public void AITurretShapeData_OnNoTarget(string thisobj, string turret)
            {
            console.Call(turret, "setAllGunsFiring", new[] {"false"});
            console.Call(turret, "recenterTurret");
            ShapeBase.playAudio(turret, 0, "TargetLostSound");
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "OnFiring", "(%this, %turret)",  2, 2900, false)]
        public void AITurretShapeData_OnFiring(string thisobj, string turret)
            {
            console.Call(turret, "setAllGunsFiring", new[] {"true"});
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "OnThrown", "(%this, %turret)",  2, 2900, false)]
        public void AiTurretShapeDataOnThrown(string thisobj, string turret)
            {
            ShapeBase.playAudio(turret, 0, "TurretThrown");
            }

        [Torque_Decorations.TorqueCallBack("", "AITurretShapeData", "OnDeploy", "(%this, %turret)",  2, 2900, false)]
        public void AITurretShapeData_OnDeploy(string thisobj, string turret)
            {
            for (int i = 0; i < console.GetVarInt(string.Format("{0}.numWeaponMountPoints", thisobj)); i++)
                ShapeBase.setImageLoaded(turret, i, true);

            ShapeBase.playAudio(turret, 0, "TurretActivatedSound");
            }

        [Torque_Decorations.TorqueCallBack("", "DeployableTurretWeapon", "onUse", "(%this, %obj)",  2, 2900, false)]
        public void DeployableTurretWeaponOnUse(string thisobj, string obj)
            {
            //con.Eval(string.Format("Weapon::onUse({0},{1});", thisobj, obj), false);
            console.Call_Classname("Weapon", "onUse", new string[] {thisobj, obj});
            }

        [Torque_Decorations.TorqueCallBack("", "DeployableTurretWeapon", "onPickup", "(%this, %obj)",  4, 2900, false)]
        public void DeployableTurretWeaponOnPickup(string thisobj, string obj, string shape, string amount)
            {
            //con.Eval(string.Format("Weapon::onPickup({0},{1},{2},{3});", thisobj, obj, shape, amount), false);
            console.Call_Classname("Weapon", "onPickup", new string[] {thisobj, obj, shape, amount});
            }

        [Torque_Decorations.TorqueCallBack("", "DeployableTurretWeapon", "onInventory", "(%this, %obj, %amount)",  3, 2900, false)]
        public void DeployableTurretWeaponOnInventory(string thisobj, string obj, string amount)
            {
            if (console.GetVarInt(string.Format("{0}.client", obj)) != 0 && console.GetVarInt(string.Format("{0}.isAiControlled", obj)) == 0)
                GameConnectionSetAmmoAmountHud(console.GetVarString(obj + ".client"), "1", amount);


            //Sometimes....  Alice to the moon.
            /*
             * So here I was thinking that this line of code in the torque script...
             *    if ( !%amount && ( %slot = %obj.getMountSlot( %this.image ) ) != -1 )
             *    was checking to see if the slot was != to -1... but that is not the case.
             *    What it is actually checking is if %slot is blank or not.  As in is the temporary 
             *    variable %slot a variable or not, (i.e. it's id is -1)
             *    
             * It's nice how they use error suppression in there tokens to hide
             * the error message that the function doesn't exist from the console.
             * But... really?  I mean, you couldn't comment that that was what the fuck you
             * where doing??????
             *    
             * Well shit, I do believe there could be an easier way.....
             */
            //error("Amount = " + amount);
            if (amount == "0" && console.objectHasMethod(obj, "getMountSlot"))
                console.Call(obj, "cycleWeapon", new[] {"prev"});
            }

        [Torque_Decorations.TorqueCallBack("", "DeployableTurretWeaponImage", "onMount", "(%this, %obj, %slot)",  3, 2900, false)]
        public void DeployableTurretWeaponImageOnMount(string thisobj, string obj, string slot)
            {
            ShapeBase.setImageAmmo(obj, slot.AsInt(), true);
            string numTurrets = console.Call(obj, "getInventory", new[] {console.GetVarString(thisobj + ".item")});
            if (console.GetVarString(string.Format("{0}.client", obj)) != "" && console.GetVarString(string.Format("{0}.isAiControlled", obj)) == "0")
                console.Eval(string.Format("{0}.client.RefreshWeaponHud( 1, {1}.item.previewImage,{1}.item.reticle, {1}.item.zoomReticle, {2});", obj, thisobj, numTurrets), false);
            }

        [Torque_Decorations.TorqueCallBack("", "DeployableTurretWeaponImage", "onUnmount", "(%this, %obj, %slot)",  3, 2900, false)]
        public void DeployableTurretWeaponImageOnUnmount(string thisobj, string obj, string slot)
            {
            //print("AI CONTROL 7");
            if (console.GetVarString(string.Format("{0}.client", obj)) != "" && console.GetVarString(string.Format("{0}.isAiControlled", obj)) == "0")
                console.Call(obj + ".client", "RefreshWeaponHud", new[] {"0", "", "", "", ""});
            }

        [Torque_Decorations.TorqueCallBack("", "DeployableTurretWeaponImage", "onFire", "(%this, %obj, %slot)",  3, 2900, false)]
        public void DeployableTurretWeaponImageOnFire(string thisobj, string obj, string slot)
            {
            SimObject.schedule(obj, "0", "throw", console.GetVarString(thisobj + ".item"));
            }
        }
    }