﻿#pragma checksum "..\..\UCChartCurveGraph.xaml" "{406ea660-64cf-4c82-b6f0-42d48172a799}" "7DE5360CDB0BB774EFE2D2811FF198A5"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.1433
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

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
    /// UCChartCurveGraph
    /// </summary>
    public partial class UCChartCurveGraph : System.Windows.Controls.Grid, System.Windows.Markup.IComponentConnector {
        
        
        #line 5 "..\..\UCChartCurveGraph.xaml"
        internal SimpleWPFChart.UCChartCurveGraph gridUC;
        
        #line default
        #line hidden
        
        
        #line 34 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Border borderUC;
        
        #line default
        #line hidden
        
        
        #line 37 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Label labelTitle;
        
        #line default
        #line hidden
        
        
        #line 39 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Border borderFrame;
        
        #line default
        #line hidden
        
        
        #line 53 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.TextBlock textblockYAxis;
        
        #line default
        #line hidden
        
        
        #line 60 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.DockPanel dockpanelYTicks;
        
        #line default
        #line hidden
        
        
        #line 61 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Label labelMinYValue;
        
        #line default
        #line hidden
        
        
        #line 62 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Label labelMaxYValue;
        
        #line default
        #line hidden
        
        
        #line 72 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Canvas borderChart;
        
        #line default
        #line hidden
        
        
        #line 75 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.DockPanel dockpanelXTicks;
        
        #line default
        #line hidden
        
        
        #line 76 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Label labelMinXValue;
        
        #line default
        #line hidden
        
        
        #line 77 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.Label labelMaxXValue;
        
        #line default
        #line hidden
        
        
        #line 80 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.TextBlock textblockXAxis;
        
        #line default
        #line hidden
        
        
        #line 82 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.StackPanel stackpanelLegendTable;
        
        #line default
        #line hidden
        
        
        #line 84 "..\..\UCChartCurveGraph.xaml"
        internal System.Windows.Controls.StackPanel stackpanelLegend;
        
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
            System.Uri resourceLocater = new System.Uri("/SimpleWPFChart;component/ucchartcurvegraph.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\UCChartCurveGraph.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            this.gridUC = ((SimpleWPFChart.UCChartCurveGraph)(target));
            
            #line 4 "..\..\UCChartCurveGraph.xaml"
            this.gridUC.Loaded += new System.Windows.RoutedEventHandler(this.UserControl_Loaded);
            
            #line default
            #line hidden
            return;
            case 2:
            this.borderUC = ((System.Windows.Controls.Border)(target));
            return;
            case 3:
            this.labelTitle = ((System.Windows.Controls.Label)(target));
            return;
            case 4:
            this.borderFrame = ((System.Windows.Controls.Border)(target));
            return;
            case 5:
            this.textblockYAxis = ((System.Windows.Controls.TextBlock)(target));
            return;
            case 6:
            this.dockpanelYTicks = ((System.Windows.Controls.DockPanel)(target));
            return;
            case 7:
            this.labelMinYValue = ((System.Windows.Controls.Label)(target));
            return;
            case 8:
            this.labelMaxYValue = ((System.Windows.Controls.Label)(target));
            return;
            case 9:
            this.borderChart = ((System.Windows.Controls.Canvas)(target));
            return;
            case 10:
            this.dockpanelXTicks = ((System.Windows.Controls.DockPanel)(target));
            return;
            case 11:
            this.labelMinXValue = ((System.Windows.Controls.Label)(target));
            return;
            case 12:
            this.labelMaxXValue = ((System.Windows.Controls.Label)(target));
            return;
            case 13:
            this.textblockXAxis = ((System.Windows.Controls.TextBlock)(target));
            return;
            case 14:
            this.stackpanelLegendTable = ((System.Windows.Controls.StackPanel)(target));
            return;
            case 15:
            this.stackpanelLegend = ((System.Windows.Controls.StackPanel)(target));
            return;
            }
            this._contentLoaded = true;
        }
    }
}
