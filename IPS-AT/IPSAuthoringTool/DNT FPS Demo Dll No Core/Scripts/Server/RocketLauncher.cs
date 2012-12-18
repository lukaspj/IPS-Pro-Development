using System;
using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Containers;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "RocketLauncherImage", "readyLoad", "(%this)",  1, 2000, false)]
        public void RocketLauncherImageReadyLoad(string thisobj)
            {
            console.SetVar(string.Format("{0}.loadCount", thisobj), 1);
            }

        [Torque_Decorations.TorqueCallBack("", "RocketLauncherImage", "incLoad", "(%this)",  1, 2000, false)]
        public void RocketLauncerincLoad(string thisobj)
            {
            console.SetVar(string.Format("{0}.loadCount", thisobj), console.GetVarInt(string.Format("{0}.loadCount", thisobj)) + 1);
            }

        // ----------------------------------------------------------------------------
        // The fire method that does all of the work
        // ----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "RocketLauncherImage", "onAltFire", "(%this, %obj, %slot)",  3, 2000, false)]
        public string RocketLauncherImageOnAltFire(string thisobj, string obj, string slot)
            {
            string currentAmmo = ShapeBaseShapeBaseGetInventory(obj, console.GetVarString(string.Format("{0}.ammo", thisobj))).AsString();
            if (currentAmmo.AsInt() < console.GetVarInt(string.Format("{0}.loadCount", thisobj)))
                console.SetVar(string.Format("{0}.loadCount", thisobj), currentAmmo);
            string project = "";
            for (int shotCount = 0; shotCount < console.GetVarInt(string.Format("{0}.loadCount", thisobj)); shotCount++)
                {
                // Decrement inventory ammo. The image's ammo state is updated
                // automatically by the ammo inventory hooks.
                ShapeBaseShapeBaseDecInventory(obj, console.GetVarString(string.Format("{0}.ammo", thisobj)), "1");
                // We fire our weapon using the straight ahead aiming point of the gun
                // We'll need to "skew" the projectile a little bit.  We start by getting
                // the straight ahead aiming point of the gun
                Point3F vec = ShapeBase.getMuzzleVector(obj, slot.AsInt());
                Random r = new Random();
                TransformF matrix = new TransformF();
                matrix.MPosition.x = (float)((r.NextDouble() - .5) * 2 * Math.PI * 0.008);
                matrix.MPosition.y = (float)((r.NextDouble() - .5) * 2 * Math.PI * 0.008);
                matrix.MPosition.z = (float)((r.NextDouble() - .5) * 2 * Math.PI * 0.008);
                TransformF mat = math.MatrixCreateFromEuler(matrix);

                // Which we'll use to alter the projectile's initial vector with
                TransformF muzzleVector = math.MatrixMulVector(mat, vec);

                // Get the player's velocity, we'll then add it to that of the projectile
                TransformF objectVelocity = new TransformF(ShapeBase.getVelocity(obj));

                muzzleVector = muzzleVector.vectorScale(console.GetVarFloat(string.Format("{0}.projectile.muzzleVelocity", thisobj)));
                objectVelocity = objectVelocity.vectorScale(console.GetVarFloat(string.Format("{0}.projectile.velInheritFactor", thisobj)));
                TransformF muzzleVelocity = muzzleVector + objectVelocity;

                Torque_Class_Helper tch = new Torque_Class_Helper(console.GetVarString(string.Format("{0}.projectileType", thisobj)), "");
                tch.Props.Add("dataBlock", console.GetVarString(string.Format("{0}.projectile", thisobj)));
                tch.Props.Add("initialVelocity", '"' + muzzleVelocity.ToString() + '"');
                tch.Props.Add("initialPosition", '"' + ShapeBase.getMuzzlePoint(obj, slot.AsInt()).ToString() + '"');
                tch.Props.Add("sourceObject", obj);
                tch.Props.Add("sourceSlot", slot);
                tch.Props.Add("client", console.GetVarString(string.Format("{0}.client", obj)));

                project = tch.Create(m_ts).ToString(CultureInfo.InvariantCulture);
                SimSet.pushToBack("MissionCleanup", project);

                }
            return project;
            }
        }
    }