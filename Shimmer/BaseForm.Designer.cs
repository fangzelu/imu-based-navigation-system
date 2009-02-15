namespace SensorNode
{
    partial class BaseForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                TV.ReleaseAll(); //When the form closing, release all render resource
                TV = null;

                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            ChartFX.WinForms.Adornments.SolidBackground solidBackground1 = new ChartFX.WinForms.Adornments.SolidBackground();
            ChartFX.WinForms.TitleDockable titleDockable1 = new ChartFX.WinForms.TitleDockable();
            ChartFX.WinForms.TitleDockable titleDockable2 = new ChartFX.WinForms.TitleDockable();
            ChartFX.WinForms.Adornments.SolidBackground solidBackground2 = new ChartFX.WinForms.Adornments.SolidBackground();
            ChartFX.WinForms.TitleDockable titleDockable3 = new ChartFX.WinForms.TitleDockable();
            ChartFX.WinForms.TitleDockable titleDockable4 = new ChartFX.WinForms.TitleDockable();
            ChartFX.WinForms.TitleDockable titleDockable5 = new ChartFX.WinForms.TitleDockable();
            this.menu = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveLogToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionToolStripMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.optionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.howToToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.LocationPanel = new System.Windows.Forms.Panel();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripAuthorLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.DisconButton = new System.Windows.Forms.Button();
            this.ConButton = new System.Windows.Forms.Button();
            this.ScanButton = new System.Windows.Forms.Button();
            this.PortBox = new System.Windows.Forms.ComboBox();
            this.gyroGroupBox = new System.Windows.Forms.GroupBox();
            this.GyroZData = new System.Windows.Forms.Label();
            this.GyroYData = new System.Windows.Forms.Label();
            this.GyroXData = new System.Windows.Forms.Label();
            this.accelGroupBox = new System.Windows.Forms.GroupBox();
            this.AccelZData = new System.Windows.Forms.Label();
            this.AccelYData = new System.Windows.Forms.Label();
            this.AccelXData = new System.Windows.Forms.Label();
            this.ChartA = new ChartFX.WinForms.Chart();
            this.ChartP = new ChartFX.WinForms.Chart();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.AngularZData = new System.Windows.Forms.Label();
            this.AngularYData = new System.Windows.Forms.Label();
            this.AngularXData = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.VelocityZData = new System.Windows.Forms.Label();
            this.VelocityYData = new System.Windows.Forms.Label();
            this.VelocityXData = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.PositionZData = new System.Windows.Forms.Label();
            this.PositionYData = new System.Windows.Forms.Label();
            this.PositionXData = new System.Windows.Forms.Label();
            this.MagnetoZData = new System.Windows.Forms.Label();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.MagnetoYData = new System.Windows.Forms.Label();
            this.MagnetoXData = new System.Windows.Forms.Label();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.YawLabel = new System.Windows.Forms.Label();
            this.PitchLabel = new System.Windows.Forms.Label();
            this.RollLabel = new System.Windows.Forms.Label();
            this.YawData = new System.Windows.Forms.Label();
            this.PitchData = new System.Windows.Forms.Label();
            this.RollData = new System.Windows.Forms.Label();
            this.menu.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.gyroGroupBox.SuspendLayout();
            this.accelGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ChartA)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChartP)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.SuspendLayout();
            // 
            // menu
            // 
            this.menu.BackColor = System.Drawing.SystemColors.MenuBar;
            this.menu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.optionToolStripMenu,
            this.helpToolStripMenuItem});
            this.menu.Location = new System.Drawing.Point(0, 0);
            this.menu.Name = "menu";
            this.menu.Size = new System.Drawing.Size(616, 24);
            this.menu.TabIndex = 0;
            this.menu.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveLogToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // saveLogToolStripMenuItem
            // 
            this.saveLogToolStripMenuItem.Name = "saveLogToolStripMenuItem";
            this.saveLogToolStripMenuItem.Size = new System.Drawing.Size(118, 22);
            this.saveLogToolStripMenuItem.Text = "Save Log";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(118, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            // 
            // optionToolStripMenu
            // 
            this.optionToolStripMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.optionToolStripMenuItem});
            this.optionToolStripMenu.Name = "optionToolStripMenu";
            this.optionToolStripMenu.Size = new System.Drawing.Size(51, 20);
            this.optionToolStripMenu.Text = "Option";
            // 
            // optionToolStripMenuItem
            // 
            this.optionToolStripMenuItem.Name = "optionToolStripMenuItem";
            this.optionToolStripMenuItem.Size = new System.Drawing.Size(106, 22);
            this.optionToolStripMenuItem.Text = "Option";
            this.optionToolStripMenuItem.Click += new System.EventHandler(this.OpenOptionForm);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.howToToolStripMenuItem,
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // howToToolStripMenuItem
            // 
            this.howToToolStripMenuItem.Name = "howToToolStripMenuItem";
            this.howToToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.howToToolStripMenuItem.Text = "How To";
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.aboutToolStripMenuItem.Text = "About";
            // 
            // LocationPanel
            // 
            this.LocationPanel.Location = new System.Drawing.Point(12, 27);
            this.LocationPanel.Name = "LocationPanel";
            this.LocationPanel.Size = new System.Drawing.Size(426, 259);
            this.LocationPanel.TabIndex = 1;
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel,
            this.toolStripAuthorLabel});
            this.statusStrip1.Location = new System.Drawing.Point(0, 532);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(616, 22);
            this.statusStrip1.TabIndex = 3;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel
            // 
            this.toolStripStatusLabel.Name = "toolStripStatusLabel";
            this.toolStripStatusLabel.Size = new System.Drawing.Size(59, 17);
            this.toolStripStatusLabel.Text = "Disconnect";
            // 
            // toolStripAuthorLabel
            // 
            this.toolStripAuthorLabel.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.toolStripAuthorLabel.Name = "toolStripAuthorLabel";
            this.toolStripAuthorLabel.Size = new System.Drawing.Size(124, 17);
            this.toolStripAuthorLabel.Text = "(Created by Chon.John)";
            this.toolStripAuthorLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.DisconButton);
            this.groupBox1.Controls.Add(this.ConButton);
            this.groupBox1.Controls.Add(this.ScanButton);
            this.groupBox1.Controls.Add(this.PortBox);
            this.groupBox1.Location = new System.Drawing.Point(444, 27);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(160, 141);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Port Setting";
            // 
            // DisconButton
            // 
            this.DisconButton.Enabled = false;
            this.DisconButton.Location = new System.Drawing.Point(7, 108);
            this.DisconButton.Name = "DisconButton";
            this.DisconButton.Size = new System.Drawing.Size(146, 23);
            this.DisconButton.TabIndex = 3;
            this.DisconButton.Text = "Disconnect";
            this.DisconButton.UseVisualStyleBackColor = true;
            this.DisconButton.Click += new System.EventHandler(this.DisconButton_Click);
            // 
            // ConButton
            // 
            this.ConButton.Location = new System.Drawing.Point(7, 78);
            this.ConButton.Name = "ConButton";
            this.ConButton.Size = new System.Drawing.Size(146, 23);
            this.ConButton.TabIndex = 2;
            this.ConButton.Text = "Connect";
            this.ConButton.UseVisualStyleBackColor = true;
            this.ConButton.Click += new System.EventHandler(this.ConButton_Click);
            // 
            // ScanButton
            // 
            this.ScanButton.Location = new System.Drawing.Point(7, 48);
            this.ScanButton.Name = "ScanButton";
            this.ScanButton.Size = new System.Drawing.Size(146, 23);
            this.ScanButton.TabIndex = 1;
            this.ScanButton.Text = "Port Scan";
            this.ScanButton.UseVisualStyleBackColor = true;
            this.ScanButton.Click += new System.EventHandler(this.ScanButton_Click);
            // 
            // PortBox
            // 
            this.PortBox.FormattingEnabled = true;
            this.PortBox.Location = new System.Drawing.Point(7, 21);
            this.PortBox.Name = "PortBox";
            this.PortBox.Size = new System.Drawing.Size(146, 20);
            this.PortBox.TabIndex = 0;
            // 
            // gyroGroupBox
            // 
            this.gyroGroupBox.Controls.Add(this.GyroZData);
            this.gyroGroupBox.Controls.Add(this.GyroYData);
            this.gyroGroupBox.Controls.Add(this.GyroXData);
            this.gyroGroupBox.Location = new System.Drawing.Point(12, 411);
            this.gyroGroupBox.Name = "gyroGroupBox";
            this.gyroGroupBox.Size = new System.Drawing.Size(100, 102);
            this.gyroGroupBox.TabIndex = 6;
            this.gyroGroupBox.TabStop = false;
            this.gyroGroupBox.Text = "Angular Rate";
            // 
            // GyroZData
            // 
            this.GyroZData.Location = new System.Drawing.Point(20, 76);
            this.GyroZData.Name = "GyroZData";
            this.GyroZData.Size = new System.Drawing.Size(60, 10);
            this.GyroZData.TabIndex = 5;
            this.GyroZData.Text = "-";
            this.GyroZData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // GyroYData
            // 
            this.GyroYData.Location = new System.Drawing.Point(20, 51);
            this.GyroYData.Name = "GyroYData";
            this.GyroYData.Size = new System.Drawing.Size(60, 10);
            this.GyroYData.TabIndex = 4;
            this.GyroYData.Text = "-";
            this.GyroYData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // GyroXData
            // 
            this.GyroXData.Location = new System.Drawing.Point(20, 26);
            this.GyroXData.Name = "GyroXData";
            this.GyroXData.Size = new System.Drawing.Size(60, 10);
            this.GyroXData.TabIndex = 3;
            this.GyroXData.Text = "-";
            this.GyroXData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // accelGroupBox
            // 
            this.accelGroupBox.Controls.Add(this.AccelZData);
            this.accelGroupBox.Controls.Add(this.AccelYData);
            this.accelGroupBox.Controls.Add(this.AccelXData);
            this.accelGroupBox.Location = new System.Drawing.Point(12, 292);
            this.accelGroupBox.Name = "accelGroupBox";
            this.accelGroupBox.Size = new System.Drawing.Size(100, 102);
            this.accelGroupBox.TabIndex = 7;
            this.accelGroupBox.TabStop = false;
            this.accelGroupBox.Text = "Acceleration";
            // 
            // AccelZData
            // 
            this.AccelZData.Location = new System.Drawing.Point(20, 76);
            this.AccelZData.Name = "AccelZData";
            this.AccelZData.Size = new System.Drawing.Size(60, 10);
            this.AccelZData.TabIndex = 5;
            this.AccelZData.Text = "-";
            this.AccelZData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // AccelYData
            // 
            this.AccelYData.Location = new System.Drawing.Point(20, 51);
            this.AccelYData.Name = "AccelYData";
            this.AccelYData.Size = new System.Drawing.Size(60, 10);
            this.AccelYData.TabIndex = 4;
            this.AccelYData.Text = "-";
            this.AccelYData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // AccelXData
            // 
            this.AccelXData.Location = new System.Drawing.Point(20, 26);
            this.AccelXData.Name = "AccelXData";
            this.AccelXData.Size = new System.Drawing.Size(60, 10);
            this.AccelXData.TabIndex = 3;
            this.AccelXData.Text = "-";
            this.AccelXData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // ChartA
            // 
            this.ChartA.AllSeries.MarkerShape = ChartFX.WinForms.MarkerShape.None;
            this.ChartA.AxisX.Grids.Minor.Visible = true;
            this.ChartA.AxisX.Max = 100;
            this.ChartA.AxisX.Min = 0;
            this.ChartA.AxisX.Title.Text = "X Axis";
            this.ChartA.AxisX.Visible = false;
            this.ChartA.AxisY.Max = 500;
            this.ChartA.AxisY.Min = -500;
            this.ChartA.AxisY.Title.Text = "Y Axis";
            this.ChartA.AxisY.Visible = false;
            this.ChartA.BackColor = System.Drawing.SystemColors.Control;
            solidBackground1.AssemblyName = "ChartFX.WinForms.Adornments";
            this.ChartA.Background = solidBackground1;
            this.ChartA.Border = new ChartFX.WinForms.Adornments.SimpleBorder(ChartFX.WinForms.Adornments.SimpleBorderType.None, System.Drawing.Color.FromArgb(((int)(((byte)(109)))), ((int)(((byte)(125)))), ((int)(((byte)(138))))));
            this.ChartA.ForeColor = System.Drawing.Color.Transparent;
            this.ChartA.Highlight.Dimmed = false;
            this.ChartA.Highlight.Enabled = false;
            this.ChartA.LegendBox.Visible = false;
            this.ChartA.Location = new System.Drawing.Point(305, 390);
            this.ChartA.Margin = new System.Windows.Forms.Padding(0);
            this.ChartA.Name = "ChartA";
            this.ChartA.PlotAreaColor = System.Drawing.Color.Transparent;
            this.ChartA.Size = new System.Drawing.Size(328, 150);
            this.ChartA.TabIndex = 8;
            titleDockable1.Dock = ChartFX.WinForms.DockArea.Bottom;
            titleDockable1.Separation = 0;
            titleDockable1.Text = "Angular Velocity (X:Blue, Y:Red, Z:Green)";
            titleDockable1.TextColor = System.Drawing.Color.Black;
            titleDockable2.Text = "Angular (X:Blue, Y:Red, Z:Green)";
            this.ChartA.Titles.AddRange(new ChartFX.WinForms.TitleDockable[] {
            titleDockable1,
            titleDockable2});
            // 
            // ChartP
            // 
            this.ChartP.AllSeries.MarkerShape = ChartFX.WinForms.MarkerShape.None;
            this.ChartP.AxisX.Grids.Minor.Visible = true;
            this.ChartP.AxisX.Max = 100;
            this.ChartP.AxisX.Min = 0;
            this.ChartP.AxisX.Title.Text = "X Axis";
            this.ChartP.AxisX.Visible = false;
            this.ChartP.AxisY.Max = 2;
            this.ChartP.AxisY.Min = -2;
            this.ChartP.AxisY.Title.Text = "Y Axis";
            this.ChartP.AxisY.Visible = false;
            this.ChartP.BackColor = System.Drawing.SystemColors.Control;
            solidBackground2.AssemblyName = "ChartFX.WinForms.Adornments";
            solidBackground2.Color = System.Drawing.SystemColors.Control;
            this.ChartP.Background = solidBackground2;
            this.ChartP.Border = new ChartFX.WinForms.Adornments.SimpleBorder(ChartFX.WinForms.Adornments.SimpleBorderType.None, System.Drawing.SystemColors.ActiveBorder);
            this.ChartP.ForeColor = System.Drawing.Color.Transparent;
            this.ChartP.Highlight.Dimmed = false;
            this.ChartP.Highlight.Enabled = false;
            this.ChartP.LegendBox.Visible = false;
            this.ChartP.Location = new System.Drawing.Point(305, 270);
            this.ChartP.Margin = new System.Windows.Forms.Padding(0);
            this.ChartP.Name = "ChartP";
            this.ChartP.PlotAreaColor = System.Drawing.Color.Transparent;
            this.ChartP.Size = new System.Drawing.Size(328, 150);
            this.ChartP.TabIndex = 9;
            titleDockable3.Dock = ChartFX.WinForms.DockArea.Bottom;
            titleDockable3.Separation = 0;
            titleDockable3.Text = "Accelaration (X:Blue, Y:Red, Z:Green)";
            titleDockable3.TextColor = System.Drawing.Color.Black;
            titleDockable4.Text = "Velocity (X:Blue, Y:Red, Z:Green)";
            titleDockable5.Text = "Position (X:Blue, Y:Red, Z:Green)";
            this.ChartP.Titles.AddRange(new ChartFX.WinForms.TitleDockable[] {
            titleDockable3,
            titleDockable4,
            titleDockable5});
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.AngularZData);
            this.groupBox2.Controls.Add(this.AngularYData);
            this.groupBox2.Controls.Add(this.AngularXData);
            this.groupBox2.Location = new System.Drawing.Point(118, 411);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(100, 102);
            this.groupBox2.TabIndex = 9;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Angular";
            // 
            // AngularZData
            // 
            this.AngularZData.Location = new System.Drawing.Point(20, 76);
            this.AngularZData.Name = "AngularZData";
            this.AngularZData.Size = new System.Drawing.Size(60, 10);
            this.AngularZData.TabIndex = 11;
            this.AngularZData.Text = "-";
            this.AngularZData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // AngularYData
            // 
            this.AngularYData.Location = new System.Drawing.Point(20, 51);
            this.AngularYData.Name = "AngularYData";
            this.AngularYData.Size = new System.Drawing.Size(60, 10);
            this.AngularYData.TabIndex = 10;
            this.AngularYData.Text = "-";
            this.AngularYData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // AngularXData
            // 
            this.AngularXData.Location = new System.Drawing.Point(20, 26);
            this.AngularXData.Name = "AngularXData";
            this.AngularXData.Size = new System.Drawing.Size(60, 10);
            this.AngularXData.TabIndex = 9;
            this.AngularXData.Text = "-";
            this.AngularXData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.VelocityZData);
            this.groupBox3.Controls.Add(this.VelocityYData);
            this.groupBox3.Controls.Add(this.VelocityXData);
            this.groupBox3.Location = new System.Drawing.Point(118, 292);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(100, 102);
            this.groupBox3.TabIndex = 12;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Velocity";
            // 
            // VelocityZData
            // 
            this.VelocityZData.Location = new System.Drawing.Point(20, 76);
            this.VelocityZData.Name = "VelocityZData";
            this.VelocityZData.Size = new System.Drawing.Size(60, 10);
            this.VelocityZData.TabIndex = 11;
            this.VelocityZData.Text = "-";
            this.VelocityZData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // VelocityYData
            // 
            this.VelocityYData.Location = new System.Drawing.Point(20, 51);
            this.VelocityYData.Name = "VelocityYData";
            this.VelocityYData.Size = new System.Drawing.Size(60, 10);
            this.VelocityYData.TabIndex = 10;
            this.VelocityYData.Text = "-";
            this.VelocityYData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // VelocityXData
            // 
            this.VelocityXData.Location = new System.Drawing.Point(20, 26);
            this.VelocityXData.Name = "VelocityXData";
            this.VelocityXData.Size = new System.Drawing.Size(60, 10);
            this.VelocityXData.TabIndex = 9;
            this.VelocityXData.Text = "-";
            this.VelocityXData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.PositionZData);
            this.groupBox4.Controls.Add(this.PositionYData);
            this.groupBox4.Controls.Add(this.PositionXData);
            this.groupBox4.Location = new System.Drawing.Point(224, 292);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(100, 102);
            this.groupBox4.TabIndex = 12;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Position";
            // 
            // PositionZData
            // 
            this.PositionZData.Location = new System.Drawing.Point(20, 76);
            this.PositionZData.Name = "PositionZData";
            this.PositionZData.Size = new System.Drawing.Size(60, 10);
            this.PositionZData.TabIndex = 14;
            this.PositionZData.Text = "-";
            this.PositionZData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // PositionYData
            // 
            this.PositionYData.Location = new System.Drawing.Point(20, 51);
            this.PositionYData.Name = "PositionYData";
            this.PositionYData.Size = new System.Drawing.Size(60, 10);
            this.PositionYData.TabIndex = 13;
            this.PositionYData.Text = "-";
            this.PositionYData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // PositionXData
            // 
            this.PositionXData.Location = new System.Drawing.Point(20, 26);
            this.PositionXData.Name = "PositionXData";
            this.PositionXData.Size = new System.Drawing.Size(60, 10);
            this.PositionXData.TabIndex = 12;
            this.PositionXData.Text = "-";
            this.PositionXData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // MagnetoZData
            // 
            this.MagnetoZData.Location = new System.Drawing.Point(20, 76);
            this.MagnetoZData.Name = "MagnetoZData";
            this.MagnetoZData.Size = new System.Drawing.Size(60, 10);
            this.MagnetoZData.TabIndex = 14;
            this.MagnetoZData.Text = "-";
            this.MagnetoZData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.MagnetoZData);
            this.groupBox5.Controls.Add(this.MagnetoYData);
            this.groupBox5.Controls.Add(this.MagnetoXData);
            this.groupBox5.Location = new System.Drawing.Point(224, 411);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(100, 102);
            this.groupBox5.TabIndex = 13;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Magnetic";
            // 
            // MagnetoYData
            // 
            this.MagnetoYData.Location = new System.Drawing.Point(20, 51);
            this.MagnetoYData.Name = "MagnetoYData";
            this.MagnetoYData.Size = new System.Drawing.Size(60, 10);
            this.MagnetoYData.TabIndex = 13;
            this.MagnetoYData.Text = "-";
            this.MagnetoYData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // MagnetoXData
            // 
            this.MagnetoXData.Location = new System.Drawing.Point(20, 26);
            this.MagnetoXData.Name = "MagnetoXData";
            this.MagnetoXData.Size = new System.Drawing.Size(60, 10);
            this.MagnetoXData.TabIndex = 12;
            this.MagnetoXData.Text = "-";
            this.MagnetoXData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.YawLabel);
            this.groupBox6.Controls.Add(this.PitchLabel);
            this.groupBox6.Controls.Add(this.RollLabel);
            this.groupBox6.Controls.Add(this.YawData);
            this.groupBox6.Controls.Add(this.PitchData);
            this.groupBox6.Controls.Add(this.RollData);
            this.groupBox6.Location = new System.Drawing.Point(444, 174);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(160, 112);
            this.groupBox6.TabIndex = 15;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Orientation";
            // 
            // YawLabel
            // 
            this.YawLabel.Location = new System.Drawing.Point(16, 80);
            this.YawLabel.Name = "YawLabel";
            this.YawLabel.Size = new System.Drawing.Size(60, 10);
            this.YawLabel.TabIndex = 17;
            this.YawLabel.Text = "Z";
            this.YawLabel.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // PitchLabel
            // 
            this.PitchLabel.Location = new System.Drawing.Point(16, 55);
            this.PitchLabel.Name = "PitchLabel";
            this.PitchLabel.Size = new System.Drawing.Size(60, 10);
            this.PitchLabel.TabIndex = 16;
            this.PitchLabel.Text = "Y";
            this.PitchLabel.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // RollLabel
            // 
            this.RollLabel.Location = new System.Drawing.Point(16, 30);
            this.RollLabel.Name = "RollLabel";
            this.RollLabel.Size = new System.Drawing.Size(60, 10);
            this.RollLabel.TabIndex = 15;
            this.RollLabel.Text = "X";
            this.RollLabel.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // YawData
            // 
            this.YawData.Location = new System.Drawing.Point(85, 80);
            this.YawData.Name = "YawData";
            this.YawData.Size = new System.Drawing.Size(60, 10);
            this.YawData.TabIndex = 14;
            this.YawData.Text = "-";
            this.YawData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // PitchData
            // 
            this.PitchData.Location = new System.Drawing.Point(85, 55);
            this.PitchData.Name = "PitchData";
            this.PitchData.Size = new System.Drawing.Size(60, 10);
            this.PitchData.TabIndex = 13;
            this.PitchData.Text = "-";
            this.PitchData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // RollData
            // 
            this.RollData.Location = new System.Drawing.Point(85, 30);
            this.RollData.Name = "RollData";
            this.RollData.Size = new System.Drawing.Size(60, 10);
            this.RollData.TabIndex = 12;
            this.RollData.Text = "-";
            this.RollData.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // BaseForm
            // 
            this.AccessibleName = "baseForm";
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(616, 554);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.accelGroupBox);
            this.Controls.Add(this.gyroGroupBox);
            this.Controls.Add(this.LocationPanel);
            this.Controls.Add(this.menu);
            this.Controls.Add(this.ChartP);
            this.Controls.Add(this.ChartA);
            this.KeyPreview = true;
            this.MainMenuStrip = this.menu;
            this.Name = "BaseForm";
            this.Text = "Initial Navigation System";
            this.Load += new System.EventHandler(this.BaseForm_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.BaseForm_KeyDown);
            this.menu.ResumeLayout(false);
            this.menu.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.gyroGroupBox.ResumeLayout(false);
            this.accelGroupBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.ChartA)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ChartP)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menu;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveLogToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem howToToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.Panel LocationPanel;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel;
        private System.Windows.Forms.ToolStripStatusLabel toolStripAuthorLabel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button DisconButton;
        private System.Windows.Forms.Button ConButton;
        private System.Windows.Forms.Button ScanButton;
        private System.Windows.Forms.ComboBox PortBox;
        private System.Windows.Forms.GroupBox gyroGroupBox;
        private System.Windows.Forms.Label GyroZData;
        private System.Windows.Forms.Label GyroYData;
        private System.Windows.Forms.Label GyroXData;
        private System.Windows.Forms.GroupBox accelGroupBox;
        private System.Windows.Forms.Label AccelZData;
        private System.Windows.Forms.Label AccelYData;
        private System.Windows.Forms.Label AccelXData;
        private System.Windows.Forms.ToolStripMenuItem optionToolStripMenu;
        private ChartFX.WinForms.Chart ChartA;
        private ChartFX.WinForms.Chart ChartP;
        private System.Windows.Forms.ToolStripMenuItem optionToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label VelocityZData;
        private System.Windows.Forms.Label VelocityYData;
        private System.Windows.Forms.Label VelocityXData;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label AngularZData;
        private System.Windows.Forms.Label AngularYData;
        private System.Windows.Forms.Label AngularXData;
        private System.Windows.Forms.Label PositionZData;
        private System.Windows.Forms.Label PositionYData;
        private System.Windows.Forms.Label PositionXData;
        private System.Windows.Forms.Label MagnetoZData;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Label MagnetoYData;
        private System.Windows.Forms.Label MagnetoXData;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Label YawData;
        private System.Windows.Forms.Label PitchData;
        private System.Windows.Forms.Label RollData;
        private System.Windows.Forms.Label YawLabel;
        private System.Windows.Forms.Label PitchLabel;
        private System.Windows.Forms.Label RollLabel;
    }
}

