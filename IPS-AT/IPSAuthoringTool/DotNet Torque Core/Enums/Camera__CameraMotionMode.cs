using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WinterLeaf.Enums
    {
   public  enum Camera__CameraMotionMode
        {
        StationaryMode = 0,

        FreeRotateMode,
        FlyMode,
        OrbitObjectMode,
        OrbitPointMode,
        TrackObjectMode,
        OverheadMode,
        EditOrbitMode,       ///< Used by the World Editor

        CameraFirstMode = 0,
        CameraLastMode = EditOrbitMode
        };
    }
