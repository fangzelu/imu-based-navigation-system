﻿using System;
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
        //*** IPC
        bool sendFlag;
        IntPtr wndPtr;
        string wndName = "WifiLoc";

        //*** Coordinates
        const int GRID = 2;
        int stageSize = 256;
        int stageIndex = 0;
        float x_init = 0, y_init = 0;
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

        //*** Step Count
        List<int> mAccelRawY = new List<int>();
        
        List<int> mStanceRaw = new List<int>();
        List<float> mStanceHead = new List<float>();
        float mStanceHeadAvg = 0.0f;
        float mBeforeStanceHeadAvg = 0.0f;

        List<float> mStanceHeadAvgList = new List<float>();

        float mStanceHeadAvgTest = 0.0f;
        List<float> mStanceHeadTest = new List<float>();
        List<float> mStanceHeadAvgListTest = new List<float>();

        float mStanceHeadAvgSecond = 0.0f;
        List<float> mStanceHeadSecond = new List<float>();
        List<float> mStanceHeadAvgListSecond = new List<float>();

        List<int> oneStepRaw = new List<int>();
        List<float> oneStepAcc = new List<float>();
        int oneStepSampleCount = 0;
        float oneStepVariance = 0.0f;
        float oneStepAccVariance = 0.0f;

        int mStanceRawSum = 0;
        int mStanceRawSquareSum = 0;
        const int STANCE_WINDOW = 8;
        float mStanceStdev = 0.0f;
        int mStanceState = 0;

        float upperBound = 4096.0f, lowerBound = 0.0f;
        bool trainingFlag = false;
        const int T_SIZE = 10;

        const float AVG_STEP = 0.7f;
        const float DEV_STEP = 0.1f;
        const int STEP_TIME = 32;
        float p = 1.0f;
        float pc = 1.0f;
        float pc_gap = 0.001f;

        int stepState = 0;
        int stepCount = 0;
        int stepInterval = 0;
        bool stepSampleFlag = false;
        bool needStepCount = false;

        float mStepCountTiltHeadingAvg = 0.0f;
        float mStepCountMovingTiltHeadingAvg = 0.0f;

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

        Random rand = new Random();
        const float AVG_DISTANCE = 3.0f;
        const float DEV_DISTANCE = 0.3f;
        
        //*** MotionNode SDK
        Client motionSensor, motionRaw;

        int mHeadingIndex = 0;
        int mTiltHeadingIndex = 0;
        int mHeadSize = 32;
        float mHeadingSum = 0.0f;

        int mTiltHeadingIndexSecond = 0;
        float mTiltHeadingSumSecond = 0.0f;
        float mTiltHeadingSecondAvg = 0.0f;
        float[] mTiltHeadingSecond;
        float mTiltHeadingAvgSecond;

        float mTiltHeadingSum = 0.0f;
        float mHeadingAvg = 0.0f;
        float mTiltHeadingAvgTest = 0.0f;
        float mTiltHeadingAvg = 0.0f;
        float mTiltHeadingAvgBeforeMoving = 0.0f;
        float[] mHeading;
        float[] mTiltHeading;
        bool mMoving = false;

        float mMovingTiltHeadingSum = 0.0f;
        float mMovingTiltHeadingAvg = 0.0f;
        const float MOVING_HEAD_WEIGHT = 0.1f;

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
            mTiltHeadingSecond = new float[mHeadSize];

            for(int i = 0 ; i < mHeadSize ; i++)
            {
                mHeading[i] = 0.0f;
                mTiltHeading[i] = 0.0f;
                mTiltHeadingSecond[i] = 0.0f;
            }

            mAccelRawY.Clear();
            mStanceRaw.Clear();
            mStanceHead.Clear();
            mStanceHeadAvgList.Clear();
            mStanceHeadTest.Clear();
            mStanceHeadAvgListTest.Clear();
            oneStepRaw.Clear();

            posLog.WriteLine("시간" + "," +
                //"실시간X" + "," + "실시간Y" + "," +
                //"린지X" + "," + "린지Y" + "," +
                "실시간SCX,실시간SCY," +
                "SCX,SCY," +
                "SC상태,SC수,SC시간," +
                "OneStep수,OneStep분산Raw,OneStep분산,Stance상태,Stance분산," +
                "Stance헤딩,Stance헤딩S,Stance헤딩Raw,"+
                "AccleRawY," +
                "변화Y,상태Y,방향Y,피크RawY,피크Y," +
                "AccelRawX" + "," + "AccelRawZ" + "," +
                //"RawAvg" + "," + "VeloRaw" + "," + "PosRaw" + "," +
                "AccelX," + "AccelY," + "AccelZ" + "," +
                //"Avg" + "," + "Velo" + "," + "Pos" + "," +
                "샘플수" + "," + "진동수" + "," +
                "최소" + "," + "최대" + "," + "범위" + "," +
                "분산" + "," + "최대분산" + "," + "SC이동거리," +
                //"린지이동거리" + "," +
                "변화Z,상태Z,방향Z,피크RawZ,피크Z," +
                "변화X,상태X,방향X,피크RawX,피크X," +
                //movingDev.ToString() + "," + movingDistanceR.ToString() + "," +
                "자기X" + "," + "자기Y" + "," + "자기Z" + "," +
                //mHeadingAvg.ToString() + "," + mTiltHeadingAvg.ToString() + "," +
                "일반헤딩" + "," + "실시간일반헤딩," +
                "틸트헤딩S,실시간틸트헤딩S," +
                "틸트헤딩" + "," + "실시간헤딩," +
                "Moving헤딩,Weight헤딩," +
                "SC헤딩,"+
                "SCMoving헤딩,SCWeight헤딩," +
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
            //if (yval < -25.0f)
               //yval = -25.0f;
            //else if (yval > 25.0f)
              //  yval = 25.0f;
            float xval = x.mGetMag();
            //if (xval < -25.0f)
              //  xval = -25.0f;
            //else if (xval > 25.0f)
              //  xval = 25.0f;

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
            
            //if (yval < -25.0f)
            //    yval = -25.0f;
            //else if (yval > 25.0f)
            //    yval = 25.0f;
            float xval = x.mGetMag();
            //if (xval < -25.0f)
            //    xval = -25.0f;
            //else if (xval > 25.0f)
            //    xval = 25.0f;
            float zval = x.mGetMag();
            //if (zval < -25.0f)
            //    zval = -25.0f;
            //else if (zval > 25.0f)
            //    zval = 25.0f;
            

            //double x_prime = x.mGetMag() * Math.Cos(x.mGetAngle()) + y.mGetMag() * Math.Sin(y.mGetAngle()) * Math.Sin(x.mGetAngle()) + z.mGetMag() * Math.Cos(y.mGetAngle()) * Math.Sin(x.mGetAngle());
            //double y_prime = y.mGetMag() * Math.Cos(y.mGetAngle()) - z.mGetMag() * Math.Sin(x.mGetAngle());

            double x_prime = xval * Math.Cos(x.mGetAngle()) + yval * Math.Sin(y.mGetAngle()) * Math.Sin(x.mGetAngle()) + zval * Math.Cos(y.mGetAngle()) * Math.Sin(x.mGetAngle());
            double y_prime = yval * Math.Cos(y.mGetAngle()) - zval * Math.Sin(x.mGetAngle());

            float cur = (float)(Math.Atan2(y_prime, x_prime));

            double x_prime_s = xval * Math.Cos(x.mGetAngle()) + yval * Math.Sin(y.mGetAngle()) * Math.Sin(x.mGetAngle()) - zval * Math.Cos(y.mGetAngle()) * Math.Sin(x.mGetAngle());
            double y_prime_s = yval * Math.Cos(y.mGetAngle()) + zval * Math.Sin(x.mGetAngle());

            float cur_s = (float)(Math.Atan2(y_prime_s, x_prime_s));

            mTiltHeadingSum += cur;
            mTiltHeadingSum -= mTiltHeading[mTiltHeadingIndex];

            mTiltHeading[mTiltHeadingIndex] = cur;
            mTiltHeadingIndex++;
            mTiltHeadingIndex = (mTiltHeadingIndex >= mHeadSize) ? 0 : mTiltHeadingIndex;

            mTiltHeadingSumSecond += cur_s;
            mTiltHeadingSumSecond -= mTiltHeadingSecond[mTiltHeadingIndexSecond];

            mTiltHeadingSecond[mTiltHeadingIndexSecond] = cur_s;
            mTiltHeadingIndexSecond++;
            mTiltHeadingIndexSecond = (mTiltHeadingIndexSecond >= mHeadSize) ? 0 : mTiltHeadingIndexSecond;

            if(x.mAccStop && y.mAccStop && z.mAccStop)
            {
                mTiltHeadingAvg = mTiltHeadingSum / mHeadSize;
                mTiltHeadingAvg = mTiltHeadingAvg - (float)(20.0f * Math.PI / 180.0f);
                if (mTiltHeadingAvg < -Math.PI)
                    mTiltHeadingAvg += (float)(2 * Math.PI);

                mTiltHeadingAvgSecond = mTiltHeadingSumSecond / mHeadSize;
                mTiltHeadingAvgSecond = mTiltHeadingAvgSecond - (float)(20.0f * Math.PI / 180.0f);
                if (mTiltHeadingAvgSecond < -Math.PI)
                    mTiltHeadingAvgSecond += (float)(2 * Math.PI);

                mStepCountTiltHeadingAvg = mTiltHeadingAvg;
                mStepCountTiltHeadingAvg = mStepCountTiltHeadingAvg - (float)(90.0f * Math.PI / 180.0f);
                if (mStepCountTiltHeadingAvg < -Math.PI)
                    mStepCountTiltHeadingAvg += (float)(2 * Math.PI);
            }

            //mTiltHeadingAvgTest = mTiltHeadingSum / mHeadSize;
            //mTiltHeadingAvgTest = mTiltHeadingAvgTest - (float)(20.0f * Math.PI / 180.0f);
            //if (mTiltHeadingAvgTest < -Math.PI)
            //    mTiltHeadingAvgTest += (float)(2 * Math.PI);
        }
        private float GetTiltHeading()
        {
            int b = mTiltHeadingIndex - 1;
            if (b < 0)
                b = mHeadSize - 1;
            return mTiltHeading[b];
        }
        private float GetTiltHeadingSecond()
        {
            int b = mTiltHeadingIndexSecond - 1;
            if (b < 0)
                b = mHeadSize - 1;
            return mTiltHeadingSecond[b];
        }
        private float GetHeading()
        {
            int b = mHeadingIndex - 1;
            if (b < 0)
                b = mHeadSize - 1;
            return mHeading[b];
        }
        private void mUpdateMovingTiltHeading(ref Axis x, ref Axis y, ref Axis z)
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

            double x_prime = xval * Math.Cos(x.mGetAngle()) + yval * Math.Sin(y.mGetAngle()) * Math.Sin(x.mGetAngle()) + zval * Math.Cos(y.mGetAngle()) * Math.Sin(x.mGetAngle());
            double y_prime = yval * Math.Cos(y.mGetAngle()) - zval * Math.Sin(x.mGetAngle());

            float cur = (float)(Math.Atan2(y_prime, x_prime));

            mMovingTiltHeadingSum += cur;

            mMovingTiltHeadingAvg = mMovingTiltHeadingSum / movingCount;
            mMovingTiltHeadingAvg = mMovingTiltHeadingAvg - (float)(20.0f * Math.PI / 180.0f);
            if (mMovingTiltHeadingAvg < -Math.PI)
                mMovingTiltHeadingAvg += (float)(2 * Math.PI);

            mStepCountMovingTiltHeadingAvg = mMovingTiltHeadingAvg;
            mStepCountMovingTiltHeadingAvg = mStepCountMovingTiltHeadingAvg - (float)(90.0f * Math.PI / 180.0f);
            if (mStepCountMovingTiltHeadingAvg < -Math.PI)
                mStepCountMovingTiltHeadingAvg += (float)(2 * Math.PI);
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

                                    if (trainingFlag)
                                        mAccelRawY.Add(itr.Value.getAccelerometer()[1]);

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

                                mUpdateHeading(ref z, ref y);

                                mUpdateTiltHeading(ref z, ref y, ref x);

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

                                        /*
                                        if(movingCount > 100)
                                        {
                                            float modifyDistance;
                                            if (movingDistanceR > AVG_DISTANCE + DEV_DISTANCE)
                                            {
                                                modifyDistance = (float)((movingDistanceR - (AVG_DISTANCE - DEV_DISTANCE)) * rand.NextDouble());
                                                movingDistanceR -= modifyDistance;
                                                UpdateWorldPosition(-modifyDistance, mTiltHeadingAvgBeforeMoving, ref xc_tilt, ref yc_tilt, ref stageIndex_tilt);
                                                UpdateWorldPosition(-modifyDistance, mTiltHeadingAvgBeforeMoving, ref xcR_tilt, ref ycR_tilt, ref stageIndexR_tilt);
                                            }
                                            else if (movingDistanceR < AVG_DISTANCE - DEV_DISTANCE)
                                            {
                                                modifyDistance = (float)(((AVG_DISTANCE + DEV_DISTANCE) - movingDistanceR) * rand.NextDouble());
                                                movingDistanceR += modifyDistance;
                                                UpdateWorldPosition(modifyDistance, mTiltHeadingAvgBeforeMoving, ref xc_tilt, ref yc_tilt, ref stageIndex_tilt);
                                                UpdateWorldPosition(modifyDistance, mTiltHeadingAvgBeforeMoving, ref xcR_tilt, ref ycR_tilt, ref stageIndexR_tilt);
                                            }

                                            if (logFlag)
                                            {
                                                //x.WriteMotionLog(log, "MotionX", mHeading);
                                                //y.WriteMotionLog(log, "MotionY", mHeading);
                                                //z.WriteMotionLog(log, "MotionZ", mHeading);

                                                int bb = stageIndex - 1;
                                                if (bb < 0)
                                                    bb = stageSize - 1;
                                                int bb_tilt = stageIndex_tilt - 1;
                                                if (bb_tilt < 0)
                                                    bb_tilt = stageSize - 1;
                                                int bbR = (stageIndexR - 1 < 0) ? (stageSize + (stageIndexR - 1)) : (stageIndexR - 1);
                                                int bbR_tilt = (stageIndexR_tilt - 1 < 0) ? (stageSize + (stageIndexR_tilt - 1)) : (stageIndexR_tilt - 1);
                                                int b_heading = (mTiltHeadingIndex - 1 < 0) ? (mHeadSize - 1) : (mTiltHeadingIndex - 1);

                                                posLog.WriteLine(DateTime.Now.ToString() + "," +
                                                    //xc_tilt[bb_tilt].ToString() + "," + yc_tilt[bb_tilt].ToString() + "," +
                                                    //xcR_tilt[bbR_tilt].ToString() + "," + ycR_tilt[bbR_tilt].ToString() + "," +
                                                    xc[bb].ToString() + "," + yc[bb].ToString() + "," +
                                                    xcR[bbR].ToString() + "," + ycR[bbR].ToString() + "," +
                                                    stepState.ToString() + "," + stepCount.ToString() + "," + stepInterval.ToString() + "," +
                                                    x.GetMotionAccelRaw().ToString() + "," + y.GetMotionAccelRaw().ToString() + "," + z.GetMotionAccelRaw().ToString() + "," +
                                                    //z.mAccRawAvg.ToString() + "," + z.GetMotionVeloRaw().ToString() + "," + z.GetMotionPositionRRaw().ToString() + "," +
                                                    x.GetMotionAccel().ToString() + "," + y.GetMotionAccel().ToString() + "," + z.GetMotionAccel().ToString() + "," +
                                                    //z.mAccAvg.ToString() + "," + z.GetMotionVelo().ToString() + "," + z.GetMotionPositionR().ToString() + "," +
                                                    movingCount.ToString() + "," + movingVibe.ToString() + "," +
                                                    movingMin.ToString() + "," + movingMax.ToString() + "," + ((movingMax - movingMin) / 800.0f).ToString() + "," +
                                                    movingDev.ToString() + "," + movingDevMax.ToString() + "," + movingDistance.ToString() + "," +
                                                    //movingDistanceR.ToString() + "," +
                                                    peakChangeZ.ToString() + "," + peakFlagZ.ToString() + "," + peakDirectionZ.ToString() + "," + peakAccelRawZ.ToString() + "," + peakAccelZ.ToString() + "," +
                                                    peakChangeX.ToString() + "," + peakFlagX.ToString() + "," + peakDirectionX.ToString() + "," + peakAccelRawX.ToString() + "," + peakAccelX.ToString() + "," +
                                                    peakChangeY.ToString() + "," + peakFlagY.ToString() + "," + peakDirectionY.ToString() + "," + peakAccelRawY.ToString() + "," + peakAccelY.ToString() + "," +
                                                    //movingDev.ToString() + "," + movingDistanceR.ToString() + "," +
                                                    x.mGetMag().ToString() + "," + y.mGetMag().ToString() + "," + z.mGetMag() + "," +
                                                    //mHeadingAvg.ToString() + "," + mTiltHeadingAvg.ToString() + "," +
                                                    (mTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," + (mTiltHeading[b_heading] * 180.0f / Math.PI).ToString() + "," +
                                                    (mMovingTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," + ((mTiltHeadingAvg * (1 - MOVING_HEAD_WEIGHT) + mMovingTiltHeadingAvg * (MOVING_HEAD_WEIGHT)) * 180.0f / Math.PI).ToString() + "," +
                                                    (mStepCountTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," +
                                                    (mStepCountMovingTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," + ((mStepCountTiltHeadingAvg * (1 - MOVING_HEAD_WEIGHT) + mStepCountMovingTiltHeadingAvg * (MOVING_HEAD_WEIGHT)) * 180.0f / Math.PI).ToString() + "," +
                                                    (z.mGetAngle() * 180.0f / Math.PI).ToString() + "," + (y.mGetAngle() * 180.0f / Math.PI).ToString());
                                            }

                                        }
                                        */

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
                                    movingStep++;
                                    peakChangeX = 0;
                                    peakChangeY = 0;
                                    peakChangeZ = 0;
                                    mMovingTiltHeadingSum = 0.0f;

                                    stepState = 0;
                                    stepInterval = 0;
                                    stepCount = 0;
                                    oneStepSampleCount = 0;
                                    oneStepVariance = 0.0f;
                                }

                                if (!x.mAccStop || !y.mAccStop || !z.mAccStop)
                                {
                                    stepInterval++;
                                    movingCount++;

                                    mTiltHeadingAvgBeforeMoving = mTiltHeadingAvg;
                                    mUpdateMovingTiltHeading(ref z, ref y, ref x);

                                    //x.UpdateMotionPosition();
                                    //y.UpdateMotionPosition();
                                    //z.UpdateMotionPosition();
                                    z.UpdateMotionPositionR();
                                    //z.UpdateMotionPositionRRaw();

                                    //x_diff = z.mGetPositionDiff();
                                    //xR_diff = z.mGetPositionRDiff();
                                    //x_diff = z.mGetPositionRRawDiff();

                                    //x_diff = Math.Abs(x_diff);
                                    //xR_diff = Math.Abs(xR_diff);

                                    //UpdateWorldPosition(x_diff, mHeadingAvg, ref xc, ref yc, ref stageIndex);
                                    //UpdateWorldPosition(xR_diff, mTiltHeadingAvg * (1-MOVING_HEAD_WEIGHT) + mMovingTiltHeadingAvg * (MOVING_HEAD_WEIGHT), ref xc_tilt, ref yc_tilt, ref stageIndex_tilt);

                                    //UpdateWorldPosition(xR_diff, mHeadingAvg, ref xcR, ref ycR, ref stageIndexR);
                                    //UpdateWorldPosition(xR_diff, mTiltHeadingAvg, ref xcR_tilt, ref ycR_tilt, ref stageIndexR_tilt);

                                    //movingDistanceR += xR_diff;

                                    //UpdateWorldPosition(x_diff, mTiltHeadingAvg, ref xc_tilt, ref yc_tilt, ref stageIndex_tilt);
                                    //movingDistance += x_diff;

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

                                    if(stepState == 1 && stepInterval > STEP_TIME)
                                    {
                                        stepState = 0;
                                        stepInterval = 0;
                                    }

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

                                    int val_accel = y.GetMotionAccelRaw();
                                    float val_accel_g = y.GetMotionAccel();
                                    float val_head = GetTiltHeading();
                                    float val_head_raw = GetHeading();
                                    float val_head_s = GetTiltHeadingSecond();
                                    switch (stepState)
                                    {
                                        case 0:
                                            oneStepSampleCount = 0;
                                            oneStepRaw.Clear();
                                            oneStepAcc.Clear();
                                            mStanceState = 0;

                                            mStanceRaw.Add(val_accel);
                                            mStanceRawSum += val_accel;
                                            mStanceRawSquareSum += val_accel * val_accel;

                                            mStanceHead.Add(val_head);
                                            if (mStanceHead.Count > STANCE_WINDOW)
                                                mStanceHead.RemoveAt(0);
                                            mStanceHeadTest.Add(val_head_raw);
                                            if (mStanceHeadTest.Count > STANCE_WINDOW)
                                                mStanceHeadTest.RemoveAt(0);
                                            mStanceHeadSecond.Add(val_head_s);
                                            if (mStanceHeadSecond.Count > STANCE_WINDOW)
                                                mStanceHeadSecond.RemoveAt(0);
                                            
                                            if(mStanceRaw.Count > STANCE_WINDOW)
                                            {
                                                mStanceRawSum -= mStanceRaw[0];
                                                mStanceRawSquareSum -= mStanceRaw[0] * mStanceRaw[0];

                                                mStanceRaw.RemoveAt(0);

                                                mStanceStdev = (float)Math.Sqrt((float)mStanceRawSquareSum / STANCE_WINDOW - ((float)mStanceRawSum / STANCE_WINDOW) * ((float)mStanceRawSum / STANCE_WINDOW));

                                                if(mStanceStdev < 100.0f)
                                                {
                                                    mStanceState = 1;
                                                    mStanceHeadAvgList.Add(mStanceHead.Sum() / STANCE_WINDOW);

                                                    mStanceHeadAvgListTest.Add(mStanceHeadTest.Sum() / STANCE_WINDOW);

                                                    mStanceHeadAvgListSecond.Add(mStanceHeadSecond.Sum() / STANCE_WINDOW);
                                                }
                                            }

                                            if (peakFlagY == 1 && temp == 1 && stepInterval >= 0)
                                            {
                                                if(peakAccelRawY <= lowerBound)
                                                {
                                                    stepState = 1;
                                                    stepInterval = 0;
                                                    oneStepSampleCount++;
                                                    if (mStanceHeadAvgList.Count > 0)
                                                    {
                                                        mStanceHeadAvg = mStanceHeadAvgList.Sum() / mStanceHeadAvgList.Count;
                                                        mStanceHeadAvg = mStanceHeadAvg - (float)(90.0f * Math.PI / 180.0f);
                                                        if (mStanceHeadAvg < -Math.PI)
                                                            mStanceHeadAvg += (float)(2 * Math.PI);

                                                        mStanceHeadAvgTest = mStanceHeadAvgListTest.Sum() / mStanceHeadAvgListTest.Count;
                                                        mStanceHeadAvgTest = mStanceHeadAvgTest - (float)(90.0f * Math.PI / 180.0f);
                                                        if(mStanceHeadAvgTest < -Math.PI)
                                                            mStanceHeadAvgTest += (float)(2 * Math.PI);

                                                        mStanceHeadAvgSecond = mStanceHeadAvgListSecond.Sum() / mStanceHeadAvgListSecond.Count;
                                                        mStanceHeadAvgSecond = mStanceHeadAvgSecond - (float)(90.0f * Math.PI / 180.0f);
                                                        if (mStanceHeadAvgSecond < -Math.PI)
                                                            mStanceHeadAvgSecond += (float)(2 * Math.PI);
                                                    }
                                                    else
                                                    {
                                                        mStanceHeadAvg = -(float)(90.0f * Math.PI / 180.0f);
                                                        mStanceHeadAvgTest = -(float)(90.0f * Math.PI / 180.0f);
                                                        mStanceHeadAvgSecond = -(float)(90.0f * Math.PI / 180.0f);
                                                    }

                                                    mStanceHeadAvgList.Clear();
                                                    mStanceHeadAvgListTest.Clear();
                                                    mStanceHeadAvgListSecond.Clear();
                                                    oneStepRaw.Add(val_accel);
                                                    oneStepAcc.Add(val_accel_g);
                                                }
                                            }
                                            break;
                                        case 1:
                                            mStanceState = 0;
                                            oneStepSampleCount++;
                                            oneStepRaw.Add(val_accel);
                                            oneStepAcc.Add(val_accel_g);

                                            if (peakFlagY == 1 && temp == 1 && peakAccelRawY <= lowerBound)
                                            {
                                                stepInterval = 0;
                                            }
                                            else if (peakFlagY == 1 && temp == 2 && peakAccelRawY >= upperBound)
                                            {
                                                stepState = 2;
                                                stepInterval = 0;
                                            }
                                            break;
                                        case 2:
                                            mStanceState = 0;
                                            oneStepSampleCount++;
                                            oneStepRaw.Add(val_accel);
                                            oneStepAcc.Add(val_accel_g);

                                            mStanceRaw.Add(val_accel);
                                            mStanceRawSum += val_accel;
                                            mStanceRawSquareSum += val_accel * val_accel;

                                            mStanceHead.Add(val_head);
                                            if (mStanceHead.Count > STANCE_WINDOW)
                                                mStanceHead.RemoveAt(0);
                                            mStanceHeadTest.Add(val_head_raw);
                                            if(mStanceHeadTest.Count > STANCE_WINDOW)
                                                mStanceHeadTest.RemoveAt(0);
                                            mStanceHeadSecond.Add(val_head_s);
                                            if (mStanceHeadSecond.Count > STANCE_WINDOW)
                                                mStanceHeadSecond.RemoveAt(0);

                                            if (mStanceRaw.Count > STANCE_WINDOW)
                                            {
                                                mStanceRawSum -= mStanceRaw[0];
                                                mStanceRawSquareSum -= mStanceRaw[0] * mStanceRaw[0];

                                                mStanceRaw.RemoveAt(0);

                                                mStanceStdev = (float)Math.Sqrt((float)mStanceRawSquareSum / STANCE_WINDOW - ((float)mStanceRawSum / STANCE_WINDOW) * ((float)mStanceRawSum / STANCE_WINDOW));

                                                if (mStanceStdev < 100.0f)
                                                {
                                                    mStanceHeadAvgList.Add(mStanceHead.Sum() / STANCE_WINDOW);
                                                    mStanceHeadAvgListTest.Add(mStanceHeadTest.Sum() / STANCE_WINDOW);
                                                    mStanceHeadAvgListSecond.Add(mStanceHeadSecond.Sum() / STANCE_WINDOW);
                                                    stepState = 0;
                                                    mStanceState = 1;

                                                    oneStepSampleCount -= STANCE_WINDOW;
                                                    int temp_count = 0;
                                                    float temp_sum = 0.0f, temp_square_sum = 0.0f;
                                                    for (int i = 0; i < oneStepRaw.Count - STANCE_WINDOW; i++)
                                                    {
                                                        temp_count++;
                                                        temp_sum += oneStepRaw[i];
                                                        temp_square_sum += oneStepRaw[i] * oneStepRaw[i];
                                                    }
                                                    oneStepVariance = (float)Math.Sqrt(temp_square_sum / temp_count - (temp_sum / temp_count) * (temp_sum / temp_count));
                                                    oneStepRaw.Clear();

                                                    temp_count = 0; temp_sum = 0.0f; temp_square_sum = 0.0f;
                                                    for (int i = 0; i < oneStepAcc.Count - STANCE_WINDOW; i++)
                                                    {
                                                        temp_count++;
                                                        temp_sum += oneStepAcc[i];
                                                        temp_square_sum += oneStepAcc[i] * oneStepAcc[i];
                                                    }
                                                    oneStepAccVariance = (float)Math.Sqrt(temp_square_sum / temp_count - (temp_sum / temp_count) * (temp_sum / temp_count));
                                                    oneStepAcc.Clear();

                                                    // 스텝 카운트
                                                    stepCount++;

                                                    if (rand.NextDouble() >= 0.5)
                                                        x_diff = 2.0f * (float)(AVG_STEP + DEV_STEP * rand.NextDouble());
                                                    else
                                                        x_diff = 2.0f * (float)(AVG_STEP - DEV_STEP * rand.NextDouble());
                                                    movingDistance += x_diff;

                                                    UpdateWorldPosition(x_diff, mStanceHeadAvg, ref xc, ref yc, ref stageIndex);

                                                    UpdateWorldPosition(x_diff, mStepCountTiltHeadingAvg, ref xcR, ref ycR, ref stageIndexR);

                                                    p *= (pc - (float)stepCount * pc_gap);

                                                    posLog.WriteLine("One Step");
                                                    if (sendFlag)
                                                    {
                                                        int bR = (stageIndexR - 1 < 0) ? (stageSize + (stageIndexR - 1)) : (stageIndexR - 1);
                                                        int bbR = (stageIndexR - 2 < 0) ? (stageSize + (stageIndexR - 2)) : (stageIndexR - 2);

                                                        int curX = (int)(xcR[bR] / 2.0f);
                                                        int curY = (int)(ycR[bR] / 2.0f);
                                                        int beforeX = (int)(xcR[bbR] / 2.0f);
                                                        int beforeY = (int)(ycR[bbR] / 2.0f);

                                                        if (curX != beforeX || curY != beforeY)
                                                        {
                                                            string sendMsg = curX.ToString() + "," + curY.ToString() + "," + p.ToString();

                                                            SendMessage("WifiLoc", sendMsg);
                                                        }

                                                        posLog.WriteLine("Send Grid," + curX.ToString() + "," + curY.ToString() + "," + p.ToString());
                                                    }
                                                }
                                            }
                                            break;
                                    }

                                    movingMin = Math.Min(movingMin, z.GetMotionAccelRaw());
                                    movingMax = Math.Max(movingMax, z.GetMotionAccelRaw());
                                    movingDev = z.GetMotionAccelDev();
                                    movingDevMax = Math.Max(movingDevMax, movingDev);
                                    mMoving = true;
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
                                        int bbb_heading = (mTiltHeadingIndexSecond - 1 < 0) ? (mHeadSize - 1) : (mTiltHeadingIndexSecond - 1);
                                        int bb_heading = (mHeadingIndex - 1 < 0) ? (mHeadSize - 1) : (mHeadingIndex - 1);
                                        int b_heading = (mTiltHeadingIndex - 1 < 0) ? (mHeadSize - 1) : (mTiltHeadingIndex - 1);

                                        posLog.WriteLine(DateTime.Now.ToString() + "," +
                                            //xc_tilt[bb_tilt].ToString() + "," + yc_tilt[bb_tilt].ToString() + "," +
                                            //xcR_tilt[bbR_tilt].ToString() + "," + ycR_tilt[bbR_tilt].ToString() + "," +
                                            xc[bb].ToString() + "," + yc[bb].ToString() + "," +
                                            xcR[bbR].ToString() + "," + ycR[bbR].ToString() + "," +
                                            stepState.ToString() + "," + stepCount.ToString() + "," + stepInterval.ToString() + "," +
                                            oneStepSampleCount.ToString() + "," + oneStepVariance.ToString() + "," + oneStepAccVariance.ToString() + "," +
                                            mStanceState.ToString() + "," + mStanceStdev.ToString() + "," +
                                            mStanceHeadAvg.ToString() + "," + mStanceHeadAvgSecond.ToString() + "," + mStanceHeadAvgTest.ToString() + "," + 
                                            y.GetMotionAccelRaw().ToString() + "," +
                                            peakChangeY.ToString() + "," + peakFlagY.ToString() + "," + peakDirectionY.ToString() + "," + peakAccelRawY.ToString() + "," + peakAccelY.ToString() + "," +
                                            x.GetMotionAccelRaw().ToString() + "," + z.GetMotionAccelRaw().ToString() + "," +
                                            //z.mAccRawAvg.ToString() + "," + z.GetMotionVeloRaw().ToString() + "," + z.GetMotionPositionRRaw().ToString() + "," +
                                            x.GetMotionAccel().ToString() + "," + y.GetMotionAccel().ToString() + "," + z.GetMotionAccel().ToString() + "," +
                                            //z.mAccAvg.ToString() + "," + z.GetMotionVelo().ToString() + "," + z.GetMotionPositionR().ToString() + "," +
                                            movingCount.ToString() + "," + movingVibe.ToString() + "," +
                                            movingMin.ToString() + "," + movingMax.ToString() + "," + ((movingMax - movingMin) / 800.0f).ToString() + "," +
                                            movingDev.ToString() + "," + movingDevMax.ToString() + "," + movingDistance.ToString() + "," +
                                            //movingDistanceR.ToString() + "," +
                                            peakChangeZ.ToString() + "," + peakFlagZ.ToString() + "," + peakDirectionZ.ToString() + "," + peakAccelRawZ.ToString() + "," + peakAccelZ.ToString() + "," +
                                            peakChangeX.ToString() + "," + peakFlagX.ToString() + "," + peakDirectionX.ToString() + "," + peakAccelRawX.ToString() + "," + peakAccelX.ToString() + "," +
                                            //movingDev.ToString() + "," + movingDistanceR.ToString() + "," +
                                            x.mGetMag().ToString() + "," + y.mGetMag().ToString() + "," + z.mGetMag() + "," +
                                            //mHeadingAvg.ToString() + "," + mTiltHeadingAvg.ToString() + "," +
                                            (mHeadingAvg * 180.0f / Math.PI).ToString() + "," + (mHeading[bb_heading] * 180.0f / Math.PI).ToString() + "," +
                                            (mTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," + (mTiltHeading[b_heading] * 180.0f / Math.PI).ToString() + "," +
                                            (mTiltHeadingAvgSecond * 180.0f / Math.PI).ToString() + "," + (mTiltHeadingSecond[bbb_heading] * 180.0f / Math.PI).ToString() + "," +
                                            (mMovingTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," + ((mTiltHeadingAvg * (1 - MOVING_HEAD_WEIGHT) + mMovingTiltHeadingAvg * (MOVING_HEAD_WEIGHT)) * 180.0f / Math.PI).ToString() + "," +
                                            (mStepCountTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," +
                                            (mStepCountMovingTiltHeadingAvg * 180.0f / Math.PI).ToString() + "," + ((mStepCountTiltHeadingAvg * (1 - MOVING_HEAD_WEIGHT) + mStepCountMovingTiltHeadingAvg * (MOVING_HEAD_WEIGHT)) * 180.0f / Math.PI).ToString() + "," +
                                            (z.mGetAngle() * 180.0f / Math.PI).ToString() + "," + (y.mGetAngle() * 180.0f / Math.PI).ToString());
                                    }
                                }


                                this.Invoke(new MethodInvoker(delegate()
                                {
                                    //this.mAccPosX.Text = x.GetMotionPosition().ToString();
                                    //this.mAccPosY.Text = y.GetMotionPosition().ToString();
                                    //this.mAccPosY.Text = z.GetMotionPositionR().ToString();
                                    //this.mAccPosZ.Text = z.GetMotionPositionRRaw().ToString();

                                    this.mMagX.Text = x.mGetMag().ToString();
                                    this.mMagY.Text = y.mGetMag().ToString();
                                    this.mMagZ.Text = z.mGetMag().ToString();

                                    //this.mHead.Text = ((mTiltHeadingAvg * (1 - MOVING_HEAD_WEIGHT) + mMovingTiltHeadingAvg * (MOVING_HEAD_WEIGHT)) * 180.0f / Math.PI).ToString();
                                    //this.mHeadTilt.Text = (mTiltHeadingAvg * 180.0f / Math.PI).ToString();

                                    this.mHeadStep.Text = (mStanceHeadAvg * 180.0f / Math.PI).ToString();
                                    this.mHeadStepTilt.Text = (mStepCountTiltHeadingAvg * 180.0f / Math.PI).ToString();
                                    this.StepP.Text = p.ToString();

                                    //this.mLocalX.Text = xR_diff.ToString();

                                    int b = stageIndex - 1;
                                    if(b < 0)
                                        b = stageSize - 1;
                                    this.mStageX.Text = (xc[b]).ToString();
                                    this.mStageY.Text = (yc[b]).ToString();

                                    //b = stageIndex_tilt - 1;
                                    //if (b < 0)
                                    //    b = stageSize - 1;
                                    //this.mStageTiltX.Text = (xc_tilt[b]).ToString();
                                    //this.mStageTiltY.Text = (yc_tilt[b]).ToString();

                                    b = (stageIndexR - 1 < 0) ? (stageSize + (stageIndexR - 1)) : (stageIndexR - 1);
                                    this.mStageRX.Text = (xcR[b]).ToString();
                                    this.mStageRY.Text = (ycR[b]).ToString();

                                    //b = (stageIndexR_tilt - 1 < 0) ? (stageSize + (stageIndexR_tilt - 1)) : (stageIndexR_tilt - 1);
                                    //this.mStageRTX.Text = (xcR_tilt[b]).ToString();
                                    //this.mStageRTY.Text = (ycR_tilt[b]).ToString();

                                    this.mRoll.Text = (y.mGetAngle() * 180.0f / Math.PI).ToString();
                                    this.mPitch.Text = (z.mGetAngle() * 180.0f / Math.PI).ToString();

                                    this.StepCountLabel.Text = stepCount.ToString();
                                    this.StepIntervalLabel.Text = stepInterval.ToString();
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

            x_init = (float)StartX.Value;
            y_init = (float)StartY.Value;
            p = 1.0f;

            for (int i = 0; i < stageSize; i++)
            {
                xc[i] = x_init;
                yc[i] = y_init;
                xc_tilt[i] = x_init;
                yc_tilt[i] = y_init;
                xcR[i] = x_init;
                ycR[i] = y_init;
                xcR_tilt[i] = x_init;
                ycR_tilt[i] = y_init;
            }
            logFlag = true;
        }

        private void Pause_Bt_Click(object sender, EventArgs e)
        {
            logFlag = false;
        }

        void SendMessage(string strProgramName, string message)
        {
            try
            {
                Win32API.COPYDATASTRUCT copyDataStruct = new Win32API.COPYDATASTRUCT();
                copyDataStruct.dwData = (IntPtr)1; // 임시값

                copyDataStruct.cbData = message.Length * 2 + 1; // 한글 코드 지원
                //copyDataStruct.cbData = sizeof(int) * 2 + sizeof(double);
                copyDataStruct.lpData = message; // 보낼 메시지

                if (wndPtr == IntPtr.Zero)
                {
                    MessageBox.Show("No Window");
                    return;
                }

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
            wndPtr = Win32API.FindWindow(wndName, null);
            sendFlag = true;

            if (wndPtr == null)
                MessageBox.Show("Fail to Find Window");

            int bR = (stageIndexR - 1 < 0) ? (stageSize + (stageIndexR - 1)) : (stageIndexR - 1);
            int bbR = (stageIndexR - 2 < 0) ? (stageSize + (stageIndexR - 2)) : (stageIndexR - 2);

            int curX = (int)(xcR[bR] / 2.0f);
            int curY = (int)(ycR[bR] / 2.0f);

            string sendMsg = curX.ToString() + "," + curY.ToString() + "," + p.ToString();

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
                case Keys.T:
                    MessageBox.Show("Test");
                    break;
                default:
                    break;
            }
        }

        private void TraningStart_Click(object sender, EventArgs e)
        {
            mAccelRawY.Clear();
            trainingFlag = true;
        }

        private void TrainingDone_Click(object sender, EventArgs e)
        {
            mAccelRawY.Sort();

            int sum = 0, sumSquare = 0;
            float avg, stdev;
            for(int i=0 ; i < T_SIZE * 2 ; i++)
            {
                sum += mAccelRawY[i];
                sumSquare += mAccelRawY[i] * mAccelRawY[i];
            }
            avg = (float)sum / (T_SIZE * 2);
            stdev = (float)Math.Sqrt((float)sumSquare / (T_SIZE * 2) - avg * avg);
            lowerBound = avg + 2 * stdev;

            sum = 0;
            sumSquare = 0;
            for (int i = mAccelRawY.Count - 1; i > mAccelRawY.Count - (T_SIZE * 2 +1); i--)
            {
                sum += mAccelRawY[i];
                sumSquare += mAccelRawY[i] * mAccelRawY[i];
            }
            avg = (float)sum / (T_SIZE * 2);
            stdev = (float)Math.Sqrt((float)sumSquare / (T_SIZE * 2) - avg * avg);
            upperBound = avg - 2 * stdev;

            this.Invoke(new MethodInvoker(delegate()
            {
                Lower.Text = lowerBound.ToString();
                Upper.Text = upperBound.ToString();
            }));

            trainingFlag = false;
        }

        private void Stop_Bt_Click(object sender, EventArgs e)
        {
            sendFlag = false;
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
            if (mAccRawDev > 2048.0f)
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

    public struct msg
    {
        public int x;
        public int y;
        public double p;
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
