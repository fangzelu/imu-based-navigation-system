namespace StepCount
{
    partial class ControlPanel
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
            this.Connect_Bt = new System.Windows.Forms.Button();
            this.Disconnect_Bt = new System.Windows.Forms.Button();
            this.Send_Bt = new System.Windows.Forms.Button();
            this.Stop_Bt = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.Start_Bt = new System.Windows.Forms.Button();
            this.Pause_Bt = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.groupBox11 = new System.Windows.Forms.GroupBox();
            this.mHeadZY = new System.Windows.Forms.Label();
            this.groupBox10 = new System.Windows.Forms.GroupBox();
            this.mMagZ = new System.Windows.Forms.Label();
            this.mMagY = new System.Windows.Forms.Label();
            this.mMagX = new System.Windows.Forms.Label();
            this.groupBox9 = new System.Windows.Forms.GroupBox();
            this.mAccPosZ = new System.Windows.Forms.Label();
            this.mAccPosY = new System.Windows.Forms.Label();
            this.mAccPosX = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.mAccAvgZ = new System.Windows.Forms.Label();
            this.mAccAvgY = new System.Windows.Forms.Label();
            this.mAccAvgX = new System.Windows.Forms.Label();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.mAccDevZ = new System.Windows.Forms.Label();
            this.mAccDevY = new System.Windows.Forms.Label();
            this.mAccDevX = new System.Windows.Forms.Label();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.mAccZ = new System.Windows.Forms.Label();
            this.mAccY = new System.Windows.Forms.Label();
            this.mAccX = new System.Windows.Forms.Label();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.mAccRawZ = new System.Windows.Forms.Label();
            this.mAccRawY = new System.Windows.Forms.Label();
            this.mAccRawX = new System.Windows.Forms.Label();
            this.groupBox12 = new System.Windows.Forms.GroupBox();
            this.mLocalZ = new System.Windows.Forms.Label();
            this.mLocalY = new System.Windows.Forms.Label();
            this.mLocalX = new System.Windows.Forms.Label();
            this.groupBox13 = new System.Windows.Forms.GroupBox();
            this.mWorldZ = new System.Windows.Forms.Label();
            this.mWorldY = new System.Windows.Forms.Label();
            this.mWorldX = new System.Windows.Forms.Label();
            this.groupBox14 = new System.Windows.Forms.GroupBox();
            this.mStageZ = new System.Windows.Forms.Label();
            this.mStageY = new System.Windows.Forms.Label();
            this.mStageX = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox11.SuspendLayout();
            this.groupBox10.SuspendLayout();
            this.groupBox9.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox12.SuspendLayout();
            this.groupBox13.SuspendLayout();
            this.groupBox14.SuspendLayout();
            this.SuspendLayout();
            // 
            // Connect_Bt
            // 
            this.Connect_Bt.Location = new System.Drawing.Point(18, 23);
            this.Connect_Bt.Name = "Connect_Bt";
            this.Connect_Bt.Size = new System.Drawing.Size(91, 23);
            this.Connect_Bt.TabIndex = 0;
            this.Connect_Bt.Text = "Connect";
            this.Connect_Bt.UseVisualStyleBackColor = true;
            this.Connect_Bt.Click += new System.EventHandler(this.Connect_Bt_Click);
            // 
            // Disconnect_Bt
            // 
            this.Disconnect_Bt.Location = new System.Drawing.Point(18, 57);
            this.Disconnect_Bt.Name = "Disconnect_Bt";
            this.Disconnect_Bt.Size = new System.Drawing.Size(91, 23);
            this.Disconnect_Bt.TabIndex = 1;
            this.Disconnect_Bt.Text = "Disconnect";
            this.Disconnect_Bt.UseVisualStyleBackColor = true;
            this.Disconnect_Bt.Click += new System.EventHandler(this.Disconnect_Bt_Click);
            // 
            // Send_Bt
            // 
            this.Send_Bt.Location = new System.Drawing.Point(18, 23);
            this.Send_Bt.Name = "Send_Bt";
            this.Send_Bt.Size = new System.Drawing.Size(75, 23);
            this.Send_Bt.TabIndex = 2;
            this.Send_Bt.Text = "Send";
            this.Send_Bt.UseVisualStyleBackColor = true;
            // 
            // Stop_Bt
            // 
            this.Stop_Bt.Location = new System.Drawing.Point(18, 57);
            this.Stop_Bt.Name = "Stop_Bt";
            this.Stop_Bt.Size = new System.Drawing.Size(75, 23);
            this.Stop_Bt.TabIndex = 3;
            this.Stop_Bt.Text = "Stop";
            this.Stop_Bt.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.Connect_Bt);
            this.groupBox1.Controls.Add(this.Disconnect_Bt);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(128, 95);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Hardware";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.Send_Bt);
            this.groupBox2.Controls.Add(this.Stop_Bt);
            this.groupBox2.Location = new System.Drawing.Point(146, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(111, 95);
            this.groupBox2.TabIndex = 7;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Socket";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.Start_Bt);
            this.groupBox3.Controls.Add(this.Pause_Bt);
            this.groupBox3.Location = new System.Drawing.Point(263, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(111, 95);
            this.groupBox3.TabIndex = 8;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Log";
            // 
            // Start_Bt
            // 
            this.Start_Bt.Location = new System.Drawing.Point(18, 23);
            this.Start_Bt.Name = "Start_Bt";
            this.Start_Bt.Size = new System.Drawing.Size(75, 23);
            this.Start_Bt.TabIndex = 2;
            this.Start_Bt.Text = "Start";
            this.Start_Bt.UseVisualStyleBackColor = true;
            this.Start_Bt.Click += new System.EventHandler(this.Start_Bt_Click);
            // 
            // Pause_Bt
            // 
            this.Pause_Bt.Location = new System.Drawing.Point(18, 57);
            this.Pause_Bt.Name = "Pause_Bt";
            this.Pause_Bt.Size = new System.Drawing.Size(75, 23);
            this.Pause_Bt.TabIndex = 3;
            this.Pause_Bt.Text = "Pause";
            this.Pause_Bt.UseVisualStyleBackColor = true;
            this.Pause_Bt.Click += new System.EventHandler(this.Pause_Bt_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.groupBox14);
            this.groupBox4.Controls.Add(this.groupBox13);
            this.groupBox4.Controls.Add(this.groupBox12);
            this.groupBox4.Controls.Add(this.groupBox11);
            this.groupBox4.Controls.Add(this.groupBox10);
            this.groupBox4.Controls.Add(this.groupBox9);
            this.groupBox4.Controls.Add(this.groupBox8);
            this.groupBox4.Controls.Add(this.groupBox7);
            this.groupBox4.Controls.Add(this.groupBox6);
            this.groupBox4.Controls.Add(this.groupBox5);
            this.groupBox4.Location = new System.Drawing.Point(12, 114);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(430, 216);
            this.groupBox4.TabIndex = 9;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "MotionNode";
            // 
            // groupBox11
            // 
            this.groupBox11.Controls.Add(this.mHeadZY);
            this.groupBox11.Location = new System.Drawing.Point(91, 118);
            this.groupBox11.Name = "groupBox11";
            this.groupBox11.Size = new System.Drawing.Size(78, 91);
            this.groupBox11.TabIndex = 5;
            this.groupBox11.TabStop = false;
            this.groupBox11.Text = "Heading";
            // 
            // mHeadZY
            // 
            this.mHeadZY.Location = new System.Drawing.Point(14, 44);
            this.mHeadZY.Name = "mHeadZY";
            this.mHeadZY.Size = new System.Drawing.Size(50, 13);
            this.mHeadZY.TabIndex = 1;
            this.mHeadZY.Text = "-";
            this.mHeadZY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox10
            // 
            this.groupBox10.Controls.Add(this.mMagZ);
            this.groupBox10.Controls.Add(this.mMagY);
            this.groupBox10.Controls.Add(this.mMagX);
            this.groupBox10.Location = new System.Drawing.Point(7, 118);
            this.groupBox10.Name = "groupBox10";
            this.groupBox10.Size = new System.Drawing.Size(78, 91);
            this.groupBox10.TabIndex = 4;
            this.groupBox10.TabStop = false;
            this.groupBox10.Text = "Mag";
            // 
            // mMagZ
            // 
            this.mMagZ.Location = new System.Drawing.Point(14, 65);
            this.mMagZ.Name = "mMagZ";
            this.mMagZ.Size = new System.Drawing.Size(50, 13);
            this.mMagZ.TabIndex = 2;
            this.mMagZ.Text = "-";
            this.mMagZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mMagY
            // 
            this.mMagY.Location = new System.Drawing.Point(14, 44);
            this.mMagY.Name = "mMagY";
            this.mMagY.Size = new System.Drawing.Size(50, 13);
            this.mMagY.TabIndex = 1;
            this.mMagY.Text = "-";
            this.mMagY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mMagX
            // 
            this.mMagX.Location = new System.Drawing.Point(14, 22);
            this.mMagX.Name = "mMagX";
            this.mMagX.Size = new System.Drawing.Size(50, 13);
            this.mMagX.TabIndex = 0;
            this.mMagX.Text = "-";
            this.mMagX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox9
            // 
            this.groupBox9.Controls.Add(this.mAccPosZ);
            this.groupBox9.Controls.Add(this.mAccPosY);
            this.groupBox9.Controls.Add(this.mAccPosX);
            this.groupBox9.Location = new System.Drawing.Point(343, 21);
            this.groupBox9.Name = "groupBox9";
            this.groupBox9.Size = new System.Drawing.Size(78, 91);
            this.groupBox9.TabIndex = 6;
            this.groupBox9.TabStop = false;
            this.groupBox9.Text = "Pos";
            // 
            // mAccPosZ
            // 
            this.mAccPosZ.Location = new System.Drawing.Point(14, 65);
            this.mAccPosZ.Name = "mAccPosZ";
            this.mAccPosZ.Size = new System.Drawing.Size(50, 13);
            this.mAccPosZ.TabIndex = 2;
            this.mAccPosZ.Text = "-";
            this.mAccPosZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccPosY
            // 
            this.mAccPosY.Location = new System.Drawing.Point(14, 44);
            this.mAccPosY.Name = "mAccPosY";
            this.mAccPosY.Size = new System.Drawing.Size(50, 13);
            this.mAccPosY.TabIndex = 1;
            this.mAccPosY.Text = "-";
            this.mAccPosY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccPosX
            // 
            this.mAccPosX.Location = new System.Drawing.Point(14, 22);
            this.mAccPosX.Name = "mAccPosX";
            this.mAccPosX.Size = new System.Drawing.Size(50, 13);
            this.mAccPosX.TabIndex = 0;
            this.mAccPosX.Text = "-";
            this.mAccPosX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.mAccAvgZ);
            this.groupBox8.Controls.Add(this.mAccAvgY);
            this.groupBox8.Controls.Add(this.mAccAvgX);
            this.groupBox8.Location = new System.Drawing.Point(259, 21);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(78, 91);
            this.groupBox8.TabIndex = 5;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Avg";
            // 
            // mAccAvgZ
            // 
            this.mAccAvgZ.Location = new System.Drawing.Point(14, 65);
            this.mAccAvgZ.Name = "mAccAvgZ";
            this.mAccAvgZ.Size = new System.Drawing.Size(50, 13);
            this.mAccAvgZ.TabIndex = 2;
            this.mAccAvgZ.Text = "-";
            this.mAccAvgZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccAvgY
            // 
            this.mAccAvgY.Location = new System.Drawing.Point(14, 44);
            this.mAccAvgY.Name = "mAccAvgY";
            this.mAccAvgY.Size = new System.Drawing.Size(50, 13);
            this.mAccAvgY.TabIndex = 1;
            this.mAccAvgY.Text = "-";
            this.mAccAvgY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccAvgX
            // 
            this.mAccAvgX.Location = new System.Drawing.Point(14, 22);
            this.mAccAvgX.Name = "mAccAvgX";
            this.mAccAvgX.Size = new System.Drawing.Size(50, 13);
            this.mAccAvgX.TabIndex = 0;
            this.mAccAvgX.Text = "-";
            this.mAccAvgX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.mAccDevZ);
            this.groupBox7.Controls.Add(this.mAccDevY);
            this.groupBox7.Controls.Add(this.mAccDevX);
            this.groupBox7.Location = new System.Drawing.Point(175, 21);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(78, 91);
            this.groupBox7.TabIndex = 4;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Dev";
            // 
            // mAccDevZ
            // 
            this.mAccDevZ.Location = new System.Drawing.Point(14, 65);
            this.mAccDevZ.Name = "mAccDevZ";
            this.mAccDevZ.Size = new System.Drawing.Size(50, 13);
            this.mAccDevZ.TabIndex = 2;
            this.mAccDevZ.Text = "-";
            this.mAccDevZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccDevY
            // 
            this.mAccDevY.Location = new System.Drawing.Point(14, 44);
            this.mAccDevY.Name = "mAccDevY";
            this.mAccDevY.Size = new System.Drawing.Size(50, 13);
            this.mAccDevY.TabIndex = 1;
            this.mAccDevY.Text = "-";
            this.mAccDevY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccDevX
            // 
            this.mAccDevX.Location = new System.Drawing.Point(14, 22);
            this.mAccDevX.Name = "mAccDevX";
            this.mAccDevX.Size = new System.Drawing.Size(50, 13);
            this.mAccDevX.TabIndex = 0;
            this.mAccDevX.Text = "-";
            this.mAccDevX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.mAccZ);
            this.groupBox6.Controls.Add(this.mAccY);
            this.groupBox6.Controls.Add(this.mAccX);
            this.groupBox6.Location = new System.Drawing.Point(91, 21);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(78, 91);
            this.groupBox6.TabIndex = 3;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Acc";
            // 
            // mAccZ
            // 
            this.mAccZ.Location = new System.Drawing.Point(14, 65);
            this.mAccZ.Name = "mAccZ";
            this.mAccZ.Size = new System.Drawing.Size(50, 13);
            this.mAccZ.TabIndex = 2;
            this.mAccZ.Text = "-";
            this.mAccZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccY
            // 
            this.mAccY.Location = new System.Drawing.Point(14, 44);
            this.mAccY.Name = "mAccY";
            this.mAccY.Size = new System.Drawing.Size(50, 13);
            this.mAccY.TabIndex = 1;
            this.mAccY.Text = "-";
            this.mAccY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccX
            // 
            this.mAccX.Location = new System.Drawing.Point(14, 22);
            this.mAccX.Name = "mAccX";
            this.mAccX.Size = new System.Drawing.Size(50, 13);
            this.mAccX.TabIndex = 0;
            this.mAccX.Text = "-";
            this.mAccX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.mAccRawZ);
            this.groupBox5.Controls.Add(this.mAccRawY);
            this.groupBox5.Controls.Add(this.mAccRawX);
            this.groupBox5.Location = new System.Drawing.Point(7, 21);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(78, 91);
            this.groupBox5.TabIndex = 0;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Acc Raw";
            // 
            // mAccRawZ
            // 
            this.mAccRawZ.Location = new System.Drawing.Point(14, 65);
            this.mAccRawZ.Name = "mAccRawZ";
            this.mAccRawZ.Size = new System.Drawing.Size(50, 13);
            this.mAccRawZ.TabIndex = 2;
            this.mAccRawZ.Text = "-";
            this.mAccRawZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccRawY
            // 
            this.mAccRawY.Location = new System.Drawing.Point(14, 44);
            this.mAccRawY.Name = "mAccRawY";
            this.mAccRawY.Size = new System.Drawing.Size(50, 13);
            this.mAccRawY.TabIndex = 1;
            this.mAccRawY.Text = "-";
            this.mAccRawY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mAccRawX
            // 
            this.mAccRawX.Location = new System.Drawing.Point(14, 22);
            this.mAccRawX.Name = "mAccRawX";
            this.mAccRawX.Size = new System.Drawing.Size(50, 13);
            this.mAccRawX.TabIndex = 0;
            this.mAccRawX.Text = "-";
            this.mAccRawX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox12
            // 
            this.groupBox12.Controls.Add(this.mLocalZ);
            this.groupBox12.Controls.Add(this.mLocalY);
            this.groupBox12.Controls.Add(this.mLocalX);
            this.groupBox12.Location = new System.Drawing.Point(175, 118);
            this.groupBox12.Name = "groupBox12";
            this.groupBox12.Size = new System.Drawing.Size(78, 91);
            this.groupBox12.TabIndex = 5;
            this.groupBox12.TabStop = false;
            this.groupBox12.Text = "Local";
            // 
            // mLocalZ
            // 
            this.mLocalZ.Location = new System.Drawing.Point(14, 65);
            this.mLocalZ.Name = "mLocalZ";
            this.mLocalZ.Size = new System.Drawing.Size(50, 13);
            this.mLocalZ.TabIndex = 2;
            this.mLocalZ.Text = "-";
            this.mLocalZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mLocalY
            // 
            this.mLocalY.Location = new System.Drawing.Point(14, 44);
            this.mLocalY.Name = "mLocalY";
            this.mLocalY.Size = new System.Drawing.Size(50, 13);
            this.mLocalY.TabIndex = 1;
            this.mLocalY.Text = "-";
            this.mLocalY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mLocalX
            // 
            this.mLocalX.Location = new System.Drawing.Point(14, 22);
            this.mLocalX.Name = "mLocalX";
            this.mLocalX.Size = new System.Drawing.Size(50, 13);
            this.mLocalX.TabIndex = 0;
            this.mLocalX.Text = "-";
            this.mLocalX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox13
            // 
            this.groupBox13.Controls.Add(this.mWorldZ);
            this.groupBox13.Controls.Add(this.mWorldY);
            this.groupBox13.Controls.Add(this.mWorldX);
            this.groupBox13.Location = new System.Drawing.Point(259, 118);
            this.groupBox13.Name = "groupBox13";
            this.groupBox13.Size = new System.Drawing.Size(78, 91);
            this.groupBox13.TabIndex = 6;
            this.groupBox13.TabStop = false;
            this.groupBox13.Text = "World";
            // 
            // mWorldZ
            // 
            this.mWorldZ.Location = new System.Drawing.Point(14, 65);
            this.mWorldZ.Name = "mWorldZ";
            this.mWorldZ.Size = new System.Drawing.Size(50, 13);
            this.mWorldZ.TabIndex = 2;
            this.mWorldZ.Text = "-";
            this.mWorldZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mWorldY
            // 
            this.mWorldY.Location = new System.Drawing.Point(14, 44);
            this.mWorldY.Name = "mWorldY";
            this.mWorldY.Size = new System.Drawing.Size(50, 13);
            this.mWorldY.TabIndex = 1;
            this.mWorldY.Text = "-";
            this.mWorldY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mWorldX
            // 
            this.mWorldX.Location = new System.Drawing.Point(14, 22);
            this.mWorldX.Name = "mWorldX";
            this.mWorldX.Size = new System.Drawing.Size(50, 13);
            this.mWorldX.TabIndex = 0;
            this.mWorldX.Text = "-";
            this.mWorldX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox14
            // 
            this.groupBox14.Controls.Add(this.mStageZ);
            this.groupBox14.Controls.Add(this.mStageY);
            this.groupBox14.Controls.Add(this.mStageX);
            this.groupBox14.Location = new System.Drawing.Point(343, 118);
            this.groupBox14.Name = "groupBox14";
            this.groupBox14.Size = new System.Drawing.Size(78, 91);
            this.groupBox14.TabIndex = 7;
            this.groupBox14.TabStop = false;
            this.groupBox14.Text = "Stage";
            // 
            // mStageZ
            // 
            this.mStageZ.Location = new System.Drawing.Point(14, 65);
            this.mStageZ.Name = "mStageZ";
            this.mStageZ.Size = new System.Drawing.Size(50, 13);
            this.mStageZ.TabIndex = 2;
            this.mStageZ.Text = "-";
            this.mStageZ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mStageY
            // 
            this.mStageY.Location = new System.Drawing.Point(14, 44);
            this.mStageY.Name = "mStageY";
            this.mStageY.Size = new System.Drawing.Size(50, 13);
            this.mStageY.TabIndex = 1;
            this.mStageY.Text = "-";
            this.mStageY.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mStageX
            // 
            this.mStageX.Location = new System.Drawing.Point(14, 22);
            this.mStageX.Name = "mStageX";
            this.mStageX.Size = new System.Drawing.Size(50, 13);
            this.mStageX.TabIndex = 0;
            this.mStageX.Text = "-";
            this.mStageX.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ControlPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(457, 379);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "ControlPanel";
            this.Text = "Pedestrian Navigation System";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox11.ResumeLayout(false);
            this.groupBox10.ResumeLayout(false);
            this.groupBox9.ResumeLayout(false);
            this.groupBox8.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox12.ResumeLayout(false);
            this.groupBox13.ResumeLayout(false);
            this.groupBox14.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button Connect_Bt;
        private System.Windows.Forms.Button Disconnect_Bt;
        private System.Windows.Forms.Button Send_Bt;
        private System.Windows.Forms.Button Stop_Bt;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button Start_Bt;
        private System.Windows.Forms.Button Pause_Bt;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Label mAccRawX;
        private System.Windows.Forms.Label mAccRawZ;
        private System.Windows.Forms.Label mAccRawY;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.Label mAccAvgZ;
        private System.Windows.Forms.Label mAccAvgY;
        private System.Windows.Forms.Label mAccAvgX;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.Label mAccDevZ;
        private System.Windows.Forms.Label mAccDevY;
        private System.Windows.Forms.Label mAccDevX;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Label mAccZ;
        private System.Windows.Forms.Label mAccY;
        private System.Windows.Forms.Label mAccX;
        private System.Windows.Forms.GroupBox groupBox9;
        private System.Windows.Forms.Label mAccPosZ;
        private System.Windows.Forms.Label mAccPosY;
        private System.Windows.Forms.Label mAccPosX;
        private System.Windows.Forms.GroupBox groupBox11;
        private System.Windows.Forms.Label mHeadZY;
        private System.Windows.Forms.GroupBox groupBox10;
        private System.Windows.Forms.Label mMagZ;
        private System.Windows.Forms.Label mMagY;
        private System.Windows.Forms.Label mMagX;
        private System.Windows.Forms.GroupBox groupBox14;
        private System.Windows.Forms.Label mStageZ;
        private System.Windows.Forms.Label mStageY;
        private System.Windows.Forms.Label mStageX;
        private System.Windows.Forms.GroupBox groupBox13;
        private System.Windows.Forms.Label mWorldZ;
        private System.Windows.Forms.Label mWorldY;
        private System.Windows.Forms.Label mWorldX;
        private System.Windows.Forms.GroupBox groupBox12;
        private System.Windows.Forms.Label mLocalZ;
        private System.Windows.Forms.Label mLocalY;
        private System.Windows.Forms.Label mLocalX;
    }
}

