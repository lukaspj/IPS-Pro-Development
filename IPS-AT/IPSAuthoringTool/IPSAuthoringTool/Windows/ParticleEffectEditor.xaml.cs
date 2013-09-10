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
using System.Windows.Media.Animation;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Collections.ObjectModel;

using IPSAuthoringTool.Utility;
using MahApps.Metro.Controls;

namespace IPSAuthoringTool.Windows
{
     
    /// <summary>
    /// Interaction logic for ParticleEffectEditor.xaml
    /// </summary>
    public partial class ParticleEffectEditor : UserControl
    {
        public Emitter pEmitter;
        public ParticleEffect pEffect;
        public Emitter.value Value;

        private MainWindow main;

        public ObservableCollection<ParticleEffect> pEffects;
        public ObservableCollection<Emitter> pEmitters;
        public ObservableCollection<Emitter.value> pValues;

        public MainWindow Main
        {
            get { return main; }
            set { main = value; }
        }

        public ParticleEffectEditor()
        {
            InitializeComponent();
            pEffects = new ObservableCollection<ParticleEffect>(ParticleEffect.loadLatestFile());
            pEmitters = new ObservableCollection<Emitter>();
            pValues = new ObservableCollection<Emitter.value>();
            Binding bind = new Binding();
            Emitters.SetBinding(ComboBox.ItemsSourceProperty, new Binding() { Source = pEmitters, UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged });
            Effects.ItemsSource = pEffects;
            //Emitters.ItemsSource = pEmitters;
            Values.ItemsSource = pValues;
            UpdateComboBoxes();
        }

        public void UpdateComboBoxes()
        {
            if (pEffect == null)
                return;

            foreach (Emitter emi in pEffect.Emitters)
                if(!pEmitters.Contains(emi))
                    pEmitters.Add(emi);
            Emitters.Items.Refresh();
            if (pEmitter == null)
                return;

            foreach (Emitter.value val in pEmitter.Values)
                if (!pValues.Contains(val))
                    pValues.Add(val);
        }

        #region EditValueTab
        /*
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
            foreach (object o in objs)
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

                List<object> objs = Tweener.getEaseMethods();
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
            ComputeGraph(Points, pEmitter.Values.IndexOf(Value));
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
            if (Value != null)
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
         */
        #endregion

        #region GraphComputing

        protected void ComputeGraph(ObservableCollection<double> Points, int idx)
        {
            int[] relPoints = GetRelevantPointIndexes(0, idx);
            for (int i = 0; i < 1000; i++)
            {
                int[] newPoints = GetRelevantPointIndexes((float)i / 1000, idx);
                if (newPoints[0] == newPoints[1])
                    Points.Add(pEmitter.Values[idx].points[newPoints[0]].point.Y * 1000);
                else
                    Points.Add(GetY(i, pEmitter.Values[idx].points[newPoints[0]], pEmitter.Values[idx].points[newPoints[1]], idx));
                //Points.Add(new Point(i, GetY(i, emitter.Values[idx].points[newPoints[0]], emitter.Values[idx].points[newPoints[1]], idx)));
            }
        }

        private int GetY(int x, Emitter.PointOnValue p1, Emitter.PointOnValue p3, int idx)
        {
            double ease = Utility.Tweener.EaseFromString((double)x / 1000.0f - p1.point.X, p1.point.Y, p3.point.Y - p1.point.Y, p3.point.X - p1.point.X, p3.Easing, p3.EaseIn, p3.EaseOut);
            int ret = (int)(ease * 1000.0f);
            return ret;
        }

