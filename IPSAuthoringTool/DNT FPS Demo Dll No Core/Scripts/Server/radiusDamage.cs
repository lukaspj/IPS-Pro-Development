using System;
using System.Collections.Generic;
using System.Globalization;
using WinterLeaf.Classes;
using WinterLeaf.Enums;
using WinterLeaf.Containers;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "radiusDamage", "",  6, 1700, false)]
        public void RadiusDamage(string sourceobject, string position, string radius, string damage, string damageType, string simpulse)
            {
            float impulse = float.Parse(simpulse);
            // Use the container system to iterate through all the objects
            // within our explosion radius.  We'll apply damage to all ShapeBase
            // objects.
            Dictionary<uint, float> r = console.initContainerRadiusSearch(new Point3F(position), radius.AsFloat(), (uint) SceneObjectTypesAsUint.ShapeBaseObjectType);
            float halfRadius = radius.AsFloat()/(float) 2.0;
            foreach (uint targetObject in r.Keys)
                {
                // Calculate how much exposure the current object has to
                // the explosive force.  The object types listed are objects
                // that will block an explosion.  If the object is totally blocked,
                // then no damage is applied.

                UInt32 mask = (uint) SceneObjectTypesAsUint.InteriorObjectType | (uint) SceneObjectTypesAsUint.TerrainObjectType | (uint) SceneObjectTypesAsUint.StaticShapeObjectType | (uint) SceneObjectTypesAsUint.VehicleObjectType;

                float coverage = Util.calcExplosionCoverage(new Point3F(position), (int)targetObject, mask);
                if (!coverage.AsBool()) continue;
                float dist = r[targetObject];
                // Calculate a distance scale for the damage and the impulse.
                // Full damage is applied to anything less than half the radius away,
                // linear scale from there.
                float distScale = (float) ((dist < halfRadius) ? 1.0 : 1 - ((dist - halfRadius)/halfRadius));
                // Apply the damage
                ShapeBaseDamage(targetObject.AsString(), sourceobject, position, ((float.Parse(damage) * coverage * distScale)).AsString(), damageType);

                
                // Apply the impulse
                if (!impulse.AsBool()) continue;
                TransformF impulseVec =new TransformF( SceneObject.getWorldBoxCenter(targetObject.AsString())) - new TransformF(position);
                impulseVec = impulseVec.normalizeSafe();
                impulseVec = impulseVec.vectorScale(impulse*distScale);
                ShapeBase.applyImpulse(targetObject.AsString(), new Point3F(position), impulseVec.MPosition);
                }
            }
        }
    }