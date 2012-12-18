﻿using System;
using System.Collections.Generic;
using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    //-----------------------------------------------------------------------------
    // Torque
    // Copyright GarageGames, LLC 2011
    //-----------------------------------------------------------------------------

    // // Damage Rate for entering Liquid
    // $DamageLava = 0.01;
    // $DamageHotLava = 0.01;
    // $DamageCrustyLava = 0.01;

    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "Player_Init_Globals", "", 0, 2400, true)]
        public void PlayerInitGlobals()
            {
            console.SetVar("$CorpseTimeoutValue", 45 * 1000);
            console.SetVar("$PlayerDeathAnim::TorsoFrontFallForward", 1);
            console.SetVar("$PlayerDeathAnim::TorsoFrontFallBack", 2);
            console.SetVar("$PlayerDeathAnim::TorsoBackFallForward", 3);
            console.SetVar("$PlayerDeathAnim::TorsoLeftSpinDeath", 4);
            console.SetVar("$PlayerDeathAnim::TorsoRightSpinDeath", 5);
            console.SetVar("$PlayerDeathAnim::LegsLeftGimp", 6);
            console.SetVar("$PlayerDeathAnim::LegsRightGimp", 7);
            console.SetVar("$PlayerDeathAnim::TorsoBackFallForward", 8);
            console.SetVar("$PlayerDeathAnim::HeadFrontDirect", 9);
            console.SetVar("$PlayerDeathAnim::HeadBackFallForward", 10);
            console.SetVar("$PlayerDeathAnim::ExplosionBlowBack", 11);
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onAdd", "(%this, %obj)", 2, 2400, false)]
        public void ArmorOnAdd(string datablock, string player)
            {
            console.SetVar(string.Format("{0}.mountVehicle", player), true);
            ShapeBase.setRechargeRate(player, float.Parse(console.GetVarString(string.Format("{0}.rechargeRate", datablock))));
            ShapeBase.setRepairRate(player, 0);
            SimObject.schedule(player, "50", "updateHealth");
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onRemove", "(%this, %obj)", 2, 2400, false)]
        public void ArmorOnRemove(string datablock, string player)
            {
            if (console.GetVarString(string.Format("{0}.client.player", player)) == player)
                console.SetVar(string.Format("{0}.client.player", player), 0);
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onNewDataBlock", "(%this, %obj)", 2, 2400, false)]
        public void ArmorOnNewDataBlock(string datablock, string player)
            {
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onMount", "(%this, %obj, %vehicle, %node)", 4, 2400, false)]
        public void ArmorOnMount(string datablock, string player, string vehicle, string node)
            {
            string vehicleDataBlock = console.getDatablock(vehicle).ToString(CultureInfo.InvariantCulture);
            if (node == "0")
                {
                SceneObject.setTransform(player, new TransformF("0 0 0 0 0 1 0"));
                string mountPose = console.GetVarString(string.Format("{0}.mountPose[{1}]", vehicleDataBlock, node));
                Player.setActionThread(player, mountPose, true, true);


                console.SetVar(string.Format("{0}.lastWeapon", player), ShapeBase.getMountedImage(player, WeaponSlot).ToString(CultureInfo.InvariantCulture));

                if (console.isObject(string.Format("{0}.client", player)))
                    {
                    string client = console.GetVarString(string.Format("{0}.client", player));
                    bool isfirstperson = GameConnection.isFirstPerson(client);
                    console.SetVar(string.Format("{0}.lastperson", player), isfirstperson);
                    }

                ShapeBase.unmountImage(player, WeaponSlot);

                Player.setControlObject(player, vehicle);
                if (console.GetClassName(player) == "Player")
                    {
                    console.commandToClient(console.GetVarString(string.Format("{0}.client", player)), console.addTaggedString("toggleVehicleMap"),
                                            new[] { "true" });
                    }
                }
            else
                {
                string pose = console.GetVarString(string.Format("{0}.mountPose[{1}]", vehicleDataBlock, node));
                Player.setActionThread(player, pose != "" ? pose : "root", false, true);
                }
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onUnmount", "(%this, %obj, %vehicle, %node)", 4, 2400, false)]
        public void ArmorOnUnmount(string datablock, string player, string vehicle, string node)
            {
            if (node != "0") return;
            ShapeBase.mountImage(player, console.GetVarString(string.Format("{0}.lastWeapon", player)), WeaponSlot, true, "");
            Player.setControlObject(player, "");
            if (!console.GetVarBool(string.Format("{0}.lastperson", player))) return;
            string client = console.GetVarString(string.Format("{0}.client", player));
            if (console.isObject(client))
                GameConnection.setFirstPerson(client, console.GetVarBool(string.Format("{0}.lastperson", player)));
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "doDismount", "(%this, %obj,%forced)", 3, 2400, false)]
        public void ArmorDoDismount(string datablock, string player, string sforced)
            {
            bool forced = sforced.AsBool();

            string vehicle = console.GetVarString(string.Format("{0}.mVehicle", player));
            if ((!SceneObject.isMounted(player)) || (!console.isObject(vehicle)))
                return;

            Point3F vvel = ShapeBase.getVelocity(vehicle);
            string vdb = console.getDatablock(vehicle).ToString(CultureInfo.InvariantCulture);
            int maxDismountSpeed = console.GetVarInt(string.Format("{0}.maxDismountSpeed", vdb));
            if ((vvel.len() <= maxDismountSpeed) || (forced))
                {
                TransformF pos = SceneObject.getTransform(player);
                TransformF rot = pos;
                TransformF oldpos = pos.copy();

                var vecs = new List<Point3F>();
                vecs.Add(new Point3F(-1, 0, 0));
                vecs.Add(new Point3F(0, 0, 1));
                vecs.Add(new Point3F(0, 0, -1));
                vecs.Add(new Point3F(1, 0, 0));
                vecs.Add(new Point3F(0, -1, 0));
                vecs.Add(new Point3F(0, 0, 0));

                var impulsevec = new Point3F(0, 0, 0);


                TransformF r = math.MatrixMulVector(SceneObject.getTransform(player), vecs[0]);

                vecs[0] = r.MPosition;
                pos.MPosition.x = 0;
                pos.MPosition.y = 0;
                pos.MPosition.z = 0;

                const int numofAttempts = 5;
                int success = -1;

                for (int i = 0; i < numofAttempts; i++)
                    {
                    Point3F vectorscale = vecs[i].vectorScale(3);

                    pos = oldpos + new TransformF(vectorscale);
                    if (!Player.checkDismountPoint(player, oldpos.MPosition, pos.MPosition)) continue;
                    success = i;
                    impulsevec = vecs[i].copy();
                    break;
                    }
                if ((forced) && (success == -1))
                    pos = oldpos.copy();

                console.SetVar(string.Format("{0}.mountVehicle", player), false);

                SimObject.schedule(player, "4000", "mountVehicles", "true");
                SceneObject.unmount(player);
                SceneObject.setTransform(player,
                                         new TransformF(pos.MPosition.x, pos.MPosition.y, pos.MPosition.z, rot.MOrientation.x, rot.MOrientation.y,
                                                        rot.MOrientation.z, rot.MAngle));

                Point3F velo = impulsevec.vectorScale(console.GetVarFloat(console.getDatablock(player) + ".mass"));
                velo.z = 1;

                ShapeBase.applyImpulse(player, pos.MPosition, velo);

                Point3F vel = ShapeBase.getVelocity(player);
                float vec = Point3F.vectorDot(vel, vel.normalizeSafe());
                if (vec > 50)
                    {
                    float scale = 50 / vec;
                    ShapeBase.setVelocity(player, vel.vectorScale(scale));
                    }
                }
            else
                {
                MessageClient(console.GetVarString(player + ".client"), "msgUnmount", @"\c2Cannot exit %1 while moving.", console.GetVarString(vdb + ".nameTag"));
                }
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onCollision", "(%this, %obj,%%col)", 3, 2400, false)]
        public void ArmorOnCollision(string datablock, string player, string col)
            {
            if ((!console.isObject(col)) && (Player.getState(player) == "Dead"))
                {
                return;
                }
            // Try and pickup all items
            if (console.GetClassName(col) == "Item")
                {
                console.Call(player, "pickup", new[] { col });
                return;
                }
            //AI are not allowed to drive they are lousey drivers....
            if (!console.isObject(player + ".client"))
                return;

            //Mount Vehicle.
            if ((console.getTypeMask(col) & (UInt32)SceneObjectTypesAsUint.GameBaseObjectType) != (UInt32)SceneObjectTypesAsUint.GameBaseObjectType) return;

            //ok, time will tell if I have enums right...
            //hell... time will tell if I have a clue on what is going on here.... 
            string db = console.getDatablock(col).ToString(CultureInfo.InvariantCulture);
            if ((console.GetClassName(db) != "WheeledVehicleData") || !console.GetVarBool(player + ".mountVehicle") || Player.getState(player) != "Move" ||
                !console.GetVarBool(string.Format("{0}.mountable", col))) return;
            // Only mount drivers for now.


            GameConnection.setFirstPerson(console.GetVarString(player + ".client"), false);

            //if (con.isObject(string.Format("{0}.client", obj)))
            //    con.Call(string.Format("{0}.client", obj), "setFirstPerson", new[] { "0" });
            // For this specific example, only one person can fit
            // into a vehicle
            int mount = SceneObject.getMountNodeObject(col, 0);
            if (mount > 0)
                return;

            // For this specific FPS Example, always mount the player
            // to node 0
            SceneObject.mountObject(col, player, 0, new TransformF(true));
            console.SetVar(string.Format("{0}.mVehicle", player), col);
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onImpact", "%this, %obj, %collidedObject, %vec, %vecLen)", 5, 2400, false)]
        public void ArmorOnImpact(string datablock, string player, string collidedObject, string vec, string vecLen)
            {
            var p = SceneObject.getTransform(player);
            p = p + new TransformF(vec);
            float speedDamageScale = vecLen.AsFloat() + console.GetVarFloat(string.Format("{0}.speedDamageScale", this));
            ShapeBaseDamage(player, "0", p.ToString(), speedDamageScale.ToString("0.000"), "Impact");
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "damage", "(%this, %obj, %sourceObject, %position, %damage, %damageType)", 6, 2400, false)]
        public void ArmorDamage(string datablock, string player, string sourceobject, string position, string damage, string damageType)
            {
            if (!console.isObject(player) || Player.getState(player) == "Dead" || damage.AsFloat() == 0)
                return;

            ShapeBase.applyDamage(player, damage.AsFloat());

            const string location = "Body";

            PlayerUpdateHealth(player);

            string client = console.GetVarString(string.Format("{0}.client", player));

            string sourceClient = sourceobject.Trim().Length > 0 ? console.GetVarString(string.Format("{0}.client", sourceobject)) : "0";

            //Only continue if it is a player, if it is an AI return.
            if (!console.isObject(client)) return;

            if (ShapeBase.getDamageLevel(player) >= 99)
                ShapeBase.unmountImage(player, 0);

            // Determine damage direction
            if (damageType != "Suicide")
                PlayerSetDamageDirection(player, sourceobject, position);

            if (Player.getState(player) == "Dead")
                GameConnectionOnDeath(client, sourceobject, sourceClient, damageType, location);

            
            }

        


        [Torque_Decorations.TorqueCallBack("", "Armor", "ondamage", "(%this, %obj, %delta)", 3, 2400, false)]
        public void ArmorOnDamage(string datablock, string player, string sdelta)
            {
            // This method is invoked by the ShapeBase code whenever the
            // object's damage level changes.
            float delta = float.Parse(sdelta);
            if ((delta <= 0) || Player.getState(player) == "Dead") return;
            // Apply a damage flash
            ShapeBase.setDamageFlash(player, 1);
            // If the pain is excessive, let's hear about it.
            if (delta > 10)
                PlayerPlayPain(player);
            }

        // ----------------------------------------------------------------------------
        // The player object sets the "disabled" state when damage exceeds it's
        // maxDamage value. This is method is invoked by ShapeBase state mangement code.

        // If we want to deal with the damage information that actually caused this
        // death, then we would have to move this code into the script "damage" method.
        [Torque_Decorations.TorqueCallBack("", "Armor", "onDisabled", "(%this, %obj, %state)", 3, 2400, false)]
        public void ArmorOnDisabled(string datablock, string player, string state)
            {
            ShapeBase.setImageTrigger(player, 0, false);

            string item = console.GetVarString(ShapeBase.getMountedImage(player, WeaponSlot).AsString() + ".item");

            if (console.isObject(item))
                {
                int amount = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.image.ammo", item)));

                if (amount.AsBool())
                    ShapeBaseShapeBaseThrow(player, console.GetVarString(string.Format("{0}.image.clip", item)), "1");
                }
            ShapeBaseTossPatch(player);
            PlayerPlayDeathCry(player);
            PlayerPlayDeathAnimation(player);


            //If it's a player check.....
            if (console.GetVarBool(player + ".client"))
                console.commandToClient(console.GetVarString(player + ".client"), console.addTaggedString("toggleVehicleMap"), new[] { "false" });


            int ctov = console.GetVarInt("$CorpseTimeoutValue");

            SimObject.schedule(player, (ctov - 1000).AsString(), "startFade", "1000", "0", "true");
            SimObject.schedule(player, ctov.AsString(), "delete");
            }


        [Torque_Decorations.TorqueCallBack("", "Armor", "onLeaveMissionArea", "(%this, %obj)", 2, 2400, false)]
        public void ArmorOnLeaveMissionArea(string datablock, string player)
            {
            GameConnectionOnLeaveMissionArea(console.GetVarString(player + ".client"));
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onEnterMissionArea", "(%this, %obj)", 2, 2400, false)]
        public void ArmorOnEnterMissionArea(string datablock, string player)
            {
            GameConnectionOnEnterMissionArea(console.GetVarString(player + ".client"));
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onEnterLiquid", "(%this, %obj, %coverage, %type)", 4, 2400, false)]
        public void ArmorOnEnterLiquid(string datablock, string player, string coverage, string type)
            {
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onLeaveLiquid", "(%this, %obj, %type)", 3, 2400, false)]
        public void ArmorOnLeaveLiquid(string datablock, string player, string type)
            {
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onTrigger", "(%this, %obj, %triggerNum, %val)", 4, 2400, false)]
        public void ArmorOnTrigger(string datablock, string player, string triggernum, string val)
            {
            // This method is invoked when the player receives a trigger move event.
            // The player automatically triggers slot 0 and slot one off of triggers #
            // 0 & 1.  Trigger # 2 is also used as the jump key.
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onPoseChange", "(%this, %obj, %oldPose, %newPose)", 4, 2400, false)]
        public void ArmorOnPosechange(string datablock, string player, string oldpose, string newpose)
            {
            ShapeBase.setImageScriptAnimPrefix(player, WeaponSlot, console.addTaggedString(newpose));
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onStartSprintMotion", "(%this, %obj)", 2, 2400, false)]
        public void ArmorOnStartSprintMotion(string datablock, string player)
            {
            ShapeBase.setImageGenericTrigger(player, WeaponSlot, 0, true);
            }

        [Torque_Decorations.TorqueCallBack("", "Armor", "onStopSprintMotion", "(%this, %obj)", 2, 2400, false)]
        public void ArmorOnStopSprintMotion(string datablock, string player)
            {
            ShapeBase.setImageGenericTrigger(player, WeaponSlot, 0, false);
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "kill", "(%this, %damageType)", 2, 2400, false)]
        public void PlayerKill(string player, string damageType)
            {
            ShapeBaseDamage(player, "0", SceneObject.getTransform(player).AsString(), "10000", damageType);
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "mountVehicles", "(%this, %bool)", 2, 2400, false)]
        public void PlayerMountVehicles(string player, string boolvalue)
            {
            console.SetVar(string.Format("{0}.mountVehicle", player), boolvalue);
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "isPilot", "(%this)", 1, 2400, false)]
        public bool PlayerIsPilot(string player)
            {
            string vehicle = SceneObject.getObjectMount(player).AsString();
            if (vehicle.AsBool())
                {
                if (SceneObject.getMountNodeObject(vehicle, 0).AsString() == player)
                    return true;
                }
            return false;
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "playDeathAnimation", "(%this)", 1, 2400, false)]
        public void PlayerPlayDeathAnimation(string player)
            {
            int numDeathAnimations = Player.getNumDeathAnimations(player);
            if (numDeathAnimations <= 0) return;
            if (console.isObject(console.GetVarString(player + ".client")))
                {
                int deathidx = console.GetVarInt(player + ".client.deathIdx");
                if ((deathidx > numDeathAnimations) || (deathidx == 0))
                    {
                    deathidx = 1;
                    }
                Player.setActionThread(player, string.Format("Death{0}", deathidx), false, true);
                deathidx++;
                console.SetVar(player + ".client.deathIdx", deathidx);
                }
            else
                {
                int r = new Random().Next(1, numDeathAnimations);
                Player.setActionThread(player, string.Format("Death{0}", r), false, true);
                }
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "playCelAnimation", "(%this, %anim)", 2, 2400, false)]
        public void PlayerPlayCelAnimation(string player, string anim)
            {
            if (Player.getState(player) != "Dead")
                Player.setActionThread(player, string.Format("cel{0}", anim), false, true);
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "playDeathCry", "(%this)", 1, 2400, false)]
        public void PlayerPlayDeathCry(string player)
            {
            ShapeBase.playAudio(player, 0, "DeathCrySound");
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "playPain", "(%this)", 1, 2400, false)]
        public void PlayerPlayPain(string player)
            {
            ShapeBase.playAudio(player, 0, "PainCrySound");
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "updateHealth", "(%player)", 1, 2400, false)]
        public void PlayerUpdateHealth(string player)
            {
            if (console.GetVarString(player + ".client").Trim() == "") return;

            float maxDamage = console.GetVarInt(string.Format("{0}.maxDamage", console.getDatablock(player)));

            float damagelevel = ShapeBase.getDamageLevel(player);

            double curhealth = maxDamage - damagelevel;

            curhealth = Math.Ceiling(curhealth);

            console.commandToClient(console.GetVarString(player + ".client"), console.addTaggedString("setNumericalHealthHUD"), new[] { ((int)curhealth).AsString() });

            }

        [Torque_Decorations.TorqueCallBack("", "Player", "setDamageDirection", "(%player, %sourceObject, %damagePos)", 3, 2400, false)]
        public void PlayerSetDamageDirection(string player, string sourceObject, string damagePos)
            {
            if (console.isObject(sourceObject))
                {
                damagePos = console.isField(sourceObject, "initialPosition")
                                ? console.GetVarString(string.Format("{0}.initialPosition", sourceObject))
                                : SceneObject.getTransform(sourceObject).AsString();
                }

            TransformF dp = new TransformF(damagePos);
            Point3F wbc = SceneObject.getWorldBoxCenter(player);

            TransformF damagevec = dp - new TransformF(wbc);
            damagevec = damagevec.normalizeSafe();

            string client = console.GetVarString(player + ".client");
            string cameraobject = GameConnection.getCameraObject(client).ToString(CultureInfo.InvariantCulture);

            TransformF inverseTransform = SceneObject.getInverseTransform(cameraobject);
            damagevec = math.MatrixMulVector(inverseTransform, damagevec.MPosition);

            var components = new float[6];
            var directions = new string[6];
            directions[0] = "Left";
            directions[1] = "Right";
            directions[2] = "Bottom";
            directions[3] = "Front";
            directions[4] = "Bottom";
            directions[5] = "Top";

            components[0] = -damagevec.MPosition.x;
            components[1] = damagevec.MPosition.x;
            components[2] = -damagevec.MPosition.y;
            components[3] = damagevec.MPosition.y;
            components[4] = -damagevec.MPosition.z;
            components[5] = damagevec.MPosition.z;
            string damagedirection = string.Empty;
            float max = 0;
            for (int i = 0; i < 6; i++)
                {
                if (components[i] <= max) continue;
                damagedirection = directions[i];
                max = components[i];
                }
            console.commandToClient(client, console.addTaggedString("setDamageDirection"), new[] { damagedirection });
            }

        [Torque_Decorations.TorqueCallBack("", "Player", "use", "(%player, %data,nameSpaceDepth)", 3, 2400, false)]
        public void PlayerUse(string player, string data, string nameSpaceDepth)
            {
            if (PlayerIsPilot(player))
                return;
            int nsd = (nameSpaceDepth.AsInt() + 1);
            console.ParentExecute(player, "use", nsd - 1, new[] { player, data, nsd.AsString() });
            }
        }
    }