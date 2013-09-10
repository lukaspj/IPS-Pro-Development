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
using System.IO;
using System.Windows.Threading;

namespace IPSAuthoringTool.Windows
{
    /// <summary>
    /// Interaction logic for SceneSetupEditor.xaml
    /// </summary>
    public partial class SceneSetupEditor : UserControl
    {
        private MainWindow main;

        public MainWindow Main
        {
            get { return main; }
            set { main = value; }
        }

        private Microsoft.Win32.OpenFileDialog FindModelDialog = new Microsoft.Win32.OpenFileDialog();
        private Microsoft.Win32.OpenFileDialog FindSequenceDialog = new Microsoft.Win32.OpenFileDialog();
        public Utility.Model SelectedModel = null;
        public string SelectedScene = "";
        public List<Utility.Model> Models = new List<Utility.Model>();
        public List<Utility.Sequence> Sequences = new List<Utility.Sequence>();
        public Dictionary<string, Utility.Sequence> SequenceDict = new Dictionary<string, Utility.Sequence>();
        public SceneSetupEditor()
        {
            InitializeComponent();
            Utility.IOHandler.ReadSceneFile(this);
            InitializeModel();
            InitializeScene();
        }

        #region Model
        public void InitializeModel()
        {
            FindModelDialog.Filter = "DTS and COLLADA files (*.dts;*.dae)|*.dts;*.dae";
            FindSequenceDialog.Filter = "DTS, DSQ and COLLADA files (*.dts;*.dsq;*.dae)|*.dts;*.dsq;*.dae";
            UpdateModel();
        }

        public void UpdateModel()
        {
            ModelListView.Items.Clear();
            SequenceListView.Items.Clear();
            foreach (Utility.Model model in Models)
            {
                Label l = new Label();
                l.Content = model;
                System.Windows.Controls.ContextMenu contextMenu = new System.Windows.Controls.ContextMenu();

                MenuItem delete = new MenuItem() { Header = "Delete" };
                MenuItem locateMatCS = new MenuItem() { Header = "Locate material.cs" };

                delete.Click += (s, e) => DeleteModel();
                locateMatCS.Click += (s, e) => LocateMatCS();

                contextMenu.Items.Add(locateMatCS);
                contextMenu.Items.Add(delete);

                l.ContextMenu = contextMenu;
                l.Tag = ModelListView;
                l.PreviewMouseDoubleClick += ListViewItem_PreviewMouseDoubleClick;
                ModelListView.Items.Add(l);
            }
            UpdateSequenceDict();

            foreach (string key in SequenceDict.Keys)
            {
                Label l = new Label();
                l.Content = key;
                l.Width = SequenceListView.Width;
                l.HorizontalAlignment = System.Windows.HorizontalAlignment.Stretch;
                System.Windows.Controls.ContextMenu contextMenu = new System.Windows.Controls.ContextMenu();

                MenuItem editFromTo = new MenuItem() { Header = "Change frames" };
                MenuItem editName = new MenuItem() { Header = "Edit name" };
                MenuItem editID = new MenuItem() { Header = "Edit id" };
                MenuItem delete = new MenuItem() { Header = "Delete" };

                editFromTo.Click += (s, e) => EditSequenceFromTo();
                editName.Click += (s, e) => EditSequenceName();
                editID.Click += (s, e) => EditSequenceID();
                delete.Click += (s, e) => DeleteSequence();

                contextMenu.Items.Add(editFromTo);
                contextMenu.Items.Add(editName);
                contextMenu.Items.Add(editID);
                contextMenu.Items.Add(delete);

                l.ContextMenu = contextMenu;
                l.Tag = SequenceListView;
                l.PreviewMouseDoubleClick += ListViewItem_PreviewMouseDoubleClick;
                SequenceListView.Items.Add(l);
            }
            SelectModel(SelectedModel);
        }

