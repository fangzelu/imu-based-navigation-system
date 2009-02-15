using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SensorNode
{
    public partial class OptionForm : Form
    {
        BaseForm _baseForm;

        public OptionForm()
        {
            InitializeComponent();
            
        }

        public OptionForm(BaseForm baseForm)
        {
            InitializeComponent();
            InitData(baseForm);
        }

        private void InitData(BaseForm baseForm)
        {
            this.AcceptButton = this.confirmButton;
            this.CancelButton = this.cancelButton;

            switch (baseForm.DeviceRule)
            {
                case Rule.SensorNode:
                    DeviceBox.SelectedIndex = 0;
                    break;
                case Rule.Wiimote:
                    DeviceBox.SelectedIndex = 1;
                    break;
                case Rule.MotionNode:
                    DeviceBox.SelectedIndex = 2;
                    break;
            }

            DataBufferSize.Value = (decimal)baseForm.DATA_BUFFER_MAX;
            NumberOfData.Value = (decimal)baseForm.DATA_LENGTH;
            NumberOfPackage.Value = (decimal)baseForm.PACKAGE_COUNT;

            GyroCalibration.Value = (decimal)baseForm.GYRO_CALIBRATION_COUNT;
            GyroAnchorWindow.Value = (decimal)baseForm.GYRO_ANCHOR_WINDOW_SIZE;

            AccelCalibration.Value = (decimal)baseForm.ACCEL_CALIBRATION_COUNT;
            AccelAnchorWindow.Value = (decimal)baseForm.ACCEL_ANCHOR_WINDOW_SIZE;

            MagCalibration.Value = (decimal)baseForm.MAG_CALIBRATION_COUNT;
            MagAnchorWindow.Value = (decimal)baseForm.MAG_ANCHOR_WINDOW_SIZE;

            _baseForm = baseForm;
        }

        private void confirmButton_Click(object sender, EventArgs e)
        {
            switch (DeviceBox.SelectedIndex)
            {
                case 0:
                    _baseForm.DeviceRule = Rule.SensorNode;
                    break;
                case 1:
                    _baseForm.DeviceRule = Rule.Wiimote;
                    break;
                case 2:
                    _baseForm.DeviceRule = Rule.MotionNode;
                    break;
            }

            _baseForm.DATA_BUFFER_MAX = (int)DataBufferSize.Value;
            _baseForm.DATA_LENGTH = (int)NumberOfData.Value;
            _baseForm.PACKAGE_COUNT = (int)NumberOfPackage.Value;

            _baseForm.GYRO_CALIBRATION_COUNT = (int)GyroCalibration.Value;
            _baseForm.GYRO_ANCHOR_WINDOW_SIZE = (int)GyroAnchorWindow.Value;

            _baseForm.ACCEL_CALIBRATION_COUNT = (int)AccelCalibration.Value;
            _baseForm.ACCEL_ANCHOR_WINDOW_SIZE = (int)AccelAnchorWindow.Value;

            _baseForm.MAG_CALIBRATION_COUNT = (int)MagCalibration.Value;
            _baseForm.MAG_ANCHOR_WINDOW_SIZE = (int)MagAnchorWindow.Value;

            _baseForm.saveOptionAsFile(false);
        }
    }
}
