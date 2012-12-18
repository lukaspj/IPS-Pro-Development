using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WinterLeaf.Enums
    {
    /// Rolloff curve used for distance volume attenuation of 3D sounds.
    public enum SFXDistanceModel
        {
        SFXDistanceModelLinear,             ///< Volume decreases linearly from min to max where it reaches zero.
        SFXDistanceModelLogarithmic,        ///< Volume halves every min distance steps starting from min distance; attenuation stops at max distance.
        };
    }