        public void UpdateSequenceDict()
        {
            SequenceDict.Clear();
            Dictionary<string, int> encounteredNames = new Dictionary<string,int>();
            foreach (Utility.Sequence seq in Sequences)
            {
                string name = System.IO.Path.GetFileNameWithoutExtension(seq.File);
                if (seq.Name != "" && seq.Name != null)
                    name = seq.Name;
                if (!encounteredNames.ContainsKey(name))
                    encounteredNames.Add(name, 0);
                else
                {
                    encounteredNames[name]++;
                    name += encounteredNames[name].ToString();
                }
                SequenceDict.Add(seq.Name, seq);
            }
        }

        public void SelectModel(Utility.Model s)
        {
            foreach (object o in ModelListView.Items)
            {
                Label l = (Label)o;
                if (l.Content.Equals(s))
                {
                    ModelListView.SelectedItem = o;
                    return;
                }
            }
        }

        public void SelectSequence(string file)
        {
            string seq = System.IO.Path.GetFileNameWithoutExtension(file);
            Utility.Sequence sequence = null;
            foreach (Utility.Sequence sequ in Sequences)
                if (sequ.File == file)
                    sequence = sequ;
            if (sequence == null) return;
            foreach (object o in Sel_SequenceListView.Items)
                if (((Label)o).Content.Equals(sequence.Name))
                    return;
            Label newLabel = new Label();
            newLabel.Content = sequence.Name;
            newLabel.Width = Sel_SequenceListView.Width;
            newLabel.Tag = Sel_SequenceListView;
            newLabel.PreviewMouseDoubleClick += ListViewItem_PreviewMouseDoubleClick;
            Sel_SequenceListView.Items.Add(newLabel);
        }

