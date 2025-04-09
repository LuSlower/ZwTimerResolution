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
        [DllImport("kernel32.dll")]
        static extern bool FreeConsole();
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                FreeConsole();
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new SleepTester());
                return;
            }

            if (args.Length == 1 && (args.Contains("--help") || args.Contains("/?")))
            {
                ShowHelp();
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

        static void ShowHelp()
        {
            Console.WriteLine("Usage:");
            Console.WriteLine("  --auto         Automatically runs the test.");
            Console.WriteLine("  --cpu-stress   Enables CPU stress during the test.");
            Console.WriteLine("  --help, /?     Shows this help message.");
        }
    }
}
