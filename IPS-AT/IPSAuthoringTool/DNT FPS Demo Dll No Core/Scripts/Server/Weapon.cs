﻿using System;
using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        public const int WeaponSlot = 0;

        [Torque_Decorations.TorqueCallBack("", "", "WeaponInitialize", "", 0, 1900, true)]
        public void WeaponInitialize()
            {
            console.SetVar("$WeaponSlot", 0);
            }

        [Torque_Decorations.TorqueCallBack("", "Weapon", "onUse", "(%data, %obj)", 2, 1900, false)]
        public void WeaponOnUser(string data, string obj)
            {
            if (ShapeBase.getMountedImage(obj, WeaponSlot) == console.Call(data + ".image", "getID").AsInt()) return;

            AudioServerPlay3D("WeaponUseSound", SceneObject.getTransform(obj).AsString());


            
            ShapeBase.mountImage(obj, console.GetVarString(data + ".image"), WeaponSlot, true, "");
            if (console.GetVarBool(string.Format("{0}.client", obj)))
                {
                if (console.GetVarString(data + ".description") != "")
                    MessageClient(console.GetVarString(string.Format("{0}.client", obj)), "MsgWeaponUsed", "\\c3%1 \\c5 selected.",
                                  console.GetVarString(data + ".description"));
                else
                    MessageClient(console.GetVarString(string.Format("{0}.client", obj)), "MsgWeaponUsed", console.addTaggedString(@"\c0Weapon selected"));
                }

            if (!console.isInNamespaceHierarchy(obj, "Player")) return;
            Player.allowAllPoses(obj);
            string image = console.GetVarString(string.Format("{0}.image", data));

            if (console.GetVarBool(string.Format("{0}.jumpingDisallowed", image)))
                Player.allowJumping(obj, false);
            if (console.GetVarBool(string.Format("{0}.jetJumpingDisallowed", image)))
                Player.allowJetJumping(obj, false);
            if (console.GetVarBool(string.Format("{0}.sprintDisallowed", image)))
                Player.allowSprinting(obj, false);
            if (console.GetVarBool(string.Format("{0}.crouchDisallowed", image)))
                Player.allowCrouching(obj, false);
            if (console.GetVarBool(string.Format("{0}.proneDisallowed", image)))
                Player.allowProne(obj, false);
            if (console.GetVarBool(string.Format("{0}.swimmingDisallowed", image)))
                Player.allowSwimming(obj, false);
            }

        [Torque_Decorations.TorqueCallBack("", "Weapon", "onPickup", "(%this, %obj, %shape, %amount)", 5, 1900, false)]
        public void WeaponOnPickup(string thisobj, string obj, string shape, string amount, string nameSpaceDepth)
            {
            int nsd = (nameSpaceDepth.AsInt() + 1);
            if (!console.ParentExecute(thisobj, "onPickup", nsd, new[] { thisobj, obj, shape, amount }).AsBool()) return;
            

            AudioServerPlay3D("WeaponPickupSound", SceneObject.getTransform(shape).AsString());

            if (console.GetClassName(shape) == "Player" && ShapeBase.getMountedImage(shape, WeaponSlot) == 0)
                console.Call(shape, "use", new[] { thisobj });
            }

        [Torque_Decorations.TorqueCallBack("", "Weapon", "onInventory", "(%this, %obj, %amount)", 3, 1900, false)]
        public void WeaponOnInventory(string thisobj, string obj, string amount)
            {
            if (amount != "0" || !console.objectHasMethod(obj, "getMountSlot")) return;
            int slot = ShapeBase.getMountSlot(obj, console.GetVarString(string.Format("{0}.image", this)));
            ShapeBase.unmountImage(obj, slot);
            }

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "onMount", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageOnMount(string thisobj, string player, string slot)
            {
            string ammoName = console.getName(console.GetVarString(thisobj + ".ammo"));
            string currentammo = "";
            int ammountinClips = 0;
            if (console.isField(thisobj, "clip"))
                {
                if (ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.ammo", thisobj))).AsBool())
                    {
                    ShapeBase.setImageAmmo(player, slot.AsInt(), true);
                    currentammo = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.ammo", thisobj))).AsString();
                    }
                else if (ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.clip", thisobj))) > 0)
                    {
                    ShapeBaseShapeBaseSetInventory(player, console.GetVarString(string.Format("{0}.ammo", thisobj)),
                                                   console.GetVarString(string.Format("{0}.ammo.maxInventory", thisobj)));
                    ShapeBase.setImageAmmo(player, slot.AsInt(), true);

                    currentammo = console.GetVarString(string.Format("{0}.ammo.maxInventory", thisobj));
                    ammountinClips += console.GetVarInt(string.Format("{0}.remaining{1}", player, ammoName));
                    }
                else
                    {
                    currentammo = console.GetVarString(string.Format("{0}.remaining{1}", player, ammoName));
                    }

                ammountinClips = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.clip", thisobj)));
                ammountinClips *= console.GetVarInt(string.Format("{0}.ammo.maxInventory", thisobj));

                if ((console.GetVarString(player + ".client") != "") && !console.GetVarBool(string.Format("{0}.isAiControlled", player)))
                    {
                    GameConnectionRefreshWeaponHud(console.GetVarString(player + ".client"), currentammo, console.GetVarString(thisobj + ".item.previewImage"), console.GetVarString(thisobj + ".item.reticle"),
                                     console.GetVarString(thisobj + ".item.zoomReticle"), ammountinClips.AsString());
                    }
                }
            else if (console.GetVarString(string.Format("{0}.ammo", thisobj)) != "")
                {
                if (ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.ammo", thisobj))).AsBool())
                    {
                    ShapeBase.setImageAmmo(player, slot.AsInt(), true);
                    currentammo = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.ammo", thisobj))).AsString();
                    }
                else
                    currentammo = "0";


                if (console.GetVarString(string.Format("{0}.client", player)) != "" && !console.GetVarBool(string.Format("{0}.isAiControlled", player)))
                    GameConnectionRefreshWeaponHud(console.GetVarString(player + ".client"), "1", console.GetVarString(thisobj + ".item.previewImage"), console.GetVarString(thisobj + ".item.reticle"),
                                     console.GetVarString(thisobj + ".item.zoomReticle"), currentammo);
                }
            }

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "onUnmount", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageOnUnmount(string thisobj, string player, string slot)
            {
            if (console.GetVarString(string.Format("{0}.client", player)) != "" && console.GetVarBool(string.Format("{0}.isAiControlled", player)))
                GameConnectionRefreshWeaponHud(console.GetVarString(player + ".client"), "0", "", "", "", "");
            }

        // ----------------------------------------------------------------------------
        // A "generic" weaponimage onFire handler for most weapons.  Can be overridden
        // with an appropriate namespace method for any weapon that requires a custom
        // firing solution.

        // projectileSpread is a dynamic property declared in the weaponImage datablock
        // for those weapons in which bullet skew is desired.  Must be greater than 0,
        // otherwise the projectile goes straight ahead as normal.  lower values give
        // greater accuracy, higher values increase the spread pattern.
        // ----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "onFire", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageOnFire(string thisobj, string obj, string slot)
            {
            if (!console.isObject(string.Format("{0}.projectile", thisobj)))
                {
                console.error("WeaponImage::onFire() - Invalid projectile datablock");
                return;
                }
            // Decrement inventory ammo. The image's ammo state is updated
            // automatically by the ammo inventory hooks.
            if (!console.GetVarBool(string.Format("{0}.infiniteAmmo", thisobj)))
                ShapeBaseShapeBaseDecInventory(obj, console.GetVarString(string.Format("{0}.ammo", thisobj)), "1");

            // Get the player's velocity, we'll then add it to that of the projectile
            int numProjectiles = console.GetVarInt(string.Format("{0}.projectileNum", thisobj));
            if (numProjectiles == 0)
                numProjectiles = 1;
            TransformF muzzleVector = new TransformF();

            for (int i = 0; i < numProjectiles; i++)
                {
                if (console.GetVarBool(string.Format("{0}.projectileSpread", thisobj)))
                    {
                    // We'll need to "skew" this projectile a little bit.  We start by
                    // getting the straight ahead aiming point of the gun
                    Point3F vec = ShapeBase.getMuzzleVector(obj, slot.AsInt());
                    // Then we'll create a spread matrix by randomly generating x, y, and z
                    // points in a circle
                    Random r = new Random();
                    TransformF matrix = new TransformF();
                    matrix.MPosition.x = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.projectileSpread ", thisobj)));
                    matrix.MPosition.y = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.projectileSpread ", thisobj)));
                    matrix.MPosition.z = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.projectileSpread ", thisobj)));
                    TransformF mat = math.MatrixCreateFromEuler(matrix);

                    muzzleVector = math.MatrixMulVector(mat, vec);
                    }
                else
                    {
                    muzzleVector = new TransformF(ShapeBase.getMuzzleVector(obj, slot.AsInt()));
                    }
                Point3F objectVelocity = ShapeBase.getVelocity(obj);

                muzzleVector = muzzleVector.vectorScale(console.GetVarFloat(string.Format("{0}.projectile.muzzleVelocity", thisobj)));
                objectVelocity = objectVelocity.vectorScale(console.GetVarFloat(string.Format("{0}.projectile.velInheritFactor", thisobj)));
                Point3F muzzleVelocity = muzzleVector.MPosition + objectVelocity;

                Torque_Class_Helper tch = new Torque_Class_Helper(console.GetVarString(string.Format("{0}.projectileType", thisobj)), "");
                tch.Props.Add("dataBlock", console.GetVarString(string.Format("{0}.projectile", thisobj)));
                tch.Props.Add("initialVelocity", '"' + muzzleVelocity.ToString() + '"');
                tch.Props.Add("initialPosition", '"' + ShapeBase.getMuzzlePoint(obj, slot.AsInt()).ToString() + '"');
                tch.Props.Add("sourceObject", obj);
                tch.Props.Add("sourceSlot", slot);
                tch.Props.Add("client", console.GetVarString(string.Format("{0}.client", obj)));
                tch.Props.Add("sourceClass", console.GetClassName(obj));

                string projectile = tch.Create(m_ts).ToString(CultureInfo.InvariantCulture);

                SimSet.pushToBack("MissionCleanup", projectile);
                }
            }


        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "onAltFire", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageonAltFire(string thisobj, string obj, string slot)
            {
            if (!console.isObject(string.Format("{0}.projectile", thisobj)))
                {
                console.error("WeaponImage::onFire() - Invalid projectile datablock");
                return;
                }
            // Decrement inventory ammo. The image's ammo state is updated
            // automatically by the ammo inventory hooks.
            if (!console.GetVarBool(string.Format("{0}.infiniteAmmo", thisobj)))
                ShapeBaseShapeBaseDecInventory(obj, console.GetVarString(string.Format("{0}.ammo", thisobj)), "1");

            // Get the player's velocity, we'll then add it to that of the projectile
            int numProjectiles = console.GetVarInt(string.Format("{0}.altProjectileNum", thisobj));
            if (numProjectiles == 0)
                numProjectiles = 1;
            TransformF muzzleVector = new TransformF();

            for (int i = 0; i < numProjectiles; i++)
                {
                if (console.GetVarBool(string.Format("{0}.altProjectileSpread", thisobj)))
                    {
                    // We'll need to "skew" this projectile a little bit.  We start by
                    // getting the straight ahead aiming point of the gun
                    Point3F vec = ShapeBase.getMuzzleVector(obj, slot.AsInt());
                    // Then we'll create a spread matrix by randomly generating x, y, and z
                    // points in a circle
                    Random r = new Random();
                    TransformF matrix = new TransformF();
                    matrix.MPosition.x = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.altProjectileSpread ", thisobj)));
                    matrix.MPosition.y = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.altProjectileSpread ", thisobj)));
                    matrix.MPosition.z = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.altProjectileSpread ", thisobj)));
                    TransformF mat = math.MatrixCreateFromEuler(matrix);

                    muzzleVector = math.MatrixMulVector(mat, vec);
                    }
                else
                    {
                    muzzleVector = new TransformF(ShapeBase.getMuzzleVector(obj, slot.AsInt()));
                    }
                Point3F objectVelocity = ShapeBase.getVelocity(obj);

                muzzleVector = muzzleVector.vectorScale(console.GetVarFloat(string.Format("{0}.altProjectile.muzzleVelocity", thisobj)));
                objectVelocity = objectVelocity.vectorScale(console.GetVarFloat(string.Format("{0}.altProjectile.velInheritFactor", thisobj)));
                Point3F muzzleVelocity = muzzleVector.MPosition + objectVelocity;

                Torque_Class_Helper tch = new Torque_Class_Helper(console.GetVarString(string.Format("{0}.altProjectileType", thisobj)), "");
                tch.Props.Add("dataBlock", console.GetVarString(string.Format("{0}.altProjectile", thisobj)));
                tch.Props.Add("initialVelocity", '"' + muzzleVelocity.ToString() + '"');
                tch.Props.Add("initialPosition", '"' + ShapeBase.getMuzzlePoint(obj, slot.AsInt()).ToString() + '"');
                tch.Props.Add("sourceObject", obj);
                tch.Props.Add("sourceSlot", slot);
                tch.Props.Add("client", console.GetVarString(string.Format("{0}.client", obj)));
                tch.Props.Add("sourceClass", console.GetClassName(obj));

                string project = tch.Create(m_ts).ToString(CultureInfo.InvariantCulture);
                SimSet.pushToBack("MissionCleanup", project);
                }
            }

        // ----------------------------------------------------------------------------
        // A "generic" weaponimage onWetFire handler for most weapons.  Can be
        // overridden with an appropriate namespace method for any weapon that requires
        // a custom firing solution.
        // ----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "onWetFire", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageonWetFire(string thisobj, string obj, string slot)
            {
            if (!console.isObject(thisobj + ".projectile"))
                {
                console.error("WeaponImage::onFire() - Invalid projectile datablock");
                return;
                }
            // Decrement inventory ammo. The image's ammo state is updated
            // automatically by the ammo inventory hooks.
            if (!console.GetVarBool(string.Format("{0}.infiniteAmmo", thisobj)))
                ShapeBaseShapeBaseDecInventory(obj, console.GetVarString(string.Format("{0}.ammo", thisobj)), "1");

            // Get the player's velocity, we'll then add it to that of the projectile
            int numProjectiles = console.GetVarInt(string.Format("{0}.projectileNum", thisobj));
            if (numProjectiles == 0)
                numProjectiles = 1;
            TransformF muzzleVector = new TransformF();

            for (int i = 0; i < numProjectiles; i++)
                {
                if (console.GetVarBool(string.Format("{0}.wetProjectileSpread", thisobj)))
                    {
                    // We'll need to "skew" this projectile a little bit.  We start by
                    // getting the straight ahead aiming point of the gun
                    Point3F vec = ShapeBase.getMuzzleVector(obj, slot.AsInt());
                    // Then we'll create a spread matrix by randomly generating x, y, and z
                    // points in a circle
                    Random r = new Random();
                    TransformF matrix = new TransformF();
                    matrix.MPosition.x = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.wetProjectileSpread ", thisobj)));
                    matrix.MPosition.y = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.wetProjectileSpread ", thisobj)));
                    matrix.MPosition.z = (float)((r.NextDouble() - .5) * 2 * Math.PI * console.GetVarFloat(string.Format("{0}.wetProjectileSpread ", thisobj)));
                    TransformF mat = math.MatrixCreateFromEuler(matrix);

                    muzzleVector = math.MatrixMulVector(mat, vec);
                    }
                else
                    {
                    muzzleVector = new TransformF(ShapeBase.getMuzzleVector(obj, slot.AsInt()));
                    }
                Point3F objectVelocity = ShapeBase.getVelocity(obj);

                muzzleVector = muzzleVector.vectorScale(console.GetVarFloat(string.Format("{0}.wetProjectile.muzzleVelocity", thisobj)));
                objectVelocity = objectVelocity.vectorScale(console.GetVarFloat(string.Format("{0}.wetProjectile.velInheritFactor", thisobj)));
                Point3F muzzleVelocity = muzzleVector.MPosition + objectVelocity;

                Torque_Class_Helper tch = new Torque_Class_Helper(console.GetVarString(string.Format("{0}.projectileType", thisobj)), "");
                tch.Props.Add("dataBlock", console.GetVarString(string.Format("{0}.wetProjectile", thisobj)));
                tch.Props.Add("initialVelocity", '"' + muzzleVelocity.ToString() + '"');
                tch.Props.Add("initialPosition", '"' + ShapeBase.getMuzzlePoint(obj, slot.AsInt()).ToString() + '"');
                tch.Props.Add("sourceObject", obj);
                tch.Props.Add("sourceSlot", slot);
                tch.Props.Add("client", console.GetVarString(string.Format("{0}.client", obj)));
                tch.Props.Add("sourceClass", console.GetClassName(obj));

                string project = tch.Create(m_ts).ToString(CultureInfo.InvariantCulture);
                SimSet.pushToBack("MissionCleanup", project);
                }
            }

        //-----------------------------------------------------------------------------
        // Clip Management
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "onClipEmpty", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageOnClipEmpty(string thisobj, string obj, string slot)
            {
            SimObject.schedule(thisobj, "0", "reloadAmmoClip", obj, slot);
            }

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "reloadAmmoClip", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageReloadAmmoClip(string thisobj, string obj, string slot)
            {
            string ammoname = console.getName(console.GetVarString(string.Format("{0}.ammo", thisobj)));

            if (thisobj.AsInt() != ShapeBase.getMountedImage(obj, slot.AsInt()))
                return;

            if (!console.isField(thisobj, "clip")) return;
            if (ShapeBaseShapeBaseGetInventory(obj, console.GetVarString(string.Format("{0}.clip", thisobj))) > 0)
                {
                ShapeBaseShapeBaseDecInventory(obj, console.GetVarString(string.Format("{0}.clip", thisobj)), "1");

                ShapeBaseShapeBaseSetInventory(obj, console.GetVarString(string.Format("{0}.ammo", thisobj)),
                                               console.GetVarString(string.Format("{0}.ammo.maxInventory", thisobj)));

                ShapeBase.setImageAmmo(obj, slot.AsInt(), true);
                }
            else
                {
                int amountInPocket = console.GetVarInt(string.Format("{0}.remaining{1}", obj, console.getName(console.GetVarString(thisobj + ".ammo"))));
                if (amountInPocket > 0)
                    {
                    console.SetVar(string.Format("{0}remaining{1}", obj, ammoname), 0);
                    ShapeBaseShapeBaseSetInventory(obj, console.GetVarString(string.Format("{0}.ammo", thisobj)),
                                                   amountInPocket.ToString(CultureInfo.InvariantCulture));
                    ShapeBase.setImageAmmo(obj, slot.AsInt(), true);
                    }
                }
            }

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "clearAmmoClip", "(%this, %obj, %slot)", 3, 1900, false)]
        public void WeaponImageClearAmmoClip(string thisobj, string obj, string slot)
            {
            //echo("WeaponImage::clearAmmoClip: " SPC %this SPC %obj SPC %slot);

            // if we're not empty put the remaining bullets from the current clip
            // in to the player's "pocket".

            if (!console.isField(thisobj, "clip")) return;
            // Commenting out this line will use a "hard clip" system, where
            // A player will lose any ammo currently in the gun when reloading.
            string pocketAmount = console.Call(thisobj, "stashSpareAmmo", new[] { obj });
            if (ShapeBaseShapeBaseGetInventory(obj, console.GetVarString(string.Format("{0}.clip", thisobj))) > 0 || pocketAmount.AsInt() != 0)
                ShapeBase.setImageAmmo(obj, slot.AsInt(), false);
            }

        [Torque_Decorations.TorqueCallBack("", "WeaponImage", "stashSpareAmmo", "( %this, %player )", 2, 1900, false)]
        public string WeaponImageStashSpareAmmo(string thisobj, string player)
            {
            // If the amount in our pocket plus what we are about to add from the clip
            // Is over a clip, add a clip to inventory and keep the remainder
            // on the player
            string ammoname = console.getName(console.GetVarString(thisobj + ".ammo"));
            if (ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.ammo", thisobj))) <
                console.GetVarInt(string.Format("{0}.ammo.maxInventory", thisobj)))
                {
                string nameOfAmmoField = string.Format("remaining{0}", ammoname);
                int amountInPocket = console.GetVarInt(string.Format("{0}.{1}", player, nameOfAmmoField));
                int amountIngun = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.ammo", thisobj)));
                int combinedammo = amountInPocket + amountIngun;
                if (combinedammo >= console.GetVarInt(string.Format("{0}.ammo.maxInventory", thisobj)))
                    {
                    console.SetVar(player + "." + nameOfAmmoField, combinedammo - console.GetVarInt(string.Format("{0}.ammo.maxInventory", thisobj)));
                    ShapeBaseShapeBaseIncInventory(player, console.GetVarString(string.Format("{0}.clip", thisobj)), "1");
                    }
                else if (ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.clip", thisobj))) > 0)
                    console.SetVar(player + "." + nameOfAmmoField, combinedammo);

                return console.GetVarString(player + "." + nameOfAmmoField);
                }

            return "0";
            }

        [Torque_Decorations.TorqueCallBack("", "AmmoClip", "onPickup", "(%this, %obj, %shape, %amount, %nameSpaceDepth)", 5, 1900, false)]
        public void AmmoClipOnPickup(string thisobj, string obj, string shape, string amount, string nameSpaceDepth)
            {
            int nsd = (nameSpaceDepth.AsInt() + 1);

            if (!console.ParentExecute(thisobj, "onPickup", nsd, new[] { thisobj, obj, shape, amount }).AsBool()) return;

            AudioServerPlay3D("WeaponPickupSound", SceneObject.getTransform(shape).AsString());

            string image = ShapeBase.getMountedImage(shape, WeaponSlot).ToString(CultureInfo.InvariantCulture);

            if (!image.AsBool()) return;

            if (!console.isField(image, "clip") || console.Call(string.Format("{0}.clip", image), "getID") != console.Call(thisobj, "getID")) return;

            bool outOfAmmo = !ShapeBase.getImageAmmo(shape, WeaponSlot);
            int amountInClips = 0;
            string currentAmmo = ShapeBaseShapeBaseGetInventory(shape, console.GetVarString(string.Format("{0}.ammo", image))).AsString();
            //con.error(string.Format("Current Ammo is {0}", currentAmmo));
            if (console.isObject(string.Format("{0}.clip", image)))
                amountInClips = ShapeBaseShapeBaseGetInventory(shape, console.GetVarString(string.Format("{0}.clip", image)));
            //con.debug(string.Format("Amount in clips is {0}", amountInClips));
            int t = console.GetVarInt(string.Format("{0}.ammo.maxInventory", image));
            //con.debug(string.Format("Ammount per clip = {0}", t));
            amountInClips *= t;
            //con.debug(string.Format("Amount of single shells in clips {0}", amountInClips));
            int amountloadedingun = console.GetVarInt(string.Format("{0}.remaining{1}", obj, console.GetVarString(string.Format("{0}.ammo", this))));
            //con.debug(string.Format("Amount loaded in gun {0}", amountloadedingun));
            amountInClips += amountloadedingun;
            //con.debug(string.Format("Amount in clips {0}", amountInClips));
            if (outOfAmmo)
                console.Call(image, "onClipEmpty", new[] { shape, WeaponSlot.AsString() });
            }

        [Torque_Decorations.TorqueCallBack("", "Ammo", "onPickup", "(%this, %obj, %shape, %amount, %nameSpaceDepth)", 5, 1900, false)]
        public void AmmoOnPickup(string thisobj, string obj, string shape, string amount, string nameSpaceDepth)
            {
            int nsd = (nameSpaceDepth.AsInt() + 1);
            if (console.ParentExecute(thisobj, "onPickup", nsd, new[] { thisobj, obj, shape, amount }).AsBool())
                AudioServerPlay3D("AmmoPickupSound", SceneObject.getTransform(shape).AsString());
            }

        [Torque_Decorations.TorqueCallBack("", "Ammo", "onInventory", "(%this, %obj, %amount)", 3, 1900, false)]
        public void AmmoOnInventory(string thisobj, string player, string amount)
            {
            for (int i = 0; i < 8; i++)
                {
                int image = ShapeBase.getMountedImage(player, i);
                if (image <= 0) continue;
                if (!console.isObject(string.Format("{0}.ammo", image)) || console.Call(image + ".ammo", "getID") != console.Call(thisobj, "getID")) continue;
                ShapeBase.setImageAmmo(player, i, amount.AsInt() != 0);
                int currentammo = ShapeBaseShapeBaseGetInventory(player, thisobj);
                if (console.GetClassName(player) != "Player") continue;
                int amountInClips;
                if (console.isObject(string.Format("{0}.clip", thisobj)))
                    {
                    amountInClips = ShapeBaseShapeBaseGetInventory(player, console.GetVarString(string.Format("{0}.clip", thisobj)));
                    amountInClips *= console.GetVarInt(string.Format("{0}.maxInventory", thisobj));
                    string name = string.Format("{0}.remaining{1}", player, console.getName(thisobj));
                    int tamount = console.GetVarInt(name);
                    amountInClips += tamount;
                    }
                else
                    {
                    amountInClips = currentammo;
                    currentammo = 1;
                    }
                if (console.GetVarString(string.Format("{0}.client", player)) != "" && !console.GetVarBool(string.Format("{0}.isAiControlled", player)))
                    {
                    GameConnectionSetAmmoAmountHud(console.GetVarString(player + ".client"), currentammo.AsString(), amountInClips.AsString());
                    }
                }
            }


        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "clearWeaponCycle", "(%this", 1, 1900, false)]
        public void ShapeBaseClearWeaponCycle(string thisobj)
            {
            console.SetVar(string.Format("{0}.totalCycledWeapons", thisobj), 0);
            }

        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "addToWeaponCycle", "(%this,%weapon)", 2, 1900, false)]
        public void ShapeBaseAddToWeaponCycle(string player, string weapon)
            {
            int totalweapons = console.GetVarInt(string.Format("{0}.totalCycledWeapons", player));
            console.SetVar(string.Format("{0}.totalCycledWeapons", player), totalweapons + 1);
            console.SetVar(player + ".cycleWeapon[" + totalweapons + "]", weapon);
            }

        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "cycleWeapon", "(%this, %direction)", 2, 1900, false)]
        public void ShapeBaseCycleWeapon(string thisobj, string direction)
            {
            //I sure seem to retrieve "thisobj.totalCycledWeapons" alot,
            //So to save so work, I'm just gonna grab it here.
            int totalCycledWeapons = console.GetVarInt(thisobj + ".totalCycledWeapons");


            // Can't cycle what we don't have
            if (totalCycledWeapons == 0)
                return;
            // Find out the index of the current weapon, if any (not all
            // available weapons may be part of the cycle)
            int currentIndex = -1;

            int mountedimage = ShapeBase.getMountedImage(thisobj, WeaponSlot);

            if (mountedimage != 0)
                {
                string curWeapon = console.getName(string.Format("{0}.item", mountedimage.AsString()));
                for (int i = 0; i < totalCycledWeapons; i++)
                    {
                    if (console.GetVarString(thisobj + ".cycleWeapon[" + i + "]") != curWeapon) continue;
                    currentIndex = i;
                    break;
                    }
                }

            int nextIndex = 0;
            int dir = 1;
            if (currentIndex != -1)
                {
                if (direction == "prev")
                    {
                    dir = -1;
                    nextIndex = currentIndex - 1;
                    if (nextIndex < 0)
                        {
                        nextIndex = totalCycledWeapons - 1;
                        }
                    }
                else
                    {
                    nextIndex = currentIndex + 1;
                    if (nextIndex >= totalCycledWeapons)
                        {
                        nextIndex = 0;
                        }
                    }
                }
            bool found = false;
            string weapon = "";
            for (int i = 0; i < totalCycledWeapons; i++)
                {
                weapon = console.GetVarString(thisobj + ".cycleWeapon[" + i + "]");
                if ((weapon != "") && (ShapeBaseShapeBaseHasInventory(thisobj, weapon)))
                    {
                    found = true;
                    break;
                    }
                nextIndex = nextIndex + dir;
                if (nextIndex < 0)
                    nextIndex = totalCycledWeapons - 1;
                else if (nextIndex >= totalCycledWeapons)
                    nextIndex = 0;
                }
            if (!found) return;
            weapon = console.GetVarString(thisobj + ".cycleWeapon[" + nextIndex + "]");
            ShapeBaseShapeBaseUse(thisobj, weapon);
            }
        }
    }