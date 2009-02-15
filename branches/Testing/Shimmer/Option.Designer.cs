namespace SensorNode
{
    partial class OptionForm
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.DataBufferSize = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.DeviceBox = new System.Windows.Forms.ComboBox();
            this.NumberOfPackage = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.NumberOfData = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.GyroAnchorWindow = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.GyroCalibration = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.AccelAnchorWindow = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.AccelCalibration = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.confirmButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.MagAnchorWindow = new System.Windows.Forms.NumericUpDown();
            this.label9 = new System.Windows.Forms.Label();
            this.MagCalibration = new System.Windows.Forms.NumericUpDown();
            this.label10 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.DataBufferSize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfPackage)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfData)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GyroAnchorWindow)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GyroCalibration)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AccelAnchorWindow)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.AccelCalibration)).BeginInit();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MagAnchorWindow)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.MagCalibration)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox1.Controls.Add(this.DataBufferSize);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.DeviceBox);
            this.groupBox1.Controls.Add(this.NumberOfPackage);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.NumberOfData);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(277, 149);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Common";
            // 
            // DataBufferSize
            // 
            this.DataBufferSize.Increment = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.DataBufferSize.Location = new System.Drawing.Point(179, 50);
            this.DataBufferSize.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.DataBufferSize.Minimum = new decimal(new int[] {
            64,
            0,
            0,
            0});
            this.DataBufferSize.Name = "DataBufferSize";
            this.DataBufferSize.Size = new System.Drawing.Size(80, 21);
            this.DataBufferSize.TabIndex = 10;
            this.DataBufferSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.DataBufferSize.Value = new decimal(new int[] {
            64,
            0,
            0,
            0});
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(20, 50);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(120, 20);
            this.label6.TabIndex = 9;
            this.label6.Text = "Data Buffer Size";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // DeviceBox
            // 
            this.DeviceBox.FormattingEnabled = true;
            this.DeviceBox.Items.AddRange(new object[] {
            "SensorNode",
            "Wii-Mote",
            "MotionNode"});
            this.DeviceBox.Location = new System.Drawing.Point(179, 20);
            this.DeviceBox.Name = "DeviceBox";
            this.DeviceBox.Size = new System.Drawing.Size(80, 20);
            this.DeviceBox.TabIndex = 8;
            // 
            // NumberOfPackage
            // 
            this.NumberOfPackage.Location = new System.Drawing.Point(179, 110);
            this.NumberOfPackage.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.NumberOfPackage.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.NumberOfPackage.Name = "NumberOfPackage";
            this.NumberOfPackage.Size = new System.Drawing.Size(80, 21);
            this.NumberOfPackage.TabIndex = 7;
            this.NumberOfPackage.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.NumberOfPackage.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(20, 110);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(120, 20);
            this.label3.TabIndex = 6;
            this.label3.Text = "Number of Package";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // NumberOfData
            // 
            this.NumberOfData.Location = new System.Drawing.Point(179, 80);
            this.NumberOfData.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.NumberOfData.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.NumberOfData.Name = "NumberOfData";
            this.NumberOfData.Size = new System.Drawing.Size(80, 21);
            this.NumberOfData.TabIndex = 5;
            this.NumberOfData.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.NumberOfData.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(20, 80);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(120, 20);
            this.label2.TabIndex = 4;
            this.label2.Text = "Number of Data";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(20, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(120, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "Device";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox2
            // 
            this.groupBox2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox2.Controls.Add(this.GyroAnchorWindow);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.GyroCalibration);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Location = new System.Drawing.Point(12, 167);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(277, 87);
            this.groupBox2.TabIndex = 9;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Gyroscope";
            // 
            // GyroAnchorWindow
            // 
            this.GyroAnchorWindow.Increment = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.GyroAnchorWindow.Location = new System.Drawing.Point(179, 50);
            this.GyroAnchorWindow.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.GyroAnchorWindow.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.GyroAnchorWindow.Name = "GyroAnchorWindow";
            this.GyroAnchorWindow.Size = new System.Drawing.Size(80, 21);
            this.GyroAnchorWindow.TabIndex = 7;
            this.GyroAnchorWindow.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.GyroAnchorWindow.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(20, 50);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(140, 20);
            this.label4.TabIndex = 6;
            this.label4.Text = "Anchor Window Size";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // GyroCalibration
            // 
            this.GyroCalibration.Increment = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.GyroCalibration.Location = new System.Drawing.Point(179, 20);
            this.GyroCalibration.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.GyroCalibration.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.GyroCalibration.Name = "GyroCalibration";
            this.GyroCalibration.Size = new System.Drawing.Size(80, 21);
            this.GyroCalibration.TabIndex = 5;
            this.GyroCalibration.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.GyroCalibration.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(20, 20);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(140, 20);
            this.label5.TabIndex = 4;
            this.label5.Text = "Calibration Count";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox3
            // 
            this.groupBox3.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox3.Controls.Add(this.AccelAnchorWindow);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.AccelCalibration);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Location = new System.Drawing.Point(12, 260);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(277, 87);
            this.groupBox3.TabIndex = 10;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Accelerometer";
            // 
            // AccelAnchorWindow
            // 
            this.AccelAnchorWindow.Increment = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.AccelAnchorWindow.Location = new System.Drawing.Point(179, 50);
            this.AccelAnchorWindow.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.AccelAnchorWindow.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.AccelAnchorWindow.Name = "AccelAnchorWindow";
            this.AccelAnchorWindow.Size = new System.Drawing.Size(80, 21);
            this.AccelAnchorWindow.TabIndex = 7;
            this.AccelAnchorWindow.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.AccelAnchorWindow.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(20, 50);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(140, 20);
            this.label7.TabIndex = 6;
            this.label7.Text = "Anchor Window Size";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // AccelCalibration
            // 
            this.AccelCalibration.Increment = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.AccelCalibration.Location = new System.Drawing.Point(179, 20);
            this.AccelCalibration.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.AccelCalibration.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.AccelCalibration.Name = "AccelCalibration";
            this.AccelCalibration.Size = new System.Drawing.Size(80, 21);
            this.AccelCalibration.TabIndex = 5;
            this.AccelCalibration.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.AccelCalibration.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(20, 20);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(140, 20);
            this.label8.TabIndex = 4;
            this.label8.Text = "Calibration Count";
            this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // confirmButton
            // 
            this.confirmButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.confirmButton.Location = new System.Drawing.Point(40, 452);
            this.confirmButton.Name = "confirmButton";
            this.confirmButton.Size = new System.Drawing.Size(100, 25);
            this.confirmButton.TabIndex = 11;
            this.confirmButton.Text = "Confirm";
            this.confirmButton.UseVisualStyleBackColor = true;
            this.confirmButton.Click += new System.EventHandler(this.confirmButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(160, 452);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(100, 25);
            this.cancelButton.TabIndex = 12;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.groupBox4.Controls.Add(this.MagAnchorWindow);
            this.groupBox4.Controls.Add(this.label9);
            this.groupBox4.Controls.Add(this.MagCalibration);
            this.groupBox4.Controls.Add(this.label10);
            this.groupBox4.Location = new System.Drawing.Point(12, 353);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(277, 87);
            this.groupBox4.TabIndex = 11;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Magnetometer";
            // 
            // MagAnchorWindow
            // 
            this.MagAnchorWindow.Increment = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.MagAnchorWindow.Location = new System.Drawing.Point(179, 50);
            this.MagAnchorWindow.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.MagAnchorWindow.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.MagAnchorWindow.Name = "MagAnchorWindow";
            this.MagAnchorWindow.Size = new System.Drawing.Size(80, 21);
            this.MagAnchorWindow.TabIndex = 7;
            this.MagAnchorWindow.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.MagAnchorWindow.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(20, 50);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(140, 20);
            this.label9.TabIndex = 6;
            this.label9.Text = "Anchor Window Size";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // MagCalibration
            // 
            this.MagCalibration.Increment = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.MagCalibration.Location = new System.Drawing.Point(179, 20);
            this.MagCalibration.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.MagCalibration.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.MagCalibration.Name = "MagCalibration";
            this.MagCalibration.Size = new System.Drawing.Size(80, 21);
            this.MagCalibration.TabIndex = 5;
            this.MagCalibration.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.MagCalibration.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // label10
            // 
            this.label10.Location = new System.Drawing.Point(20, 20);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(140, 20);
            this.label10.TabIndex = 4;
            this.label10.Text = "Calibration Count";
            this.label10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // OptionForm
            // 
            this.AccessibleName = "optionForm";
            this.AccessibleRole = System.Windows.Forms.AccessibleRole.Dialog;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(301, 490);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.confirmButton);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "OptionForm";
            this.Text = "Option";
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.DataBufferSize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfPackage)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.NumberOfData)).EndInit();
            this.groupBox2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.GyroAnchorWindow)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GyroCalibration)).EndInit();
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.AccelAnchorWindow)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.AccelCalibration)).EndInit();
            this.groupBox4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.MagAnchorWindow)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.MagCalibration)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown NumberOfData;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox DeviceBox;
        private System.Windows.Forms.NumericUpDown NumberOfPackage;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.NumericUpDown GyroAnchorWindow;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown GyroCalibration;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown DataBufferSize;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.NumericUpDown AccelAnchorWindow;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown AccelCalibration;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button confirmButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.NumericUpDown MagAnchorWindow;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown MagCalibration;
        private System.Windows.Forms.Label label10;
    }
}