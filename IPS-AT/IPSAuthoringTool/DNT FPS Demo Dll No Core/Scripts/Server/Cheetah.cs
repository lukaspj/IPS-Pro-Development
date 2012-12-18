using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "CheetahCar", "onAdd", "(%this, %obj,nameSpaceDepth)",  3, 2800, false)]
        public void CheetahCarOnAdd(string thisobj, string obj, string nameSpaceDepth)
            {
            int nsd = (nameSpaceDepth.AsInt() + 1);
            console.ParentExecute(thisobj, "onAdd", nsd, new[] { thisobj, obj });
            WheeledVehicle.setWheelTire(obj, 0, "CheetahCarTire");
            WheeledVehicle.setWheelTire(obj, 1, "CheetahCarTire");
            WheeledVehicle.setWheelTire(obj, 2, "CheetahCarTireRear");
            WheeledVehicle.setWheelTire(obj, 3, "CheetahCarTireRear");
            // Setup the car with some tires & springs
            for (int i = WheeledVehicle.getWheelCount(obj) - 1; i >= 0; i--)
                {
                WheeledVehicle.setWheelPowered(obj, i, true);
                WheeledVehicle.setWheelSpring(obj, i, "CheetahCarSpring");
                }
            // Steer with the front tires
            WheeledVehicle.setWheelSteering(obj, 0, 1);
            WheeledVehicle.setWheelSteering(obj, 1, 1);
            // Add tail lights
            var tc = new Torque_Class_Helper("PointLight", "");
            tc.Props.Add("radius", "1");
            tc.Props.Add("isEnabled", "0");
            tc.Props.Add("color", @"""1 0 0.141176 1""");
            tc.Props.Add("brightness", "2");
            tc.Props.Add("castShadows", "1");
            tc.Props.Add("priority", "1");
            tc.Props.Add("animate", "0");
            tc.Props.Add("animationPeriod", "1");
            tc.Props.Add("animationPhase", "1");
            tc.Props.Add("flareScale", "1");
            tc.Props.Add("attenuationRatio", @"""0 1 1""");
            tc.Props.Add("shadowType", @"""DualParaboloidSinglePass""");
            tc.Props.Add("texSize", "512");
            tc.Props.Add("overDarkFactor", @"""2000 1000 500 100""");
            tc.Props.Add("shadowDistance", "400");
            tc.Props.Add("shadowSoftness", "0.15");
            tc.Props.Add("numSplits", "1");
            tc.Props.Add("logWeight", "0.91");
            tc.Props.Add("fadeStartDistance", "0");
            tc.Props.Add("lastSplitTerrainOnly", "0");
            tc.Props.Add("representedInLightmap", "0");
            tc.Props.Add("shadowDarkenColor", @"""0 0 0 -1""");
            tc.Props.Add("includeLightmappedGeometryInShadow", "0");
            tc.Props.Add("rotation", @"""1 0 0 0 """);
            tc.Props.Add("canSave", "1");
            tc.Props.Add("canSaveDynamicFields", "1");
            tc.Props.Add("splitFadeDistances", @"""10 20 30 40""");

            string rightbrakelight = tc.Create(m_ts).ToString(CultureInfo.InvariantCulture);
            console.SetVar(string.Format("{0}.rightBrakeLight", obj), rightbrakelight);

            string leftbrakelight = tc.Create(m_ts).ToString(CultureInfo.InvariantCulture);
            console.SetVar(string.Format("{0}.leftBrakeLight", obj), leftbrakelight);

            console.SetVar(string.Format("{0}.inv[BulletAmmo]", obj), 1000);


            // Mount a ShapeBaseImageData
            //Current T3d Turrets are broken in dedicated mode.
            // ShapeBase.mountImage(obj, "TurretImage", con.GetVarInt(thisobj + ".turretSlot"), true, "");
            // Mount the brake lights

            SceneObject.mountObject(obj, rightbrakelight, console.GetVarInt(string.Format("{0}.rightBrakeSlot", thisobj)), new TransformF(true));
            SceneObject.mountObject(obj, leftbrakelight, console.GetVarInt(string.Format("{0}.leftBrakeSlot", thisobj)), new TransformF(true));
            }


        [Torque_Decorations.TorqueCallBack("", "CheetahCar", "onRemove", "(%this, %obj,nameSpaceDepth)",  3, 2800, false)]
        public void CheetahCarOnRemove(string thisob, string obj, string nameSpaceDepth)
            {
            int nsd = (nameSpaceDepth.AsInt() + 1);
            console.ParentExecute(thisob, "onRemove", nsd, new[] { thisob, obj });
            if (console.isObject(console.GetVarString(string.Format("{0}.rightBrakeLight", obj))))
                console.Call(string.Format("{0}.rightBrakeLight", obj), "delete");
            if (console.isObject(console.GetVarString(string.Format("{0}.leftBrakeLight", obj))))
                console.Call(string.Format("{0}.leftBrakeLight", obj), "delete");
            if (console.isObject(console.GetVarString(string.Format("{0}.turret", obj))))
                console.Call(string.Format("{0}.turret", obj), "delete");
            }

        [Torque_Decorations.TorqueCallBack("", "", "serverCmdtoggleBrakeLights", "(%client)",  1, 2800, false)]
        public void CheetahCarServerCmdtoggleBrakeLights(string client)
            {
            string player = console.GetVarString(string.Format("{0}.player", client));
            //Remember to pay attention to what type of object your looking at.
            string car = Player.getControlObject(player).AsString();
            if (console.GetClassName(car) != "WheeledVehicle") return;

            if (console.GetVarInt(string.Format("{0}.rightBrakeLight.isEnabled", car)) == 1)
                {
                LightBase.setLightEnabled(console.GetVarString(string.Format("{0}.rightBrakeLight", car)), false);
                LightBase.setLightEnabled(console.GetVarString(string.Format("{0}.leftBrakeLight", car)), false);
                }
            else
                {
                LightBase.setLightEnabled(console.GetVarString(string.Format("{0}.rightBrakeLight", car)), true);
                LightBase.setLightEnabled(console.GetVarString(string.Format("{0}.leftBrakeLight", car)), true);
                }
            }

        // Callback invoked when an input move trigger state changes when the CheetahCar
        //  is the control object
        [Torque_Decorations.TorqueCallBack("", "CheetahCar", "onTrigger", "(%this, %obj, %index, %state)",  4, 2800, false)]
        public void CheetahCarOnTrigger(string thisobj, string obj, string index, string state)
            {
            // Pass trigger states on to TurretImage (to fire weapon)
            switch (int.Parse(index))
                {
                case 0:
                    ShapeBase.setImageTrigger(obj, console.GetVarInt(string.Format("{0}.turretSlot", thisobj)), (state == "1" ? true : false));
                    break;
                case 1:
                    ShapeBase.setImageAltTrigger(obj, console.GetVarInt(string.Format("{0}.turretSlot", thisobj)), (state == "1" ? true : false));
                    break;
                }
            }

        [Torque_Decorations.TorqueCallBack("", "CheetahCar", "onMount", "(%this, %obj, %slot)",  3, 2800, false)]
        public void CheetahCarOnMount(string thisobj, string obj, string slot)
            {
            // Load the gun
            ShapeBase.setImageAmmo(obj, slot.AsInt(), true);
            }
        }
    }