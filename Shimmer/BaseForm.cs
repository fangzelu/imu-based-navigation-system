using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Threading;
using ChartFX.WinForms.DataProviders;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using D3D = Microsoft.DirectX.Direct3D;
using MTV3D65;
using MotionNode.SDK;

namespace SensorNode
{
    public partial class BaseForm : Form
    {
        //*** Constant
        public int DATA_LENGTH = 6;                // number of sensor data
        public int PACKAGE_COUNT = 3;              // data package number to average data
        public int DATA_BUFFER_SIZE;

        public int ACCEL_CALIBRATION_COUNT = 512;        // accelerometer calibration package number to confirm base value
        public int ACCEL_ANCHOR_WINDOW_SIZE = 128;
        public int GYRO_CALIBRATION_COUNT = 512;   // gyro calibration package number to confirm base value (2 power of this value)
        public int GYRO_ANCHOR_WINDOW_SIZE = 128;
        public int MAG_CALIBRATION_COUNT = 512;
        public int MAG_ANCHOR_WINDOW_SIZE = 128;

        public int DATA_BUFFER_MAX = 128;          // data buffer max number (actually, now using only 4 steps)
        public float GYRO_REF_VOLTAGE = 2500.0f / 4095.0f / 2.0f;  // Ref Voltage : 2.5V, Using bit : 12, Sensitivity : 2mv / degree / sec
        public float GYRO_X_REF_VOLTAGE;
        public float GYRO_Y_REF_VOLTAGE;
        public float GYRO_Z_REF_VOLTAGE;

        public float MAG_X_REF_VOLTAGE;
        public float MAG_Y_REF_VOLTAGE;
        public float MAG_Z_REF_VOLTAGE;

        private const int ACCEL_SENSITIVITY = 800;
        private const int ACCEL_ZERO_G = 1650;
        private const int RETOS_REF_VOLTAGE = 2500;
        private const int RETOS_ADC_RANGE = 4095;
        private const int G_RANGE = 565;  // 1g = 800 * cos(45deg) = 565.6854249492...
      
        public Rule DeviceRule = Rule.MotionNode;     // which device to use
        public Rule CoordinateRule = Rule.EulerAngle;      // which rule to transform coordinate

        private SerialPort serialPort = new SerialPort();
        private Thread readThread;      // thread to read sensor data
        private Thread tvThread;        // thread to render

        private bool dataFlag = false;
        private int nodeState = 1;
        private int beforeState = 0;

        //*** To Control MotionNode, Using MotionNode SDK
        Client motionPreview, motionSensor, motionRaw;
        TextWriter motionLog;
        TV_3DQUATERNION quaternion;
        const float MOTIONNODE_REFVOLT = 2500.0f;
        const float MOTIONNODE_RAWRANGE = 4095.0f;
        const float MOTIONNODE_ZEROG = 1273.21844731f;
        const float MOTIONNODE_ONEG = 505.34164883f;
        const float MOTIONNODE_FREQ = 1.0f / 60.0f;

        Axis axisX = new Axis(0, 16);
        Axis axisY = new Axis(1, 16);
        Axis axisZ = new Axis(2, 16);

        private List<float> accX = new List<float>();
        private List<float> accY = new List<float>();
        private List<float> accZ = new List<float>();

        private List<float> gyroX = new List<float>();
        private List<float> gyroY = new List<float>();
        private List<float> gyroZ = new List<float>();

        //*** magnetometer sensor value : magnetometer
        private List<float> magX = new List<float>();
        private List<float> magY = new List<float>();
        private List<float> magZ = new List<float>();

        //*** gyro sensor value : angular velocity -> angular
        private List<int> gyroXQ = new List<int>();
        private List<int> gyroYQ = new List<int>();
        private List<int> gyroZQ = new List<int>();
        private List<int> gAngleXQ = new List<int>();
        private List<int> gAngleYQ = new List<int>();
        private List<int> gAngleZQ = new List<int>();

        //*** accelrometer sensor value : acceleration -> velocity -> position
        private List<int> accelXQ = new List<int>();
        private List<int> accelYQ = new List<int>();
        private List<int> accelZQ = new List<int>();
        private List<int> angleXQ = new List<int>();
        private List<int> angleYQ = new List<int>();
        private List<int> angleZQ = new List<int>();

        private List<double> velocityX = new List<double>();
        private List<double> velocityY = new List<double>();
        private List<double> velocityZ = new List<double>();
        private List<int> positionX = new List<int>();
        private List<int> positionY = new List<int>();
        private List<int> positionZ = new List<int>();

        //*** real sensing time. it generated in shimmer
        private List<float> senseT = new List<float>();

        //*** sensor base value on each sensor
        private int gyroXBase, gyroYBase, gyroZBase, accelXBase, accelYBase, accelZBase, magXBase, magYBase, magZBase;
        private int _gyroXBase = 0; private int _gyroYBase = 0; private int _gyroZBase = 0;
        private int _accelXBase = 0; private int _accelYBase = 0; private int _accelZBase = 0;
        private int _magXBase = 0; private int _magYBase = 0; private int _magZBase = 0;

        //*** sensor calibration value
        private int accelCalibrationCount = 0;
        private bool accelCalibrationFlag = false;
        private int gyroCalibrationCount = 0;
        private bool gyroCalibrationFlag = false;
        private int magCalibrationCount = 0;
        private bool magCalibrationFlag = false;

        //*** to decide state - static or dynamic
        private int accelXN, accelXVar, accelXAvg, accelYN, accelYVar, accelYAvg, accelZN, accelZVar, accelZAvg;
        private const int ACCEL_STATE_WINDOW = 16;

        private System.Collections.ArrayList AccelChartList = new System.Collections.ArrayList();
        private ListProvider AccelChartProvider;

        private System.Collections.ArrayList GyroChartList = new System.Collections.ArrayList();
        private ListProvider GyroChartProvider;

        private System.Collections.ArrayList MagChartList = new System.Collections.ArrayList();
        private ListProvider MagChartProvider;

        //*** TrueVision3D variables to render 3D
        private TVEngine TV;
        private TVScene Scene;      // This is used for instantiating resource types - Geometry, Render Targets, Shaders and so on.
        private TVTextureFactory TF;    // basic texturing functionality
        private TVMaterialFactory Mats; // Lit meshes need a material.
        private TVLightEngine Lights;   // Light engine
        private TVMathLibrary Maths;    //
        
        private TVMesh SensorNode;     //SensorNode mesh
        private TVMesh Room;        //Room mesh
        private TVMesh WiimoteMesh; //Wiimote mesh
        private TVCamera Cam;

        private bool bLoop = true;
        private int IDLight;            //This is an ID for our point light. You can use strings also but strings are slower.
        private int IDBackLight;
        private int FloorTex;
        private int WallTex;

        public BaseForm()
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.Opaque, false);

            InitMenuStrip();
            InitData();