        private int[] GetRelevantPointIndexes(float x, int id)
        {
            if (pEmitter.Values[id].points.Count < 2)
            {
                pEmitter.Values[id].points.Add(new Emitter.PointOnValue());
                pEmitter.Values[id].points.Add(new Emitter.PointOnValue());
                pEmitter.Values[id].points[0].point.X = 0;
                pEmitter.Values[id].points[0].point.Y = 0;
                pEmitter.Values[id].points[1].point.X = 1;
                pEmitter.Values[id].points[1].point.Y = 1;
            }
            int[] retArr = new int[2];
            int p1 = 0;
            int p2 = pEmitter.Values[id].points.Count - 1;
            for (int i = 0; i < pEmitter.Values[id].points.Count; i++)
            {
                Emitter.PointF p = pEmitter.Values[id].points[i].point;
                Emitter.PointF pp1 = pEmitter.Values[id].points[p1].point;
                Emitter.PointF pp2 = pEmitter.Values[id].points[p2].point;
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

        public void ToggleFlyouts(int prevTab = -1)
        {
            if(prevTab == -1 || Main.Flyouts[TabToFlyout(prevTab)].IsOpen)
            {
                if (Main.MainTabControl.SelectedIndex == 1)
                    Main.ToggleFlyout(1);
                /*if (MainTabControl.SelectedIndex == 1)
                    Main.ToggleFlyout(2);
                if (MainTabControl.SelectedIndex == 2)
                    Main.ToggleFlyout(3);
                if (MainTabControl.SelectedIndex == 3)
                    Main.ToggleFlyout(0);*/
            }
        }

        private int TabToFlyout(int tab)
        {
            switch (tab)
            {
                case 0:
                    return 1;
                case 1:
                    return 2;
                case 2:
                    return 3;
                case 3:
                    return 0;
                default:
                    return -1;
            }
        }

        public void ShutDown()
        {
            ParticleEffect.writeLatestFile(pEffects);
        }

        public List<object> getFieldList()
        {
            List<string> strings = Emitter.getFields(pEmitter.Type);
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

        public enum ArrowEffectHandle
        {
            BEGIN_ARROW_1,
            RESET_ARROW_1,
            BEGIN_ARROW_2,
            RESET_ARROW_2
        };

        private void ShowArrowEffect(ArrowEffectHandle handle)
        {
            switch (handle)
            {
                case ArrowEffectHandle.BEGIN_ARROW_1:
                    DoubleAnimation da = new DoubleAnimation()
                    {
                        From = 0,
                        To = 0.99999999,
                        Duration = new Duration(TimeSpan.FromSeconds(0.6))
                    };
                    arrow1.tarrowS1.BeginAnimation(GradientStop.OffsetProperty, da);
                    DoubleAnimation da2 = new DoubleAnimation()
                    {
                        From = 0.00000001,
                        To = 1.0,
                        Duration = new Duration(TimeSpan.FromSeconds(0.6))
                    };
                    arrow1.tarrowS2.BeginAnimation(GradientStop.OffsetProperty, da2);
                    break;
                case ArrowEffectHandle.RESET_ARROW_1:
                    arrow1.tarrowS1.Offset = 0;
                    arrow1.tarrowS2.Offset = 0;
                    break;
                case ArrowEffectHandle.BEGIN_ARROW_2:
                    DoubleAnimation dda = new DoubleAnimation()
                    {
                        From = 0,
                        To = 0.99999999,
                        Duration = new Duration(TimeSpan.FromSeconds(0.6))
                    };
                    arrow2.tarrowS1.BeginAnimation(GradientStop.OffsetProperty, dda);
                    DoubleAnimation dda2 = new DoubleAnimation()
                    {
                        From = 0.00000001,
                        To = 1.0,
                        Duration = new Duration(TimeSpan.FromSeconds(0.6))
                    };
                    arrow2.tarrowS2.BeginAnimation(GradientStop.OffsetProperty, dda2);
                    break;
                case ArrowEffectHandle.RESET_ARROW_2:
                    arrow2.tarrowS1.BeginAnimation(GradientStop.OffsetProperty, null);
                    arrow2.tarrowS2.BeginAnimation(GradientStop.OffsetProperty, null);
                    break;
            }
        }

        private void Effects_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (Effects.SelectedItem == null)
                return;
            ParticleEffect newEffect = null;
            foreach (ParticleEffect pe in pEffects)
                if (pe == Effects.SelectedItem)
                    newEffect = pe;
            if (newEffect != null && pEffect != newEffect)
            {
                pEffect = newEffect;
                pEmitter = null;
                Value = null;
                pEmitters.Clear();
                pValues.Clear();
                UpdateComboBoxes();
                ReloadValueTabContent();
                ShowArrowEffect(ArrowEffectHandle.BEGIN_ARROW_1);
                ShowArrowEffect(ArrowEffectHandle.RESET_ARROW_2);
                Main.UpdateClone();
            }
        }

        public void SelectEffect(ParticleEffect newEffect)
        {
            pEffect = newEffect;
            pEmitter = null;
            Value = null;
            pEmitters.Clear();
            pValues.Clear();
            UpdateComboBoxes();
            ReloadValueTabContent();
            ShowArrowEffect(ArrowEffectHandle.BEGIN_ARROW_1);
            ShowArrowEffect(ArrowEffectHandle.RESET_ARROW_2);
        }

        private void Emitters_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Emitter newEmitter = null;
            if(Emitters.SelectedItem != null)
                foreach (Emitter emi in pEffect.Emitters)
                    if (emi == Emitters.SelectedItem)
                        newEmitter = emi;
            if (newEmitter != null && pEmitter != newEmitter)
            {
                pEmitter = newEmitter;

                UpdateEditEmitterFlyout();
                Value = null;
                pValues.Clear();
                UpdateComboBoxes();
                ReloadValueTabContent();
                ShowArrowEffect(ArrowEffectHandle.BEGIN_ARROW_2);
            }
        }

        public void UpdateEditEmitterFlyout()
        {
            Main.EmitterDatablockComboBox.Items.Clear();
            Main.NodeDatablockComboBox.Items.Clear();
            string datablockType = pEmitter.getDatablockType();
            Main.datablocks[datablockType].ForEach(x => Main.EmitterDatablockComboBox.Items.Add(x));
            string nodeDatablockType = pEmitter.getNodeDatablockType();
            Main.datablocks[nodeDatablockType].ForEach(x => Main.NodeDatablockComboBox.Items.Add(x));
            Main.EmitterDatablockComboBox.SelectedIndex = Main.datablocks[datablockType].IndexOf(pEmitter.emitter);
            Main.NodeDatablockComboBox.SelectedIndex = Main.datablocks[nodeDatablockType].IndexOf(pEmitter.datablock);

            Main.xControl.Text = pEmitter.x.ToString();
            Main.yControl.Text = pEmitter.y.ToString();
            Main.zControl.Text = pEmitter.z.ToString();
        }

        private void Values_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Emitter.value newValue = null;
            if (Values.SelectedItem != null)
                foreach (Emitter.value val in pEmitter.Values)
                    if (val == Values.SelectedItem)
                        newValue = val;
            if (newValue != null && Value != newValue)
            {
                Value = newValue;
                ReloadValueTabContent();
            }
        }
        #region EditValueTab
        #region Pointlist and resets
        public void ReloadValueTabContent()
        {
            if (Value == null)
            {
                ValueTypeBox.Items.Clear();
                PointList.Items.Clear();
                EaseCheckBox.IsChecked = false;
                DeltaTextBox.Text = "";
                GraphContainer.Children.Clear();
            }
            else
            {
                ResetValueComboBox();
                if (Value.Ease)
                    EaseCheckBox.IsChecked = true;
                else
                    EaseCheckBox.IsChecked = false;
                DeltaTextBox.Text = Value.deltaValue.ToString();
                reloadPoints();
            }
        }

        public void ResetValueComboBox()
        {
            ValueTypeBox.Items.Clear();
            List<object> objs = getFieldList();
            foreach (object o in objs)
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

                List<object> objs = Tweener.getEaseMethods();
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
                    float.TryParse(((TextBox)g.Children[1]).Text, out point.point.X);
                else
                    point.point.X = 0.0f;

                if (((TextBox)g.Children[3]).Text != "")
                    float.TryParse(((TextBox)g.Children[3]).Text, out point.point.Y);
                else
                    point.point.Y = 0.0f;

                if (Extensions.TryClamp(0, 1, ref point.point.X))
                    ((TextBox)g.Children[1]).Text = point.point.X.ToString();

                if (Extensions.TryClamp(0, 1, ref point.point.Y))
                    ((TextBox)g.Children[3]).Text = point.point.Y.ToString();

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
            ComputeGraph(Points, pEmitter.Values.IndexOf(Value));
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
            if (Value == null)
                return;
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
            if (Value != null)
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
    }
}
