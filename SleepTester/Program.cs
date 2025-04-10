using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using static SleepTester.SleepTester;

namespace SleepTester
{
    internal static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new SleepTester());
                return;
            }

            bool isAuto = args.Contains("--auto");
            bool isCpuStress = args.Contains("--cpu-stress");

            int start = (int)TimerResolution.GetMaxResolution();
            int end = start + 1000;
            int increment = 13;
            int samples = 20;

            SleepTester sleepTester = new SleepTester();

            if (isAuto)
            {
                Console.WriteLine("Executing test automatically...");
                sleepTester.ExecuteTestFromCommandLine(start, end, increment, samples, isCpuStress);
            }
            else
            {
                Console.WriteLine("Please provide valid arguments. Use --help for usage details.");
            }
        }
    }
}
