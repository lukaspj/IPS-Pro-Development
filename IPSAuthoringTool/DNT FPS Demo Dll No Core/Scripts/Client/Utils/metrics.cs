using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WinterLeaf;
using WinterLeaf.Classes;
using WinterLeaf.Containers;
using WinterLeaf.Enums;
namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {
        /// Shortcut for typi1ng dbgSetParameters with the default values torsion uses.
        [Torque_Decorations.TorqueCallBack("", "", "initmetrics", "", 0, 36000, true)]
        public void initmetrics()
            {
            //con.error("------------------------>LOADING FRAMEOVERLAYGUI!!!!!!!!!!!!!!!!!!!!!");
            Util.exec("core/art/gui/FrameOverlayGui.gui", false, false);
            }
        [Torque_Decorations.TorqueCallBack("", "", "fpsMetricsCallback", "", 0, 36000, false)]
        public string fpsMetricsCallback()
            {
            return "  | FPS |" +
                "  " + console.GetVarString("$fps::real") +
                "  max: " + console.GetVarString("$fps::realMax") +
                "  min: " + console.GetVarString("$fps::realMin") +
                "  mspf: " + (1000 / (console.GetVarInt("$fps::real") > 0 ? console.GetVarInt("$fps::real") : 1));
            }
        [Torque_Decorations.TorqueCallBack("", "", "gfxMetricsCallback", "", 0, 36000, false)]
        public string gfxMetricsCallback()
            {
            return "  | GFX |" +
          "  PolyCount: " + console.GetVarString("$GFXDeviceStatistics::polyCount") +
          "  DrawCalls: " + console.GetVarString("$GFXDeviceStatistics::drawCalls") +
          "  RTChanges: " + console.GetVarString("$GFXDeviceStatistics::renderTargetChanges");
            }
        [Torque_Decorations.TorqueCallBack("", "", "terrainMetricsCallback", "", 0, 36000, false)]
        public string terrainMetricsCallback()
            {
            return "  | Terrain |" +
          "  Cells: " + console.GetVarString("$TerrainBlock::cellsRendered") +
          "  Override Cells: " + console.GetVarString("$TerrainBlock::overrideCells") +
          "  DrawCalls: " + console.GetVarString("$TerrainBlock::drawCalls");
            }
        [Torque_Decorations.TorqueCallBack("", "", "netMetricsCallback", "", 0, 36000, false)]
        public string netMetricsCallback()
            {
            return "  | Net |" +
          "  BitsSent: " + console.GetVarString("$Stats::netBitsSent") +
          "  BitsRcvd: " + console.GetVarString("$Stats::netBitsReceived") +
          "  GhostUpd: " + console.GetVarString("$Stats::netGhostUpdates");
            }

        [Torque_Decorations.TorqueCallBack("", "", "groundCoverMetricsCallback", "", 0, 36000, false)]
        public string groundCoverMetricsCallback()
            {
            return "  | GroundCover |" +
          "  Cells: " + console.GetVarString("$GroundCover::renderedCells") +
          "  Billboards: " + console.GetVarString("$GroundCover::renderedBillboards") +
          "  Batches: " + console.GetVarString("$GroundCover::renderedBatches") +
          "  Shapes: " + console.GetVarString("$GroundCover::renderedShapes");
            }
        [Torque_Decorations.TorqueCallBack("", "", "sfxMetricsCallback", "", 0, 36000, false)]
        public string sfxMetricsCallback()
            {
            return "  | SFX |" +
          "  Sounds: " + console.GetVarString("$SFX::numSounds") +
          "  Lists: " + (console.GetVarInt("$SFX::numSources") - console.GetVarInt("$SFX::numSounds") - console.GetVarInt("$SFX::Device::fmodNumEventSource")) +
          "  Events: " + console.GetVarString("$SFX::fmodNumEventSources") +
          "  Playing: " + console.GetVarString("$SFX::numPlaying") +
          "  Culled: " + console.GetVarString("$SFX::numCulled") +
          "  Voices: " + console.GetVarString("$SFX::numVoices") +
          "  Buffers: " + console.GetVarString("$SFX::Device::numBuffers") +
          "  Memory: " + (console.GetVarFloat("$SFX::Device::numBufferBytes") / 1024.0 / 1024.0) + " MB" +
          "  Time/S: " + console.GetVarString("$SFX::sourceUpdateTime") +
          "  Time/P: " + console.GetVarString("$SFX::parameterUpdateTime") +
          "  Time/A: " + console.GetVarString("$SFX::ambientUpdateTime");
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxSourcesMetricsCallback", "", 0, 36000, false)]
        public string sfxSourcesMetricsCallback()
            {
            return console.Call("sfxDumpSourcesToString");
            }

        [Torque_Decorations.TorqueCallBack("", "", "sfxStatesMetricsCallback", "", 0, 36000, false)]
        public string sfxStatesMetricsCallback()
            {
            return "  | SFXStates |" + console.Call("sfxGetActiveStates");
            }
        [Torque_Decorations.TorqueCallBack("", "", "timeMetricsCallback", "", 0, 36000, false)]
        public string timeMetricsCallback()
            {
            return "  | Time |" +
          "  Sim Time: " + console.Call("getSimTime") +
          "  Mod: " + (console.Call("getSimTime").AsInt() % 32);

            }
        [Torque_Decorations.TorqueCallBack("", "", "reflectMetricsCallback", "", 0, 36000, false)]
        public string reflectMetricsCallback()
            {
            return "  | REFLECT |" +
          "  Objects: " + console.Call("$Reflect::numObjects") +
          "  Visible: " + console.Call("$Reflect::numVisible") +
          "  Occluded: " + console.Call("$Reflect::numOccluded") +
          "  Updated: " + console.Call("$Reflect::numUpdated") +
          "  Elapsed: " + console.Call("$Reflect::elapsed") + "\n" +

          "  Allocated: " + console.Call("$Reflect::renderTargetsAllocated") +
          "  Pooled: " + console.Call("$Reflect::poolSize") + "\n" +



          "  " + console.GetVarString("$Reflect::textureStats").Split(' ')[1] + "\t" +
          "  " + console.GetVarString("$Reflect::textureStats").Split(' ')[2] + "MB" + "\t" +
          "  " + console.GetVarString("$Reflect::textureStats").Split(' ')[0];

            }
        [Torque_Decorations.TorqueCallBack("", "", "decalMetricsCallback", "", 0, 36000, false)]
        public string decalMetricsCallback()
            {
            return "  | DECAL |" +
         " Batches: " + console.GetVarString("$Decal::Batches") +
         " Buffers: " + console.GetVarString("$Decal::Buffers") +
         " DecalsRendered: " + console.GetVarString("$Decal::DecalsRendered");
            }
        [Torque_Decorations.TorqueCallBack("", "", "renderMetricsCallback", "", 0, 36000, false)]
        public string renderMetricsCallback()
            {
            return "  | Render |" +
          "  Int: " + console.GetVarString("$RenderMetrics::RIT_Interior") +
          "  IntDL: " + console.GetVarString("$RenderMetrics::RIT_InteriorDynamicLighting") +
          "  Mesh: " + console.GetVarString("$RenderMetrics::RIT_Mesh") +
          "  MeshDL: " + console.GetVarString("$RenderMetrics::RIT_MeshDynamicLighting") +
          "  Shadow: " + console.GetVarString("$RenderMetrics::RIT_Shadow") +
          "  Sky: " + console.GetVarString("$RenderMetrics::RIT_Sky") +
          "  Obj: " + console.GetVarString("$RenderMetrics::RIT_Object") +
          "  ObjT: " + console.GetVarString("$RenderMetrics::RIT_ObjectTranslucent") +
          "  Decal: " + console.GetVarString("$RenderMetrics::RIT_Decal") +
          "  Water: " + console.GetVarString("$RenderMetrics::RIT_Water") +
          "  Foliage: " + console.GetVarString("$RenderMetrics::RIT_Foliage") +
          "  Trans: " + console.GetVarString("$RenderMetris::RIT_Translucent") +
          "  Custom: " + console.GetVarString("$RenderMetrics::RIT_Custom");
            }

        [Torque_Decorations.TorqueCallBack("", "", "shadowMetricsCallback", "", 0, 36000, false)]
        public string shadowMetricsCallback()
            {
            return "  | Shadow |" +
          "  Active: " + console.GetVarString("$ShadowStats::activeMaps") +
          "  Updated: " + console.GetVarString("$ShadowStats::updatedMaps") +
          "  PolyCount: " + console.GetVarString("$ShadowStats::polyCount") +
          "  DrawCalls: " + console.GetVarString("$ShadowStats::drawCalls") +
          "   RTChanges: " + console.GetVarString("$ShadowStats::rtChanges") +
          "   PoolTexCount: " + console.GetVarString("$ShadowStats::poolTexCount") +
          "   PoolTexMB: " + console.GetVarString("$ShadowStats::poolTexMemory") + "MB";
            }
        [Torque_Decorations.TorqueCallBack("", "", "basicShadowMetricsCallback", "", 0, 36000, false)]
        public string basicShadowMetricsCallback()
            {
            return "  | Shadow |" +
          "  Active: " + console.GetVarString("$BasicLightManagerStats::activePlugins") +
          "  Updated: " + console.GetVarString("$BasicLightManagerStats::shadowsUpdated") +
          "  Elapsed Ms: " + console.GetVarString("$BasicLightManagerStats::elapsedUpdateMs");
            }

        [Torque_Decorations.TorqueCallBack("", "", "lightMetricsCallback", "", 0, 36000, false)]
        public string lightMetricsCallback()
            {
            return "  | Deferred Lights |" +
          "  Active: " + console.GetVarString("$lightMetrics::activeLights") +
          "  Culled: " + console.GetVarString("$lightMetrics::culledLights");
            }

        [Torque_Decorations.TorqueCallBack("", "", "particleMetricsCallback", "", 0, 36000, false)]
        public string particleMetricsCallback()
            {
            return "  | Particles |" +
          "  # Simulated " + console.GetVarString("$particle::numSimulated");
            }
        [Torque_Decorations.TorqueCallBack("", "", "partMetricsCallback", "", 0, 36000, false)]
        public string partMetricsCallback()
            {
            return console.Call("particleMetricsCallback");
            }
        [Torque_Decorations.TorqueCallBack("", "", "audioMetricsCallback", "", 0, 36000, false)]
        public string audioMetricsCallback()
            {
            return console.Call("sfxMetricsCallback");
            }

        [Torque_Decorations.TorqueCallBack("", "", "videoMetricsCallback", "", 0, 36000, false)]
        public string videoMetricsCallback()
            {
            return console.Call("gfxMetricsCallback");
            }
        // Add a metrics HUD.  %expr can be a vector of names where each element
        // must have a corresponding '<name>MetricsCallback()' function defined
        // that will be called on each update of the GUI control.  The results
        // of each function are stringed together.
        //
        // Example: metrics( "fps gfx" );
        [Torque_Decorations.TorqueCallBack("", "", "metrics", "%expr", 1, 36000, false)]
        public void metrics(string expr)
            {
            string metricsExpr = "";
            if (expr != "")
                {
                foreach (string name in expr.Split(' '))
                    {
                    string cb = name.Trim() + "MetricsCallback";
                    if (!console.isFunction(cb))
                        {
                        console.error("metrics - undefined callback: " + cb);
                        }
                    else
                        {
                        cb = cb + "()";
                        if (metricsExpr.Length > 0)
                            metricsExpr += "\r";
                        metricsExpr += cb;
                        break;
                        //right now, for some reason it can only handle one metric...
                        }
                    }
                //if (metricsExpr != "")
                //    metricsExpr += " @ " + '\\' + '"' + " " + '\\' + '"';
                }
            if (metricsExpr != "")
                {
                console.Call("Canvas", "pushDialog", new string[] { "FrameOverlayGui", "1000" });
                console.Call("TextOverlayControl", "setValue", new string[] { metricsExpr });
                }
            else
                {
                console.Call("Canvas", "popDialog", new string[] { "FrameOverlayGui" });
                }

            }
        }
    }
