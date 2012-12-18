using System;

namespace WinterLeaf.Enums
{
    /// <summary>
    /// The Scene Object type flags.
    /// </summary>
    [Flags]
    public enum SceneObjectTypesAsUint : uint
    {
        DefaultObjectType = 0x00000000,
        StaticObjectType = 0x00000001,
        EnvironmentObjectType = 0x00000002,
        TerrainObjectType = 0x00000004,
        InteriorObjectType = 0x00000008,
        WaterObjectType = 0x00000010,
        TriggerObjectType = 0x00000020,
        MarkerObjectType = 0x00000040,
        LightObjectType = 0x00000080,
        ZoneObjectType = 0x00000100,
        StaticShapeObjectType = 0x00000200,
        DynamicShapeObjectType = 0x00000400,
        GameBaseObjectType = 0x00000800,
        GameBaseHiFiObjectType = 0x00001000,
        ShapeBaseObjectType = 0x00002000,
        CameraObjectType = 0x00004000,
        PlayerObjectType = 0x00008000,
        ItemObjectType = 0x00010000,
        VehicleObjectType = 0x00020000,
        VehicleBlockerObjectType = 0x00040000,
        ProjectileObjectType = 0x00080000,
        ExplosionObjectType = 0x00100000,
        CorpseObjectType = 0x00200000,
        DebrisObjectType = 0x00400000,
        PhysicalZoneObjectType = 0x00800000,
        InteriorLikeObjectType = 0x01000000,
        TerrainLikeObjectType = 0x02000000,
        afxModelObjectType = 0x04000000,
        Flag28 = 0x08000000,
        Flag29 = 0x10000000,
        Flag30 = 0x20000000,
        Flag31 = 0x40000000,
        Flag32 = 0x80000000
    }
    /// <summary>
    /// The Scene Object type flags.
    /// </summary>
    [Flags]
    public enum SceneObjectTypesAsInt : int
        {
        DefaultObjectType = 0x00000000,
        StaticObjectType = 0x00000001,
        EnvironmentObjectType = 0x00000002,
        TerrainObjectType = 0x00000004,
        InteriorObjectType = 0x00000008,
        WaterObjectType = 0x00000010,
        TriggerObjectType = 0x00000020,
        MarkerObjectType = 0x00000040,
        LightObjectType = 0x00000080,
        ZoneObjectType = 0x00000100,
        StaticShapeObjectType = 0x00000200,
        DynamicShapeObjectType = 0x00000400,
        GameBaseObjectType = 0x00000800,
        GameBaseHiFiObjectType = 0x00001000,
        ShapeBaseObjectType = 0x00002000,
        CameraObjectType = 0x00004000,
        PlayerObjectType = 0x00008000,
        ItemObjectType = 0x00010000,
        VehicleObjectType = 0x00020000,
        VehicleBlockerObjectType = 0x00040000,
        ProjectileObjectType = 0x00080000,
        ExplosionObjectType = 0x00100000,
        CorpseObjectType = 0x00200000,
        DebrisObjectType = 0x00400000,
        PhysicalZoneObjectType = 0x00800000,
        InteriorLikeObjectType = 0x01000000,
        TerrainLikeObjectType = 0x02000000,
        afxModelObjectType = 0x04000000,
        Flag28 = 0x08000000,
        Flag29 = 0x10000000,
        Flag30 = 0x20000000,
        Flag31 = 0x40000000
        }
}