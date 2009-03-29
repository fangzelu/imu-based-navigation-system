using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using MotionNode.SDK;
using WiimoteLib;
using System.IO;
using System.IO.Ports;
using System.Runtime.InteropServices;

namespace StepCount
{
    public partial class ControlPanel : Form
    {
        //*** Coordinates
        int stageSize = 256;
        int stageIndex = 0;
        int x_init = 0, y_init = 0;
        float[] xc;
        float[] yc;
        float x_diff, x_prime, y_prime;

        //*** to Control
        bool motionUse = true, wiiUse = false, ymoteUse = false;
        bool readFlag = false, logFlag = false;
        TextWriter log = new StreamWriter("log.dat");
        TextWriter posLog = new StreamWriter("posLog.dat");
        Thread readThread;
        Axis x = new Axis(0);
        Axis y = new Axis(1);
        Axis z = new Axis(2);
        int movingCount = 0;

        //*** MotionNode SDK
        Client motionSensor, motionRaw;
        static int DATA_LENGTH = 6;
        static int PACKAGE_COUNT = 3;
        static int DATA_BUFFER_SIZE = DATA_LENGTH * PACKAGE_COUNT * sizeof(Int16) + sizeof(Int32);
        float mHeading = 0.0f;
        bool mMoving = false;

        //*** Y-Mote
        SerialPort serialPort = new SerialPort();


        public ControlPanel()
        {
            InitializeComponent();

            xc = new float[stageSize];
            yc = new float[stageSize];

            for (int i = 0; i < stageSize; i++)
            {
                xc[i] = 0.0f;
                yc[i] = 0.0f;
            }
        }

        private void Connect_Bt_Click(object sender, EventArgs e)
        {
            if(motionUse)
            {
                motionSensor = new Client("",32078);
                motionRaw = new Client("",32077);
            }

            if(ymoteUse)
            {
                serialPort.PortName = "COM5";
                serialPort.BaudRate = 57600;
                serialPort.Parity = Parity.None;
                serialPort.DataBits = 8;
                serialPort.StopBits = StopBits.One;
                serialPort.ReadBufferSize = 512;

                serialPort.Open();

                Thread.Sleep(1000);
            }
            readFlag = true;
            readThread = new Thread(ReadSensorData);
            readThread.IsBackground = true;
            readThread.Start();
        }

        private void Disconnect_Bt_Click(object sender, EventArgs e)
        {
            readFlag = false;

            if (motionUse)
            {
                motionSensor.close();
                motionRaw.close();
            }

            readThread.Abort();
        }

