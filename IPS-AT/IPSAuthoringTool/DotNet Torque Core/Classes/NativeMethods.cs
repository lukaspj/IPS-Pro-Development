using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Interop;
using WinterLeaf.Enums;
using System.Windows.Forms;

namespace WinterLeaf.Classes
    {
    internal class WinPosition
        {
        internal int Left { get; set; }
        internal int Top { get; set; }
        internal int Right { get; set; }
        internal int Bottom { get; set; }

        internal WinPosition()
            {
            }

        internal WinPosition(NativeMethods.TITLEBARINFO pti)
            {
            this.Left = pti.rcTitleBar.left;
            this.Top = pti.rcTitleBar.top;
            this.Right = pti.rcTitleBar.right;
            this.Bottom = pti.rcTitleBar.bottom;
            }
        internal WinPosition(NativeMethods.WINDOWINFO wti)
            {
            this.Left = wti.rcClient.left;
            this.Top = wti.rcClient.top;
            this.Right = wti.rcClient.right;
            this.Bottom = wti.rcClient.bottom;
            }
        }
    internal class Helpers
        {
        internal static IntPtr Find(string ModuleName, string MainWindowTitle)
            {
            //Search the window using Module and Title
            IntPtr WndToFind = NativeMethods.FindWindow(ModuleName, MainWindowTitle);
            if (WndToFind.Equals(IntPtr.Zero))
                {
                if (!string.IsNullOrEmpty(MainWindowTitle))
                    {
                    //Search window using TItle only.
                    WndToFind = NativeMethods.FindWindowByCaption(WndToFind, MainWindowTitle);
                    if (WndToFind.Equals(IntPtr.Zero))
                        return new IntPtr(0);
                    }
                }
            return WndToFind;
            }

        internal static WinPosition GetWindowPosition(IntPtr wnd)
            {
            NativeMethods.WINDOWINFO wi = new NativeMethods.WINDOWINFO();
            wi.cbSize = (uint)Marshal.SizeOf(wi);
            bool result = NativeMethods.GetWindowInfo(wnd, ref wi);
            MoniterInfo.MONITORINFOEX m = MoniterInfo.GetDesktopBoundsForWindow(wnd);
            WinPosition winpos = result ? new WinPosition(wi) : new WinPosition();
            return winpos;
            }

        internal static IntPtr SetWindowLongPtr(HandleRef hWnd, int nIndex, IntPtr dwNewLong)
            {
            return IntPtr.Size == 8 ? NativeMethods.SetWindowLongPtr64(hWnd, nIndex, dwNewLong) : new IntPtr(NativeMethods.SetWindowLong32(hWnd, nIndex, dwNewLong.ToInt32()));
            }

        //Specifies the zero-based offset to the value to be set.
        //Valid values are in the range zero through the number of bytes of extra window memory, minus the size of an integer.
        internal enum GWLParameter
            {
            GWL_EXSTYLE = -20, //Sets a new extended window style
            GWL_HINSTANCE = -6, //Sets a new application instance handle.
            GWL_HWNDPARENT = -8, //Set window handle as parent
            GWL_ID = -12, //Sets a new identifier of the window.
            GWL_STYLE = -16, // Set new window style
            GWL_USERDATA = -21, //Sets the user data associated with the window. This data is intended for use by the application that created the window. Its value is initially zero.
            GWL_WNDPROC = -4 //Sets a new address for the window procedure.
            }

        internal static int SetWindowLong(IntPtr windowHandle, GWLParameter nIndex, int dwNewLong)
            {
            if (IntPtr.Size == 8) //Check if this window is 64bit
                {
                return (int)NativeMethods.SetWindowLongPtr64(windowHandle, nIndex, new IntPtr(dwNewLong));
                }
            return NativeMethods.SetWindowLong32(windowHandle, nIndex, dwNewLong);
            }
        }
    internal static class NativeMethods
        {
        //internal enum MouseMessages
        //    {
        //    WM_LBUTTONDOWN = 0x0201,
        //    WM_LBUTTONUP = 0x0202,
        //    WM_MOUSEMOVE = 0x0200,
        //    WM_MOUSEWHEEL = 0x020A,
        //    WM_RBUTTONDOWN = 0x0204,
        //    WM_RBUTTONUP = 0x0205
        //    }

        //[StructLayout(LayoutKind.Sequential)]
        //internal struct POINT
        //    {
        //    public int x;
        //    public int y;
        //    }

        //[StructLayout(LayoutKind.Sequential)]
        //internal struct MOUSEHOOKSTRUCT
        //    {
        //    public POINT pt;
        //    public IntPtr hwnd;
        //    public uint wHitTestCode;
        //    public uint dwExtraInfo;
        //    }

        //[StructLayout(LayoutKind.Sequential)]
        //internal struct MSLLHOOKSTRUCT
        //    {
        //    public POINT pt;
        //    public uint mouseData;
        //    public uint flags;
        //    public uint time;
        //    public IntPtr dwExtraInfo;
        //    }

        //[DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        //internal static extern IntPtr SetWindowsHookEx(int idHook,
        //    LowLevelMouseProc lpfn, IntPtr hMod, uint dwThreadId);

        //[DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        //[return: MarshalAs(UnmanagedType.Bool)]
        //internal static extern bool UnhookWindowsHookEx(IntPtr hhk);

        //[DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        //internal static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode,
        //    IntPtr wParam, IntPtr lParam);

        //[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        //internal static extern IntPtr GetModuleHandle(string lpModuleName);

        //internal delegate IntPtr LowLevelMouseProc(int nCode, IntPtr wParam, IntPtr lParam);






        [DllImport("user32.dll")]
        public static extern bool ClientToScreen(IntPtr hWnd, ref Point lpPoint);

        [DllImport("user32.dll")]
        public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);

        [DllImportAttribute("user32.dll")]
        internal static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);

        [DllImport("user32.dll")]
        internal static extern bool ReleaseCapture();

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetWindowRect(IntPtr hWnd, ref RECT lpRect);


        [DllImport("user32.dll", ExactSpelling = true)]
        internal static extern IntPtr MonitorFromWindow(IntPtr handle, int flags);

        [DllImport("user32.dll")]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern IntPtr GetSystemMenu(IntPtr hWnd, bool bRevert);
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern int GetMenuItemCount(IntPtr hMenu);
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern bool DrawMenuBar(IntPtr hWnd);
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern bool RemoveMenu(IntPtr hMenu, uint uPosition, uint uFlags);
        private const Int32 MF_BYPOSITION = 0x400;
        private const Int32 MF_REMOVE = 0x1000;
        [DllImport("user32.dll")]
        static extern IntPtr CreateMenu();

        [DllImport("user32.dll", SetLastError = true)]
        static extern UInt32 GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll")]
        static extern int SetWindowLong(IntPtr hWnd, int nIndex, uint dwNewLong);

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll", SetLastError = true)]
        internal static extern bool GetWindowInfo(IntPtr hwnd, ref WINDOWINFO pwi);



        static internal void FakeFullScreen(IntPtr handle, bool On)
            {
            if (On)
                {

                SetWindowLong(handle, (int)SetWindowLongNIndex.GWL_STYLE, (uint)WindowStyles.WS_MAXIMIZE);
                ShowWindow(handle, (int)ShowWindownCMDShow.SW_SHOWMAXIMIZED);
                TaskbarSettings.SetAutoHide(true);
                }
            else
                {
                SetWindowLong(handle, (int)SetWindowLongNIndex.GWL_STYLE, (uint)(WindowStyles.WS_TILEDWINDOW));
                ShowWindow(handle, (int)ShowWindownCMDShow.SW_SHOWNORMAL);
                TaskbarSettings.SetAutoHide(false);
                }
            }
        [DllImport("user32.dll", SetLastError = true)]
        internal static extern bool SetForegroundWindow(IntPtr hWnd);

        [StructLayout(LayoutKind.Sequential)]
        internal struct WINDOWINFO
            {
            internal uint cbSize;
            internal RECT rcWindow;
            internal RECT rcClient;
            internal uint dwStyle;
            internal uint dwExStyle;
            internal uint dwWindowStatus;
            internal uint cxWindowBorders;
            internal uint cyWindowBorders;
            internal ushort atomWindowType;
            internal ushort wCreatorVersion;

            internal WINDOWINFO(Boolean? filler)
                : this()   // Allows automatic initialization of "cbSize" with "new WINDOWINFO(null/true/false)".
                {
                cbSize = (UInt32)(Marshal.SizeOf(typeof(WINDOWINFO)));
                }

            }


        internal class TaskbarSettings
            {
            private struct APPBARDATA
                {
                internal int cbSize;
                internal IntPtr hWnd;
                internal int uCallbackMessage;
                internal int uEdge;
                internal TaskbarSettings.RECT rc;
                internal IntPtr lParam;
                }
            private struct RECT
                {
                internal int Left;
                internal int Top;
                internal int Right;
                internal int Bottom;
                }
            private enum AppBarMessages
                {
                ABM_NEW,
                ABM_REMOVE,
                ABM_QUERYPOS,
                ABM_SETPOS,
                ABM_GETSTATE,
                ABM_GETTASKBARPOS,
                ABM_ACTIVATE,
                ABM_GETAUTOHIDEBAR,
                ABM_SETAUTOHIDEBAR,
                ABM_WINDOWPOSCHANGED,
                ABM_SETSTATE
                }
            private enum AppBarState
                {
                ABS_AUTOHIDE = 1,
                ABS_ALWAYSONTOP
                }
            private TaskbarSettings()
                {
                }
            [DllImport("shell32.dll")]
            private static extern int SHAppBarMessage(int message, ref TaskbarSettings.APPBARDATA data);
            private static int GetState()
                {
                TaskbarSettings.APPBARDATA aPPBARDATA = default(TaskbarSettings.APPBARDATA);
                aPPBARDATA.cbSize = Marshal.SizeOf(aPPBARDATA);
                return TaskbarSettings.SHAppBarMessage(4, ref aPPBARDATA);
                }
            private static void SetState(int state)
                {
                TaskbarSettings.APPBARDATA aPPBARDATA = default(TaskbarSettings.APPBARDATA);
                aPPBARDATA.cbSize = Marshal.SizeOf(aPPBARDATA);
                IntPtr lParam = new IntPtr(state);
                aPPBARDATA.lParam = lParam;
                TaskbarSettings.SHAppBarMessage(10, ref aPPBARDATA);
                }
            internal static bool GetAutoHide()
                {
                return (TaskbarSettings.GetState() & 1) == 1;
                }
            internal static bool GetAlwaysOnTop()
                {
                return (TaskbarSettings.GetState() & 2) == 2;
                }



            internal static void SetAutoHide(bool autohide)
                {
                int num = TaskbarSettings.GetState();
                if (autohide)
                    {
                    num |= 1;
                    }
                else
                    {
                    num &= -2;
                    }
                TaskbarSettings.SetState(num);
                }
            internal static void SetAlwaysOnTop(bool alwaysontop)
                {
                int num = TaskbarSettings.GetState();
                if (alwaysontop)
                    {
                    num |= 2;
                    }
                else
                    {
                    num &= -3;
                    }
                TaskbarSettings.SetState(num);
                }
            }




        // Get a handle to an application window.
        [DllImport("user32.dll", SetLastError = true)]
        internal static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        // Find window by Caption only. Note you must pass IntPtr.Zero as the first parameter.
        [DllImport("user32.dll", EntryPoint = "FindWindow", SetLastError = true)]
        internal static extern IntPtr FindWindowByCaption(IntPtr ZeroOnly, string lpWindowName);

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll")]
        internal static extern bool GetTitleBarInfo(IntPtr hwnd, ref TITLEBARINFO pti);

        //GetLastError- retrieves the last system error.
        [DllImport("coredll.dll", SetLastError = true)]
        internal static extern Int32 GetLastError();

        [StructLayout(LayoutKind.Sequential)]
        internal struct TITLEBARINFO
            {
            internal const int CCHILDREN_TITLEBAR = 5;
            internal uint cbSize; //Specifies the size, in bytes, of the structure. 
            //The caller must set this to sizeof(TITLEBARINFO).

            internal RECT rcTitleBar; //Pointer to a RECT structure that receives the 
            //coordinates of the title bar. These coordinates include all title-bar elements
            //except the window menu.

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]

            //Add reference for System.Windows.Forms
            internal AccessibleStates[] rgstate;
            //0	The title bar itself.
            //1	Reserved.
            //2	Minimize button.
            //3	Maximize button.
            //4	Help button.
            //5	Close button.
            }

        [StructLayout(LayoutKind.Sequential)]
        internal struct RECT
            {
            internal int left;
            internal int top;
            internal int right;
            internal int bottom;
            }
        ///The SetWindowLongPtr function changes an attribute of the specified window

        [DllImport("user32.dll", EntryPoint = "SetWindowLong")]
        internal static extern int SetWindowLong32(HandleRef hWnd, int nIndex, int dwNewLong);

        [DllImport("user32.dll", EntryPoint = "SetWindowLong")]
        internal static extern int SetWindowLong32(IntPtr windowHandle, Helpers.GWLParameter nIndex, int dwNewLong);

        [DllImport("user32.dll", EntryPoint = "SetWindowLongPtr")]
        internal static extern IntPtr SetWindowLongPtr64(IntPtr windowHandle, Helpers.GWLParameter nIndex, IntPtr dwNewLong);

        [DllImport("user32.dll", EntryPoint = "SetWindowLongPtr")]
        internal static extern IntPtr SetWindowLongPtr64(HandleRef hWnd, int nIndex, IntPtr dwNewLong);


        [DllImport("user32.dll")]
        internal static extern IntPtr SetWinEventHook(
            AccessibleEvents eventMin, //Specifies the event constant for the lowest event value in the range of events that are handled by the hook function. This parameter can be set to EVENT_MIN to indicate the lowest possible event value.
            AccessibleEvents eventMax, //Specifies the event constant for the highest event value in the range of events that are handled by the hook function. This parameter can be set to EVENT_MAX to indicate the highest possible event value.
            IntPtr eventHookAssemblyHandle, //Handle to the DLL that contains the hook function at lpfnWinEventProc, if the WINEVENT_INCONTEXT flag is specified in the dwFlags parameter. If the hook function is not located in a DLL, or if the WINEVENT_OUTOFCONTEXT flag is specified, this parameter is NULL.
            WinEventProc eventHookHandle, //Pointer to the event hook function. For more information about this function
            uint processId, //Specifies the ID of the process from which the hook function receives events. Specify zero (0) to receive events from all processes on the current desktop.
            uint threadId,//Specifies the ID of the thread from which the hook function receives events. If this parameter is zero, the hook function is associated with all existing threads on the current desktop.
            SetWinEventHookParameter parameterFlags //Flag values that specify the location of the hook function and of the events to be skipped. The following flags are valid:
            );

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll")]
        internal static extern bool UnhookWinEvent(IntPtr eventHookHandle);

        internal delegate void WinEventProc(IntPtr winEventHookHandle, AccessibleEvents accEvent, IntPtr windowHandle, int objectId, int childId, uint eventThreadId, uint eventTimeInMilliseconds);

        [DllImport("user32.dll")]
        internal static extern IntPtr SetFocus(IntPtr hWnd);

        [Flags]
        internal enum SetWinEventHookParameter
            {
            WINEVENT_INCONTEXT = 4,
            WINEVENT_OUTOFCONTEXT = 0,
            WINEVENT_SKIPOWNPROCESS = 2,
            WINEVENT_SKIPOWNTHREAD = 1

            }
        }


    internal static class MoniterInfo
        {
        internal enum MC_DISPLAY_TECHNOLOGY_TYPE
            {
            MC_SHADOW_MASK_CATHODE_RAY_TUBE,

            MC_APERTURE_GRILL_CATHODE_RAY_TUBE,

            MC_THIN_FILM_TRANSISTOR,

            MC_LIQUID_CRYSTAL_ON_SILICON,

            MC_PLASMA,

            MC_ORGANIC_LIGHT_EMITTING_DIODE,

            MC_ELECTROLUMINESCENT,

            MC_MICROELECTROMECHANICAL,

            MC_FIELD_EMISSION_DEVICE,
            }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        internal struct PHYSICAL_MONITOR
            {
            internal IntPtr hPhysicalMonitor;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            internal string szPhysicalMonitorDescription;
            }

        #region Imports

        [DllImport("user32.dll", EntryPoint = "MonitorFromWindow")]
        internal static extern IntPtr MonitorFromWindow([In] IntPtr hwnd, uint dwFlags);

        [DllImport("dxva2.dll", EntryPoint = "GetMonitorTechnologyType")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetMonitorTechnologyType(IntPtr hMonitor, ref MC_DISPLAY_TECHNOLOGY_TYPE pdtyDisplayTechnologyType);

        [DllImport("dxva2.dll", EntryPoint = "GetMonitorCapabilities")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetMonitorCapabilities(IntPtr hMonitor, ref uint pdwMonitorCapabilities, ref uint pdwSupportedColorTemperatures);

        [DllImport("dxva2.dll", EntryPoint = "DestroyPhysicalMonitors")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool DestroyPhysicalMonitors(uint dwPhysicalMonitorArraySize, ref PHYSICAL_MONITOR[] pPhysicalMonitorArray);

        [DllImport("dxva2.dll", EntryPoint = "GetNumberOfPhysicalMonitorsFromHMONITOR")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetNumberOfPhysicalMonitorsFromHMONITOR(IntPtr hMonitor, ref uint pdwNumberOfPhysicalMonitors);

        [DllImport("dxva2.dll", EntryPoint = "GetPhysicalMonitorsFromHMONITOR")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetPhysicalMonitorsFromHMONITOR(IntPtr hMonitor, uint dwPhysicalMonitorArraySize, [Out] PHYSICAL_MONITOR[] pPhysicalMonitorArray);

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        internal static extern int GetSystemMetrics(int nIndex);

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        private static extern UInt32 MonitorFromPoint(Point pt, UInt32 dwFlags);

        //[DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        //private static extern bool GetMonitorInfo(UInt32 monitorHandle, ref MonitorInfo mInfo);
        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        internal static extern bool GetMonitorInfo(IntPtr hmonitor, [In, Out]MONITORINFOEX info);
        [DllImport("User32.dll", ExactSpelling = true)]
        internal static extern IntPtr MonitorFromPoint(POINTSTRUCT pt, int flags);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto, Pack = 4)]
        internal class MONITORINFOEX
            {
            internal int cbSize = Marshal.SizeOf(typeof(MONITORINFOEX));
            internal RECT rcMonitor = new RECT();
            internal RECT rcWork = new RECT();
            internal int dwFlags = 0;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            internal char[] szDevice = new char[32];
            }

        [StructLayout(LayoutKind.Sequential)]
        internal struct POINTSTRUCT
            {
            internal int x;
            internal int y;
            internal POINTSTRUCT(int x, int y)
                {
                this.x = x;
                this.y = y;
                }
            }

        [StructLayout(LayoutKind.Sequential)]
        internal struct RECT
            {
            internal int left;
            internal int top;
            internal int right;
            internal int bottom;
            }


        #endregion



        static internal MONITORINFOEX GetDesktopBoundsForWindow(IntPtr handle)
            {

            IntPtr ptr = MonitorFromWindow(handle, (uint)Enums.MonitorFromWindow_Flags.MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX m = new MONITORINFOEX();
            GetMonitorInfo(ptr, m);
            return m;

            }



        }
    }