            PortScan();
        }

        //*** Window Form Method
        private void InitMenuStrip()
        {
        }

        private void clickMenuStrip(ToolStripMenuItem selectedItem)
        {

        }

        private void ScanButton_Click(object sender, EventArgs e)
        {
            PortScan();
        }

        private void ConButton_Click(object sender, EventArgs e)
        {
            switch (DeviceRule)
            {
                case Rule.SensorNode:
                    //Serial port setting
                    serialPort.PortName = this.PortBox.Text;
                    serialPort.BaudRate = 57600;
                    serialPort.Parity = Parity.None;
                    serialPort.DataBits = 8;
                    serialPort.StopBits = StopBits.One;
                    serialPort.ReadBufferSize = 512;

                    serialPort.Open();
                    break;
                case Rule.MotionNode:
                    motionPreview = new Client("", 32079);
                    motionSensor = new Client("", 32078);
                    motionRaw = new Client("", 32077);
                    motionLog = new StreamWriter("motionLog.dat");
                    break;
            }
            Thread.Sleep(1000);

            dataFlag = true;
            readThread = new Thread(ReadData);
            readThread.IsBackground = true;
            readThread.Start();

            this.toolStripStatusLabel.Text = "Calibration Now, Please Don't Move Your Sensor Mote.";
            this.ConButton.Enabled = false;
            this.DisconButton.Enabled = true;
            this.optionToolStripMenu.Enabled = false;
            this.ScanButton.Enabled = false;
        }

        private void DisconButton_Click(object sender, EventArgs e)
        {
            dataFlag = false;

            this.ScanButton.Enabled = true;
            this.DisconButton.Enabled = false;
            this.ConButton.Enabled = true;
            this.optionToolStripMenu.Enabled = true;

            this.AccelXData.Text = "-";
            this.AccelYData.Text = "-";
            this.AccelZData.Text = "-";
            this.VelocityXData.Text = "-";
            this.VelocityYData.Text = "-";
            this.VelocityZData.Text = "-";
            this.PositionXData.Text = "-";
            this.PositionYData.Text = "-";
            this.PositionZData.Text = "-";

            this.GyroXData.Text = "-";
            this.GyroYData.Text = "-";
            this.GyroZData.Text = "-";
            this.AngularXData.Text = "-";
            this.AngularYData.Text = "-";
            this.AngularZData.Text = "-";

            InitData();

            readThread.Join();
            serialPort.Close();
        }

        private void PortScan()
        {
            // Get a list of serial port names.
            string[] ports = SerialPort.GetPortNames();

            this.PortBox.Items.Clear();

            // Display each port name to the console.
            foreach (string port in ports)
            {
                this.PortBox.Items.Add(port);
            }
        }

        private void OpenOptionForm(object sender, EventArgs e)
        {
            OptionForm optionForm = new OptionForm(this);
            DialogResult optionResult = optionForm.ShowDialog(this);

            if (optionForm.DialogResult == DialogResult.OK)
            {
                switch (DeviceRule)
                {
                    case Rule.SensorNode:
                        DATA_BUFFER_SIZE = DATA_LENGTH * PACKAGE_COUNT * sizeof(Int16) + sizeof(Int32);
                        break;
                }

                gyroXQ.Clear();
                gyroXQ.Clear();
                gyroXQ.Clear();
                gAngleXQ.Clear();
                gAngleYQ.Clear();
                gAngleZQ.Clear();

                accelXQ.Clear();
                accelYQ.Clear();
                accelZQ.Clear();
                angleXQ.Clear();
                angleYQ.Clear();
                angleZQ.Clear(); 
                
                velocityX.Clear();
                velocityY.Clear();
                velocityZ.Clear();
                positionX.Clear();
                positionY.Clear();
                positionZ.Clear();

                magX.Clear();
                magY.Clear();
                magZ.Clear();

                senseT.Clear();

                //Initial list to avoid garbage calculation
                while (DATA_BUFFER_MAX > gyroXQ.Count)
                {
                    gyroXQ.Add(0);
                    gyroYQ.Add(0);
                    gyroZQ.Add(0);
                    gAngleXQ.Add(0);
                    gAngleYQ.Add(0);
                    gAngleZQ.Add(0);

                    accelXQ.Add(0);
                    accelYQ.Add(0);
                    accelZQ.Add(0);
                    angleXQ.Add(0);
                    angleYQ.Add(0);
                    angleZQ.Add(0);
                    
                    velocityX.Add(0);
                    velocityY.Add(0);
                    velocityZ.Add(0);
                    positionX.Add(0);
                    positionY.Add(0);
                    positionZ.Add(0);

                    magX.Add(0);
                    magY.Add(0);
                    magZ.Add(0);

                    senseT.Add(0.0f);
                }

                switch (DeviceRule)
                {
                    case Rule.SensorNode:
                        ChartP.DataSourceSettings.DataSource = this.AccelChartProvider;
                        ChartA.DataSourceSettings.DataSource = this.GyroChartProvider;
                        break;
                }

                optionForm.Dispose();
            }
            else
            {
                optionForm.Dispose();
            }
        }

        private void BaseForm_KeyDown(object sender, KeyEventArgs e)
        {
            TV_3DVECTOR targetVector = new TV_3DVECTOR();
            switch (e.KeyCode)
            {
                case Keys.Space:
                    // set position and angle to Zero
                    SensorNode.SetRotation(0.0f, 0.0f, 0.0f);
                    break;
                case Keys.D1:
                case Keys.NumPad1:
                    // change camera view : top view
                    targetVector = Room.GetWorldPosition(SensorNode.GetPosition());
                    Cam.SetPosition(targetVector.x, targetVector.y + 150.0f, targetVector.z);
                    Lights.SetLightPosition(IDLight, targetVector.x, targetVector.y + 150.0f, targetVector.z);
                    Lights.SetLightRange(IDLight, 150.0f);
                    Lights.SetLightPosition(IDBackLight, targetVector.x, targetVector.y - 150.0f, targetVector.z);
                    Lights.SetLightRange(IDBackLight, 150.0f);
                    Cam.LookAtMesh(SensorNode);
                    break;
                case Keys.D2:
                case Keys.NumPad2:
                    // change camera view : up 45 degree view
                    targetVector = Room.GetWorldPosition(SensorNode.GetPosition());
                    Cam.SetPosition(targetVector.x + 150.0f, targetVector.y + 100.0f, targetVector.z + 150.0f);
                    Lights.SetLightPosition(IDLight, targetVector.x + 150.0f, targetVector.y + 100.0f, targetVector.z + 150.0f);
                    Lights.SetLightRange(IDLight, 300.0f);
                    Lights.SetLightPosition(IDBackLight, targetVector.x + 150.0f, targetVector.y + 100.0f, targetVector.z - 150.0f);
                    Lights.SetLightRange(IDBackLight, 300.0f);
                    Cam.LookAtMesh(SensorNode);
                    break;
                case Keys.D3:
                case Keys.NumPad3:
                    // change camera view : up 45 degree close view
                    targetVector = Room.GetWorldPosition(SensorNode.GetPosition());
                    Cam.SetPosition(targetVector.x, targetVector.y + 100.0f, targetVector.z + 200.0f);
                    Lights.SetLightPosition(IDLight, targetVector.x, targetVector.y + 100.0f, targetVector.z + 200.0f);
                    Lights.SetLightRange(IDLight, 300.0f);
                    Lights.SetLightPosition(IDBackLight, targetVector.x, targetVector.y + 100.0f, targetVector.z - 200.0f);
                    Cam.LookAtMesh(SensorNode);
                    break;
                case Keys.D4:
                case Keys.NumPad4:
                    // change camera view : right view
                    targetVector = Room.GetWorldPosition(SensorNode.GetPosition());
                    Cam.SetPosition(targetVector.x, targetVector.y, targetVector.z + 150.0f);
                    Lights.SetLightPosition(IDLight, targetVector.x, targetVector.y, targetVector.z + 150.0f);
                    Lights.SetLightRange(IDLight, 250.0f);
                    Lights.SetLightPosition(IDBackLight, targetVector.x, targetVector.y, targetVector.z - 150.0f);
                    Lights.SetLightRange(IDBackLight, 250.0f);
                    Cam.LookAtMesh(SensorNode);
                    break;
                case Keys.D5:
                case Keys.NumPad5:
                    targetVector = Room.GetWorldPosition(SensorNode.GetPosition());
                    Cam.SetPosition(targetVector.x, targetVector.y, targetVector.z - 150.0f);
                    Lights.SetLightPosition(IDLight, targetVector.x, targetVector.y, targetVector.z - 150.0f);
                    Lights.SetLightRange(IDLight, 250.0f);
                    Lights.SetLightPosition(IDBackLight, targetVector.x, targetVector.y, targetVector.z + 150.0f);
                    Lights.SetLightRange(IDBackLight, 250.0f);
                    Cam.LookAtMesh(SensorNode);
                    break;
                case Keys.D6:
                case Keys.NumPad6:
                    targetVector = Room.GetWorldPosition(SensorNode.GetPosition());
                    Cam.SetPosition(targetVector.x - 150.0f, targetVector.y, targetVector.z);
                    Lights.SetLightPosition(IDLight, targetVector.x - 150.0f, targetVector.y, targetVector.z);
                    Lights.SetLightRange(IDLight, 250.0f);
                    Lights.SetLightPosition(IDBackLight, targetVector.x + 150.0f, targetVector.y, targetVector.z);
                    Lights.SetLightRange(IDBackLight, 250.0f);
                    Cam.LookAtMesh(SensorNode);
                    break;
                case Keys.D7:
                case Keys.NumPad7:
                    targetVector = Room.GetWorldPosition(SensorNode.GetPosition());
                    Cam.SetPosition(targetVector.x + 150.0f, targetVector.y, targetVector.z);
                    Lights.SetLightPosition(IDLight, targetVector.x + 150.0f, targetVector.y, targetVector.z);
                    Lights.SetLightRange(IDLight, 250.0f);
                    Lights.SetLightPosition(IDBackLight, targetVector.x - 150.0f, targetVector.y, targetVector.z);
                    Lights.SetLightRange(IDBackLight, 250.0f);
                    Cam.LookAtMesh(SensorNode);
                    break;
                case Keys.C:
                    Room.RotateY(5.0f, true);
                    break;
                case Keys.Z:
                    Room.RotateY(-5.0f, true);
                    break;
                case Keys.B:
                    SensorNode.RotateY(5.0f, true);
                    break;
                case Keys.M:
                    SensorNode.RotateY(-5.0f, true);
                    break;
                case Keys.H:
                    SensorNode.RotateX(5.0f, true);
                    break;
                case Keys.N:
                    SensorNode.RotateX(-5.0f, true);
                    break;
                default:
                    break;
            }
        }

        private void BaseForm_Load(object sender, EventArgs e)
        {
            

            SetupScene(this.LocationPanel.Handle);

            tvThread = new Thread(TV_Loop);
            tvThread.IsBackground = true;
            tvThread.Start();
        }

        private void ShowVector(string name, TV_3DVECTOR vector)
        {
            MessageBox.Show(name + " X : " + vector.x + " Y : " + vector.y + " Z : " + vector.z);
        }
        //*** Sensor Method
        private void InitData()
        {
            loadOptionFromFile();

            switch (DeviceRule)
            {
                case Rule.SensorNode:
                    DATA_BUFFER_SIZE = DATA_LENGTH * PACKAGE_COUNT * sizeof(Int16) + sizeof(Int32);

                    //Chart data binding
                    this.AccelChartProvider = new ListProvider(AccelChartList);
                    this.AccelChartProvider.List.Add(accelXQ);
                    this.AccelChartProvider.List.Add(accelYQ);
                    this.AccelChartProvider.List.Add(accelZQ);

                    this.GyroChartProvider = new ListProvider(GyroChartList);
                    this.GyroChartProvider.List.Add(gyroXQ);
                    this.GyroChartProvider.List.Add(gyroYQ);
                    this.GyroChartProvider.List.Add(gyroZQ);

                    ChartP.DataSourceSettings.DataSource = this.AccelChartProvider;
                    ChartA.DataSourceSettings.DataSource = this.GyroChartProvider;

                    break;
                case Rule.MotionNode:
                    this.AccelChartProvider = new ListProvider(AccelChartList);
                    this.AccelChartProvider.List.Add(accX);
                    this.AccelChartProvider.List.Add(accY);
                    this.AccelChartProvider.List.Add(accZ);

                    this.GyroChartProvider = new ListProvider(GyroChartList);
                    this.GyroChartProvider.List.Add(gyroX);
                    this.GyroChartProvider.List.Add(gyroY);
                    this.GyroChartProvider.List.Add(gyroZ);

                    this.MagChartProvider = new ListProvider(MagChartList);
                    this.MagChartProvider.List.Add(magX);
                    this.MagChartProvider.List.Add(magY);
                    this.MagChartProvider.List.Add(magZ);

                    ChartP.DataSourceSettings.DataSource = this.AccelChartProvider;
                    ChartP.AxisY.Max = 2.0f;
                    ChartP.AxisY.Min = -2.0f;

                    ChartA.DataSourceSettings.DataSource = this.GyroChartProvider;
                    ChartA.AxisY.Max = 500.0f;
                    ChartA.AxisY.Min = -500.0f;

                    break;

            }


            gyroXQ.Clear();
            gyroXQ.Clear();
            gyroXQ.Clear();
            gAngleXQ.Clear();
            gAngleYQ.Clear();
            gAngleZQ.Clear();

            accelXQ.Clear();
            accelYQ.Clear();
            accelZQ.Clear();
            angleXQ.Clear();
            angleYQ.Clear();
            angleZQ.Clear();

            velocityX.Clear();
            velocityY.Clear();
            velocityZ.Clear();
            positionX.Clear();
            positionY.Clear();
            positionZ.Clear();

            accX.Clear();
            accY.Clear();
            accZ.Clear();
            gyroX.Clear();
            gyroY.Clear();
            gyroZ.Clear();
            magX.Clear();
            magY.Clear();
            magZ.Clear();

            senseT.Clear();

            //Initial list to avoid garbage calculation
            while (gyroXQ.Count < DATA_BUFFER_MAX)
            {
                gyroXQ.Add(0);
                gyroYQ.Add(0);
                gyroZQ.Add(0);
                gAngleXQ.Add(0);
                gAngleYQ.Add(0);
                gAngleZQ.Add(0);

                accelXQ.Add(0);
                accelYQ.Add(0);
                accelZQ.Add(0);
                angleXQ.Add(0);
                angleYQ.Add(0);
                angleZQ.Add(0);

                velocityX.Add(0);
                velocityY.Add(0);
                velocityZ.Add(0);
                positionX.Add(0);
                positionY.Add(0);
                positionZ.Add(0);

                accX.Add(0.0f);
                accY.Add(0.0f);
                accZ.Add(0.0f);
                gyroX.Add(0.0f);
                gyroY.Add(0.0f);
                gyroZ.Add(0.0f);
                magX.Add(0.0f);
                magY.Add(0.0f);
                magZ.Add(0.0f);

                senseT.Add(0.0f);
            }
        }

        private void ManipulateAccelerometer(Int16 x, Int16 y, Int16 z)
        {
            int _gX = 0; int _gY = 0; int _gZ = 0; int _angleX = 0; int _angleY = 0; int _angleZ = 0;
            int _gAngleX, _gAngleY, _gAngleZ;

            //*** Accelerometer data handling
            if (accelCalibrationFlag)
            {
                accelXQ.Add((int)(x * RETOS_REF_VOLTAGE / RETOS_ADC_RANGE));
                accelYQ.Add((int)(y * RETOS_REF_VOLTAGE / RETOS_ADC_RANGE));
                accelZQ.Add((int)(z * RETOS_REF_VOLTAGE / RETOS_ADC_RANGE));

                if (accelXQ.Count >= DATA_BUFFER_MAX)
                {
                    accelXQ.RemoveAt(0);
                    accelYQ.RemoveAt(0);
                    accelZQ.RemoveAt(0);
                }

                if (accelXN < ACCEL_STATE_WINDOW)
                {
                    accelXN++;
                    accelYN++;
                    accelZN++;
                    accelXAvg += accelXQ.Last();
                    accelXVar += accelXQ.Last() * accelXQ.Last();
                    accelYAvg += accelYQ.Last();
                    accelYVar += accelYQ.Last() * accelYQ.Last();
                    accelZAvg += accelZQ.Last();
                    accelZVar += accelZQ.Last() * accelZQ.Last();
                }
                else
                {
                    accelXAvg -= accelXQ.ElementAt(accelXQ.Count - ACCEL_STATE_WINDOW - 1);
                    accelXVar -= accelXQ.ElementAt(accelXQ.Count - ACCEL_STATE_WINDOW - 1) * accelXQ.ElementAt(accelXQ.Count - ACCEL_STATE_WINDOW - 1);
                    accelYAvg -= accelYQ.ElementAt(accelYQ.Count - ACCEL_STATE_WINDOW - 1);
                    accelYVar -= accelYQ.ElementAt(accelYQ.Count - ACCEL_STATE_WINDOW - 1) * accelYQ.ElementAt(accelYQ.Count - ACCEL_STATE_WINDOW - 1);
                    accelZAvg -= accelZQ.ElementAt(accelZQ.Count - ACCEL_STATE_WINDOW - 1);
                    accelZVar -= accelZQ.ElementAt(accelZQ.Count - ACCEL_STATE_WINDOW - 1) * accelZQ.ElementAt(accelZQ.Count - ACCEL_STATE_WINDOW - 1);

                    accelXAvg += accelXQ.Last();
                    accelXVar += accelXQ.Last() * accelXQ.Last();
                    accelYAvg += accelYQ.Last();
                    accelYVar += accelYQ.Last() * accelYQ.Last();
                    accelZAvg += accelZQ.Last();
                    accelZVar += accelZQ.Last() * accelZQ.Last();
                }

                this.Invoke(new MethodInvoker(delegate()
                {
                    this.PositionXData.Text = ((int)(((float)accelXVar / accelXN) - ((float)accelXAvg / accelXN) * ((float)accelXAvg / accelXN))).ToString();
                    this.PositionYData.Text = ((int)(((float)accelYVar / accelYN) - ((float)accelYAvg / accelYN) * ((float)accelYAvg / accelYN))).ToString();
                    this.PositionZData.Text = ((int)(((float)accelZVar / accelZN) - ((float)accelZAvg / accelZN) * ((float)accelZAvg / accelZN))).ToString();
                }));

                _gX = accelXQ.Last() - ACCEL_ZERO_G;
                _gY = accelYQ.Last() - ACCEL_ZERO_G;
                _gZ = accelZQ.Last() - ACCEL_ZERO_G;

                _gX = Math.Min(_gX, ACCEL_SENSITIVITY);
                _gX = Math.Max(_gX, -ACCEL_SENSITIVITY);
                _gY = Math.Min(_gY, ACCEL_SENSITIVITY);
                _gY = Math.Max(_gY, -ACCEL_SENSITIVITY);
                _gZ = Math.Min(_gZ, ACCEL_SENSITIVITY);
                _gZ = Math.Max(_gZ, -ACCEL_SENSITIVITY);

                _angleX = (int)(Math.Acos((double)_gX / ACCEL_SENSITIVITY) * 180.0f / Math.PI);
                _angleY = (int)(Math.Acos((double)_gY / ACCEL_SENSITIVITY) * 180.0f / Math.PI);
                _angleZ = (int)(Math.Acos((double)_gZ / ACCEL_SENSITIVITY) * 180.0f / Math.PI);

                //this.Invoke(new MethodInvoker(delegate()
                //{
                //   this.positionXData.Text = _angleX.ToString();
                //    this.positionYData.Text = _angleY.ToString();
                //    this.positionZData.Text = _angleZ.ToString();
                //}));
                
                // No.4 State
                if (_gX <= -G_RANGE)
                {
                    nodeState = 4;
                    if (beforeState != nodeState)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            gyroYQ.RemoveAt(0);
                            gyroYQ.Add(0);
                        }
                    }
                    beforeState = nodeState;
                    angleXQ.Add(_angleZ - 90);
                    angleYQ.Add(-gAngleYQ.Last());
                    angleZQ.Add(180 - _angleY);
                }
                // No.1 State
                else if (_gY <= -G_RANGE)
                {
                    _angleX = (int)(Math.Atan((double)_gX / _gY) * 180.0f / Math.PI);
                    _angleZ = (int)(Math.Atan((double)_gZ / _gY) * 180.0f / Math.PI);
                    nodeState = 1;
                    if (beforeState != nodeState)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            gyroXQ.RemoveAt(0);
                            gyroXQ.Add(0);
                        }
                        gAngleXQ.RemoveAt(0);
                        gAngleXQ.Add(angleYQ.Last());
                    }
                    beforeState = nodeState;
                    angleXQ.Add(_angleZ);
                    angleYQ.Add(-gAngleXQ.Last());
                    //angleZQ.Add(_angleX - 90);
                    angleZQ.Add(_angleX);
                }
                // No.5 State
                else if (_gZ <= -G_RANGE)
                {
                    nodeState = 5;
                    if (beforeState != nodeState)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            gyroZQ.RemoveAt(0);
                            gyroZQ.Add(0);
                        }
                    }
                    beforeState = nodeState;
                    angleXQ.Add(180 - _angleY);
                    angleYQ.Add(0);
                    angleZQ.Add(gAngleZQ.Last());
                }
                // No.2 State
                else if (_gX > G_RANGE)
                {
                    nodeState = 2;
                    if (beforeState != nodeState)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            gyroYQ.RemoveAt(0);
                            gyroYQ.Add(0);
                        }
                    }
                    beforeState = nodeState;
                    angleXQ.Add(_angleZ - 90);
                    angleYQ.Add(gAngleYQ.Last());
                    angleZQ.Add(-180 + _angleY);
                }
                // No.3 State
                else if (_gY > G_RANGE)
                {
                    _angleX = (int)(Math.Atan((double)_gX / _gY) * 180.0f / Math.PI);
                    _angleZ = (int)(Math.Atan((double)_gZ / _gY) * 180.0f / Math.PI);
                    nodeState = 3;
                    if (beforeState != nodeState)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            gyroXQ.RemoveAt(0);
                            gyroXQ.Add(0);
                        }
                    }
                    beforeState = nodeState;
                    //angleXQ.Add(_angleZ - 90);
                    angleXQ.Add(-_angleZ);
                    //angleYQ.Add(0);
                    angleZQ.Add(180 + _angleX);
                    //angleZQ.Add(180);
                    angleYQ.Add(gAngleXQ.Last());
                    //angleZQ.Add(270 - _angleX);
                }
                // No.6 State
                else if (_gZ > G_RANGE)
                {
                    nodeState = 6;
                    if (beforeState != nodeState)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            gyroZQ.RemoveAt(0);
                            gyroZQ.Add(0);
                        }
                    }
                    beforeState = nodeState;
                    angleXQ.Add(270 + (_angleY-90));
                    angleYQ.Add(0);
                    angleZQ.Add(gAngleZQ.Last());
                }

                if (angleXQ.Count >= DATA_BUFFER_MAX)
                {
                    angleXQ.RemoveAt(0);
                    angleYQ.RemoveAt(0);
                    angleZQ.RemoveAt(0);
                }

                //UpdateScene();
            }
            else if (accelCalibrationCount < ACCEL_CALIBRATION_COUNT && !accelCalibrationFlag)
            {
                _accelXBase += x;
                _accelYBase += y;
                _accelZBase += z;
                accelCalibrationCount++;
            }
            else if (!accelCalibrationFlag)
            {
                accelXBase = _accelXBase / ACCEL_CALIBRATION_COUNT;
                accelYBase = _accelYBase / ACCEL_CALIBRATION_COUNT;
                accelZBase = _accelZBase / ACCEL_CALIBRATION_COUNT;
                _accelXBase = 0;
                _accelYBase = 0;
                _accelZBase = 0;
                accelXN = 0; accelYN = 0; accelZN = 0;
                accelXVar = 0; accelYVar = 0; accelZVar = 0;
                accelXAvg = 0; accelYAvg = 0; accelZAvg = 0;
                accelCalibrationFlag = true;
            }

            //Display data on the form
            this.Invoke(new MethodInvoker(delegate()
            {
                //Display raw data on the form
                this.AccelXData.Text = accelXQ.Last().ToString();
                this.AccelYData.Text = accelYQ.Last().ToString();
                this.AccelZData.Text = accelZQ.Last().ToString();

                //Display integrated angular data on the form
                if (accelCalibrationFlag)
                {
                    this.VelocityXData.Text = angleXQ.Last().ToString();
                    this.VelocityYData.Text = angleYQ.Last().ToString();
                    this.VelocityZData.Text = angleZQ.Last().ToString();
                }
            }));
        }

        private void ManipulateGyro(Int16 x, Int16 y, Int16 z)
        {
            //Variables to store gyro integration data temporarily
            int _gAngleX, _gAngleY, _gAngleZ;

            //*** Gyro data handling
            //Calibration done
            if (gyroCalibrationFlag)
            {
                gyroXQ.Add(x - gyroXBase);
                gyroYQ.Add(y - gyroYBase);
                gyroZQ.Add(z - gyroZBase);

                if (gyroXQ.Count > DATA_BUFFER_MAX)
                {
                    gyroXQ.RemoveAt(0);
                    gyroYQ.RemoveAt(0);
                    gyroZQ.RemoveAt(0);
                }

                
                _gAngleX = (int)(gAngleXQ.Last() +
                    ((gyroXQ.ElementAt(gyroXQ.Count - 4) * senseT.ElementAt(senseT.Count - 4) +
                    2 * gyroXQ.ElementAt(gyroXQ.Count - 3) * senseT.ElementAt(senseT.Count - 3) +
                    2 * gyroXQ.ElementAt(gyroXQ.Count - 2) * senseT.ElementAt(senseT.Count - 2) +
                    gyroXQ.Last() * senseT.Last()) / 6.0f) * GYRO_X_REF_VOLTAGE);
                _gAngleY = (int)(gAngleYQ.Last() +
                    ((gyroYQ.ElementAt(gyroYQ.Count - 4) * senseT.ElementAt(senseT.Count - 4) +
                    2 * gyroYQ.ElementAt(gyroYQ.Count - 3) * senseT.ElementAt(senseT.Count - 3) +
                    2 * gyroYQ.ElementAt(gyroYQ.Count - 2) * senseT.ElementAt(senseT.Count - 2) +
                    gyroYQ.Last() * senseT.Last()) / 6.0f) * GYRO_X_REF_VOLTAGE);
                _gAngleZ = (int)(gAngleZQ.Last() +
                    ((gyroZQ.ElementAt(gyroZQ.Count - 4) * senseT.ElementAt(senseT.Count - 4) +
                    2 * gyroZQ.ElementAt(gyroZQ.Count - 3) * senseT.ElementAt(senseT.Count - 3) +
                    2 * gyroZQ.ElementAt(gyroZQ.Count - 2) * senseT.ElementAt(senseT.Count - 2) +
                    gyroZQ.Last() * senseT.Last()) / 6.0f) * GYRO_X_REF_VOLTAGE);

                if (_gAngleX > 360)
                    _gAngleX -= 360;
                else if (_gAngleX < -360)
                    _gAngleX += 360;
                if (_gAngleY > 360)
                    _gAngleY -= 360;
                else if (_gAngleY < -360)
                    _gAngleY += 360;
                if (_gAngleZ > 360)
                    _gAngleZ -= 360;
                else if (_gAngleZ < -360)
                    _gAngleZ += 360;

                gAngleXQ.Add(_gAngleX);
                gAngleYQ.Add(_gAngleY);
                gAngleZQ.Add(_gAngleZ);

                if (gAngleXQ.Count > DATA_BUFFER_MAX)
                {
                    gAngleXQ.RemoveAt(0);
                    gAngleYQ.RemoveAt(0);
                    gAngleZQ.RemoveAt(0);
                }
                //UpdateScene();
            }
            //Calibration gyro sensor. it's only one time when started
            else if (gyroCalibrationCount < GYRO_CALIBRATION_COUNT && !gyroCalibrationFlag)
            {
                _gyroXBase += x;
                _gyroYBase += y;
                _gyroZBase += z;
                gyroCalibrationCount++;
            }
            //Calculate sample gyro data to find base value
            else if (!gyroCalibrationFlag)
            {
                gyroXBase = _gyroXBase / GYRO_CALIBRATION_COUNT;
                gyroYBase = _gyroYBase / GYRO_CALIBRATION_COUNT;
                gyroZBase = _gyroZBase / GYRO_CALIBRATION_COUNT;
                GYRO_X_REF_VOLTAGE = (gyroXBase / 4096.0f) / (1500.0f / 2500.0f * 2.0f);
                GYRO_Y_REF_VOLTAGE = (gyroYBase / 4096.0f) / (1500.0f / 2500.0f * 2.0f);
                GYRO_Z_REF_VOLTAGE = (gyroZBase / 4096.0f) / (1500.0f / 2500.0f * 2.0f);
                gyroCalibrationFlag = true;
            }

            //Display data on the form
            this.Invoke(new MethodInvoker(delegate()
            {
                //Display raw data on the form
                this.GyroXData.Text = x.ToString(); 
                this.GyroYData.Text = y.ToString();
                this.GyroZData.Text = z.ToString();

                //Display integrated angular data on the form
                if (gyroCalibrationFlag)
                {
                    this.AngularXData.Text = gAngleXQ.Last().ToString();
                    this.AngularYData.Text = gAngleYQ.Last().ToString();
                    this.AngularZData.Text = gAngleZQ.Last().ToString();
                }

            }));
        }

        private void ManipulateMagnetometer(Int16 x, Int16 y, Int16 z)
        {
            //*** Gyro data handling
            //Calibration done
            if (magCalibrationFlag)
            {
                magX.Insert(0, x - magXBase);
                magY.Insert(0, y - magYBase);
                magZ.Insert(0, z - magZBase);

                if (magX.Count >= DATA_BUFFER_MAX)
                {
                    magX.RemoveAt(DATA_BUFFER_MAX - 1);
                    magY.RemoveAt(DATA_BUFFER_MAX - 1);
                    magZ.RemoveAt(DATA_BUFFER_MAX - 1);
                }

                //UpdateScene();
            }
            //Calibration gyro sensor. it's only one time when started
            else if (magCalibrationCount < MAG_CALIBRATION_COUNT && !magCalibrationFlag)
            {
                _magXBase += x;
                _magYBase += y;
                _magZBase += z;
                magCalibrationCount++;
            }
            //Calculate sample gyro data to find base value
            else if (!magCalibrationFlag)
            {
                magXBase = (Int16)(_magXBase / MAG_CALIBRATION_COUNT);
                magYBase = (Int16)(_magYBase / MAG_CALIBRATION_COUNT);
                magZBase = (Int16)(_magZBase / MAG_CALIBRATION_COUNT);
                MAG_X_REF_VOLTAGE = (magXBase / 4096.0f) / (1500.0f / 2500.0f);
                MAG_Y_REF_VOLTAGE = (magYBase / 4096.0f) / (1500.0f / 2500.0f);
                MAG_Z_REF_VOLTAGE = (magZBase / 4096.0f) / (1500.0f / 2500.0f);
                magCalibrationFlag = true;
            }

            //Display data on the form
            this.Invoke(new MethodInvoker(delegate()
            {
                //Display raw data on the form
                this.AngularXData.Text = x.ToString();
                this.AngularYData.Text = y.ToString();
                this.AngularZData.Text = z.ToString();

            }));
        }

        private bool UpdateGyroAngle(ref Axis s)
        {
            float temp = 0.0f;
            switch(s.gyroIndex)
            {
                case 0:
                    temp += (s.gyro[0] + 2 * s.gyro[1] + 2 * s.gyro[2] + s.gyro[3]) * MOTIONNODE_FREQ / 6;
                    break;
                case 1:
                    temp += (s.gyro[1] + 2 * s.gyro[2] + 2 * s.gyro[3] + s.gyro[0]) * MOTIONNODE_FREQ / 6;
                    break;
                case 2:
                    temp += (s.gyro[2] + 2 * s.gyro[3] + 2 * s.gyro[0] + s.gyro[1]) * MOTIONNODE_FREQ / 6;
                    break;
                case 3:
                    temp += (s.gyro[3] + 2 * s.gyro[0] + 2 * s.gyro[1] + s.gyro[2]) * MOTIONNODE_FREQ / 6;
                    break;
            }

            s.angle += temp * 2.0f;

            return true;
        }

        private int GetMaxG(ref Axis s1, ref Axis s2, ref Axis s3)
        {
            if (Math.Abs(s1.accel) >= Math.Abs(s2.accel) && Math.Abs(s1.accel) >= Math.Abs(s3.accel))
                return s1.index;

            if (Math.Abs(s2.accel) >= Math.Abs(s1.accel) && Math.Abs(s2.accel) >= Math.Abs(s3.accel))
                return s2.index;

            if (Math.Abs(s3.accel) >= Math.Abs(s1.accel) && Math.Abs(s3.accel) >= Math.Abs(s2.accel))
                return s3.index;

            return -1;
        }

        private void UpdateRollAngle(ref Axis s1, ref Axis s2, ref Axis target)
        {
            if ((s1.state & 1) == 0 && (s2.state & 1) == 0 && (target.state & 1) == 0)
            {
                int maxGIndex = GetMaxG(ref s1, ref s2, ref target);

                switch(maxGIndex)
                {
                    case 0:
                        break;
                    case 1:
                    case 2:
                        if (s1.accel == 0.0f)
                        {
                            if (s2.accel >= 0)
                                target.angle = 90.0f;
                            else
                                target.angle = -90.0f;
                        }
                        else
                        {
                            target.angle = Math.Abs((float)(Math.Atan(s2.accel / s1.accel) * 180.0f / Math.PI));
                            if (s1.accel >= 0)
                                target.angle = 180.0f - target.angle;
                            if (s2.accel <= 0)
                                target.angle = -target.angle;
                        }

                        target.angle += 180.0f;
                        if (target.angle > 180.0f)
                            target.angle -= 360.0f;

                        if(target.mag < 0)
                        {
                            target.angle -= 180.0f;
                            if (target.angle < -180.0f)
                                target.angle += 360.0f;
                        }

                        break;
                    default:
                        break;
                }

            }

            return;
        }
        private void UpdatePitchAngle(ref Axis s1, ref Axis s2, ref Axis target)
        {
            if ((s1.state & 1) == 0 && (s2.state & 1) == 0 && (target.state & 1) == 0)
            {
                int maxGIndex = GetMaxG(ref s1, ref s2, ref target);

                switch(maxGIndex)
                {
                    case 0:
                    case 1:
                        if (s1.accel == 0.0f)
                        {
                            if (s2.accel >= 0)
                                target.angle = 90.0f;
                            else
                                target.angle = -90.0f;
                        }
                        else
                        {
                            target.angle = Math.Abs((float)(Math.Atan(s2.accel / s1.accel) * 180.0f / Math.PI));
                            if (s1.accel >= 0)
                                target.angle = 180.0f - target.angle;
                            if (s2.accel <= 0)
                                target.angle = -target.angle;
                        }

                        target.angle -= 90.0f;
                        if (target.angle < -180.0f)
                            target.angle += 360.0f;

                        break;
                    case 2:
                        break;
                    default:
                        break;
                }

            }

            return;
        }
        private bool UpdateAccelAngle(ref Axis s1, ref Axis s2, ref Axis target)
        {
            if ((s1.state & 1) == 0 && (s2.state & 1) == 0)
            {
                if (s1.accel == 0.0f)
                {
                    if (s2.accel >= 0)
                        target.angle = 90.0f;
                    else
                        target.angle = -90.0f;
                }
                else
                {
                    target.angle = Math.Abs((float)(Math.Atan(s2.accel / s1.accel) * 180.0f / Math.PI));
                    if (s1.accel >= 0)
                        target.angle = 180.0f - target.angle;
                    if (s2.accel <= 0)
                        target.angle = -target.angle;
                }
                
                if(target.index == 0)
                {
                    target.angle += 180.0f;
                    if (target.angle > 180.0f)
                        target.angle -= 360.0f;

                    if(target.mag < 0)
                    {
                        target.angle -= 180.0f;
                        if (target.angle < -180.0f)
                            target.angle += 360.0f;
                    }
                }
                
                if (target.index == 2)
                {
                    target.angle -= 90.0f;
                    if (target.angle < -180.0f)
                        target.angle += 360.0f;
                }
                
                target.state |= (1 << 2);

                return true;
            }

            target.state &= ~(1 << 2);

            return false;
        }
        private bool UpdateMagneticAngle(ref Axis s1, ref Axis s2, ref Axis target)
        {
            if((s1.state & 1) == 0 && (s2.state & 1)==0)
            {
                if (s1.mag == 0.0f)
                    target.mAngle = 0.0f;
                else
                {
                    target.mAngle = (float)(Math.Atan(s2.mag / s1.mag) * 180.0f / Math.PI);
                    
                    /*
                    target.mAngle = Math.Abs((float)(Math.Atan(s2.mag / s1.mag) * 180.0f / Math.PI));
                    if (s1.mag > 0)
                        target.mAngle = 180.0f - target.mAngle;
                    if (s2.mag < 0)
                        target.mAngle = -(target.mAngle);
                    */
                }

                target.state |= (1 << 3);

                return true;
            }

            target.state &= ~(1 << 3);

            
            return false;
        }
        private void ReadData()
        {
            int recvBytes;  //To know receive bytes from serial port
            int index = 0; int startIndex = 0; int packetCount = 0;

            //Variables to store sensor data to average package
            Int16 _gyroX, _gyroY, _gyroZ, _accelX, _accelY, _accelZ, _magX, _magY, _magZ;

            try
            {
                while (dataFlag)
                {
                    _gyroX = 0; _gyroY = 0; _gyroZ = 0;
                    _accelX = 0; _accelY = 0; _accelZ = 0;
                    _magX = 0; _magY = 0; _magZ = 0;

                    switch (DeviceRule)
                    {
                        case Rule.SensorNode:
                            {
                                Byte[] buffer = new Byte[DATA_BUFFER_SIZE];

                                //When sufficient data to receive
                                if (serialPort.BytesToRead >= DATA_BUFFER_SIZE)
                                {
                                    //Receive data
                                    recvBytes = serialPort.Read(buffer, 0, DATA_BUFFER_SIZE);

                                    //Confirm received data byte size
                                    if (recvBytes >= DATA_BUFFER_SIZE)
                                    {
                                        for (int i = 0; i < PACKAGE_COUNT; i++)
                                        {
                                            _accelX += BitConverter.ToInt16(buffer, i * DATA_LENGTH * sizeof(Int16) + sizeof(Int16) * 0);
                                            _accelY += BitConverter.ToInt16(buffer, i * DATA_LENGTH * sizeof(Int16) + sizeof(Int16) * 1);
                                            _accelZ += BitConverter.ToInt16(buffer, i * DATA_LENGTH * sizeof(Int16) + sizeof(Int16) * 2);
                                            _gyroX += BitConverter.ToInt16(buffer, i * DATA_LENGTH * sizeof(Int16) + sizeof(Int16) * 3);
                                            _gyroY += BitConverter.ToInt16(buffer, i * DATA_LENGTH * sizeof(Int16) + sizeof(Int16) * 4);
                                            _gyroZ += BitConverter.ToInt16(buffer, i * DATA_LENGTH * sizeof(Int16) + sizeof(Int16) * 5);
                                        }
                                        senseT.Add(BitConverter.ToInt32(buffer, DATA_LENGTH * PACKAGE_COUNT * sizeof(Int16)) / 1000.0f);
                                        if (senseT.Count > DATA_BUFFER_MAX)
                                            senseT.RemoveAt(0);

                                        //Average package data
                                        _gyroX = (Int16)(_gyroX / PACKAGE_COUNT);
                                        _gyroY = (Int16)(_gyroY / PACKAGE_COUNT);
                                        _gyroZ = (Int16)(_gyroZ / PACKAGE_COUNT);
                                        _accelX = (Int16)(_accelX / PACKAGE_COUNT);
                                        _accelY = (Int16)(_accelY / PACKAGE_COUNT);
                                        _accelZ = (Int16)(_accelZ / PACKAGE_COUNT);

                                        ManipulateGyro(_gyroX, _gyroY, _gyroZ);

                                        ManipulateAccelerometer(_accelX, _accelY, _accelZ);

                                        //Display data on the form
                                        this.Invoke(new MethodInvoker(delegate()
                                        {
                                            //Chart re-paint
                                            ChartP.DataSourceSettings.ReloadData();
                                            ChartA.DataSourceSettings.ReloadData();

                                        }));
                                    }
                                    else
                                    {
                                        Thread.Sleep(1);
                                    }

                                }
                            }
                            break;
                        case Rule.MotionNode:
                            {
                                bool rawFlag = false;

                                if (motionRaw.waitForData())
                                {
                                    byte[] rawBuffer = motionRaw.readData();
                                    if (rawBuffer != null)
                                    {
                                        IDictionary<int, MotionNode.SDK.Format.RawElement> rawMotion = MotionNode.SDK.Format.Raw(rawBuffer);

                                        foreach (KeyValuePair<int, MotionNode.SDK.Format.RawElement> itr in rawMotion)
                                        {
                                            axisX.updateAccelRaw(itr.Value.getAccelerometer()[0]);
                                            axisY.updateAccelRaw(itr.Value.getAccelerometer()[1]);
                                            axisZ.updateAccelRaw(itr.Value.getAccelerometer()[2]);

                                            rawFlag = true;
                                        }
                                    }
                                }

                                if (rawFlag && motionSensor.waitForData())
                                {
                                    byte[] sensorBuffer = motionSensor.readData();
                                    if (sensorBuffer != null)
                                    {
                                        IDictionary<int, MotionNode.SDK.Format.SensorElement> sensorMotion = MotionNode.SDK.Format.Sensor(sensorBuffer);
                                        
                                        foreach (KeyValuePair<int, MotionNode.SDK.Format.SensorElement> itr in sensorMotion)
                                        {
                                            // Add Sensor Data to Graph
                                            accX.Add(itr.Value.getAccelerometer()[0]);
                                            accY.Add(itr.Value.getAccelerometer()[1]);
                                            accZ.Add(itr.Value.getAccelerometer()[2]);

                                            gyroX.Add(itr.Value.getGyroscope()[0]);
                                            gyroY.Add(itr.Value.getGyroscope()[1]);
                                            gyroZ.Add(itr.Value.getGyroscope()[2]);

                                            magX.Add(itr.Value.getMagnetometer()[0]);
                                            magY.Add(itr.Value.getMagnetometer()[1]);
                                            magZ.Add(itr.Value.getMagnetometer()[2]);

                                            if (accX.Count > DATA_BUFFER_MAX)
                                            {
                                                accX.RemoveAt(0); accY.RemoveAt(0); accZ.RemoveAt(0);
                                                gyroX.RemoveAt(0); gyroY.RemoveAt(0); gyroZ.RemoveAt(0);
                                                magX.RemoveAt(0); magY.RemoveAt(0); magZ.RemoveAt(0);
                                            }

                                            axisX.updateSensor(itr.Value.getAccelerometer()[0], itr.Value.getGyroscope()[0], itr.Value.getMagnetometer()[0]);
                                            axisY.updateSensor(itr.Value.getAccelerometer()[1], itr.Value.getGyroscope()[1], itr.Value.getMagnetometer()[1]);
                                            axisZ.updateSensor(itr.Value.getAccelerometer()[2], itr.Value.getGyroscope()[2], itr.Value.getMagnetometer()[2]);

                                            //UpdateAccelAngle(ref axisX, ref axisY, ref axisZ);
                                            //UpdateAccelAngle(ref axisY, ref axisZ, ref axisX);
                                            //UpdateAccelAngle(ref axisZ, ref axisX, ref axisY);

                                            UpdateMagneticAngle(ref axisX, ref axisY, ref axisZ);
                                            UpdateMagneticAngle(ref axisY, ref axisZ, ref axisX);
                                            UpdateMagneticAngle(ref axisZ, ref axisX, ref axisY);

                                            UpdateGyroAngle(ref axisX);
                                            //UpdateGyroAngle(ref axisY);
                                            UpdateGyroAngle(ref axisZ);

                                            UpdateRollAngle(ref axisY, ref axisZ, ref axisX);
                                            UpdatePitchAngle(ref axisX, ref axisY, ref axisZ);

                                            this.Invoke(new MethodInvoker(delegate()
                                            {
                                                this.AccelXData.Text = itr.Value.getAccelerometer()[0].ToString();
                                                this.AccelYData.Text = itr.Value.getAccelerometer()[1].ToString();
                                                this.AccelZData.Text = itr.Value.getAccelerometer()[2].ToString();

                                                this.GyroXData.Text = itr.Value.getGyroscope()[0].ToString();
                                                this.GyroYData.Text = itr.Value.getGyroscope()[1].ToString();
                                                this.GyroZData.Text = itr.Value.getGyroscope()[2].ToString();

                                                this.MagnetoXData.Text = itr.Value.getMagnetometer()[0].ToString();
                                                this.MagnetoYData.Text = itr.Value.getMagnetometer()[1].ToString();
                                                this.MagnetoZData.Text = itr.Value.getMagnetometer()[2].ToString();

                                                this.VelocityXData.Text = axisX.mAngle.ToString();
                                                this.VelocityYData.Text = axisY.mAngle.ToString();
                                                this.VelocityZData.Text = axisZ.mAngle.ToString();

                                                this.PositionXData.Text = axisX.state.ToString();
                                                this.PositionYData.Text = axisY.state.ToString();
                                                this.PositionZData.Text = axisZ.state.ToString();

                                                this.RollData.Text = axisX.angle.ToString();
                                                this.PitchData.Text = axisY.angle.ToString();
                                                this.YawData.Text = axisZ.angle.ToString();

                                                ChartP.DataSourceSettings.ReloadData();
                                                ChartA.DataSourceSettings.ReloadData();
                                            }));
                                        }
                                    }
                                }

                                if(motionPreview.waitForData())
                                {
                                    byte[] previewBuffer = motionPreview.readData();
                                    if(previewBuffer != null)
                                    {
                                        IDictionary<int, MotionNode.SDK.Format.PreviewElement> previewMotion = MotionNode.SDK.Format.Preview(previewBuffer);

                                        foreach (KeyValuePair<int, MotionNode.SDK.Format.PreviewElement> itr in previewMotion)
                                        {
                                            axisX.eulerAngle = itr.Value.getEuler()[0] * 180.0f / (float)Math.PI;
                                            axisY.eulerAngle = itr.Value.getEuler()[1] * 180.0f / (float)Math.PI;
                                            axisZ.eulerAngle = itr.Value.getEuler()[2] * 180.0f / (float)Math.PI;
                                        }

                                        this.Invoke(new MethodInvoker(delegate()
                                            {
                                                this.AngularXData.Text = axisX.eulerAngle.ToString();
                                                this.AngularYData.Text = axisY.eulerAngle.ToString();
                                                this.AngularZData.Text = axisZ.eulerAngle.ToString();
                                            }));
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                    }    
                    Thread.Sleep(1);
                }
            }
            catch (TimeoutException e)
            {
                MessageBox.Show(e.ToString());
            }
            catch (Exception e)
            {
                MessageBox.Show("Index : " + index.ToString() + " Start_Index : " + startIndex.ToString() + " " + e.ToString());
            }
        }

        //*** Render Method
        private void SetupScene(IntPtr hWnd)
        {
            TV_3DVECTOR Min = new TV_3DVECTOR();
            TV_3DVECTOR Max = new TV_3DVECTOR();
            TV_3DVECTOR Offset = new TV_3DVECTOR();
            TV_3DVECTOR SphereCenter = new TV_3DVECTOR();
            TV_3DVECTOR posVector = new TV_3DVECTOR();

            TV = new TVEngine();
            Scene = new TVScene();
            Cam = new TVCamera();
            TF = new TVTextureFactory();
            Mats = new TVMaterialFactory();
            Lights = new TVLightEngine();
            Maths = new TVMathLibrary();
            
            //Initialize the TV engine
            TV.SetDebugFile(Application.StartupPath + "\\debug.txt");
            TV.Init3DWindowed(hWnd, true);
            TV.SetSearchDirectory(Application.StartupPath);
            TV.SetAngleSystem(CONST_TV_ANGLE.TV_ANGLE_DEGREE);  // use degree system

            Scene.SetBackgroundColor(0.6f, 0.6f, 0.6f);

            //Load the texture into the integer ID holder
            FloorTex = TF.LoadTexture("smallGrid.bmp", "Grid", -1, -1, CONST_TV_COLORKEY.TV_COLORKEY_NO, true);
            WallTex = TF.LoadTexture("smallGridWall.bmp", "Wall", -1, -1, CONST_TV_COLORKEY.TV_COLORKEY_NO, true);

            Room = Scene.CreateMeshBuilder("Room");
            Room.AddFloor(FloorTex, -300.0f, -300.0f, 300.0f, 300.0f, 0, 15.0f, 15.0f, true);
            Room.AddWall(WallTex, -300.0f, 300.0f, 300.0f, 300.0f, 400.0f, 0, 15.0f, 15.0f, true);
            Room.AddWall(WallTex, 300.0f, -300.0f, -300.0f, -300.0f, 400.0f, 0, 15.0f, 15.0f, true);
            Room.AddWall(WallTex, 300.0f, 300.0f, 300.0f, -300.0f, 400.0f, 0, 15.0f, 15.0f, true);
            Room.AddWall(WallTex, -300.0f, -300.0f, -300.0f, 300.0f, 400.0f, 0, 15.0f, 15.0f, true);
            //ShowVector("RoomLocal", Room.GetPosition());
            //ShowVector("RoomWorld", Room.GetWorldPosition(Room.GetPosition()));

            SensorNode = Scene.CreateMeshBuilder("SensorNode");
            SensorNode.LoadXFile("N70.X", true, true);
            SensorNode.SetParent(CONST_TV_NODETYPE.TV_NODETYPE_MESH, Room.GetIndex(), 1);
            SensorNode.SetPosition(0.0f, 150.0f, 0.0f);
            TV_3DVECTOR oriScale = SensorNode.GetScale();
            SensorNode.SetScale(oriScale.x, oriScale.y * 0.4f, oriScale.z * 3.0f);
            SensorNode.SetLightingMode(CONST_TV_LIGHTINGMODE.TV_LIGHTING_NORMAL, 0, 1);
            SensorNode.SetRotation(90.0f, 0.0f, 0.0f);
            //SensorNode.GetBoundingBox(ref Min, ref Max, true);
            //SensorNode.ShowBoundingBox(true);
            //ShowVector("MoteLocal", SensorNode.GetPosition());
            //ShowVector("MoteWorld", Room.GetWorldPosition(SensorNode.GetPosition()));
            
            //WiimoteMesh = Scene.CreateMeshBuilder("Wiimote");
            //WiimoteMesh.LoadXFile("Wiimote.X", true, true);
            //TV_3DVECTOR oriScale = WiimoteMesh.GetScale();
            //WiimoteMesh.SetScale(oriScale.x * 0.15f, oriScale.y * 0.15f, oriScale.z * 0.15f);
            //WiimoteMesh.SetPosition(0.0f, 70.0f, 0.0f);
            //WiimoteMesh.SetLightingMode(CONST_TV_LIGHTINGMODE.TV_LIGHTING_NORMAL, 0, 1);//Tell TV we want normal, per-vertex, lighting with 1 point light.

            Cam.SetParent(CONST_TV_NODETYPE.TV_NODETYPE_MESH, Room.GetIndex(), 0);
            Cam.SetPosition(0.0f, 250.0f, 200.0f);
            Cam.LookAtMesh(SensorNode);
            //ShowVector("CamLocal", Cam.GetPosition());
            //ShowVector("CamWorld", Room.GetWorldPosition(Cam.GetPosition()));

            //Create a point light. Again, these can be more complex.
            IDLight = Lights.CreatePointLight(new TV_3DVECTOR(0.0f, 250.0f, 200.0f), 0.9f, 0.9f, 0.9f, 250.0f);
            IDBackLight = Lights.CreatePointLight(new TV_3DVECTOR(0.0f, 250.0f, -200.0f), 0.1f, 0.1f, 0.1f, 250.0f);
            Lights.SetSpecularLighting(false);

        }

        private void TV_Loop()
        {
            try
            {
                while (bLoop)
                {
                    //Render a frame
                    TV.Clear(false);

                    //UpdateScene();
                    switch (DeviceRule)
                    {
                        case Rule.SensorNode:
                        case Rule.MotionNode:
                            SensorNode.Render();
                            break;
                        case Rule.Wiimote:
                            WiimoteMesh.Render();
                            break;
                        default:
                            break;
                    }
                    
                    Room.Render();
                    
                    TV.RenderToScreen();

                    UpdateScene();
                    Application.DoEvents();
                    Thread.Sleep(1);
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }

        private void UpdateScene()
        {
            //Rotate mote
            switch (DeviceRule)
            {
                case Rule.SensorNode:
                    SensorNode.SetRotation((float)angleXQ.Last(), (float)angleYQ.Last(), (float)angleZQ.Last());
                    break;
                case Rule.MotionNode:
                    //SensorNode.SetRotation(0.0f, axisY.mAngle, axisZ.angle);
                    SensorNode.SetRotation(axisX.angle, axisY.mAngle, axisZ.angle);
                    break;
                case Rule.Wiimote:
                    break;
                default:
                    break;
            }
        }

        //*** Option Load
        public void loadOptionFromFile()
        {
            // create reader & open file
            try
            {
                TextReader tr = new StreamReader("config.dat");
                char[] seps = { ':' };
                String line;
                String[] values;

                line = tr.ReadLine();
                values = line.Split(seps);
                switch (int.Parse(values[1]))
                {
                    case 0:
                        DeviceRule = Rule.SensorNode;
                        break;
                    case 1:
                        DeviceRule = Rule.Wiimote;
                        break;
                    case 2:
                        DeviceRule = Rule.MotionNode;
                        break;
                }

                line = tr.ReadLine();
                values = line.Split(seps);
                DATA_BUFFER_MAX = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                DATA_LENGTH = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                PACKAGE_COUNT = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                GYRO_CALIBRATION_COUNT = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                GYRO_ANCHOR_WINDOW_SIZE = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                ACCEL_CALIBRATION_COUNT = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                ACCEL_ANCHOR_WINDOW_SIZE = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                MAG_CALIBRATION_COUNT = int.Parse(values[1]);

                line = tr.ReadLine();
                values = line.Split(seps);
                MAG_ANCHOR_WINDOW_SIZE = int.Parse(values[1]);

                // close the stream
                tr.Close();
            }
            catch (System.NullReferenceException)
            {

            }
            catch (System.FormatException)
            {
                //bad config, ignore
                throw new Exception("Config file is mal-formatted.");

            }
            catch (System.IO.FileNotFoundException)
            {
                saveOptionAsFile(true);

                return;
            }
        }

        public void saveOptionAsFile(bool errorFlag)
        {
            //no prexsting config, create one with the deafult values

            TextWriter tw = new StreamWriter("config.dat");

            // write a line of text to the file
            if (errorFlag)
            {
                tw.WriteLine("device(0-SensorNode, 1-Wiimote. 2-MotionNode): 2");
            }
            else
            {
                switch (DeviceRule)
                {
                    case Rule.SensorNode:
                        tw.WriteLine("device(0-SensorNode, 1-Wiimote, 2-MotionNode): 0");
                        break;
                    case Rule.Wiimote:
                        tw.WriteLine("device(0-SensorNode, 1-Wiimote, 2-MotionNode): 1");
                        break;
                    case Rule.MotionNode:
                        tw.WriteLine("device(0-SensorNode, 1-Wiimote, 2-MotionNode): 2");
                        break;
                }
            }

            tw.WriteLine("data buffer size:" + DATA_BUFFER_MAX);
            tw.WriteLine("sensor data length in one package:" + DATA_LENGTH);
            tw.WriteLine("package number in one receive time:" + PACKAGE_COUNT);
            tw.WriteLine("gyro calibration count:" + GYRO_CALIBRATION_COUNT);
            tw.WriteLine("gyro anchor window size:" + GYRO_ANCHOR_WINDOW_SIZE);
            tw.WriteLine("accel calibration count:" + ACCEL_CALIBRATION_COUNT);
            tw.WriteLine("accel anchor window size:" + ACCEL_ANCHOR_WINDOW_SIZE);
            tw.WriteLine("mag calibration count:" + MAG_CALIBRATION_COUNT);
            tw.WriteLine("mag anchor window size:" + MAG_ANCHOR_WINDOW_SIZE);

            // close the stream
            tw.Close();
        }
    }


    //*** Axis Information Struct
    // state - Flag variables to store some information
    // use 5 bit from lower
    // 1st bit - define dynamic(1) or static(0)
    // 2nd bit - define close g(1) or not(0)
    // 3rd bit - accelerometer use(1) or not(0)
    // 4th bit - gyroscope use(1) or not(0)
    // 5th bit - magenetometer use(1) or not(0)
    public class Axis
    {
        public short index;
        public short state;
        public int avgSum;
        public int devSum;
        public int sumIndex;
        public int gyroIndex;
        public int size;
        public int[] raw;
        public float accel;
        public float[] gyro;
        public float mag;
        public float angle;
        public float mAngle;
        public float accAngle;
        public float eulerAngle;
        public float stdev;
        public Axis(short i, int input)
        {
            index = i;
            state = 0; avgSum = 0; devSum = 0; sumIndex = 0; gyroIndex = 0; accel = 0.0f; mag = 0.0f; angle = 0.0f; mAngle = 0.0f;
            accAngle = 0.0f;
            eulerAngle = 0.0f;
            size = input;
            raw = new int[size];
            gyro = new float[4];
        }
        public void updateAccelRaw(int input)
        {
            avgSum -= raw[sumIndex];
            devSum -= raw[sumIndex] * raw[sumIndex];

            raw[sumIndex] = input;
            avgSum += input;
            devSum += input * input;

            sumIndex++;
            if (sumIndex >= size)
                sumIndex = 0;

            stdev = (float)devSum / size - ((float)avgSum / size) * ((float)avgSum / size);
            if (stdev > 4096.0f)
                state |= 1;
            else
                state &= 0;
        }
        public void updateSensor(float accVal, float gyroVal, float magVal)
        {
            updateAccel(accVal);
            updateGyro(gyroVal);
            updateMag(magVal);
        }
        private void updateAccel(float accVal)
        {
            accel = accVal;
            if (Math.Abs(accel) >= 0.65f)
                state |= (1 << 1);
            else
                state &= ~(1 << 1);
        }
        private void updateGyro(float gyroVal)
        {
            gyro[gyroIndex++] = gyroVal;
            if (gyroIndex >= 4)
                gyroIndex = 0;
        }
        private void updateMag(float magVal)
        {
            mag = magVal;
        }
    }

    //Enumeration to handle option
    public enum Rule
    {
        DCEM, EulerAngle, Quaternion, SensorNode, Wiimote, MotionNode
    }
}
