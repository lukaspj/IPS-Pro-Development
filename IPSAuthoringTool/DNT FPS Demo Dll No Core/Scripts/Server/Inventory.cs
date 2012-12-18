﻿using System;
using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "serverCmdUse", "(%client, %data)",  2, 1200, false)]
        public void ServerCmdUse(string client, string data)
            {
            string controlobj = GameConnection.getControlObject(client).ToString(CultureInfo.InvariantCulture);
            console.Call(controlobj, "use", new[] { data });
            }

        [Torque_Decorations.TorqueCallBack("", "ShapeBase", "use", "(%this, %data)",  2, 1200, false)]
        public bool ShapeBaseShapeBaseUse(string thisobj, string data)
            {
            string conn = ShapeBase.getControllingClient(thisobj).ToString(CultureInfo.InvariantCulture);
            //uint iconn = 0;
            
                if (conn.AsUint() > 0)
                    {
                    double defaultfov = GameConnection.getControlCameraDefaultFov(conn);
                    double fov = GameConnection.getControlCameraFov(conn);
                    if (defaultfov != fov)
                        return false;
                    }


            if (ShapeBaseShapeBaseGetInventory(thisobj, data) > 0)
                {
                console.Call(data, "onUse", new[] { thisobj });
                return true;
                }
            return false;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "throw", "(%this, %data,%amount)",  3, 1200, false)]
        public bool ShapeBaseShapeBaseThrow(string thisobj, string data, string amount ="1")
            {
            bool functionresult = false;
            if (ShapeBaseShapeBaseGetInventory(thisobj, data) > 0)
                {
                int objectid = console.Call(data, "onThrow", new[] { thisobj, amount }).AsInt();
                    {
                    if (objectid != 0)
                        {
                        console.Call(thisobj, "throwObject", new[] { objectid.AsString() });
                        AudioServerPlay3D("ThrowSnd", SceneObject.getTransform(thisobj).AsString());
                        functionresult = true;
                        }
                    }
                }
            return functionresult;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "pickup", "(%this, %data,%amount)",  3, 1200, false)]
        public bool ShapeBaseShapeBasePickup(string thisobj, string obj, string amount)
            {
            string data = console.getDatablock(obj).ToString(CultureInfo.InvariantCulture); // con.Eval(string.Format("{0}.getDataBlock();", obj), true);
            if (amount == "")
                {
                string maxamount = "0" + console.Call(thisobj, "maxInventory", new[] { data });
                string curamount = ShapeBaseShapeBaseGetInventory(thisobj, data).AsString();
                amount = (maxamount.AsInt() - curamount.AsInt()).AsString();
                }
            if (amount.AsInt() < 0)
                amount = "0";
            if (amount.AsInt() > 0)
                {
                console.Call(data, "onPickUp", new[] { obj, thisobj, amount });
                return true;
                }
            return false;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "hasInventory", "(%this, %data)",  2, 1200, false)]
        public bool ShapeBaseShapeBaseHasInventory(string thisobj, string data)
            {
            int amount = console.GetVarInt(string.Format("{0}.inv[{1}]", thisobj, data));
            return amount > 0;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "hasAmmo", "(%this, %data)",  2, 1200, false)]
        public bool ShapeBaseShapeBaseHasAmmo(string thisob, string weapon)
            {
            string weaponimageammo = console.GetVarString(string.Format("{0}.image.ammo", weapon));
            if (weaponimageammo.Trim() == "")
                return true;
            return ShapeBaseShapeBaseGetInventory(thisob, weaponimageammo) > 0;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "maxInventory", "(%this, %data)",  2, 1200, false)]
        public string ShapeBaseShapeBaseMaxInventory(string thisobj, string data)
            {
            if (console.isField(data, "clip"))
                return console.GetVarString(string.Format("{0}.maxInventory", data));
                
            string objectname = console.getName(data);
            string datablock = console.getDatablock(thisobj).ToString(CultureInfo.InvariantCulture);
            string result = console.GetVarString(string.Format("{0}.maxInv[{1}]", datablock, objectname));
            return result;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "incInventory", "(%this, %data,%amount)",  3, 1200, false)]
        public int ShapeBaseShapeBaseIncInventory(string player, string datablock, string amount)
            {
            string maxamount = "0" + ShapeBaseShapeBaseMaxInventory(player, datablock);
            string objectname = console.getName(datablock); // 
            string total = console.GetVarString(string.Format("{0}.inv[{1}]", player, objectname));
            if (total.AsInt() < maxamount.AsInt())
                {
                if (total.AsInt() + amount.AsInt() > maxamount.AsInt())
                    {
                    amount = (maxamount.AsInt() - total.AsInt()).AsString();
                    }
                ShapeBaseShapeBaseSetInventory(player, datablock, (total.AsInt() + amount));
                return amount.AsInt();
                }
            return 0;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "setInventory", "(%this, %data,%value)",  3, 1200, false)]
        public string ShapeBaseShapeBaseSetInventory(string thisobj, string data, string value)
            {
            if (thisobj == "")
                return string.Empty;
            int max = 0;
            if (int.Parse("0" + value) < 0)
                value = "0";
            else
                {
                max = int.Parse(ShapeBaseShapeBaseMaxInventory(thisobj, data));
                if (int.Parse("0" + value) > max)
                    value = max.ToString(CultureInfo.InvariantCulture);
                }
            string name = console.getName(data);

            string amount = console.GetVarString(string.Format("{0}.inv[{1}]", thisobj, name));

            
            if (amount != value)
                {
                console.SetVar(string.Format("{0}.inv[{1}]", thisobj, name), value);
                
                if (console.isMethodInNamespace(data, "onInventory"))
                    console.Call(data, "onInventory", new[] { thisobj, value });

                string datablock = console.getDatablock(thisobj).AsString();
            
                if (console.isObject(datablock) && console.isMethodInNamespace(datablock, "onInventory"))
                    console.Call(datablock, "onInventory", new[] { data, value });
            
                }
            return value;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "decInventory", "(%this, %data,%value)",  3, 1200, false)]
        public int ShapeBaseShapeBaseDecInventory(string shapebase, string data, string amount)
            {
            
            string objectname = console.getName(data);

            string total = console.GetVarString(string.Format("{0}.inv[{1}]", shapebase, objectname));

            if (int.Parse("0" + total) > 0)
                {
                if (int.Parse("0" + total) < int.Parse(amount))
                    amount = total;
                ShapeBaseShapeBaseSetInventory(shapebase, data, (int.Parse("0" + total) - int.Parse("0" + amount)).ToString(CultureInfo.InvariantCulture));
                return amount.AsInt();
                }
            return 0;
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "onInventory", "(%this, %data, %value)",  3, 1200, false)]
        public string ShapeBaseShapeBaseOnInventory(string thisobj, string data, string value)
            {
            // Invoked on ShapeBase objects whenever their inventory changes
            // for the given datablock.
            return "0";
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBaseData", "onUse", "(%this, %user)",  2, 1200, false)]
        public string ShapeBaseDataShapeBaseDataOnUse(string thisobj, string user)
            {
            //// Invoked when the object uses this datablock, should return
            //// true if the item was used.
            return "0";
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBaseData", "onThrow", "(%this, %user, %amount)",  3, 1200, false)]
        public string ShapeBaseDataShapeBaseDataOnThrow(string thisobj, string user, string amount)
            {
            // Invoked when the object is thrown.  This method should
            // construct and return the actual mission object to be
            // physically thrown.  This method is also responsible for
            // decrementing the user's inventory.
            return "0";
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBaseData", "onPickup", "(%this, %obj, %user, %amount)",  4, 1200, false)]
        public string ShapeBaseDataShapeBaseDataOnPickup(string thisobj, string obj, string user, string amount)
            {
            // Invoked when the user attempts to pickup this datablock object.
            // The %amount argument is the space in the user's inventory for
            // this type of datablock.  This method is responsible for
            // incrementing the user's inventory is something is addded.
            // Should return true if something was added to the inventory.
            return "0";
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBaseData", "onInventory", "(%this, %obj, %value)",  3, 1200, false)]
        public string ShapeBaseDataShapeBaseDataOnInventory(string thisobj, string user, string value)
            {
            return "0";
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "clearInventory", "(%this)",  1, 1200, false)]
        public void ShapeBaseShapeBaseClearInventory(string thisobj)
            {
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "throwObject", "(%this, %obj)",  2, 1200, false)]
        public void ShapeBaseShapeBaseThrowObject(string thisobj, string obj)
            {
            // Throw the given object in the direction the shape is looking.
            // The force value is hardcoded according to the current default
            // object mass and mission gravity (20m/s^2).
            string datablock = console.getDatablock(thisobj).ToString(CultureInfo.InvariantCulture); //con.Eval(string.Format("{0}.getDataBlock();", thisobj), true);
            string throwforce = "0" + console.GetVarString(string.Format("{0}.throwForce", datablock));
            if (int.Parse(throwforce) == 0)
                throwforce = "20";

            // Start with the shape's eye vector...

            Point3F eye = ShapeBase.getEyeVector(thisobj);
            Point3F vec = eye.vectorScale(float.Parse(throwforce));

            // Add a vertical component to give the object a better arc
            double verticalForce = double.Parse(throwforce) / 2.0;
            float dot = Point3F.vectorDot(new Point3F("0 0 1"), eye);
            if (dot < 0)
                dot = dot * -1;

            vec = vec + Point3F.vectorScale(new Point3F(string.Format("0 0 {0}", verticalForce)), 1 - dot);
            vec = vec + ShapeBase.getVelocity(thisobj);

            // Set the object's position and initial velocity
            TransformF pos = new TransformF(console.Eval(string.Format("getBoxCenter({0}.getWorldBox());", thisobj), true));

            SceneObject.setTransform(obj, pos);
            ShapeBase.applyImpulse(obj, pos.MPosition, vec);
            // Since the object is thrown from the center of the shape,
            // the object needs to avoid colliding with it's thrower.
            console.Call(obj, "setCollisionTimeout", new[] { thisobj });

            SimObject.schedule(obj, ITem_PopTime.AsString(), "delete");
            }

        [Torque_Decorations.TorqueCallBack("",  "ShapeBase", "getInventory", "(%this, %data)",  2, 1200, false)]
        public int ShapeBaseShapeBaseGetInventory(string thisobj, string data)
            {
            int numberof;
            
            UInt32 test = 0;
            
            string itemname = UInt32.TryParse(data, out test) ? console.getName(data) : data;

            string inventorycount = console.GetVarString(thisobj + ".inv[" + itemname.Trim() + "]");

            return int.TryParse(inventorycount, out numberof) ? numberof : 0;
            }
        }
    }