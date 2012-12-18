using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WinterLeaf.Enums
    {

    /// The sound playback state.
    public enum SFXStatus : int
        {
        /// Initial state; no operation yet performed on sound.
        SFXStatusNull = 0,

        /// Sound is playing.
        SFXStatusPlaying = 1,

        /// Sound has been stopped.
        SFXStatusStopped = 2,

        /// Sound is paused.
        SFXStatusPaused = 3,

        /// Sound stream is starved and playback blocked.
        SFXStatusBlocked = 4,

        /// Temporary state while transitioning to another state.  This is used when multiple
        /// threads concurrently maintain a status and need to perform a sequence of actions before
        /// being able to fully go from a previous to a new current state.  In this case, the
        /// transition state marks the status as being under update on another thread.
        ///
        /// @note Not all places that use SFXStatus actually use this state.
        SFXStatusTransition = 5,
        };

    }
