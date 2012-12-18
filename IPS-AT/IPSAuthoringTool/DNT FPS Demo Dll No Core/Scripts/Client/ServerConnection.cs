using WinterLeaf.Classes;

namespace DNT_FPS_Demo_Game_Dll.Scripts.Client
    {
    public partial class Main : TorqueScriptTemplate
        {
        [Torque_Decorations.TorqueCallBack("", "", "ServerConnection_init", "", 0, 9000, true)]
        public void ServerConnection_init()
            {
            console.Eval("addMessageCallback( 'MsgConnectionError', handleConnectionErrorMessage );");

            }

        [Torque_Decorations.TorqueCallBack("", "", "handleConnectionErrorMessage", "(%msgType, %msgString, %msgError)", 3, 9000, false)]
        public void handleConnectionErrorMessage(string msgType, string msgString, string msgError)
            {
            console.SetVar("$ServerConnectionErrorMessage", msgError);

            }

        //----------------------------------------------------------------------------
        // GameConnection client callbacks
        //----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "initialControlSet", "(%this)", 1, 9000, false)]
        public void GameConnectioninitialControlSet(string thisobj)
            {
            console.print("*** Initial Control Object");

            // The first control object has been set by the server
            // and we are now ready to go.

            // first check if the editor is active

            if (!Util.isToolBuild() && console.Call_Classname("Editor", "checkActiveLoadDone").AsBool()) return;
            if (GuiCanvas.getContent("Canvas") == console.Call("PlayGui", "getID").AsInt()) return;
            GuiCanvas.setContent("Canvas", "PlayGui");
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onControlObjectChange", "(%this)", 1, 9000, false)]
        public void GameConnectionOnControlObjectChange(string thisobj)
            {
            console.print("*** Control Object Changed");
            // Reset the current FOV to match the new object
            // and turn off any current zoom.

            resetCurrentFOV();


            turnOffZoom();

            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "setLagIcon", "(%this,%state)", 2, 9000, false)]
        public void GameConnectionsetLagIcon(string thisobj, bool state)
            {
            if (NetConnection.getAddress(thisobj) == "local")
                return;

            GuiControl.setVisible("LagIcon", state);

            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onFlash", "(%this,%state)", 2, 9000, false)]
        public void GameConnectiononFlash(string thisobj, string state)
            {
            if (state.AsBool())
                PostEffect.enable("FlashFx");
            else
                PostEffect.disable("FlashFx");
            }

        // Called on the new connection object after connect() succeeds.
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnectionAccepted", "(%this)", 1, 9000, false)]
        public void GameConnectiononConnectionAccepted(string thisobj)
            {
            // Called on the new connection object after connect() succeeds.
            GuiControl.setVisible("LagIcon", false);
            // Startup the physics world on the client before any
            // datablocks and objects are ghosted over.
            console.Call("physicsInitWorld", new[] { "client" });

            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnectionTimedOut", "(%this)", 1, 9000, false)]
        public void GameConnectiononConnectionTimedOut(string thisobj)
            {
            // Called when an established connection times out

            disconnectedCleanup();

            console.Call("MessageBoxOk", new[] { "TIMED OUT", "The server connection has timed out." });
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnectionDropped", "(%this, %msg)", 2, 9000, false)]
        public void GameConnectionOnConnectionDropped(string thisobj, string msg)
            {

            disconnectedCleanup();

            console.Call("MessageBoxOk", new[] { "DISCONNECT", "The server has dropped the connection: " + msg });
            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnectionError", "(%this, %msg)", 2, 9000, false)]
        public void GameConnectiononConnectionError(string thisobj, string msg)
            {

            disconnectedCleanup();

            console.Call("MessageBoxOk", new[] { "DISCONNECT", console.GetVarString("$ServerConnectionErrorMessage") + "(" + msg + ")" });
            }

        //----------------------------------------------------------------------------
        // Connection Failed Events
        //----------------------------------------------------------------------------
        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnectRequestRejected", "(%this, %msg)", 2, 9000, false)]
        public void GameConnectionOnConnectRequestRejected(string thisobj, string msg)
            {
            string error = "";
            switch (msg)
                {
                case "CR_INVALID_PROTOCOL_VERSION":
                    error = "Incompatible protocol version: Your game version is not compatible with this server.";
                    break;
                case "CR_INVALID_CONNECT_PACKET":
                    error = "Internal Error: badly formed network packet";
                    break;
                case "CR_YOUAREBANNED":
                    error = "You are not allowed to play on this server.";
                    break;
                case "CR_SERVERFULL":
                    error = "This server is full.";
                    break;
                case "CHR_PASSWORD":
                    if (console.GetVarString("$Client::Password") == "")
                        console.Call("MessageBoxOk", new[] { "REJECTED", "That server requires a password." });
                    else
                        {
                        console.SetVar("$Client::Password", "");
                        console.Call("MessageBoxOk", new[] { "REJECTED", "That password is incorrect." });
                        }
                    return;
                case "CHR_PROTOCOL":
                    error = "Incompatible protocol version: Your game version is not compatible with this server.";
                    break;
                case "CHR_CLASSCRC":
                    error = "Incompatible game classes: Your game version is not compatible with this server.";
                    break;
                case "CHR_INVALID_CHALLENGE_PACKET":
                    error = "Internal Error: Invalid server response packet";
                    break;
                default:
                    error = "Connection error.  Please try another server.  Error code: (" + msg + ")";
                    break;
                }
            disconnectedCleanup();

            console.Call("MessageBoxOk", new[] { "REJECTED", error });

            }

        [Torque_Decorations.TorqueCallBack("", "GameConnection", "onConnectRequestTimedOut", "(%this)", 1, 9000, false)]
        public void GameConnectiononConnectRequestTimedOut(string thisobj)
            {

            disconnectedCleanup();
            console.Call("MessageBoxOk", new[] { "TIMED OUT", "Your connection to the server timed out." });
            }

        //-----------------------------------------------------------------------------
        // Disconnect
        //-----------------------------------------------------------------------------

        [Torque_Decorations.TorqueCallBack("", "", "disconnect", "()", 0, 9000, false)]
        public void disconnect()
            {
            // We need to stop the client side simulation
            // else physics resources will not cleanup properly.
            console.Call("physicsStopSimulation", new[] { "client" });

            // Delete the connection if it's still there.
            if (console.isObject("ServerConnection"))
                console.Call("ServerConnection", "delete");


            disconnectedCleanup();
            // Call destroyServer in case we're hosting
            console.Call("destroyServer");


            }

        [Torque_Decorations.TorqueCallBack("", "", "disconnectedCleanup", "()", 0, 9000, false)]
        public void disconnectedCleanup()
            {
            // End mission, if it's running.
            if (console.GetVarBool("$Client::missionRunning"))

                clientEndMission();

            // Disable mission lighting if it's going, this is here
            // in case we're disconnected while the mission is loading.
            console.SetVar("$lightingMission", false);
            console.SetVar("$sceneLighting::terminateLighting", true);
            // Clear misc script stuff

            MessageVector.clear("HudMessageVector");
            GuiControl.setValue("LagIcon", false.AsString());


            PlayerListGuiClear("PlayerListGui");


            clientCmdClearBottomPrint();


            clientCmdClearCenterPrint();

            if (console.isObject("MainMenuGui"))
                GuiCanvas.setContent("Canvas", "MainMenuGui");

            else if (console.isObject("UnifiedMainMenuGui"))
                GuiCanvas.setContent("Canvas", "UnifiedMainMenuGui");

            console.Call("physicsDestroyWorld", new[] { "client" });

            showCursor();


            }
        }
    }