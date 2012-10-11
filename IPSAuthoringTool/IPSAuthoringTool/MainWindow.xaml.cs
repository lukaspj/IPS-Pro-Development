using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MahApps.Metro.Controls;
using System.Collections.ObjectModel;
using IPSAuthoringTool.Utility;
using WinCharts = System.Windows.Forms.DataVisualization.Charting;

namespace IPSAuthoringTool
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        public ObservableCollection<PanoramaGroup> EffectGroups { get; set; }
        public ObservableCollection<PanoramaGroup> EmitterGroups { get; set; }
        public ObservableCollection<PanoramaGroup> ValueGroups { get; set; }

        PanoramaGroup panEffects;
        PanoramaGroup panEmitters;
        PanoramaGroup valEffects;

        List<Grid> EffectPanels;
        List<Grid> EmitterPanels;
        List<Grid> ValuePanels;

        Emitter emitter;
        List<ParticleEffect> Effects;
        ParticleEffect Effect;
        Emitter.value Value;
        

        public MainWindow()
        {
            InitializeComponent();
            // EffectsPanorama binding
            panEffects = new PanoramaGroup("Effects");
            EffectGroups = new ObservableCollection<PanoramaGroup> { panEffects };
            EffectPanels = new List<Grid>();
            panEffects.SetSource(EffectPanels);
            EffectPan.ItemsSource = EffectGroups;

            // ValuesPanorama binding
            valEffects = new PanoramaGroup("Values");
            ValueGroups = new ObservableCollection<PanoramaGroup> { valEffects };
            ValuePanels = new List<Grid>();
            valEffects.SetSource(ValuePanels);
            //ValuesPan.ItemsSource = ValueGroups;

            // EmittersPanorama binding
            panEmitters = new PanoramaGroup("Emitters");
            EmitterGroups = new ObservableCollection<PanoramaGroup> { panEmitters };
            EmitterPanels = new List<Grid>();
            panEmitters.SetSource(EmitterPanels);
            EmitterPan.ItemsSource = EmitterGroups;
            Effects = ParticleEffect.loadLatestFile();
            reloadEffects();
            reloadEmitters();
        }

        #region EmitterWindow

        private void reloadEmitters()
        {
            EmitterPanels.Clear();
            if (Effect != null)
            {
                for (int i = 0; i < Effect.Emitters.Count; i++)
                {
                    Grid p = new Grid();
                    p.Width = 160;
                    p.Height = 160;
                    Image img = new Image();
                    img.Source = new BitmapImage( new Uri("/Icons/" + Effect.Emitters[i].ToString() + ".png", UriKind.Relative) );
                    img.Width = 160;
                    img.Height = 160;
                    img.Stretch = Stretch.Fill;
                    img.Tag = i;
                    img.PreviewMouseDown += Emitter_Click;
                    RenderOptions.SetBitmapScalingMode(img, BitmapScalingMode.Fant);
                    p.Children.Add(img);
                    EmitterPanels.Add(p);
                }
            }
        }

        void Emitter_Click(object sender, EventArgs e)
        {
            emitter = Effect.Emitters[(int)((Image)sender).Tag];
            selEmitterTab.IsEnabled = true;
            selEmitterTab.Header = emitter.ToString();
            selValueTab.Header = "Value";
            selValueTab.IsEnabled = false;

            valStackPanel.Children.Clear();
            if (emitter != null)
            {
                for (int i = 0; i < emitter.Values.Count; i++)
                {
                    Emitter.value val = emitter.Values[i];
                    Grid p = new Grid();
                    p.Height = 255;
                    p.Width = 555;
                    //List<double> Points = new List<double>();
                    ObservableCollection<double> Points = new ObservableCollection<double>();
                    ComputeGraph(Points,i);
                    //Chart.ChartCurve cc = new Chart.ChartCurve(Brushes.Red, Points.ToArray(), Chart.ChartLineType.PolylineType, "xOffset");
                    Chart.UCChartCurveGraph ucGraph = new Chart.UCChartCurveGraph();
                    ucGraph.Tag = i;
                    ucGraph.PreviewMouseDown += new MouseButtonEventHandler(ucGraph_PreviewMouseDown);
                    ObservableCollection<Chart.ChartRequestInfo> ri = new ObservableCollection<Chart.ChartRequestInfo>();
                    ri.Add(new Chart.ChartRequestInfo(Brushes.Red, Points, Chart.ChartLineType.PolylineType, val.valueName));
                    ucGraph.RequestData = ri;
                    ucGraph.ShowGridLines = true;
                    ucGraph.GraphTitle = val.valueName;
                    ucGraph.ShowTitle = true;
                    ucGraph.ShowXTicks = true;
                    ucGraph.ShowYTicks = true;
                    ucGraph.ShowXYLabel = true;
                    ucGraph.Height = 255;
                    ucGraph.Width = 555;
                    p.Children.Add(ucGraph);
                    valStackPanel.Children.Add(p);
                }
            }
        }

        void ucGraph_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            Value = emitter.Values[(int)((Chart.UCChartCurveGraph)sender).Tag];
            selValueTab.Header = Value.valueName;
            selValueTab.IsEnabled = true;
        }

        #endregion

        #region EffectTab
        private void reloadEffects()
        {
            EffectPanels.Clear();
            for (int i = 0; i < Effects.Count; i++)
            {
                Grid p = new Grid();
                p.Width = 128;
                p.Height = 48;
                Label title = new Label();
                title.Content = Effects[i].name;
                title.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Center;
                title.FontSize = 24;
                title.Width = 128;
                title.PreviewMouseDown += Effect_Click;
                //title.Name = "" + i;
                title.Tag = i;
                p.Children.Add(title);
                EffectPanels.Add(p);
            }
        }

        void Effect_Click(object sender, EventArgs e)
        {
            Effect = Effects[(int)((Control)sender).Tag];
            EmittersTab.IsEnabled = true;
            selEmitterTab.Header = "EmitterType";
            selValueTab.Header = "Value";
            selEmitterTab.IsEnabled = false;
            selValueTab.IsEnabled = false;
            reloadEffects();
            reloadEmitters();
        }
        #endregion

        #region GraphComputing

        protected void ComputeGraph(ObservableCollection<double> Points, int idx)
        {
            int[] relPoints = GetRelevantPointIndexes(0, idx);
            for (int i = 0; i < 1000; i++)
            {
                int[] newPoints = GetRelevantPointIndexes(i, idx);
                Points.Add(GetY(i, emitter.Values[idx].points[newPoints[0]], emitter.Values[idx].points[newPoints[1]], idx));
                //Points.Add(new Point(i, GetY(i, emitter.Values[idx].points[newPoints[0]], emitter.Values[idx].points[newPoints[1]], idx)));
            }
        }

        private int GetY(int x, Emitter.PointOnValue p1, Emitter.PointOnValue p3, int idx)
        {
            return (int)Utility.Tweener.EaseFromString(x - p1.point.X, p1.point.Y, p3.point.Y - p1.point.Y, p3.point.X - p1.point.X, p3.Easing, p3.EaseIn, p3.EaseOut);
        }

        private int[] GetRelevantPointIndexes(int x, int id)
        {
            int[] retArr = new int[2];
            int p1 = 0;
            int p2 = emitter.Values[id].points.Count - 1;
            for (int i = 0; i < emitter.Values[id].points.Count; i++)
            {
                Point p = emitter.Values[id].points[i].point;
                Point pp1 = emitter.Values[id].points[p1].point;
                Point pp2 = emitter.Values[id].points[p2].point;
                if (p.X - x >= pp1.X - x && p.X - x <= 0)
                    p1 = i;
                if (p.X - x < pp2.X - x && p.X - x > 0)
                    p2 = i;

            }
            retArr[0] = p1;
            retArr[1] = p2;
            return retArr;
        }

        private void applyVisualStyles(WinCharts.Series serie)
        {
            serie.ChartType = WinCharts.SeriesChartType.Line;
            serie.XValueType = WinCharts.ChartValueType.Int32;
            serie.YValueType = WinCharts.ChartValueType.Int32;
        }

        #endregion

        #region Tabs

        private void EffectsTab_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (Effect != null)
                EmittersTab.IsEnabled = true;
            else
                EmittersTab.IsEnabled = false;

            if (emitter != null)
                selEmitterTab.IsEnabled = true;
            else
                selEmitterTab.IsEnabled = false;

            if (Value.valueName != null)
                selValueTab.IsEnabled = true;
            else
                selValueTab.IsEnabled = false;
        }

        private void EmittersTab_MouseDown(object sender, MouseButtonEventArgs e)
        {
            selValueTab.IsEnabled = false;
            if (emitter != null)
                selEmitterTab.IsEnabled = true;
            else
                selEmitterTab.IsEnabled = false;

            if (Value.valueName != null)
                selValueTab.IsEnabled = true;
            else
                selValueTab.IsEnabled = false;
        }

        private void selEmitterTab_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (Value.valueName != null)
                selValueTab.IsEnabled = true;
            else
                selValueTab.IsEnabled = false;
        }

        private void selValueTab_MouseDown(object sender, MouseButtonEventArgs e)
        {

        }

        #endregion

    }
}
