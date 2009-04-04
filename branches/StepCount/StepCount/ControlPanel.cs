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
        const int GRID = 2;
        int stageSize = 256;
        int stageIndex = 0;
        int x_init = 0, y_init = 0;
        float[] xc;
        float[] yc;
        float x_diff, xR_diff;

        int stageIndex_tilt = 0;
        float[] xc_tilt;
        float[] yc_tilt;

        int stageIndexR = 0;
        int stageIndexR_tilt = 0;
        float[] xcR;
        float[] ycR;
        float[] xcR_tilt;
        float[] ycR_tilt;

        //*** to Control
        bool motionUse = true, wiiUse = false, ymoteUse = false;
        bool readFlag = false, logFlag = false;
        TextWriter log = new StreamWriter("log.dat");
        TextWriter posLog = new StreamWriter("posLog.dat");
        Thread readThread;
        Axis x = new Axis(0);
        Axis y = new Axis(1);
        Axis z = new Axis(2);
        int movingCount = 0;    // Moving Sample Count
        int movingMin = 4096;   // Moving Min
        int movingMax = 0;      // Moving Max
        int movingDirection = -1;    // Moving Direction
        int movingVibe = 0;     // Moving Vibration
        float movingDistance = 0;   // Moving Distance
        float movingDistanceR = 0;  // Moving Distance Runge-Kutta
        float movingDev = 0.0f; // Moving Standard Deviation
        float movingDevMax = 0.0f;  // Moving Max Deviation
        int movingStep = 0;

        int movingChange = 0;   // Moving Change
        int movingChangeD = 0;  // Moving Change Direction

        int peakDirectionX = 0;
        int peakDirectionY = 0;
        int peakDirectionZ = 0;
        int peakChangeX = 0;
        int peakChangeY = 0;
        int peakChangeZ = 0;

        int peakAccelRawX = 0;
        int peakAccelRawY = 0;
        int peakAccelRawZ = 0;
        int peakFlagX = 0;
        int peakFlagY = 0;
        int peakFlagZ = 0;

        float peakAccelX = 0;
        float peakAccelY = 0;
        float peakAccelZ = 0;
        
        //*** MotionNode SDK
        Client motionSensor, motionRaw;

        int mHeadingIndex = 0;
        int mTiltHeadingIndex = 0;
        int mHeadSize = 32;
        float mHeadingSum = 0.0f;
        float mTiltHeadingSum = 0.0f;
        float mHeadingAvg = 0.0f;
        float mTiltHeadingAvgTest = 0.0f;
        float mTiltHeadingAvg = 0.0f;
        float[] mHeading;
        float[] mTiltHeading;
        bool mMoving = false;

        //*** Y-Mote
        SerialPort serialPort = new SerialPort();
        static int DATA_LENGTH = 6;
        static int PACKAGE_COUNT = 3;
        static int DATA_BUFFER_SIZE = DATA_LENGTH * PACKAGE_COUNT * sizeof(Int16) + sizeof(Int32);

        public ControlPanel()
        {
            InitializeComponent();

            xc = new float[stageSize];
            yc = new float[stageSize];
            xc_tilt = new float[stageSize];
            yc_tilt = new float[stageSize];
            xcR = new float[stageSize];
            ycR = new float[stageSize];
            xcR_tilt = new float[stageSize];
            ycR_tilt = new float[stageSize];

            for (int i = 0; i < stageSize; i++)
            {
                xc[i] = 0.0f;
                yc[i] = 0.0f;
                xc_tilt[i] = 0.0f;
                yc_tilt[i] = 0.0f;
                xcR[i] = 0.0f;
                ycR[i] = 0.0f;
                xcR_tilt[i] = 0.0f;
                ycR_tilt[i] = 0.0f;
            }

            mHeading = new float[mHeadSize];
            mTiltHeading = new float[mHeadSize];

            for(int i = 0 ; i < mHeadSize ; i++)
            {
                mHeading[i] = 0.0f;
                mTiltHeading[i] = 0.0f;
            }

            posLog.WriteLine("시간" + "," +
                //xc[bb].ToString() + "," + yc[bb].ToString() + "," +
                "RawX" + "," + "RawY" + "," +
                //xcR[bbR].ToString() + "," + ycR[bbR].ToString() + "," +
                "린지X" + "," + "린지Y" + "," +
                "AccelRawX" + "," + "AccleRawY," +
                "AccelRaw" + "," + "RawAvg" + "," + "VeloRaw" + "," + "PosRaw" + "," +
                "AccelX," + "AccelY," +
                "Accel" + "," + "Avg" + "," + "Velo" + "," + "Pos" + "," +
                "샘플수" + "," + "진동수" + "," +
                "변화X,상태X,피크RawX,피크X," +
                "변화Y,상태Y,피크RawY,피크Y," +
                "변화Z,상태Z,피크RawZ,피크Z," +
                "최소" + "," + "최대" + "," + "범위" + "," +
                "분산" + "," + "최대분산" + "," + "Raw이동거리" + "," + "린지이동거리" + "," +
                //movingDev.ToString() + "," + movingDistanceR.ToString() + "," +
                "자기X" + "," + "자기Y" + "," + "자기Z" + "," +
                //mHeadingAvg.ToString() + "," + mTiltHeadingAvg.ToString() + "," +
                "틸트헤딩" + "," + "실시간헤딩," + "TestHeading," +
                "피치" + "," + "롤");
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

                log.WriteLine("End");
                posLog.WriteLine("End");
                log.Close();
                posLog.Close();
            }

            readThread.Abort();
        }

        private void UpdateWorldPosition(float diff, float heading, ref float[] x, ref float[] y, ref int index)
        {
            heading = -heading;
            float x_prime = (float)-(diff * Math.Sin(heading));
            float y_prime = (float)(diff * Math.Cos(heading));
            int b = index - 1;
            if (b < 0)
                b = stageSize - 1;
            x[index] = x[b] + x_prime;
            y[index] = y[b] + y_prime;

            index++;
            if (index >= stageSize)
                index = 0;
        }
        private void mUpdateHeading(ref Axis x, ref Axis y)
        {
            float yval = y.mGetMag();
            if (yval < -25.0f)
                yval = -25.0f;
            else if (yval > 25.0f)
                yval = 25.0f;
            float xval = x.mGetMag();
            if (xval < -25.0f)
                xval = -25.0f;
            else if (xval > 25.0f)
                xval = 25.0f;

            float cur = (float)Math.Atan2(yval, xval);

            mHeadingSum += cur;
            mHeadingSum -= mHeading[mHeadingIndex];

            mHeading[mHeadingIndex] = cur;
            mHeadingIndex++;
            mHeadingIndex = (mHeadingIndex >= mHeadSize) ? 0 : mHeadingIndex;

            mHeadingAvg = mHeadingSum / mHeadSize;
        }
        private void mUpdateTiltHeading(ref Axis x, ref Axis y, ref Axis z)
        {
            float yval = y.mGetMag();
            if (yval < -25.0f)
                yval = -25.0f;
            else if (yval > 25.0f)
                yval = 25.0f;
            float xval = x.mGetMag();
            if (xval < -25.0f)
                xval = -25.0f;
            else if (xval > 25.0f)
                xval = 25.0f;
            float zval = x.mGetMag();
            if (zval < -25.0f)
                zval = -25.0f;
            else if (zval > 25.0f)
                zval = 25.0f;

            //double x_prime = x.mGetMag() * Math.Cos(x.mGetAngle()) + y.mGetMag() * Math.Sin(y.mGetAngle()) * Math.Sin(x.mGetAngle()) + z.mGetMag() * Math.Cos(y.mGetAngle()) * Math.Sin(x.mGetAngle());
            //double y_prime = y.mGetMag() * Math.Cos(y.mGetAngle()) - z.mGetMag() * Math.Sin(x.mGetAngle());

            double x_prime = xval * Math.Cos(x.mGetAngle()) + yval * Math.Sin(y.mGetAngle()) * Math.Sin(x.mGetAngle()) + zval * Math.Cos(y.mGetAngle()) * Math.Sin(x.mGetAngle());
            double y_prime = yval * Math.Cos(y.mGetAngle()) - zval * Math.Sin(x.mGetAngle());

            float cur = (float)(Math.Atan2(y_prime, x_prime));

            mTiltHeadingSum += cur;
            mTiltHeadingSum -= mTiltHeading[mTiltHeadingIndex];

            mTiltHeading[mTiltHeadingIndex] = cur;
            mTiltHeadingIndex++;
            mTiltHeadingIndex = (mTiltHeadingIndex >= mHeadSize) ? 0 : mTiltHeadingIndex;

            if(x.mAccStop && y.mAccStop && z.mAccStop)
            {
                mTiltHeadingAvg = mTiltHeadingSum / mHeadSize;
            }
            mTiltHeadingAvgTest = mTiltHeadingSum / mHeadSize;
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

                                    z.mUpdateAngle(ref y, ref x);
                                    y.mUpdateAngle(ref z, ref x);
                                }

                                mUpdateTiltHeading(ref z, ref y, ref x);
                                mTiltHeadingAvg = mTiltHeadingAvg - (float)(20.0f * Math.PI / 180.0f);
                                if(mTiltHeadingAvg < -Math.PI)
                                    mTiltHeadingAvg += (float)(2 * Math.PI);
                                mTiltHeadingAvgTest = mTiltHeadingAvgTest - (float)(20.0f * Math.PI / 180.0f);
                                if (mTiltHeadingAvgTest < -Math.PI)
                                    mTiltHeadingAvgTest += (float)(2 * Math.PI);

                                if (x.mAccStop && y.mAccStop && z.mAccStop)
                                {
                                    if(mMoving)
                                    {
                                        //x.mBacktrackPosition(movingCount);
                                        //y.mBacktrackPosition(movingCount);
                                        //z.mBacktrackPosition(movingCount);

                                        //if(movingCount >= 30)
                                        //{
                                        //    stageIndex -= 30;
                                        //    if (stageIndex < 0)
                                        //        stageIndex = stageSize + stageIndex;
                                        //}
                                        //else
                                        //{
                                        //    stageIndex -= movingCount + 1;
                                        //    if (stageIndex < 0)
                                        //        stageIndex = stageSize + stageIndex;
                                        //}

                                        posLog.WriteLine("Stop Position");
                                    }

                                    //x.SetMotionVelocityZero();
                                    //y.SetMotionVelocityZero();
                                    z.SetMotionVelocityZero();

                                    //mUpdateHeading(ref z, ref y);

                                    mMoving = false;
                                    movingCount = 0;
                                    movingMin = 4096;
                                    movingMax = 0;
                                    movingDirection = -1;
                                    movingDistance = 0.0f;
                                    movingDistanceR = 0.0f;
                                    movingVibe = 0;
                                    movingDev = 0.0f;
                                    movingDevMax = 0.0f;
                                    movingChangeD = 0;
                                    movingChange = 0;
                                    movingStep++;
                                }

                                if (!x.mAccStop || !y.mAccStop || !z.mAccStop)
                                {
                                    //x.UpdateMotionPosition();
                                    //y.UpdateMotionPosition();
                                    //z.UpdateMotionPosition();
                                    z.UpdateMotionPositionR();
                                    z.UpdateMotionPositionRRaw();

                                    //x_diff = z.mGetPositionDiff();
                                    xR_diff = z.mGetPositionRDiff();
                                    x_diff = z.mGetPositionRRawDiff();

                                    x_diff = Math.Abs(x_diff);
                                    xR_diff = Math.Abs(xR_diff);

                                    //UpdateWorldPosition(x_diff, mHeadingAvg, ref xc, ref yc, ref stageIndex);
                                    //UpdateWorldPosition(x_diff, mTiltHeadingAvg, ref xc_tilt, ref yc_tilt, ref stageIndex_tilt);

                                    //UpdateWorldPosition(xR_diff, mHeadingAvg, ref xcR, ref ycR, ref stageIndexR);
                                    UpdateWorldPosition(xR_diff, mTiltHeadingAvg, ref xcR_tilt, ref ycR_tilt, ref stageIndexR_tilt);

                                    movingDistanceR += xR_diff;

                                    UpdateWorldPosition(x_diff, mTiltHeadingAvg, ref xc_tilt, ref yc_tilt, ref stageIndex_tilt);
                                    movingDistance += x_diff;

                                    int temp = z.GetMotionAccelDirection();
                                    if (peakDirectionZ != temp)
                                    {
                                        peakAccelRawZ = z.GetMotionAccelPeakRaw();
                                        peakAccelZ = z.GetMotionAccelPeak();
                                        peakFlagZ = 1;
                                        peakChangeZ++;
                                    }
                                    else
                                        peakFlagZ = 0;
                                    peakDirectionZ = temp;

                                    temp = x.GetMotionAccelDirection();
                                    if (peakDirectionX != temp)
                                    {
                                        peakAccelRawX = x.GetMotionAccelPeakRaw();
                                        peakAccelX = x.GetMotionAccelPeak();
                                        peakFlagX = 1;
                                        peakChangeX++;
                                    }
                                    else
                                        peakFlagX = 0;
                                    peakDirectionX = temp;

                                    temp = y.GetMotionAccelDirection();
                                    if (peakDirectionY != temp)
                                    {
                                        peakAccelRawY = y.GetMotionAccelPeakRaw();
                                        peakAccelY = y.GetMotionAccelPeak();
                                        peakFlagY = 1;
                                        peakChangeY++;
                                    }
                                    else
                                        peakFlagY = 0;
                                    peakDirectionY = temp;

                                    movingMin = Math.Min(movingMin, z.GetMotionAccelRaw());
                                    movingMax = Math.Max(movingMax, z.GetMotionAccelRaw());
                                    movingDev = z.GetMotionAccelDev();
                                    movingDevMax = Math.Max(movingDevMax, movingDev);
                                    mMoving = true;
                                    movingCount++;
                                    int curD;
                                    if (z.GetMotionAccel() > z.mAccAvg)
                                        curD = 1;
                                    else
                                        curD = 0;
                                    if (curD != movingDirection)
                                        movingVibe++;
                                    movingDirection = curD;

                                    if (logFlag)
                                    {
                                        //x.WriteMotionLog(log, "MotionX", mHeading);
                                        //y.WriteMotionLog(log, "MotionY", mHeading);
                                        //z.WriteMotionLog(log, "MotionZ", mHeading);

                                        int bb = stageIndex - 1;
                                        if(bb < 0)
                                            bb = stageSize - 1;
                                        int bb_tilt = stageIndex_tilt - 1;
                                        if (bb_tilt < 0)
                                            bb_tilt = stageSize - 1;
                                        int bbR = (stageIndexR - 1 < 0) ? (stageSize + (stageIndexR - 1)) : (stageIndexR - 1);
                                        int bbR_tilt = (stageIndexR_tilt - 1 < 0) ? (stageSize + (stageIndexR_tilt - 1)) : (stageIndexR_tilt - 1);
                                        int b_heading = (mTiltHeadingIndex - 1 < 0) ? (mHeadSize - 1) : (mTiltHeadingIndex - 1);

                                        posLog.WriteLine(DateTime.Now.ToString() + "," +
                                            //xc[bb].ToString() + "," + yc[bb].ToString() + "," +
                                            xc_tilt[bb_tilt].ToString() + "," + yc_tilt[bb_tilt].ToString() + "," + 
                                            //xcR[bbR].ToString() + "," + ycR[bbR].ToString() + "," +
                                            xcR_tilt[bbR_tilt].ToString() + "," + ycR_tilt[bbR_tilt].ToString() + "," +
                                            x.GetMotionAccelRaw().ToString() + "," + y.GetMotionAccelRaw().ToString() + "," +
                                            z.GetMotionAccelRaw().ToString() + "," + z.mAccRawAvg.ToString() + "," + z.GetMotionVeloRaw().ToString() + "," + z.GetMotionPositionRRaw().ToString() + "," +
                                            x.GetMotionAccel().ToString() + "," + y.GetMotionAccel().ToString() + "," +
                                            z.GetMotionAccel().ToString() + "," + z.mAccAvg.ToString() + "," + z.GetMotionVelo().ToString() + "," + z.GetMotionPositionR().ToString() + "," +
                                            movingCount.ToString() + "," + movingVibe.ToString() + "," +
                                            peakChangeX.ToString() + "," + peakFlagX.ToString() + "," + peakAccelRawX.ToString() + "," + peakAccelX.ToString() + "," +
                                            peakChangeY.ToString() + "," + peakFlagY.ToString() + "," + peakAccelRawY.ToString() + "," + peakAccelY.ToString() + "," +
                                            peakChangeZ.ToString() + "," + peakFlagZ.ToString() + "," + peakAccelRawZ.ToString() + "," + peakAccelZ.ToString() + "," +
                                            movingMin.ToString() + "," + movingMax.ToString() + "," + ((movingMax - movingMin)/800.0f).ToString() + "," +
                                            movingDev.ToString() + "," + movingDevMax.ToString() + "," + movingDistance.ToString() + "," + movingDistanceR.ToString() + "," +
                                            //movingDev.ToString() + "," + movingDistanceR.ToString() + "," +
                                            x.mGetMag().ToString() + "," + y.mGetMag().ToString() + "," + z.mGetMag() + "," +
                                            //mHeadingAvg.ToString() + "," + mTiltHeadingAvg.ToString() + "," +
                                            (mTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," + (mTiltHeading[b_heading] * 180.0f / Math.PI).ToString() + "," + (mTiltHeadingAvgTest * 180.0f / Math.PI).ToString() + "," +
                                            (z.mGetAngle() * 180.0f / Math.PI).ToString() + "," + (y.mGetAngle() * 180.0f / Math.PI).ToString());
                                    }
                                }


                                this.Invoke(new MethodInvoker(delegate()
                                {
                                    //this.mAccPosX.Text = x.GetMotionPosition().ToString();
                                    //this.mAccPosY.Text = y.GetMotionPosition().ToString();
                                    this.mAccPosY.Text = z.GetMotionPositionR().ToString();
                                    this.mAccPosZ.Text = z.GetMotionPositionRRaw().ToString();

                                    this.mMagX.Text = x.mGetMag().ToString();
                                    this.mMagY.Text = y.mGetMag().ToString();
                                    this.mMagZ.Text = z.mGetMag().ToString();

                                    //this.mHeadZY.Text = (mHeadingAvg * 180.0f / Math.PI).ToString();
                                    this.mHeadTilt.Text = (mTiltHeadingAvg * 180.0f / Math.PI).ToString();

                                    this.mLocalX.Text = xR_diff.ToString();

                                    //int b = stageIndex - 1;
                                    //if(b < 0)
                                    //    b = stageSize - 1;
                                    //this.mStageX.Text = (xc[b]).ToString();
                                    //this.mStageY.Text = (yc[b]).ToString();

                                    int b = stageIndex_tilt - 1;
                                    if (b < 0)
                                        b = stageSize - 1;
                                    this.mStageTiltX.Text = (xc_tilt[b]).ToString();
                                    this.mStageTiltY.Text = (yc_tilt[b]).ToString();

                                    //b = (stageIndexR - 1 < 0) ? (stageSize + (stageIndexR - 1)) : (stageIndexR - 1);
                                    //this.mStageRX.Text = (xcR[b]).ToString();
                                    //this.mStageRY.Text = (ycR[b]).ToString();

                                    b = (stageIndexR_tilt - 1 < 0) ? (stageSize + (stageIndexR_tilt - 1)) : (stageIndexR_tilt - 1);
                                    this.mStageRTX.Text = (xcR_tilt[b]).ToString();
                                    this.mStageRTY.Text = (ycR_tilt[b]).ToString();

                                    this.mRoll.Text = (y.mGetAngle() * 180.0f / Math.PI).ToString();
                                    this.mPitch.Text = (z.mGetAngle() * 180.0f / Math.PI).ToString();
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
                xc_tilt[i] = 0.0f;
                yc_tilt[i] = 0.0f;
                xcR[i] = 0.0f;
                ycR[i] = 0.0f;
                xcR_tilt[i] = 0.0f;
                ycR_tilt[i] = 0.0f;
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

        private void Send_Bt_Click(object sender, EventArgs e)
        {
            string sendMsg = "";
            int bbb = stageIndex - 1;
            if (bbb < 0)
                bbb = stageSize - 1;
            sendMsg = DateTime.Now.ToString() + "," + xc[bbb].ToString() + "," + yc[bbb].ToString();
            SendMessage("WifiLoc", sendMsg);

        }

        private void KeyDownEvent(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.B:
                    posLog.WriteLine("Break The Corner," + movingStep.ToString());
                    movingStep = 0;
                    break;
                default:
                    break;
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
        public float mAccRawAvg;
        public float mAccRawDev;
        public float mAccSum = 0;
        public bool mAccStop = true;
        public float mAccAvg;
        public float mHeading;
        public float mAngle;

        public float mT = 1.0f / 30.0f;
        public float[] mPos;

        public float mTR = 1.0f / 60.0f;
        public int mVeloIndex = 0;
        public int mPosRIndex = 0;
        public int mVeloRawIndex = 0;
        public int mPosRRawIndex = 0;
        public float[] mPosR;
        public float[] mVelo;
        public float[] mVeloRaw;
        public float[] mPosRRaw;

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
            mVelo = new float[mSize];
            mPosR = new float[mSize];
            mVeloRaw = new float[mSize];
            mPosRRaw = new float[mSize];
            for(int i=0 ; i < mSize ; i++)
            {
                mAccRaw[i] = 0;
                mAcc[i] = 0.0f;
                mPos[i] = 0.0f;
                mVelo[i] = 0.0f;
                mPosR[i] = 0.0f;
                mVeloRaw[i] = 0.0f;
                mPosRRaw[i] = 0.0f;
            }

            yAccRaw = new int[yRawSize];
            yGyroRaw = new int[yRawSize];
        }

        public float GetMotionAccelDev()
        {
            return (float)Math.Sqrt(mAccRawDev);
        }
        public int GetMotionAccelRaw()
        {
            int bIndex = mAccRawIndex - 1;
            if (bIndex < 0)
                bIndex = mSize - 1;
            return mAccRaw[bIndex];
        }
        public int GetMotionAccelDirection()
        {
            int b = (mAccRawIndex - 1 < 0) ? (mSize - 1) : (mAccRawIndex - 1);
            int bb = (mAccRawIndex - 2 < 0) ? (mSize + (mAccRawIndex - 2)) : (mAccRawIndex - 2);

            if (mAccRaw[b] - mAccRaw[bb] > 0)
                return 1;
            else if (mAccRaw[b] - mAccRaw[bb] < 0)
                return 2;
            else
                return 0;
        }
        public int GetMotionAccelPeakRaw()
        {
            int bb = (mAccRawIndex - 2 < 0) ? (mSize + (mAccRawIndex - 2)) : (mAccRawIndex - 2);

            return mAccRaw[bb];
        }
        public float GetMotionAccelPeak()
        {
            int bb = (mAccIndex - 2 < 0) ? (mSize + (mAccIndex - 2)) : (mAccIndex - 2);

            return mAcc[bb];
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
        public float GetMotionVelo()
        {
            int b = mVeloIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mVelo[b];
        }
        public float GetMotionVeloRaw()
        {
            int b = mVeloRawIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mVeloRaw[b];
        }
        public float GetMotionPositionR()
        {
            int b = mPosRIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mPosR[b];
        }
        public float GetMotionPositionRRaw()
        {
            int b = mPosRRawIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mPosRRaw[b];
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
        public float mGetPositionRDiff()
        {
            int b = (mPosRIndex - 1 < 0) ? (mSize + (mPosRIndex - 1)) : (mPosRIndex - 1);
            int bb = (mPosRIndex - 2 < 0) ? (mSize + (mPosRIndex - 2)) : (mPosRIndex - 2);

            return mPosR[b] - mPosR[bb];
        }
        public float mGetPositionRRawDiff()
        {
            int b = (mPosRRawIndex - 1 < 0) ? (mSize + (mPosRRawIndex - 1)) : (mPosRRawIndex - 1);
            int bb = (mPosRRawIndex - 2 < 0) ? (mSize + (mPosRRawIndex - 2)) : (mPosRRawIndex - 2);

            return mPosRRaw[b] - mPosRRaw[bb];
        }
        public float mGetMag()
        {
            int b = mMagIndex - 1;
            if (b < 0)
                b = mSize - 1;
            return mMag[b];
        }
        public float mGetAngle()
        {
            return mAngle;
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

            if (mAccStop)
                mAccRawAvg = (float)mAccRawSum / (float)mRawSize;
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
        public void SetMotionVelocityZero()
        {
            mVelo[mVeloIndex] = 0.0f;
            mVeloIndex++;
            if (mVeloIndex >= mSize)
                mVeloIndex = 0;

            mVeloRaw[mVeloRawIndex] = 0.0f;
            mVeloRawIndex++;
            if (mVeloRawIndex >= mSize)
                mVeloRawIndex = 0;
        }
        public void UpdateMotionPositionR()
        {
            int b = (mAccIndex - 1 < 0) ? (mSize + (mAccIndex - 1)) : (mAccIndex - 1);
            int bb = (mAccIndex - 2 < 0) ? (mSize + (mAccIndex - 2)) : (mAccIndex - 2);
            int bbb = (mAccIndex - 3 < 0) ? (mSize + (mAccIndex - 3)) : (mAccIndex - 3);
            int bbbb = (mAccIndex - 4 < 0) ? (mSize + (mAccIndex - 4)) : (mAccIndex - 4);

            int bv = (mVeloIndex - 1 < 0) ? (mSize - 1) : (mVeloIndex - 1);

            mVelo[mVeloIndex] = mVelo[bv] + ((mAcc[bbbb] - mAccAvg) + 2 * (mAcc[bbb] - mAccAvg) + 2 * (mAcc[bb] - mAccAvg) + (mAcc[b] - mAccAvg)) * G / 6.0f * mTR;

            mVeloIndex++;
            if (mVeloIndex >= mSize)
                mVeloIndex = 0;

            int bp = (mPosRIndex - 1 < 0) ? (mSize - 1) : (mPosRIndex - 1);

            b = (mVeloIndex - 1 < 0) ? (mSize + (mVeloIndex - 1)) : (mVeloIndex - 1);
            bb = (mVeloIndex - 2 < 0) ? (mSize + (mVeloIndex - 2)) : (mVeloIndex - 2);
            bbb = (mVeloIndex - 3 < 0) ? (mSize + (mVeloIndex - 3)) : (mVeloIndex - 3);
            bbbb = (mVeloIndex - 4 < 0) ? (mSize + (mVeloIndex - 4)) : (mVeloIndex - 4);

            mPosR[mPosRIndex] = mPosR[bp] + (mVelo[bbbb] + 2 * mVelo[bbb] + 2 * mVelo[bb] + mVelo[b]) / 6.0f * mTR;

            mPosRIndex++;
            if (mPosRIndex >= mSize)
                mPosRIndex = 0;
        }
        public void UpdateMotionPositionRRaw()
        {
            int b = (mAccRawIndex - 1 < 0) ? (mRawSize + (mAccRawIndex - 1)) : (mAccRawIndex - 1);
            int bb = (mAccRawIndex - 2 < 0) ? (mRawSize + (mAccRawIndex - 2)) : (mAccRawIndex - 2);
            int bbb = (mAccRawIndex - 3 < 0) ? (mRawSize + (mAccRawIndex - 3)) : (mAccRawIndex - 3);
            int bbbb = (mAccRawIndex - 4 < 0) ? (mRawSize + (mAccRawIndex - 4)) : (mAccRawIndex - 4);

            int bv = (mVeloRawIndex - 1 < 0) ? (mSize - 1) : (mVeloRawIndex - 1);

            mVeloRaw[mVeloRawIndex] = mVeloRaw[bv] + (((float)mAccRaw[bbbb] - mAccRawAvg) + 2 * ((float)mAccRaw[bbb] - mAccRawAvg) + 2 * ((float)mAccRaw[bb] - mAccRawAvg) + ((float)mAccRaw[b] - mAccRawAvg)) / 800.0f * G / 6.0f * mTR;

            mVeloRawIndex++;
            if (mVeloRawIndex >= mSize)
                mVeloRawIndex = 0;

            int bp = (mPosRRawIndex - 1 < 0) ? (mSize - 1) : (mPosRRawIndex - 1);

            b = (mVeloRawIndex - 1 < 0) ? (mSize + (mVeloRawIndex - 1)) : (mVeloRawIndex - 1);
            bb = (mVeloRawIndex - 2 < 0) ? (mSize + (mVeloRawIndex - 2)) : (mVeloRawIndex - 2);
            bbb = (mVeloRawIndex - 3 < 0) ? (mSize + (mVeloRawIndex - 3)) : (mVeloRawIndex - 3);
            bbbb = (mVeloRawIndex - 4 < 0) ? (mSize + (mVeloRawIndex - 4)) : (mVeloRawIndex - 4);

            mPosRRaw[mPosRIndex] = mPosRRaw[bp] + (mVeloRaw[bbbb] + 2 * mVeloRaw[bbb] + 2 * mVeloRaw[bb] + mVeloRaw[b]) / 6.0f * mTR;

            mPosRRawIndex++;
            if (mPosRRawIndex >= mSize)
                mPosRRawIndex = 0;
        }

        public void SetMotionPositionZero()
        {
            for (int i = 0; i < mSize; i++)
            {
                mPos[i] = 0.0f;
                mPosR[i] = 0.0f;
                mPosRRaw[i] = 0.0f;
            }
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

        public void mUpdateAngle(ref Axis hor, ref Axis ver)
        {
            int b = mAccIndex - 1;
            if(b < 0)
                b = mSize - 1;
            int b_ver = ver.mAccIndex - 1;
            if(b_ver < 0)
                b_ver = ver.mSize - 1;
            int b_hor = hor.mAccIndex - 1;
            if(b_hor < 0)
                b_hor = hor.mSize - 1;
            mAngle = (float)(Math.Atan2(mAcc[b],Math.Sqrt(ver.mAcc[b_ver]*ver.mAcc[b_ver]+hor.mAcc[b_hor]*hor.mAcc[b_hor])));
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