        private void mUpdateHeading(ref Axis x, ref Axis y)
        {
            mHeading = (float)Math.Atan2(x.mGetMag(), y.mGetMag());
        }
        private void ReadSensorData()
        {
            try
            {
                while(readFlag)
                {
                    // MotionNode Data Handling
                    if(motionUse)
                    {
                        bool rawGet = false;
                        if(motionRaw.waitForData())
                        {
                            byte[] rawBuffer = motionRaw.readData();
                            if(rawBuffer != null)
                            {
                                IDictionary<int, MotionNode.SDK.Format.RawElement> rawMotion = MotionNode.SDK.Format.Raw(rawBuffer);

                                foreach (KeyValuePair<int, MotionNode.SDK.Format.RawElement> itr in rawMotion)
                                {
                                    x.UpdateMotionAccelRaw(itr.Value.getAccelerometer()[0]);
                                    y.UpdateMotionAccelRaw(itr.Value.getAccelerometer()[1]);
                                    z.UpdateMotionAccelRaw(itr.Value.getAccelerometer()[2]);

                                    rawGet = true;

                                    this.Invoke(new MethodInvoker(delegate()
                                    {
                                        this.mAccRawX.Text = x.GetMotionAccelRaw().ToString();
                                        this.mAccRawY.Text = y.GetMotionAccelRaw().ToString();
                                        this.mAccRawZ.Text = z.GetMotionAccelRaw().ToString();

                                        this.mAccX.Text = x.GetMotionAccel().ToString();
                                        this.mAccY.Text = y.GetMotionAccel().ToString();
                                        this.mAccZ.Text = z.GetMotionAccel().ToString();

                                        this.mAccDevX.Text = x.mAccRawDev.ToString();
                                        this.mAccDevY.Text = y.mAccRawDev.ToString();
                                        this.mAccDevZ.Text = z.mAccRawDev.ToString();

                                        this.mAccAvgX.Text = x.mAccAvg.ToString();
                                        this.mAccAvgY.Text = y.mAccAvg.ToString();
                                        this.mAccAvgZ.Text = z.mAccAvg.ToString();
                                    }));
                                }
                            }
                        }

                        if(rawGet && motionSensor.waitForData())
                        {
                            byte[] sensorBuffer = motionSensor.readData();
                            if(sensorBuffer != null)
                            {
                                IDictionary<int, MotionNode.SDK.Format.SensorElement> sensorMotion = MotionNode.SDK.Format.Sensor(sensorBuffer);
                                        
                                foreach (KeyValuePair<int, MotionNode.SDK.Format.SensorElement> itr in sensorMotion)
                                {
                                    x.UpdateMotionAccel(itr.Value.getAccelerometer()[0]);
                                    y.UpdateMotionAccel(itr.Value.getAccelerometer()[1]);
                                    z.UpdateMotionAccel(itr.Value.getAccelerometer()[2]);

                                    x.UpdateMotionMag(itr.Value.getMagnetometer()[0]);
                                    y.UpdateMotionMag(itr.Value.getMagnetometer()[1]);
                                    z.UpdateMotionMag(itr.Value.getMagnetometer()[2]);
                                }

                                if (x.mAccStop && y.mAccStop && z.mAccStop)
                                {
                                    if(mMoving)
                                    {
                                        x.mBacktrackPosition(movingCount);
                                        y.mBacktrackPosition(movingCount);
                                        z.mBacktrackPosition(movingCount);

                                        if(movingCount >= 30)
                                        {
                                            stageIndex -= 30;
                                            if (stageIndex < 0)
                                                stageIndex = stageSize + stageIndex;
                                        }
                                        else
                                        {
                                            stageIndex -= movingCount + 1;
                                            if (stageIndex < 0)
                                                stageIndex = stageSize + stageIndex;
                                        }

                                        int bb = stageIndex - 1;
                                        if (bb < 0)
                                            bb = stageSize - 1;
                                        posLog.WriteLine(DateTime.Now.ToString() + "," + xc[bb].ToString() + "," + yc[bb].ToString()+ ","+movingCount.ToString());
                                    }
                                    mUpdateHeading(ref z, ref y);
                                    mMoving = false;
                                    movingCount = 0;
                                }

                                if (!x.mAccStop || !y.mAccStop || !z.mAccStop)
                                {
                                    movingCount++;

                                    x.UpdateMotionPosition();
                                    y.UpdateMotionPosition();
                                    z.UpdateMotionPosition();

                                    x_diff = z.mGetPositionDiff();
                                    x_prime = (float)(x_diff * Math.Cos(mHeading));
                                    y_prime = (float)(x_diff * Math.Sin(mHeading));
                                    int b = stageIndex - 1;
                                    if (b < 0)
                                        b = stageSize - 1;
                                    xc[stageIndex] = xc[b] + x_prime;
                                    yc[stageIndex] = yc[b] + y_prime;

                                    stageIndex++;
                                    if (stageIndex >= stageSize)
                                        stageIndex = 0;

                                    mMoving = true;

                                    if (logFlag)
                                    {
                                        x.WriteMotionLog(log, "MotionX", mHeading);
                                        y.WriteMotionLog(log, "MotionY", mHeading);
                                        z.WriteMotionLog(log, "MotionZ", mHeading);

                                        int bb = stageIndex - 1;
                                        if(bb < 0)
                                            bb = stageSize - 1;
                                        posLog.WriteLine(DateTime.Now.ToString() + "," + xc[bb].ToString() + "," + yc[bb].ToString());
                                    }
                                }

                                string sendMsg = "";
                                int bbb = stageIndex - 1;
                                if(bbb < 0)
                                    bbb = stageSize - 1;
                                sendMsg = DateTime.Now.ToString() + "," + xc[bbb].ToString() + "," + yc[bbb].ToString();
                                SendMessage("WifiLoc", sendMsg);

                                this.Invoke(new MethodInvoker(delegate()
                                {
                                    this.mAccPosX.Text = x.GetMotionPosition().ToString();
                                    this.mAccPosY.Text = y.GetMotionPosition().ToString();
                                    this.mAccPosZ.Text = z.GetMotionPosition().ToString();

                                    this.mMagX.Text = x.mGetMag().ToString();
                                    this.mMagY.Text = y.mGetMag().ToString();
                                    this.mMagZ.Text = z.mGetMag().ToString();

                                    this.mHeadZY.Text = (mHeading * 180.0f / Math.PI).ToString();

                                    this.mLocalX.Text = x_diff.ToString();

                                    this.mWorldX.Text = x_prime.ToString();
                                    this.mWorldY.Text = y_prime.ToString();

                                    int b = stageIndex - 1;
                                    if(b < 0)
                                        b = stageSize - 1;
                                    this.mStageX.Text = (xc[b]).ToString();
                                    this.mStageY.Text = (yc[b]).ToString();
                                }));
                            }
                        }
                    }

                    // WiiMote Data Handling

                    // Y-Mote Data Handling
                    if(ymoteUse)
                    {
                        Byte[] buffer = new Byte[DATA_BUFFER_SIZE];

                        if(serialPort.BytesToRead >= DATA_BUFFER_SIZE)
                        {
                            int recvBytes = serialPort.Read(buffer, 0, DATA_BUFFER_SIZE);

                            if(recvBytes >= DATA_BUFFER_SIZE)
                            {

                            }
                        }
                    }

                    Thread.Sleep(1);
                }
            }
            catch (System.Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }

        private void Start_Bt_Click(object sender, EventArgs e)
        {
            x.SetMotionPositionZero();
            y.SetMotionPositionZero();
            z.SetMotionPositionZero();

            for (int i = 0; i < stageSize; i++)
            {
                xc[i] = 0.0f;
                yc[i] = 0.0f;
            }
            logFlag = true;
        }

        private void Pause_Bt_Click(object sender, EventArgs e)
        {
            logFlag = false;
        }

        void SendMessage(string strProgramName, string strMessage)
        {
            try
            {
                Win32API.COPYDATASTRUCT copyDataStruct = new Win32API.COPYDATASTRUCT();
                copyDataStruct.dwData = (IntPtr)0; // 임시값
                copyDataStruct.cbData = strMessage.Length * 2 + 1; // 한글 코드 지원
                copyDataStruct.lpData = strMessage; // 보낼 메시지

                IntPtr wndPtr = Win32API.FindWindow(null, strProgramName); // 

                if (wndPtr == IntPtr.Zero) return;

                IntPtr tempPtr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(Win32API.COPYDATASTRUCT)));
                Marshal.StructureToPtr(copyDataStruct, tempPtr, true);
                Win32API.SendMessage(wndPtr, Win32API.WM_COPYDATA, IntPtr.Zero, tempPtr);
            }
            catch (Exception except)
            {
                MessageBox.Show(except.Message);
            }
        }

    }

    public class Axis
    {
        Random rand = new Random();
        const float G = 9.80665f;

        // Axis Index 0 = x, 1 = y, 2 = z
        short index;

        // MotionNode Data use 'm'
        public int mRawSize = 64;
        public int mSize = 64;
        public int mAccRawIndex = 0;
        public int mAccIndex = 0;
        public int mMagIndex = 0;
        public int mPosIndex = 0;
        public int[] mAccRaw;
        public int[] mGyroRaw;
        public int[] mMagRaw;
        public float[] mAcc;
        public float[] mGyro;
        public float[] mMag;
        public int mAccRawSum = 0;
        public int mAccRawSumSqr = 0;
        public float mAccRawDev;
        public float mAccSum = 0;
        public bool mAccStop = true;
        public float mAccAvg;
        public float mHeading;

        public float mT = 1.0f / 30.0f;
        public float[] mPos;

        // WiiMote Data use 'w'
        int[] wAccRaw;

        // Y-Mote Data use 'y'
        int yRawSize = 64;
        int[] yAccRaw;
        int[] yGyroRaw;

        public Axis(short val)
        {
            index = val;
            mAccRaw = new int[mRawSize];
            mGyroRaw = new int[mRawSize];
            mMagRaw = new int[mRawSize];
            mAcc = new float[mSize];
            mGyro = new float[mSize];
            mMag = new float[mSize];
            mPos = new float[mSize];
            for(int i=0 ; i < mSize ; i++)
            {
                mAccRaw[i] = 0;
                mAcc[i] = 0.0f;
                mPos[i] = 0.0f;
            }

            yAccRaw = new int[yRawSize];
            yGyroRaw = new int[yRawSize];
        }

        public int GetMotionAccelRaw()
        {
            int bIndex = mAccRawIndex - 1;
            if (bIndex < 0)
                bIndex = mSize - 1;
            return mAccRaw[bIndex];
        }
        public float GetMotionAccel()
        {
            int b = mAccIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mAcc[b];
        }
        public float GetMotionPosition()
        {
            int b = mPosIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mPos[b];
        }
        public float mGetPositionDiff()
        {
            int b = mPosIndex - 1;
            if (b < 0)
                b = mSize - 1;
            int bb = b - 1;
            if (bb < 0)
                bb = mSize - 1;

            return mPos[b] - mPos[bb];
        }
        public float mGetMag()
        {
            int b = mMagIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mMag[b];
        }
        public void UpdateMotionAccelRaw(int val)
        {
            mAccRawSum += val;
            mAccRawSum -= mAccRaw[mAccRawIndex];
            mAccRawSumSqr += val * val;
            mAccRawSumSqr -= mAccRaw[mAccRawIndex] * mAccRaw[mAccRawIndex];

            mAccRaw[mAccRawIndex] = val;
            mAccRawIndex++;
            if (mAccRawIndex >= mRawSize)
                mAccRawIndex = 0;

            mAccRawDev = (float)mAccRawSumSqr / mSize - (float)mAccRawSum / mSize * (float)mAccRawSum / mSize;
            if (mAccRawDev > 1024.0f)
                mAccStop = false;
            else
                mAccStop = true;
        }

        public void UpdateMotionAccel(float val)
        {
            mAccSum += val;
            mAccSum -= mAcc[mAccIndex];

            mAcc[mAccIndex] = val;
            mAccIndex++;
            if (mAccIndex >= mSize)
                mAccIndex = 0;

            if (mAccStop)
                mAccAvg = mAccSum / mSize;
        }

        public void UpdateMotionMag(float val)
        {
            mMag[mMagIndex] = val;
            mMagIndex++;
            if(mMagIndex >= mSize)
                mMagIndex = 0;
        }

        public void mBacktrackPosition(int step)
        {
            if(step >= 30)
            {
                mPosIndex -= 30;
                if (mPosIndex < 0)
                    mPosIndex = mSize + mPosIndex;
            }
            else
            {
                mPosIndex -= step + 1;
                if (mPosIndex < 0)
                    mPosIndex = mSize + mPosIndex;
            }
        }
        public void UpdateMotionPosition()
        {
            int bIndex = mPosIndex - 1;
            int bmAccIndex = mAccIndex - 1;
            if (bIndex < 0)
                bIndex = mSize - 1;
            if (bmAccIndex < 0)
                bmAccIndex = mSize - 1;
            mPos[mPosIndex] = mPos[bIndex] + (1.0f / 2.0f * mT * mT * (mAcc[bmAccIndex] - mAccAvg) * G);
            mPosIndex++;
            if (mPosIndex >= mSize)
                mPosIndex = 0;
        }

        public void SetMotionPositionZero()
        {
            for (int i = 0; i < mSize; i++)
                mPos[i] = 0.0f;
        }

        public void WriteMotionLog(TextWriter log, string title, float h)
        {
            log.Write(title + "," + DateTime.Now.ToString()+",");
            int bmAccRawIndex = mAccRawIndex - 1;
            int bmAccIndex = mAccIndex - 1;
            int bmPosIndex = mPosIndex - 1;
            if (bmAccRawIndex < 0)
                bmAccRawIndex = 0;
            if (bmAccIndex < 0)
                bmAccIndex = 0;
            if (bmPosIndex < 0)
                bmPosIndex = 0;
            log.Write(mAccRaw[bmAccRawIndex].ToString() + "," + mAcc[bmAccIndex].ToString() + "," + mPos[bmPosIndex].ToString()+",");
            log.Write(mAccRawDev.ToString() + "," + mAccAvg.ToString()+","+h.ToString()+",");
            if (mAccStop)
                log.WriteLine("Stop");
            else
                log.WriteLine("");
        }

    }

    class Win32API
    {
        public const int WM_COPYDATA = 0x004A;

        public struct COPYDATASTRUCT
        {
            public IntPtr dwData;
            public int cbData;
            [MarshalAs(UnmanagedType.LPStr)]
            public string lpData;
        }

        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);
        [DllImport("User32.dll")]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

    }
}