        private void ModelListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count <= 0)
                return;
            string selected = ((Label)e.AddedItems[0]).Content.ToString();
            foreach (Utility.Model s in Models)
                if (System.IO.Path.GetFileNameWithoutExtension(s.getPath()) == selected)
                    SelectedModel = s;
        }

        private void SequenceListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void ListViewItem_PreviewMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            Label LVI = (Label)sender;
            ListView LV = (ListView)LVI.Tag;
            if (LV.SelectedItem == null)
                return;
            ContextMenu CM = ((Label)LV.SelectedItem).ContextMenu;
            if (CM != null)
            {
                CM.PlacementTarget = ((Label)LV.SelectedItem);
                CM.IsOpen = true;
            }
        }

        #region ContextMenus
        private void EditSequenceFromTo()
        {
            string sequenceName = ((Label)SequenceListView.SelectedItem).Content.ToString();
            Utility.Sequence sequence = null;
            if (SequenceDict[sequenceName] != null)
                sequence = SequenceDict[sequenceName];
            else
            {
                sequence = new Utility.Sequence();
                sequence.File = sequenceName;
                SequenceDict[sequenceName] = sequence;
            }
            int[] fromto = Main.FromToControl.ShowHandlerDialog("Select the start and end frames for the sequence (1 and -1 for all frames)",
                sequence.start != int.MinValue ? sequence.start.ToString() : "",
                sequence.end != int.MaxValue ? sequence.end.ToString() : "");
            if (fromto == null)
                return;
            sequence.start = fromto[0];
            sequence.end = fromto[1];
        }

        private void EditSequenceName()
        {
            string sequenceName = ((Label)SequenceListView.SelectedItem).Content.ToString();
            Utility.Sequence sequence = null;
            if (SequenceDict[sequenceName] != null)
                sequence = SequenceDict[sequenceName];
            else
            {
                sequence = new Utility.Sequence();
                sequence.File = sequenceName;
                SequenceDict[sequenceName] = sequence;
            }
            string name = Main.StringInputControl.ShowHandlerDialog("Write sequence name:",
                sequence.Name);
            if (name == null)
                return;
            sequence.Name = name;
            UpdateSequenceDict();
            ((Label)SequenceListView.SelectedItem).Content = sequence.Name;
        }

        private void EditSequenceID()
        {
            string sequenceName = ((Label)SequenceListView.SelectedItem).Content.ToString();
            Utility.Sequence sequence = null;
            if (SequenceDict[sequenceName] != null)
                sequence = SequenceDict[sequenceName];
            else
            {
                sequence = new Utility.Sequence();
                sequence.File = sequenceName;
                SequenceDict[sequenceName] = sequence;
            }
            string id = Main.StringInputControl.ShowHandlerDialog("Write sequence id string:",
                sequence.id);
            if (id == null)
                return;
            sequence.id = id;
        }

        private void DeleteSequence()
        {
            string sequenceName = ((Label)SequenceListView.SelectedItem).Content.ToString();
            foreach(Utility.Sequence seq in Sequences)
                if (seq.Name == sequenceName)
                {
                    Sequences.Remove(seq);
                    UpdateModel();
                    return;
                }
        }

        private void DeleteModel()
        {
            Utility.Model model = (Utility.Model)((Label)ModelListView.SelectedItem).Content;
            foreach (Utility.Model s in Models)
                if (s == model)
                {
                    Models.Remove(s);
                    UpdateModel();
                    return;
                }
        }

        private void LocateMatCS()
        {
            Utility.Model model = (Utility.Model)((Label)ModelListView.SelectedItem).Content;
            string matFile = Main.FileInputControl.ShowHandlerDialog("Please locate the materials.cs file for this model:", model.materialsCS);
            if (matFile != null && File.Exists(matFile))
                model.materialsCS = matFile;
        }

        #endregion

        public List<Utility.Sequence> GetSelectedSequences()
        {
            List<Utility.Sequence> ret = new List<Utility.Sequence>();
            foreach (object o in Sel_SequenceListView.Items)
            {
                Label l = (Label)o;
                string s = ((Label)o).Content.ToString();
                ret.Add(SequenceDict[s]);
            }
            return ret;
        }

        private void SelectSeqButton_Click(object sender, RoutedEventArgs e)
        {
            if (SequenceListView.SelectedItem == null)
                return;
            string seq = ((Label)SequenceListView.SelectedItem).Content.ToString();
            if (SequenceDict[seq] == null)
                return;
            foreach (Label l in Sel_SequenceListView.Items)
                if (l.Content.Equals(seq))
                    return;
            Label newLabel = new Label();
            newLabel.HorizontalAlignment = System.Windows.HorizontalAlignment.Stretch;
            newLabel.Width = Sel_SequenceListView.Width;
            newLabel.Content = SequenceDict[seq].Name;
            newLabel.Tag = Sel_SequenceListView;
            newLabel.PreviewMouseDoubleClick += ListViewItem_PreviewMouseDoubleClick;
            Sel_SequenceListView.Items.Add(newLabel);
        }

        private void RemoveSeqButton_Click(object sender, RoutedEventArgs e)
        {
            if (Sel_SequenceListView.SelectedItem == null)
                return;
            int index = Sel_SequenceListView.SelectedIndex;
            Sel_SequenceListView.Items.Remove(Sel_SequenceListView.SelectedItem);
            if (Sel_SequenceListView.Items.Count == 0)
                return;
            else if (index + 1 <= Sel_SequenceListView.Items.Count)
                Sel_SequenceListView.SelectedIndex = index;
            else if (index + 1 >= Sel_SequenceListView.Items.Count)
                Sel_SequenceListView.SelectedIndex = Sel_SequenceListView.Items.Count - 1;
        }

        private void SequenceListView_Drop(object sender, DragEventArgs e)
        {
            SequenceListView.Background = Brushes.Transparent;
            if ((e.AllowedEffects & DragDropEffects.Copy) == DragDropEffects.Copy)
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop, false);
                foreach (string file in files)
                {
                    if (file.ToLower().EndsWith(".dts") || file.ToLower().EndsWith(".dsq") || file.ToLower().EndsWith(".dae"))
                    {
                        string name = Main.StringInputControl.ShowHandlerDialog("Sequence name (leave blank for none):");
                        if (name == null)
                            name = "";
                        string id = Main.StringInputControl.ShowHandlerDialog("Sequence ID string (leave blank for none):");
                        if (id == null)
                            id = "";
                        Utility.Sequence seq = new Utility.Sequence();
                        seq.File = file;
                        seq.Name = name;
                        seq.id = id;
                        int[] fromto = Main.FromToControl.ShowHandlerDialog("Sequence start and end frames (0 to -1 is all frames): ");
                        if (fromto != null)
                        {
                            seq.start = fromto[0];
                            seq.end = fromto[1];
                        }
                        Sequences.Add(seq);
                    }
                }
                UpdateModel();
            }
        }

        private void ModelListView_Drop(object sender, DragEventArgs e)
        {
            ModelListView.Background = Brushes.Transparent;
            if ((e.AllowedEffects & DragDropEffects.Copy) == DragDropEffects.Copy)
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop, false);
                foreach (string file in files)
                {
                    if (file.ToLower().EndsWith(".dts") || file.ToLower().EndsWith(".dae"))
                    {
                        Utility.Model model = new Utility.Model();
                        if(!model.setPath(file))
                        {
                            string matFile = Main.FileInputControl.ShowHandlerDialog("Materials.cs wasn't found please locate it");
                            if (matFile != null && File.Exists(matFile))
                                model.materialsCS = matFile;
                        }
                        Models.Add(model);
                    }
                }
                UpdateModel();
            }
        }

        private void ImportModelButton_Click(object sender, RoutedEventArgs e)
        {
            Nullable<bool> result = FindModelDialog.ShowDialog();
            if (result == true)
            {
                bool contains = false;
                foreach (Utility.Model model in Models)
                    if (model.getPath() == FindModelDialog.FileName)
                        contains = true;
                if(!contains)
                {
                    Utility.Model model = new Utility.Model();
                    if (!model.setPath(FindModelDialog.FileName))
                    {
                        string matFile = Main.FileInputControl.ShowHandlerDialog("Materials.cs wasn't found please locate it");
                        if (matFile != null && File.Exists(matFile))
                            model.materialsCS = matFile;
                    }
                    Models.Add(model);
                    UpdateModel();
                }
            }
        }

        private void ImportSequenceButton_Click(object sender, RoutedEventArgs e)
        {
            Nullable<bool> result = FindSequenceDialog.ShowDialog();
            if (result == true)
            {
                string name = Main.StringInputControl.ShowHandlerDialog("Sequence name (leave blank for none):");
                if (name == null)
                    name = "";
                Utility.Sequence seq = new Utility.Sequence();
                seq.File = FindSequenceDialog.FileName;
                seq.Name = name;
                int[] fromto = Main.FromToControl.ShowHandlerDialog("Sequence start and end frames: ");
                if (fromto != null)
                {
                    seq.start = fromto[0];
                    seq.end = fromto[1];
                }
                Sequences.Add(seq);
                UpdateModel();
            }
        }
        #endregion

        private void ListView_DragEnter(object sender, DragEventArgs e)
        {
            ((ListView)sender).Background = Brushes.AliceBlue;
        }

        private void ListView_DragLeave(object sender, DragEventArgs e)
        {
            ((ListView)sender).Background = Brushes.Transparent;
        }

        #region Scene
        private void InitializeScene()
        {
            UpdateScene();
        }
        private void UpdateScene()
        {
            string[] scenes = Directory.GetFiles("levels", "*.mis", SearchOption.AllDirectories);
            foreach (string level in scenes)
            {
                string levelName = System.IO.Path.GetFileNameWithoutExtension(level);
                Label l = new Label();
                l.Content = levelName;
                l.HorizontalAlignment = System.Windows.HorizontalAlignment.Stretch;
                SceneListView.Items.Add(l);
            }
            if ((SelectedScene == "" || SelectedScene == null) && SceneListView.Items.Count > 0)
                SelectedScene = ((Label)SceneListView.Items[0]).Content.ToString();
            SelectScene(SelectedScene);
        }

        public void SelectScene(string scene)
        {
            foreach (object o in SceneListView.Items)
            {
                Label l = (Label)o;
                if (l.Content.Equals(scene))
                {
                    SceneListView.SelectedItem = o;
                    return;
                }
            }
        }

        private void SceneListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
                SelectedScene = ((Label)e.AddedItems[0]).Content.ToString();
        }
        #endregion
        #region Emitter
        #endregion

        public void ToggleFlyouts()
        {
            /*if (theTabControl.SelectedIndex == 0)
            {
                Main.ToggleFlyout(0);
                return;
            }*/
            Main.ToggleFlyout(3);
        }
    }
}
