﻿#pragma checksum "..\..\Window1.xaml" "{406ea660-64cf-4c82-b6f0-42d48172a799}" "89844BD0CA1F116960307638D1E2AFC3"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.1433
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

using SimpleWPFChart;
using System;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;
using System.Windows.Navigation;
using System.Windows.Shapes;


namespace SimpleWPFChart {
    
    
    /// <summary>
    /// Window1
    /// </summary>
    public partial class Window1 : System.Windows.Window, System.Windows.Markup.IComponentConnector {
        
        
        #line 19 "..\..\Window1.xaml"
        internal SimpleWPFChart.UCChartCurveGraph ucGraph;
        
        #line default
        #line hidden
        
        
        #line 25 "..\..\Window1.xaml"
        internal SimpleWPFChart.UCChartCurveGraph ucGraph4;
        
        #line default
        #line hidden
        
        
        #line 39 "..\..\Window1.xaml"
        internal SimpleWPFChart.UCChartCurveGraph ucGraph2;
        
        #line default
        #line hidden
        
        
        #line 46 "..\..\Window1.xaml"
        internal SimpleWPFChart.UCChartCurveGraph ucGraph3;
        
        #line default
        #line hidden
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Uri resourceLocater = new System.Uri("/SimpleWPFChart;component/window1.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\Window1.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal System.Delegate _CreateDelegate(System.Type delegateType, string handler) {
            return System.Delegate.CreateDelegate(delegateType, this, handler);
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            this.ucGraph = ((SimpleWPFChart.UCChartCurveGraph)(target));
            return;
            case 2:
            this.ucGraph4 = ((SimpleWPFChart.UCChartCurveGraph)(target));
            return;
            case 3:
            this.ucGraph2 = ((SimpleWPFChart.UCChartCurveGraph)(target));
            return;
            case 4:
            this.ucGraph3 = ((SimpleWPFChart.UCChartCurveGraph)(target));
            return;
            }
            this._contentLoaded = true;
        }
    }
}