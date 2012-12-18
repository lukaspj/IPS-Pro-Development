using WinterLeaf.Classes;
using WinterLeaf.Enums;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Server
    {
    public partial class Main : TorqueScriptTemplate
        {
        //-----------------------------------------------------------------------------
        // Torque
        // Copyright GarageGames, LLC 2011
        //-----------------------------------------------------------------------------

        // "Universal" script methods for projectile damage handling.  You can easily
        // override these support functions with an equivalent namespace method if your
        // weapon needs a unique solution for applying damage.

        [Torque_Decorations.TorqueCallBack("", "ProjectileData", "onCollision", "(%data, %proj, %col, %fade, %pos, %normal)",  6, 1600, false)]
        public void ProjectileDataOnCollision(string datablock, string projectile, string shapebase, string fad, string pos, string normal)
            {
            // Apply damage to the object all shape base objects
            if (console.GetVarFloat(string.Format("{0}.directDamage", datablock)) > 0)
                if ((console.getTypeMask(shapebase) & (uint)SceneObjectTypesAsUint.ShapeBaseObjectType) == (uint)SceneObjectTypesAsUint.ShapeBaseObjectType)
                    ShapeBaseDamage(shapebase, projectile, pos, console.GetVarString(string.Format("{0}.directDamage", datablock)), console.GetVarString(string.Format("{0}.damageType", datablock)));
            }

        [Torque_Decorations.TorqueCallBack("", "ProjectileData", "onExplode", "(%data, %proj, %position, %mod)",  4, 1600, false)]
        public void ProjectileDataOnExplode(string data, string proj, string position, string mod)
            {
            // Damage objects within the projectiles damage radius
            string radius = console.GetVarString(string.Format("{0}.damageRadius", data));
            if (radius.AsFloat() <= 0) return;
            string damageType = console.GetVarString(string.Format("{0}.damageType", data));
            string areaImpulse = console.GetVarString(string.Format("{0}.areaImpulse", data));
            string radiusDamage = console.GetVarString(string.Format("{0}.radiusDamage", data));
            RadiusDamage(proj, position, radius, radiusDamage, damageType, areaImpulse);
            }
        }
    }