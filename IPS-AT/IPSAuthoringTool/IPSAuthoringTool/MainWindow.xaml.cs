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
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using MahApps.Metro;
using MahApps.Metro.Controls;
using System.Collections.ObjectModel;
using IPSAuthoringTool.Utility;
using WinterLeaf;
using System.Diagnostics;
using System.Threading;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;

namespace IPSAuthoringTool
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        [DllImport("user32.dll")]
        public static extern bool SetForegroundWindow(IntPtr hWnd);

        public Dictionary<string, List<string>> datablocks;

        public TorqueController TC;
        dnTorque dnt;

        private ParticleEffect resetEffect = null;

        public MainWindow()
        {
            InitializeComponent();
            PEE.Main = this;
            SSE.Main = this;
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
            
            ItemInputControl.SetParent(ModalDialogParent);
            StringInputControl.SetParent(ModalDialogParent);
            VectorInputControl.SetParent(ModalDialogParent);
            TimeInputControl.SetParent(ModalDialogParent);
            AlertControl.SetParent(ModalDialogParent);
            FromToControl.SetParent(ModalDialogParent);
            FileInputControl.SetParent(ModalDialogParent);
        }

        void bwr_DoWork(object sender, DoWorkEventArgs e)
        {
            TC = new TorqueController(ref dnt);
        }

        private void MetroWindow_Closing_1(object sender, System.ComponentModel.CancelEventArgs e)
        {
            PEE.ShutDown();
            IOHandler.WriteSceneFile(SSE);

            TC.ProperShutdown();
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
            IOHandler.WriteStaticShapeScript(SSE);
            TC.Open(SSE.SelectedScene);
        }

        void dnt_onShutDown(object sender, EventArgs e)
        {
            TC.ProperShutdown();
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
            List<string> emitters = Emitter.getEmitterLabels().Cast<string>().ToList();
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

        #region Flyouts
        #region ParticleEffects
        #region Effects

        private void CreateEffectButton_Click(object sender, RoutedEventArgs e)
        {
            string s = StringInputControl.ShowHandlerDialog("Effect name:");
            if (s == null)
            {
                AlertControl.ShowHandlerDialog("No name given...\nEffect not created.");
                return;
            }
            ParticleEffect newEff = new ParticleEffect();
            newEff.Name = s;
            PEE.pEffects.Add(newEff);
            PEE.UpdateComboBoxes();
        }
        private void LoadEffectButton_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog OFD = new System.Windows.Forms.OpenFileDialog();
            if (OFD.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                ParticleEffect newEff = new ParticleEffect();
                newEff.ReadFromFile(OFD.FileName, AlertControl);
                PEE.pEffects.Add(newEff);
                PEE.UpdateComboBoxes();
            }
        }
        private void DeleteEffectButton_Click(object sender, RoutedEventArgs e)
        {
            if (PEE.pEffect != null)
            {
                PEE.pEffects.Remove(PEE.pEffect);
                PEE.pEffect = null;
                PEE.UpdateComboBoxes();
            }
        }
        private void SaveEffectButton_Click(object sender, RoutedEventArgs e)
        {
            if (PEE.pEffect != null)
            {
                IOHandler.writeDatablockScript(PEE.pEffects);
                if(TC.getUtil() != null)
                    IOHandler.WriteAndExec(TC.getUtil(), PEE.pEffect, AlertControl, SSE);
            }
        }

        #endregion

        #region Emitters
        private void CreateEmitterButton_Click(object sender, RoutedEventArgs e)
        {
            object o = ItemInputControl.ShowHandlerDialog("Please choose emitter type:", Emitter.getEmitterLabels());
            Emitter emi = new Emitter();
            emi.Type = Emitter.stringToEnum((string)o);
            if (emi.Type == Emitter.EmitterType.Error)
                return;

            string tag = StringInputControl.ShowHandlerDialog("Please define the emitters identification tag:");
            if (tag != null && tag != "")
                emi.Tag = tag;

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

            PEE.pEffect.Emitters.Add(emi);
            PEE.UpdateComboBoxes();
        }
        private void DeleteEmitterButton_Click(object sender, RoutedEventArgs e)
        {
            PEE.pEffect.Emitters.Remove(PEE.pEmitter);
            PEE.pEmitter = null;
            PEE.Value = null;
            PEE.pEmitters.Clear();
            PEE.pValues.Clear();
            PEE.UpdateComboBoxes();
            PEE.ReloadValueTabContent();
        }
        private void SaveEmitterButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Save emitter dialog is missing");
        }
        private void TimeRange_Click(object sender, RoutedEventArgs e)
        {
            float[] timeRange = TimeInputControl.ShowHandlerDialog("Time range:", (int)(PEE.pEmitter.Start * 1000), (int)(PEE.pEmitter.End * 1000));
            if (timeRange != null)
            {
                PEE.pEmitter.Start = timeRange[0] / 1000;
                PEE.pEmitter.End = timeRange[1] / 1000;
            }
            else
                return;
        }

        private void EmitterPointChanged(object sender, TextChangedEventArgs e)
        {
            if (PEE != null && PEE.pEmitter != null)
            {
                float.TryParse(xControl.Text, out PEE.pEmitter.x);
                float.TryParse(yControl.Text, out PEE.pEmitter.y);
                float.TryParse(zControl.Text, out PEE.pEmitter.z);
            }
        }

        private void EmitterDatablocksChanged(object sender, SelectionChangedEventArgs e)
        {
            if (PEE.pEmitter != null && NodeDatablockComboBox.SelectedItem != null && EmitterDatablockComboBox.SelectedItem != null)
            {
                PEE.pEmitter.datablock = NodeDatablockComboBox.SelectedItem.ToString();
                PEE.pEmitter.emitter = EmitterDatablockComboBox.SelectedItem.ToString();
            }
        }
        #endregion

        #region Values
        private void AddValueButton_Click(object sender, RoutedEventArgs e)
        {
            List<string> fields = Emitter.getFields(PEE.pEmitter.Type);
            object o = ItemInputControl.ShowHandlerDialog("Choose value:", PEE.getFieldList());
            if (o != null)
            {
                Emitter.value val = new Emitter.value();
                val.valueName = fields.ElementAt((int)((Control)o).Tag);
                PEE.pEmitter.Values.Add(val);
                PEE.UpdateComboBoxes();
            }
        }

        private void RemoveValueButton_Click(object sender, RoutedEventArgs e)
        {
            if ((PEE.Value.Ease == false && PEE.Value.setTime != null) || (PEE.Value.Ease == true && PEE.Value.points.Count > 0))
            {
                PEE.pEmitter.Values.Remove(PEE.Value);
                PEE.Value = null;
                PEE.pValues.Clear();
                PEE.UpdateComboBoxes();
                PEE.ReloadValueTabContent();
            }
        }
        #endregion

        #region Misc
        private void TestEffectButton_Click(object sender, RoutedEventArgs e)
        {
            if (!TC.DNTWindowOpen)
            {
                AlertControl.ShowHandlerDialog("Please open the preview window first!");
                return;
            }
            if (PEE.pEffect == null)
            {
                AlertControl.ShowHandlerDialog("Please select a Particle Effect first!");
                return;
            }
            IOHandler.writeDatablockScript(PEE.pEffects);

            IOHandler.WriteAndExec(TC.getUtil(), PEE.pEffect, null, SSE);
            TC.WriteShapeGlobals(SSE.Sel_SequenceListView.Items.Count);
            TC.SpawnEffect(PEE.pEffect.Name);
            TC.UpdateShape();
            SetForegroundWindow(dnt.torque_gethwnd());
        }
        #endregion

        private void MetroWindow_PreviewMouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (MainTabControl.SelectedIndex == 1)
            {
                ToggleFlyout(1);
                //PEE.ToggleFlyouts();
            }
            if (MainTabControl.SelectedIndex == 2)
            {
                SSE.ToggleFlyouts();
            }
        }

        public void ToggleFlyout(int flyout)
        {
            foreach (Flyout f in Flyouts)
                if (f != Flyouts[flyout])
                    f.IsOpen = false;

            Flyouts[flyout].IsOpen = !Flyouts[flyout].IsOpen;
        }
        #endregion

        #endregion

        private void TabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
            {
                if(e.AddedItems[0].GetType() != typeof(TabItem))
                    return;
                if (MainTabControl.Items.IndexOf(e.AddedItems[0]) == 1)
                {
                    if(HintTextBlock != null)
                        HintTextBlock.Text = "Press the right mouse button to open up the options flyout";
                    foreach (Flyout f in Flyouts)
                        if (f.IsOpen == true)
                        { PEE.ToggleFlyouts(); break; }
                    MiLightBlue(null, null);
                }

                if (MainTabControl.Items.IndexOf(e.AddedItems[0]) == 2)
                {
                    if (HintTextBlock != null)
                        HintTextBlock.Text = "Double click an item to access the options menu for that item (e.g. try double clicking a Sequence)";
                    foreach (Flyout f in Flyouts)
                        if (f.IsOpen == true)
                        { SSE.ToggleFlyouts(); break; }
                    MiLightRed(null, null);
                }
            }
        }

        private void EditEmitterButton_Click(object sender, RoutedEventArgs e)
        {
            if (PEE.pEmitter == null)
            {
                AlertControl.ShowHandlerDialog("Please select an emitter first.");
                return;
            }
            ToggleFlyout(2);
        }
        private void ShowContextMenu_Click(object sender, RoutedEventArgs e)
        {
            ContextMenu cm = ((Control)sender).ContextMenu;
            if (cm != null)
            {
                cm.PlacementTarget = (UIElement)sender;
                cm.IsOpen = true;
            }
        }

        private void TagTextControl_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (PEE.pEmitter != null)
            {
                PEE.pEmitter.Tag = TagTextControl.Text;
                PEE.pEmitters.Remove(PEE.pEmitter);
                PEE.pEmitters.Add(PEE.pEmitter);
                PEE.Emitters.SelectedItem = PEE.pEmitter;
            }
        }

        private void ResetEffect_Click(object sender, RoutedEventArgs e)
        {
            PEE.pEffect.Copy(resetEffect);
            PEE.SelectEffect(PEE.pEffect);
        }

        public void UpdateClone()
        {
            resetEffect = PEE.pEffect.Clone();
        }

        #region Themes
        private void MiLightRed(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Red"), Theme.Light);
        }

        private void MiDarkRed(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Red"), Theme.Dark);
        }

        private void MiLightGreen(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Green"), Theme.Light);
        }

        private void MiDarkGreen(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Green"), Theme.Dark);
        }

        private void MiLightBlue(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Blue"), Theme.Light);
        }

        private void MiDarkBlue(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Blue"), Theme.Dark);
        }

        private void MiLightPurple(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Purple"), Theme.Light);
        }

        private void MiDarkPurple(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Purple"), Theme.Dark);
        }

        private void MiLightOrange(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Orange"), Theme.Light);
        }

        private void MiDarkOrange(object sender, RoutedEventArgs e)
        {
            ThemeManager.ChangeTheme(this, ThemeManager.DefaultAccents.First(a => a.Name == "Orange"), Theme.Dark);
        }
        #endregion
    }
}
