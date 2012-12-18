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
using WinterLeaf;
using System.Diagnostics;
using System.Threading;
using System.ComponentModel;
using System.IO;

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

        ObservableCollection<Grid> EffectPanels;
        ObservableCollection<Grid> EmitterPanels;
        ObservableCollection<Grid> ValuePanels;

        Dictionary<string, List<string>> datablocks;

        Emitter emitter;
        List<ParticleEffect> Effects;
        ParticleEffect Effect;
        Emitter.value Value;
        TorqueController TC;
        dnTorque dnt;

        public MainWindow()
        {
            datablocks = new Dictionary<string, List<string>>();
            List<string> EmitterDatablocks = null;
            if (File.Exists("datablocks.cache"))
                EmitterDatablocks = IOHandler.readStringDataFile("datablocks.cache");
            if (EmitterDatablocks != null)
            {
                foreach (string s in EmitterDatablocks)
                {
                    string key = s.Split('|')[0];
                    if (!datablocks.Keys.Contains(key))
                        datablocks.Add(key, new List<string>());
                    datablocks[key].Add(s.Split('|')[1]);
                }
            }
            dnt = new dnTorque(Process.GetCurrentProcess().Handle);

            dnt.onShutDown += new dnTorque.ShutDownEventHandler(dnt_onShutDown);

            using (var bwr = new System.ComponentModel.BackgroundWorker())
            {
                bwr.DoWork += bwr_DoWork;
                bwr.RunWorkerAsync();
            }
            
            InitializeComponent();

            // EffectsPanorama binding
            panEffects = new PanoramaGroup("Effects");
            EffectGroups = new ObservableCollection<PanoramaGroup> { panEffects };
            EffectPanels = new ObservableCollection<Grid>();
            panEffects.SetSource(EffectPanels);
            EffectPan.ItemsSource = EffectGroups;

            // ValuesPanorama binding
            valEffects = new PanoramaGroup("Values");
            ValueGroups = new ObservableCollection<PanoramaGroup> { valEffects };
            ValuePanels = new ObservableCollection<Grid>();
            valEffects.SetSource(ValuePanels);
            ValuesPan.ItemsSource = ValueGroups;

            // EmittersPanorama binding
            panEmitters = new PanoramaGroup("Emitters");
            EmitterGroups = new ObservableCollection<PanoramaGroup> { panEmitters };
            EmitterPanels = new ObservableCollection<Grid>();
            panEmitters.SetSource(EmitterPanels);
            EmitterPan.ItemsSource = EmitterGroups;
            Effects = ParticleEffect.loadLatestFile();
            reloadEffects();
            reloadEmitters();
            ItemInputControl.SetParent(ModalDialogParent);
            StringInputControl.SetParent(ModalDialogParent);
            VectorInputControl.SetParent(ModalDialogParent);
            TimeInputControl.SetParent(ModalDialogParent);
            AlertControl.SetParent(ModalDialogParent);
        }

        void bwr_DoWork(object sender, DoWorkEventArgs e)
        {
            TC = new TorqueController(ref dnt);
        }

        
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
            reloadEmitters();
        }


        #region buttonClicks
        private void CreateEffectButton_Click(object sender, RoutedEventArgs e)
        {
            string s = StringInputControl.ShowHandlerDialog("Effect name:");
            if (s == null) {
                AlertControl.ShowHandlerDialog("No name given...\nEffect not created.");
                return;
            }
            ParticleEffect newEff = new ParticleEffect();
            newEff.name = s;
            Effects.Add(newEff);
            reloadEffects();
        }
        private void LoadEffectButton_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog OFD = new System.Windows.Forms.OpenFileDialog();
            if (OFD.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                ParticleEffect newEff = new ParticleEffect();
                newEff.ReadFromFile(OFD.FileName, AlertControl);
                Effects.Add(newEff);
                reloadEffects();
            }
        }
        private void DeleteEffectButton_Click(object sender, RoutedEventArgs e)
        {
            if (Effect != null)
            {
                Effects.Remove(Effect);
                Effect = null;
                reloadEffects();
                reloadEmitters();
            }
        }
        private void SaveEffectButton_Click(object sender, RoutedEventArgs e)
        {
            if (Effect != null)
            {
                IOHandler.writeDatablockScript(Effects);
                IOHandler.WriteAndExec(TC.getUtil(), Effect, AlertControl);
                reloadEffects();
            }
        }
        #endregion

        #endregion

        #region EmitterTab

        private void reloadEmitters()
        {
            EmitterPanels.Clear();
            if (Effect != null)
            {
                for (int i = 0; i < Effect.Emitters.Count; i++)
                {
                    Grid p = new Grid();
                    p.Width = 140;
                    p.Height = 140;
                    Image img = new Image();
                    img.Source = new BitmapImage(new Uri("Icons/" + Effect.Emitters[i].ToString() + ".png", UriKind.Relative));
                    img.Width = 140;
                    img.Height = 140;
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
            Emitter oldEmitter = emitter;
            emitter = Effect.Emitters[(int)((Image)sender).Tag];
            if (emitter != null)
            {
                EmitterDatablockComboBox.Items.Clear();
                NodeDatablockComboBox.Items.Clear();
                string datablockType = emitter.getDatablockType();
                datablocks[datablockType].ForEach(x => EmitterDatablockComboBox.Items.Add(x));
                string nodeDatablockType = emitter.getNodeDatablockType();
                datablocks[nodeDatablockType].ForEach(x => NodeDatablockComboBox.Items.Add(x));
                EmitterDatablockComboBox.SelectedIndex = datablocks[datablockType].IndexOf(emitter.emitter);
                NodeDatablockComboBox.SelectedIndex = datablocks[nodeDatablockType].IndexOf(emitter.datablock);

                xControl.Text = emitter.x.ToString();
                yControl.Text = emitter.y.ToString();
                zControl.Text = emitter.z.ToString();
            }

            selEmitterTab.IsEnabled = true;
            selEmitterTab.Header = emitter.ToString();
            selValueTab.Header = "Value";
            selValueTab.IsEnabled = false;
            reloadValues();
        }

        #region buttonClicks
        private void CreateEmitterButton_Click(object sender, RoutedEventArgs e)
        {
            object o = ItemInputControl.ShowHandlerDialog("Please choose emitter type:", getEmitterLabels());
            Emitter emi = new Emitter();
            emi.Type = Emitter.stringToEnum((string)o);
            if (emi.Type == Emitter.EmitterType.Error)
                return;

            List<string> EmitterDatablocks = datablocks[emi.getDatablockType()];
            o = ItemInputControl.ShowHandlerDialog("Please choose emitter datablock:", EmitterDatablocks.Cast<object>().ToList());
            if (o != null)
                emi.emitter = o.ToString();
            else
                return;

            List<string> NodeDatablocks = datablocks[emi.getNodeDatablockType()];
            o = ItemInputControl.ShowHandlerDialog("Please choose node datablock:", NodeDatablocks.Cast<object>().ToList());
            if (o != null)
                emi.datablock = o.ToString();
            else
                return;

            float[] timeRange = TimeInputControl.ShowHandlerDialog("Time range:");
            if (timeRange != null)
            {
                emi.Start = timeRange[0] / 1000;
                emi.End = timeRange[1] / 1000;
            }
            else
                return;

            float[] relPos = VectorInputControl.ShowHandlerDialog("Relative position: ");
            if (relPos != null)
            {
                emi.x = relPos[0];
                emi.y = relPos[1];
                emi.z = relPos[2];
            }
            else
                return;

            Effect.Emitters.Add(emi);
            reloadEmitters();
        }
        private void DeleteEmitterButton_Click(object sender, RoutedEventArgs e)
        {
            Effect.Emitters.Remove(emitter);
            emitter = null;
            selEmitterTab.IsEnabled = false;
            selEmitterTab.Header = "EmitterType";
            reloadEmitters();
        }
        private void SaveEmitterButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Save emitter dialog is missing");
        }

        #endregion

        private List<object> getEmitterLabels()
        {
            return new List<object>() { "StockEmitter", "GraphEmitter", "GroundEmitter", "MaskEmitter" };
        }

        #endregion

        #region ValuesTab
        #region ValuesPanorama
        private void reloadValues()
        {
            ValuePanels.Clear();
            if (emitter != null)
            {
                for (int i = 0; i < emitter.Values.Count; i++)
                {
                    Emitter.value val = emitter.Values[i];
                    Grid p = new Grid();
                    p.Height = 180;
                    p.Width = 180;
                    p.Tag = i;
                    p.PreviewMouseDown += new MouseButtonEventHandler(valueGrid_PreviewMouseDown);
                    if (val.Ease == true)
                    {
                        ObservableCollection<double> Points = new ObservableCollection<double>();
                        ComputeGraph(Points, i);
                        Chart.UCChartCurveGraph ucGraph = new Chart.UCChartCurveGraph();
                        ObservableCollection<Chart.ChartRequestInfo> ri = new ObservableCollection<Chart.ChartRequestInfo>();
                        ri.Add(new Chart.ChartRequestInfo(Brushes.Red, Points, Chart.ChartLineType.PolylineType, val.valueName));
                        ucGraph.RequestData = ri;
                        ucGraph.ShowGridLines = true;
                        ucGraph.GraphTitle = val.valueName;
                        ucGraph.ShowTitle = true;
                        ucGraph.ShowXTicks = false;
                        ucGraph.ShowYTicks = false;
                        ucGraph.Height = 180;
                        ucGraph.Width = 180;
                        p.Children.Add(ucGraph);
                    }
                    else
                    {
                        Label l = new Label();
                        l.Content = val.valueName;
                        l.Tag = i;
                        l.PreviewMouseDown += LabelValue_PreviewMouseDown;
                        l.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Center;
                        l.FontSize = 24;
                        l.VerticalContentAlignment = System.Windows.VerticalAlignment.Center;
                        p.Children.Add(l);
                    }
                    ValuePanels.Add(p);
                }
            }
        }

        void valueGrid_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            Value = emitter.Values[(int)((Grid)sender).Tag];
            selValueTab.Header = Value.valueName;
            selValueTab.IsEnabled = true;
            ReloadValueTabContent();
        }

        void LabelValue_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            Value = emitter.Values[(int)((Label)sender).Tag];
            selValueTab.Header = Value.valueName;
            selValueTab.IsEnabled = true;
            ReloadValueTabContent();
        }
        #endregion


        #region ButtonClicks

        private void AddValueButton_Click(object sender, RoutedEventArgs e)
        {
            List<string> fields = Emitter.getFields(emitter.Type);
            object o = ItemInputControl.ShowHandlerDialog("Choose value:", getFieldList());
            if (o != null)
            {
                Emitter.value val = new Emitter.value();
                val.valueName = fields.ElementAt((int)((Control)o).Tag);
                emitter.Values.Add(val);
                reloadValues();
            }
        }

        private void RemoveValueButton_Click(object sender, RoutedEventArgs e)
        {
            if ((Value.Ease == false && Value.setTime != null) || ( Value.Ease == true && Value.points.Count > 0 ))
            {
                emitter.Values.Remove(Value);
                reloadValues();
                selValueTab.IsEnabled = false;
                selValueTab.Header = "Value";
            }
        }

        private void SaveValueButton_Click(object sender, RoutedEventArgs e)
        {

        }

        #endregion

        private List<object> getFieldList()
        {
            List<string> strings = Emitter.getFields(emitter.Type);
            List<object> ret = new List<object>();
            int i = 0;
            foreach (string s in strings)
            {
                Label l = new Label();
                l.Content = s;
                l.Tag = i;
                i++;
                ret.Add(l);
            }
            return ret;
        }

        private List<object> getEaseMethods()
        {
            List<object> ret = new List<object>();
            List<string> strs = Tweener.GetEasingMethodNames();
            foreach (string s in strs)
                ret.Add((object)s);
            return ret;
        }

        #endregion

        #region EditValueTab
        #region Pointlist and resets
        public void ReloadValueTabContent()
        {
            ResetValueComboBox();
            if (Value.Ease)
                EaseCheckBox.IsChecked = true;
            else
                EaseCheckBox.IsChecked = false;
            DeltaTextBox.Text = Value.deltaValue.ToString();
            reloadPoints();
        }

        public void ResetValueComboBox()
        {
            ValueTypeBox.Items.Clear();
            List<object> objs = getFieldList();
            foreach(object o in objs)
                ValueTypeBox.Items.Add(o);
            foreach (object o in ValueTypeBox.Items)
            {
                if (((Label)o).Content.ToString() == Value.valueName)
                    ValueTypeBox.SelectedItem = o;
            }
        }

        public void reloadPoints()
        {
            PointList.Items.Clear();
            GraphContainer.Children.Clear();
            if (!Value.Ease)
                return;
            foreach (Emitter.PointOnValue p in Value.points)
            {
                Grid g = new Grid();
                ColumnDefinition CD1 = new ColumnDefinition();
                CD1.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD2 = new ColumnDefinition();
                CD2.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD3 = new ColumnDefinition();
                CD3.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD4 = new ColumnDefinition();
                CD4.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD5 = new ColumnDefinition();
                CD5.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD6 = new ColumnDefinition();
                CD6.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD7 = new ColumnDefinition();
                CD7.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD8 = new ColumnDefinition();
                CD8.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD9 = new ColumnDefinition();
                CD9.Width = new GridLength(2, GridUnitType.Auto);
                ColumnDefinition CD10 = new ColumnDefinition();
                CD10.Width = new GridLength(2, GridUnitType.Auto);
                g.ColumnDefinitions.Add(CD1);
                g.ColumnDefinitions.Add(CD2);
                g.ColumnDefinitions.Add(CD3);
                g.ColumnDefinitions.Add(CD4);
                g.ColumnDefinitions.Add(CD5);
                g.ColumnDefinitions.Add(CD6);
                g.ColumnDefinitions.Add(CD7);
                g.ColumnDefinitions.Add(CD8);
                g.ColumnDefinitions.Add(CD9);
                g.ColumnDefinitions.Add(CD10);
                g.Height = 28;
                g.Margin = new Thickness(0, 5, 0, 5);

                Label xLBL = new Label();
                xLBL.Content = "X:";
                Grid.SetColumn(xLBL, 0);
                g.Children.Add(xLBL);

                TextBox xTB = new TextBox();
                xTB.Text = p.point.X.ToString();
                xTB.Width = 36;
                xTB.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                xTB.TextChanged += Point_Value_TextChanged;
                Grid.SetColumn(xTB, 1);
                g.Children.Add(xTB);

                Label yLBL = new Label();
                yLBL.Content = "Y:";
                Grid.SetColumn(yLBL, 2);
                g.Children.Add(yLBL);

                TextBox yTB = new TextBox();
                yTB.Text = p.point.Y.ToString();
                yTB.Width = 36;
                yTB.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                yTB.TextChanged += Point_Value_TextChanged;
                Grid.SetColumn(yTB, 3);
                g.Children.Add(yTB);

                Label easeLBL = new Label();
                easeLBL.Content = "Easing:";
                Grid.SetColumn(easeLBL, 4);
                g.Children.Add(easeLBL);

                ComboBox easeTB = new ComboBox();
                easeTB.Text = p.point.Y.ToString();
                easeTB.Width = 100;
                easeTB.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                Grid.SetColumn(easeTB, 5);
                g.Children.Add(easeTB);

                List<object> objs = getEaseMethods();
                foreach (object o in objs)
                {
                    easeTB.Items.Add(o);
                    if ((string)o == p.Easing)
                        easeTB.SelectedItem = o;
                }
                easeTB.SelectionChanged += easeTB_SelectionChanged;

                Label easeInLBL = new Label();
                easeInLBL.Content = "Ease in:";
                Grid.SetColumn(easeInLBL, 6);
                g.Children.Add(easeInLBL);

                CheckBox eInCB = new CheckBox();
                eInCB.IsChecked = p.EaseIn;
                eInCB.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                eInCB.Width = 18;
                eInCB.Checked += Point_CheckedChange;
                eInCB.Unchecked += Point_CheckedChange;
                Grid.SetColumn(eInCB, 7);
                g.Children.Add(eInCB);

                Label easeOutLBL = new Label();
                easeOutLBL.Content = "Ease out:";
                Grid.SetColumn(easeOutLBL, 8);
                g.Children.Add(easeOutLBL);

                CheckBox eOutCB = new CheckBox();
                eOutCB.IsChecked = p.EaseOut;
                eOutCB.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                eOutCB.Width = 18;
                eOutCB.Checked += Point_CheckedChange;
                eOutCB.Unchecked += Point_CheckedChange;
                Grid.SetColumn(eOutCB, 9);
                g.Children.Add(eOutCB);

                PointList.Items.Add(g);
            }
            updateGraphContainer();
        }

        void Point_CheckedChange(object sender, RoutedEventArgs e)
        {
            getDataFromPointList();
        }

        void easeTB_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            getDataFromPointList();
        }

        private void Point_Value_TextChanged(object sender, TextChangedEventArgs e)
        {
            getDataFromPointList();
        }

        private void getDataFromPointList()
        {
            Value.points.Clear();
            foreach (Grid g in PointList.Items)
            {
                Emitter.PointOnValue point = new Emitter.PointOnValue();
                if (((TextBox)g.Children[1]).Text != "")
                    point.point.X = float.Parse(((TextBox)g.Children[1]).Text);
                else
                    point.point.Y = 0.0f;

                if (((TextBox)g.Children[3]).Text != "")
                    point.point.Y = float.Parse(((TextBox)g.Children[3]).Text);
                else
                    point.point.Y = 0.0f;

                if (((ComboBox)g.Children[5]).SelectedItem != null)
                    point.Easing = (string)((ComboBox)g.Children[5]).SelectedItem;
                else
                    point.Easing = "";
                point.EaseIn = ((CheckBox)g.Children[7]).IsChecked.Value;
                point.EaseOut = ((CheckBox)g.Children[9]).IsChecked.Value;
                Value.points.Add(point);
            }
            Value.points.Sort(new Emitter.PointXSorter());
            updateGraphContainer();
        }

        private void updateGraphContainer()
        {
            GraphContainer.Children.Clear();
            ObservableCollection<double> Points = new ObservableCollection<double>();
            ComputeGraph(Points, emitter.Values.IndexOf(Value));
            Chart.UCChartCurveGraph ucGraph = new Chart.UCChartCurveGraph();
            ObservableCollection<Chart.ChartRequestInfo> ri = new ObservableCollection<Chart.ChartRequestInfo>();
            ri.Add(new Chart.ChartRequestInfo(Brushes.Red, Points, Chart.ChartLineType.PolylineType, Value.valueName));
            ucGraph.RequestData = ri;
            ucGraph.ShowGridLines = true;
            ucGraph.GraphTitle = Value.valueName;
            ucGraph.ShowTitle = true;
            ucGraph.ShowXTicks = false;
            ucGraph.ShowYTicks = false;
            ucGraph.Height = 164;
            ucGraph.Width = 164;
            GraphContainer.Children.Add(ucGraph);
        }
        #endregion

        #region ValueChanges
        private void EaseCheckBox_CheckChange(object sender, RoutedEventArgs e)
        {
            if (EaseCheckBox.IsChecked.HasValue)
                Value.Ease = (bool)EaseCheckBox.IsChecked;
            else
                Value.Ease = false;

            if (Value.points.Count < 2)
            {
                Value.points.Add(new Emitter.PointOnValue());
                Value.points.Add(new Emitter.PointOnValue());
                Value.points[0].point.X = 0;
                Value.points[0].point.Y = 0;
                Value.points[1].point.X = 1;
                Value.points[1].point.Y = 1;
            }
            reloadPoints();
        }


        private void AddPointButton_Click(object sender, RoutedEventArgs e)
        {
            Emitter.PointOnValue POV = new Emitter.PointOnValue();
            POV.point.X = 0.5f;
            POV.point.Y = 0.5f;
            Value.points.Add(POV);
            Value.points.Sort(new Emitter.PointXSorter());
            reloadPoints();
        }

        private void DeltaTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            string Text = ((TextBox)sender).Text;
            float result;
            if (Value != null && Text != "" && Text != null && float.TryParse(Text, out result))
                Value.deltaValue = result;
        }

        private void SetTimeSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {   
            SetTimeLabel.Content = String.Format("Set value at {0:0.000}", SetTimeSlider.Value);
            if(Value != null)
                Value.setTime = (float)SetTimeSlider.Value;
        }

        private void ValueTypeBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count <= 0)
                return;
            string Text = ((Label)e.AddedItems[0]).Content.ToString();
            if (Value != null && Text != "" && Text != null)
                Value.valueName = Text;
        }
        #endregion
        #endregion

        #region GraphComputing

        protected void ComputeGraph(ObservableCollection<double> Points, int idx)
        {
            int[] relPoints = GetRelevantPointIndexes(0, idx);
            for (int i = 0; i < 1000; i++)
            {
                int[] newPoints = GetRelevantPointIndexes((float)i/1000, idx);
                if (newPoints[0] == newPoints[1])
                    Points.Add(emitter.Values[idx].points[newPoints[0]].point.Y * 1000);
                else
                    Points.Add(GetY(i, emitter.Values[idx].points[newPoints[0]], emitter.Values[idx].points[newPoints[1]], idx));
                //Points.Add(new Point(i, GetY(i, emitter.Values[idx].points[newPoints[0]], emitter.Values[idx].points[newPoints[1]], idx)));
            }
        }

        private int GetY(int x, Emitter.PointOnValue p1, Emitter.PointOnValue p3, int idx)
        {
            double ease = Utility.Tweener.EaseFromString((double)x / 1000.0f - p1.point.X, p1.point.Y, p3.point.Y - p1.point.Y, p3.point.X - p1.point.X, p3.Easing, p3.EaseIn, p3.EaseOut);
            int ret = (int)(ease * 1000.0f);
            int I;
            if(ret > 1000)
                I = 0;
            return ret;
        }

        private int[] GetRelevantPointIndexes(float x, int id)
        {
            if (emitter.Values[id].points.Count < 2)
            {
                emitter.Values[id].points.Add(new Emitter.PointOnValue());
                emitter.Values[id].points.Add(new Emitter.PointOnValue());
                emitter.Values[id].points[0].point.X = 0;
                emitter.Values[id].points[0].point.Y = 0;
                emitter.Values[id].points[1].point.X = 1;
                emitter.Values[id].points[1].point.Y = 1;
            }
            int[] retArr = new int[2];
            int p1 = 0;
            int p2 = emitter.Values[id].points.Count - 1;
            for (int i = 0; i < emitter.Values[id].points.Count; i++)
            {
                Emitter.PointF p = emitter.Values[id].points[i].point;
                Emitter.PointF pp1 = emitter.Values[id].points[p1].point;
                Emitter.PointF pp2 = emitter.Values[id].points[p2].point;
                if (p.X - x >= pp1.X - x && p.X - x <= 0)
                    p1 = i;
                if (p.X - x < pp2.X - x && p.X - x > 0)
                    p2 = i;

            }
            retArr[0] = p1;
            retArr[1] = p2;
            return retArr;
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

            if (Value != null)
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

            if (Value != null)
                selValueTab.IsEnabled = true;
            else
                selValueTab.IsEnabled = false;
        }

        private void selEmitterTab_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (Value != null)
                selValueTab.IsEnabled = true;
            else
                selValueTab.IsEnabled = false;
        }

        #endregion

        private void MetroWindow_Closing_1(object sender, System.ComponentModel.CancelEventArgs e)
        {
            ParticleEffect.writeLatestFile(Effects);

            TC.ProperShutdown();
        }

        private void MainTabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count != 0)
            {
                if (MainTabControl.Items.IndexOf(e.RemovedItems[0]) == -1)
                    return;
                if (MainTabControl.SelectedIndex == 2)
                    reloadValues();
                if (Flyouts[MainTabControl.Items.IndexOf(e.RemovedItems[0])].IsOpen)
                {
                    ToggleFlyouts();
                }
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (TC.DNTWindowOpen)
                return;
            if (TC.Dnt == null)
            {
                if (dnt.IsRunning)
                    dnt.Stop();
                dnt = null;
                if (dnt == null)
                    dnt = new dnTorque(Process.GetCurrentProcess().Handle);
                using (var bwr = new System.ComponentModel.BackgroundWorker())
                {
                    bwr.DoWork += bwr_DoWork;
                    bwr.RunWorkerAsync();
                }
            }
            TC.Open();
        }

        private void TestEffectButton_Click(object sender, RoutedEventArgs e)
        {
            if (!TC.DNTWindowOpen)
            {
                AlertControl.ShowHandlerDialog("Please open the preview window first!");
                return;
            }
            IOHandler.writeDatablockScript(Effects);
            
            IOHandler.WriteAndExec(TC.getUtil(), Effect, null);
            TC.SpawnEffect(Effect.name);
        }

        void dnt_onShutDown(object sender, EventArgs e)
        {
            TC.ProperShutdown();
        }

        private void FlyoutsButton_Click(object sender, RoutedEventArgs e)
        {
            Flyouts[0].IsOpen = true;

            Flyouts[1].IsOpen = true;
        }

        private void ToggleFlyouts()
        {
            if (MainTabControl.SelectedIndex == 0)
            {
                Flyouts[0].IsOpen = !Flyouts[0].IsOpen;
                Flyouts[1].IsOpen = false;
                Flyouts[2].IsOpen = false;
                Flyouts[3].IsOpen = false;
            }
            if (MainTabControl.SelectedIndex == 1)
            {
                Flyouts[1].IsOpen = !Flyouts[1].IsOpen;
                Flyouts[0].IsOpen = false;
                Flyouts[2].IsOpen = false;
                Flyouts[3].IsOpen = false;
            }
            if (MainTabControl.SelectedIndex == 2)
            {
                Flyouts[2].IsOpen = !Flyouts[2].IsOpen;
                Flyouts[0].IsOpen = false;
                Flyouts[1].IsOpen = false;
                Flyouts[3].IsOpen = false;
            }
            if (MainTabControl.SelectedIndex == 3)
            {
                Flyouts[3].IsOpen = !Flyouts[3].IsOpen;
                Flyouts[0].IsOpen = false;
                Flyouts[1].IsOpen = false;
                Flyouts[2].IsOpen = false;
            }
        }

        private void MetroWindow_PreviewMouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            ToggleFlyouts();
        }

        private void Reload_Datablocks_Click(object sender, RoutedEventArgs e)
        {
            if (!TC.DNTWindowOpen)
            {
                AlertControl.ShowHandlerDialog("Please open the preview window first!\nThe preview window is needed to trace all the available datablocks");
                return;
            }
            if (datablocks == null)
                datablocks = new Dictionary<string, List<string>>();
            if(datablocks.Count > 0)
                datablocks.Clear();
            List<string> emitters = getEmitterLabels().Cast<string>().ToList();
            foreach (string s in emitters)
            {
                string datablockType = Emitter.getDatablockType(Emitter.stringToEnum(s));
                if (!datablocks.Keys.Contains(datablockType))
                    datablocks.Add(datablockType, new List<string>());
                datablocks[datablockType].AddRange(TC.getDatablocks(datablockType));
            }
            foreach (string s in emitters)
            {
                string datablockType = Emitter.getNodeDatablockType(Emitter.stringToEnum(s));
                if (!datablocks.Keys.Contains(datablockType))
                    datablocks.Add(datablockType, new List<string>());
                datablocks[datablockType].AddRange(TC.getDatablocks(datablockType));
            }
            IOHandler.writeStringDataFile(datablocks, "datablocks.cache");
        }

        private void TimeRange_Click(object sender, RoutedEventArgs e)
        {
            float[] timeRange = TimeInputControl.ShowHandlerDialog("Time range:", (int)(emitter.Start * 1000), (int)(emitter.End * 1000));
            if (timeRange != null)
            {
                emitter.Start = timeRange[0] / 1000;
                emitter.End = timeRange[1] / 1000;
            }
            else
                return;
        }

        private void EmitterPointChanged(object sender, TextChangedEventArgs e)
        {
            if (emitter != null)
            {
                float.TryParse(xControl.Text, out emitter.x);
                float.TryParse(yControl.Text, out emitter.y);
                float.TryParse(zControl.Text, out emitter.z);
            }
        }

        private void EmitterDatablocksChanged(object sender, SelectionChangedEventArgs e)
        {
            if (emitter != null && NodeDatablockComboBox.SelectedItem != null && EmitterDatablockComboBox.SelectedItem != null)
            {
                emitter.datablock = NodeDatablockComboBox.SelectedItem.ToString();
                emitter.emitter = EmitterDatablockComboBox.SelectedItem.ToString();
            }
        }
    }
}
