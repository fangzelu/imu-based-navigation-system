using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Shimmer
{
    class KalmanFilter
    {
        // n is states, m is inputs, r is outputs
        // x = Ax + Bu + w
        // y = Cx + z
        public Matrix A;    // n by n
        public Matrix B;    // n by m
        public Matrix C;    // r by n
        public Matrix x;    // n by 1
        public Matrix xhat; // n by 1
        public Matrix y;    // r by 1
        public Matrix u;    // m by 1
        public Matrix Sz;   // r by r
        public Matrix Sw;   // n by n
        public Matrix P;    // n by n
        public Matrix w;    // n by 1
        public Matrix aw;   // m by 1
        public Matrix z;    // r by 1
        public Matrix pnoise;   // n by 1
        public Matrix mnoise;   // r by 1

        public Matrix Cxhat;   // r by 1
        public Matrix yCxhat;  // r by 1
        public Matrix CPCT;    // r by r
        public Matrix CPCTSz;  // r by r
        public Matrix CPCTSzInv;    // r by r
        public Matrix APCT;    // n by r
        public Matrix APAT;    // n by n
        public Matrix CPAT;    // r by n
        public Matrix K;   // n by r
        public Matrix SzInv;   // r by r

        //public Matrix inv_test = new Matrix(3,3);

        double T;

        public KalmanFilter()
        {
            return;
        }

        public KalmanFilter(int n, int m, int r)
        {
            A = new Matrix(n,n);
            B = new Matrix(n,m);
            C = new Matrix(r,n);
            x = new Matrix(n, 1);
            xhat = new Matrix(n, 1);
            y = new Matrix(r, 1);
            u = new Matrix(m, 1);
            Sz = new Matrix(r, r);
            Sw = new Matrix(n, n);
            P = new Matrix(n, n);
            w = new Matrix(n, 1);
            aw = new Matrix(m, 1);
            z = new Matrix(r, 1);

            pnoise = new Matrix(n, 1);
            mnoise = new Matrix(r, 1);

            APCT = new Matrix(n,r);
            CPCT = new Matrix(r,r);
            CPCTSz = new Matrix(r,r);
            CPCTSzInv = new Matrix(r, r);
            K = new Matrix(n,r);
            Cxhat = new Matrix(r,1);
            yCxhat = new Matrix(r,1);
            APAT = new Matrix(n,n);
            CPAT = new Matrix(r,n);
            SzInv = new Matrix(r,r);

            setC();
        }

        private void setC()
        {
            for(int i=0; i < C.c; i++)
            {
                for(int j=0 ; j < C.r; j++)
                {
                    if (i == j)
                        C.m[i, j] = 1.0f;
                    else
                        C.m[i, j] = 0.0f;
                }
            }
            return;
        }

        public void next(System.IO.TextWriter log)
        {
            Random rand = new Random();

            double rand_val = (double)rand.NextDouble();
            if (rand.NextDouble() >= 0.5)
                rand_val = -rand_val;
            pnoise = rand_val * w;

            log.WriteLine(rand_val.ToString());
            pnoise.writeLog(log, "pnoise");

            x = A * x + B * u + pnoise;
            x.writeLog(log, "x");

            rand_val = (double)rand.NextDouble();
            if (rand.NextDouble() >= 0.5)
                rand_val = -rand_val;
            mnoise = rand_val * z;

            y = C * x + mnoise;

            y.writeLog(log, "y");

            xhat = A * xhat + B * u;

            xhat.writeLog(log, "xhat");

            Cxhat = C * xhat;
            yCxhat = y - Cxhat;

            yCxhat.writeLog(log, "yCxhat");

            CPCT = C * P * C.transpose();

            CPCT.writeLog(log, "CPCT");
            CPCTSz = CPCT + Sz;
            CPCTSz.writeLog(log, "CPCTSz");
            if (CPCTSz.hasInverse())
                CPCTSzInv = CPCTSz.inverse();
            else
                CPCTSzInv.setIdentity();

            CPCTSzInv.writeLog(log, "CPCTSzInv");

            APCT = A * P * C.transpose();

            K = APCT * CPCTSzInv;

            K.writeLog(log, "K");

            xhat = xhat + K * yCxhat;

            xhat.writeLog(log, "xhat");

            APAT = A * P * A.transpose();
            if (Sz.hasInverse())
                SzInv = Sz.inverse();
            else
                SzInv.setIdentity();
            CPAT = C * P * A.transpose();

            P = APAT + Sw - APCT * SzInv * CPAT;

            P.writeLog(log, "P");
        }
        public double[,] getState()
        {
            return x.m;
        }
        public double[,] getMeasurement()
        {
            return y.m;
        }
        public double[,] getStateHat()
        {
            return xhat.m;
        }

        public void setU(double[,] val, int c, int r)
        {
            if(u.c != c || u.r != r)
            {
                MessageBox.Show("Column or Row does not equal");
                return;
            }

            for (int i = 0; i < c; i++)
            {
                for (int j = 0; j < r; j++)
                {
                    u.m[i, j] = val[i, j];
                }
            }
        }
        public void setT(double val)
        {
            T = val;
            setA();
            setB();
        }

        public void setZ(double[,] val, int c, int r)
        {
            if (z.c != c || z.r != r)
            {
                MessageBox.Show("Column or Row does not equal");
                return;
            }

            for (int i = 0; i < c; i++)
            {
                for (int j = 0; j < r; j++)
                {
                    z.m[i, j] = val[i, j];
                }
            }
            setSz();
        }

        private void setSz()
        {
            Sz = z * z.transpose();
            return;
        }

        private void setA()
        {
            for(int i=0;i<A.c;i++)
            {
                for(int j=0;j<A.r;j++)
                {
                    if(i < 3)
                    {
                        if (j < 3 && i % 3 == j % 3)
                            A.m[i, j] = 1.0f;
                        else if (i % 3 == j % 3)
                            A.m[i, j] = T;
                        else
                            A.m[i, j] = 0.0f;
                    }
                    else
                    {
                        if (j >= 3 && i % 3 == j % 3)
                            A.m[i, j] = 1.0f;
                        else
                            A.m[i, j] = 0.0f;
                    }
                }
            }

            return;
        }

        private void setB()
        {
            for(int i=0 ; i < B.c ; i++)
            {
                for(int j=0; j < B.r ; j++)
                {
                    if(i < 3)
                    {
                        if (i % 3 == j % 3)
                            B.m[i, j] = T * T / 2.0f;
                        else
                            B.m[i, j] = 0.0f;
                    }
                    else
                    {
                        if (i % 3 == j % 3)
                            B.m[i, j] = T;
                        else
                            B.m[i, j] = 0;
                    }
                }
            }

            return;
        }

        public void setAw(double[,] val, int c, int r)
        {
            if(aw.c != c || aw.r != r)
            {
                MessageBox.Show("Column or Row does not equal");
                return;
            }

            for(int i=0 ; i < c ; i++)
            {
                for(int j=0 ; j < r ; j++)
                {
                    aw.m[i, j] = val[i, j];
                }
            }

            setW();
            setSw();
            return;
        }

        private void setW()
        {
            w = B * aw;
            return;
        }

        private void setSw()
        {
            Sw = w * w.transpose();
            return;
        }
    }

    class Matrix
    {
        public int c;
        public int r;
        public double[,] m;

        public Matrix(int i, int j)
        {
            c = i;
            r = j;
            m = new double[c, r];
            return;
        }

        public static Matrix operator +(Matrix left, Matrix right)
        {
            Matrix rlt;

            if (left.c != right.c || left.r != right.r)
            {
                MessageBox.Show("Column or Row size does not equal");
                return null;
            }
            rlt = new Matrix(left.c, left.r);

            for (int i = 0; i < rlt.c; i++)
            {
                for(int j=0; j < rlt.r; j++)
                {
                    rlt.m[i,j] = left.m[i,j] + right.m[i,j];
                }
            }

            return rlt;
        }

        public static Matrix operator +(Matrix left, double right)
        {
            Matrix rlt = new Matrix(left.c, left.r);

            for(int i=0 ; i < left.c ; i++)
            {
                for(int j=0; j < left.r ; j++)
                {
                    rlt.m[i, j] = left.m[i, j] + right;
                }
            }
            return rlt;
        }

        public static Matrix operator -(Matrix left, Matrix right)
        {
            Matrix rlt;

            if (left.c != right.c || left.r != right.r)
            {
                MessageBox.Show("Column or Row size does not equal");
                return null;
            }
            rlt = new Matrix(left.c, left.r);

            for (int i = 0; i < rlt.c; i++)
            {
                for (int j = 0; j < rlt.r; j++)
                {
                    rlt.m[i, j] = left.m[i, j] - right.m[i, j];
                }
            }

            return rlt;
        }

        public static Matrix operator *(Matrix left, Matrix right)
        {
            Matrix rlt;

            if (left.r != right.c)
            {
                MessageBox.Show("Left Row does not equal to Right Column");
                return null;
            }
            rlt = new Matrix(left.c, right.r);

            for (int i = 0; i < left.c; i++)
            {
                for (int j = 0; j < right.r; j++)
                {
                    rlt.m[i, j] = 0.0f;
                    for (int k = 0; k < right.c; k++)
                    {
                        rlt.m[i, j] += left.m[i, k] * right.m[k, j];
                    }
                }
            }

            return rlt;
        }

        public static Matrix operator *(double left, Matrix right)
        {
            Matrix rlt = new Matrix(right.c, right.r);

            for (int i = 0; i < right.c; i++)
            {
                for(int j=0; j < right.r ; j++)
                {
                    rlt.m[i, j] = left * right.m[i,j];   
                }
            }

            return rlt;
        }

        public Matrix transpose()
        {
            Matrix rlt = new Matrix(r, c);

            for(int i=0 ; i < c;i++)
            {
                for(int j=0;j< r;j++)
                {
                    rlt.m[j,i] = m[i,j];
                }
            }

            return rlt;
        }

        public Matrix inverse()
        {
            /*
            Matrix rlt = new Matrix(c, r);
            Matrix cp = new Matrix(c, r);

            for(int i=0; i < c ; i++)
            {
                for(int j=0; j < r ; j++)
                {
                    cp.m[i, j] = m[i, j];
                    if (i == j)
                        rlt.m[i, j] = 1.0f;
                    else
                        rlt.m[i, j] = 0.0f;
                }
            }

            for(int i=0 ; i < c ; i++)
            {
                double b = cp.m[i,i];
                for(int j=0 ; j < r ; j++)
                {
                    cp.m[i, j] /= b;
                    rlt.m[i, j] /= b;
                }

                for(int j=0 ; j < c ; j++)
                {
                    if(i!=j)
                    {
                        double sb = cp.m[j, i];
                        for(int k=0 ; k < r ; k++)
                        {
                            cp.m[j, k] = cp.m[j, k] - sb * cp.m[i, k];
                            rlt.m[j, k] = rlt.m[j, k] - sb * rlt.m[i, k];
                        }
                    }
                }
            }
            */

            Matrix cp = new Matrix(r, c);
            cp = this.transpose();

            Matrix rlt = new Matrix(r, c);

            double d = m[0, 0] * (m[1, 1] * m[2, 2] - m[1, 2] * m[2, 1]) +
                m[0, 1] * (m[1, 2] * m[2, 0] - m[1, 0] * m[2, 2]) +
                m[0, 2] * (m[1, 0] * m[2, 1] - m[1, 1] * m[2, 0]);

            rlt.m[0, 0] = cp.m[1, 1] * cp.m[2, 2] - cp.m[1, 2] * cp.m[2, 1];
            rlt.m[0, 1] = cp.m[1, 0] * cp.m[2, 2] - cp.m[1, 2] * cp.m[2, 0];
            rlt.m[0, 2] = cp.m[1, 0] * cp.m[2, 1] - cp.m[1, 1] * cp.m[2, 0];

            rlt.m[1, 0] = cp.m[0, 1] * cp.m[2, 2] - cp.m[0, 2] * cp.m[2, 1];
            rlt.m[1, 1] = cp.m[0, 0] * cp.m[2, 2] - cp.m[0, 2] * cp.m[2, 0];
            rlt.m[1, 2] = cp.m[0, 0] * cp.m[2, 1] - cp.m[0, 1] * cp.m[2, 0];

            rlt.m[2, 0] = cp.m[0, 1] * cp.m[1, 2] - cp.m[0, 2] * cp.m[1, 1];
            rlt.m[2, 1] = cp.m[0, 0] * cp.m[1, 2] - cp.m[0, 2] * cp.m[1, 0];
            rlt.m[2, 2] = cp.m[0, 0] * cp.m[1, 1] - cp.m[0, 1] * cp.m[1, 0];

            rlt.m[0, 1] = -rlt.m[0, 1];
            rlt.m[1, 0] = -rlt.m[1, 0];
            rlt.m[1, 2] = -rlt.m[1, 2];
            rlt.m[2, 1] = -rlt.m[2, 1];

            rlt = (1 / d) * rlt;

            return rlt;
        }

        public bool hasInverse()
        {
            if (c != r)
                return false;

            double rlt = m[0, 0] * (m[1, 1] * m[2, 2] - m[1, 2] * m[2, 1]) +
                m[0, 1] * (m[1, 2] * m[2, 0] - m[1, 0] * m[2, 2]) +
                m[0, 2] * (m[1, 0] * m[2, 1] - m[1, 1] * m[2, 0]);

            if (rlt == 0.0f)
                return false;
            else
                return true;
            /*
            double rlt = 0.0f;
            for(int i=0 ; i < r ; i++)
            {
                double plus = 1.0f, minus = 1.0f;
                for(int j=0 ; j < c ; j++)
                {
                    int p_c = i + j;
                    if (p_c >= r)
                        p_c -= r;
                    else if (p_c < 0)
                        p_c += r;
                    plus *= m[j, p_c];

                    int m_c = i - j;
                    if (m_c >= r)
                        m_c -= r;
                    else if (m_c < 0)
                        m_c += r;
                    minus *= m[j, m_c];
                }
                rlt += plus - minus;
            }
            if (rlt == 0.0f)
                return false;
            else
                return true;
            */
        }

        public void setZero()
        {
            for(int i=0 ; i < c ; i++)
            {
                for(int j=0 ; j < r ; j++)
                {
                    m[i, j] = 0.0f;
                }
            }
        }

        public void setIdentity()
        {
            for(int i=0 ; i < c ; i++)
            {
                for(int j=0 ; j < r ; j++)
                {
                    if (i == j)
                        m[i, j] = 1.0f;
                    else
                        m[i, j] = 0.0f;
                }
            }
        }

        public void setValue(double[,] val, int x, int y)
        {
            if(c != x || r != y)
            {
                MessageBox.Show("Column or Row does not equal");
                return;
            }

            for(int i=0 ; i < c ; i++)
            {
                for(int j=0 ; j < r ; j++)
                {
                    m[i, j] = val[i, j];
                }
            }
            return;
        }

        public void writeLog(System.IO.TextWriter log, string msg)
        {
            log.WriteLine(msg);
            for(int i=0;i<c;i++)
            {
                for(int j=0 ; j < r;j++)
                {
                    log.Write(m[i, j] + ",");
                }
                log.Write("\n");
            }
        }
    }
}
