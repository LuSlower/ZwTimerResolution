using System.Windows.Forms.VisualStyles;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Threading;
using System;

namespace SleepTester
{
    partial class SleepTester
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private CheckBox cpuLoadCheckBox;
        private TextBox textBoxStart;
        private TextBox textBoxEnd;
        private NumericUpDown upDownSamples;
        private NumericUpDown upDownInc;
        private Button buttonTest;
        private ProgressBar progressBar;

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
        private bool isCpuLoadRunning = false;
        private List<Thread> cpuLoadThreads = new List<Thread>();

        private void InitializeComponent()
        {
            this.ClientSize = new System.Drawing.Size(130, 240);
            this.Text = "Sleep-Tester";
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.StartPosition = FormStartPosition.CenterScreen;
            this.FormBorderStyle = FormBorderStyle.FixedSingle;

            double startValue = TimerResolution.GetMaxResolution();
            double endValue = startValue + 1000;

            Label labelStart = new Label { Text = "Start:", Location = new System.Drawing.Point(28, 15), Size = new System.Drawing.Size(32, 15) };
            this.Controls.Add(labelStart);

            textBoxStart = new TextBox { Location = new System.Drawing.Point(65, 10), Size = new System.Drawing.Size(45, 20), MaxLength = 6, Text = startValue.ToString() };
            textBoxStart.KeyPress += ValidateNumericInput;
            this.Controls.Add(textBoxStart);

            Label labelEnd = new Label { Text = "End:", Location = new System.Drawing.Point(30, 55), Size = new System.Drawing.Size(30, 15) };
            this.Controls.Add(labelEnd);

            textBoxEnd = new TextBox { Location = new System.Drawing.Point(65, 50), Size = new System.Drawing.Size(45, 20), MaxLength = 6, Text = endValue.ToString() };
            textBoxEnd.KeyPress += ValidateNumericInput;
            this.Controls.Add(textBoxEnd);

            Label labelSamples = new Label { Text = "Samples:", Location = new System.Drawing.Point(17, 90), Size = new System.Drawing.Size(52, 15) };
            this.Controls.Add(labelSamples);

            upDownSamples = new NumericUpDown { Location = new System.Drawing.Point(70, 90), Size = new System.Drawing.Size(40, 20), Maximum = 100, Minimum = 0, Value = 20 };
            this.Controls.Add(upDownSamples);

            Label labelInc = new Label { Text = "Increment:", Location = new System.Drawing.Point(10, 132), Size = new System.Drawing.Size(60, 15) };
            this.Controls.Add(labelInc);

            upDownInc = new NumericUpDown { Location = new System.Drawing.Point(70, 130), Size = new System.Drawing.Size(40, 20), Maximum = 100, Minimum = 0, Value = 13 };
            this.Controls.Add(upDownInc);

            buttonTest = new Button { Text = "Start Test", Location = new System.Drawing.Point(25, 190), Size = new System.Drawing.Size(80, 20) };
            buttonTest.Click += ButtonTest_Click;
            this.Controls.Add(buttonTest);

            cpuLoadCheckBox = new CheckBox { Text = "Cpu Stress", Location = new System.Drawing.Point(30, 160), Size = new System.Drawing.Size(80, 20) };
            this.Controls.Add(cpuLoadCheckBox);
            cpuLoadCheckBox.Checked = true;

            progressBar = new ProgressBar { Location = new System.Drawing.Point(10, 220), Size = new System.Drawing.Size(110, 10), Step = 1, Visible = true };
            this.Controls.Add(progressBar);
        }

        private void ValidateNumericInput(object sender, KeyPressEventArgs e)
        {
            if (!char.IsDigit(e.KeyChar) && e.KeyChar != (char)Keys.Back)
            {
                e.Handled = true;
            }
        }
        #endregion
    }
}

