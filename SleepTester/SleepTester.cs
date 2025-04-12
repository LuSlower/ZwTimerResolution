using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SleepTester
{
    public partial class SleepTester : Form
    {
        public SleepTester()
        {
            InitializeComponent();
        }

        public void ExecuteTest(int start, int end, int increment, int samples)
        {
            try
            {
                StringBuilder results = new StringBuilder();
                results.AppendLine("sleep(1), delta, zwres");

                int totalIterations = (int)Math.Ceiling((double)(end - start) / increment);
                Invoke(new Action(() =>
                {
                    progressBar.Minimum = 0;
                    progressBar.Maximum = totalIterations;
                    progressBar.Value = 0;
                    progressBar.Visible = true;
                }));

                bool wasCpuLoadEnabled = cpuLoadCheckBox.Checked;

                if (wasCpuLoadEnabled)
                {
                    isCpuLoadRunning = true;
                    GenerateCpuLoad();
                }

                for (int res_act = start; res_act <= end; res_act += increment)
                {
                    StartProcessNoWait(@".\zwt.exe", $"--res {res_act}");
                    System.Threading.Thread.Sleep(500);

                    double res_current = TimerResolution.GetCurrentResolution();
                    string output = ExecuteCommand($@".\zwt.exe --test {samples}");

                    foreach (string line in output.Split(new[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries))
                    {
                        var match = System.Text.RegularExpressions.Regex.Match(line, @"avg: (\d+\.\d+)");
                        if (match.Success)
                        {
                            float avgSleep = float.Parse(match.Groups[1].Value);
                            float delta = avgSleep - 1;
                            string formattedAvgSleep = avgSleep.ToString("F4");
                            string formattedDelta = delta.ToString("F4");

                            string marker = (avgSleep < 1.0150f && delta < 0.0150f) ? "*" : "";
                            results.AppendLine($"{formattedAvgSleep}, {formattedDelta}, {res_current}{marker}");
                        }
                    }

                    KillProcess("zwt");

                    Invoke(new Action(() => progressBar.PerformStep()));
                }

                if (wasCpuLoadEnabled)
                {
                    StopCpuLoad();
                }

                File.WriteAllText("SleepTest.txt", results.ToString(), Encoding.ASCII);

                Invoke(new Action(() =>
                {
                    MessageBox.Show("Test completed.", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    buttonTest.Enabled = true;
                    cpuLoadCheckBox.Enabled = true;
                }));
            }
            catch (Exception ex)
            {
                Invoke(new Action(() =>
                {
                    MessageBox.Show($"Error: {ex.Message}\n{ex.StackTrace}", "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }));
            }
        }

        public void ExecuteTestFromCommandLine(int start, int end, int increment, int samples, bool CpuStress = false)
        {
            try
            {
                StringBuilder results = new StringBuilder();
                results.AppendLine("sleep(1), delta, zwres");

                int totalIterations = (int)Math.Ceiling((double)(end - start) / increment);

                bool wasCpuLoadEnabled = CpuStress;

                if (wasCpuLoadEnabled)
                {
                    isCpuLoadRunning = true;
                    GenerateCpuLoad();
                }

                for (int res_act = start; res_act <= end; res_act += increment)
                {
                    StartProcessNoWait(@".\zwt.exe", $"--res {res_act}");
                    System.Threading.Thread.Sleep(500);

                    double res_current = TimerResolution.GetCurrentResolution();
                    string output = ExecuteCommand($@".\zwt.exe --test {samples}");

                    foreach (string line in output.Split(new[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries))
                    {
                        var match = System.Text.RegularExpressions.Regex.Match(line, @"avg: (\d+\.\d+)");
                        if (match.Success)
                        {
                            float avgSleep = float.Parse(match.Groups[1].Value);
                            float delta = avgSleep - 1;
                            string formattedAvgSleep = avgSleep.ToString("F4");
                            string formattedDelta = delta.ToString("F4");

                            string marker = (avgSleep < 1.0150f && delta < 0.0150f) ? "*" : "";
                            results.AppendLine($"{formattedAvgSleep}, {formattedDelta}, {res_current}{marker}");
                        }
                    }

                    KillProcess("zwt");

                    Console.WriteLine($"Completed iteration {res_act}/{end}");
                }

                if (wasCpuLoadEnabled)
                {
                    StopCpuLoad();
                }

                File.WriteAllText("SleepTest.txt", results.ToString(), Encoding.ASCII);
                Console.WriteLine("Test completed. Results saved to SleepTest.txt.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error: {ex.Message}\n{ex.StackTrace}");
            }
        }


        private void ButtonTest_Click(object sender, EventArgs e)
        {
            if (!File.Exists(@".\zwt.exe"))
            {
                MessageBox.Show("zwt.exe not found in the current directory.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (!int.TryParse(textBoxStart.Text, out int start) ||
                !int.TryParse(textBoxEnd.Text, out int end) ||
                !int.TryParse(upDownSamples.Text, out int samples) ||
                !int.TryParse(upDownInc.Text, out int increment))
            {
                MessageBox.Show("Invalid input.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (end <= start)
            {
                MessageBox.Show("The final resolution must be greater than the initial resolution.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            buttonTest.Enabled = false;
            cpuLoadCheckBox.Enabled = false;

            Task.Run(() =>
            {
                ExecuteTest(start, end, increment, samples);
            });
        }

        private void GenerateCpuLoad()
        {
            int numThreads = Environment.ProcessorCount;
            cpuLoadThreads.Clear();

            for (int i = 0; i < numThreads; i++)
            {
                var thread = new Thread(() =>
                {
                    while (isCpuLoadRunning)
                    {
                        double result = Math.Sqrt(12345.6789);
                        result += 0.1;

                        GC.KeepAlive(result);
                    }
                })
                {
                    IsBackground = true
                };

                cpuLoadThreads.Add(thread);
                thread.Start();
            }
        }


        private void StopCpuLoad()
        {
            isCpuLoadRunning = false;

            foreach (var thread in cpuLoadThreads)
            {
                if (!thread.Join(1000))
                {
                    thread.Interrupt();
                }
            }

            cpuLoadThreads.Clear();
        }


        private void StartProcessNoWait(string fileName, string arguments)
        {
            Process process = new Process();
            process.StartInfo.FileName = fileName;
            process.StartInfo.Arguments = arguments;
            process.StartInfo.CreateNoWindow = true;
            process.StartInfo.UseShellExecute = false;
            process.Start();
        }

        private string ExecuteCommand(string command)
        {
            Process process = new Process();
            process.StartInfo.FileName = "cmd.exe";
            process.StartInfo.Arguments = $"/C {command}";
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.RedirectStandardError = true;
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.CreateNoWindow = true;

            process.Start();
            string output = process.StandardOutput.ReadToEnd();
            process.WaitForExit();
            return output;
        }


        private void KillProcess(string processName)
        {
            foreach (var proc in Process.GetProcessesByName(processName))
            {
                proc.Kill();
            }
        }


        public class TimerResolution
        {
            [DllImport("ntdll.dll")]
            private static extern int NtQueryTimerResolution(out int MinimumResolution, out int MaximumResolution, out int ActualResolution);

            public static double GetCurrentResolution()
            {
                return NtQueryTimerResolution(out _, out _, out int actualResolution) == 0 ? actualResolution : -1;
            }

            public static double GetMinResolution()
            {
                return NtQueryTimerResolution(out int minResolution, out _, out _) == 0 ? minResolution : -1;
            }

            public static double GetMaxResolution()
            {
                return NtQueryTimerResolution(out _, out int maxResolution, out _) == 0 ? maxResolution : -1;
            }
        }

    }
}
