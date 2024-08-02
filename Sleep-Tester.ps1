# Check administrator privileges
if (-not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Start-Process powershell "-File `"$PSCommandPath`"" -Verb RunAs
    exit
}

function Console {
    param (
        [Switch]$Show,
        [Switch]$Hide,
        [string]$Text
    )

    if (-not ("Console.Window" -as [type])) {
        Add-Type -Name Window -Namespace Console -MemberDefinition '
        [DllImport("Kernel32.dll")]
        public static extern IntPtr GetConsoleWindow();
        [DllImport("user32.dll")]
        public static extern bool ShowWindow(IntPtr hWnd, Int32 nCmdShow);
        '
    }

    if ($Show) {
        $consolePtr = [Console.Window]::GetConsoleWindow()
        $null = [Console.Window]::ShowWindow($consolePtr, 5)
    }

    if ($Hide) {
        $consolePtr = [Console.Window]::GetConsoleWindow()
        $null = [Console.Window]::ShowWindow($consolePtr, 0)
    }

    if ($Text) {
        $syncHash.TextBoxOutPut.AppendText("$Text`r`n")
    }
}

# Valores de inicio, final, conteo, iteraciones
$start_value = 5000
$end_value = 5200
$samples_value = 20
$increment_value = 13

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

Add-Type @"
using System;
using System.Runtime.InteropServices;

public class TimerResolution
{
    [DllImport("ntdll.dll")]
    public static extern uint ZwQueryTimerResolution(out uint MinimumResolution, out uint MaximumResolution, out uint CurrentResolution);

    public static uint GetCurrentResolution()
    {
        uint min, max, current;
        ZwQueryTimerResolution(out min, out max, out current);
        return current;
    }
}
"@

function test_precision {
    # Verificar si zwtimer.exe existe en el directorio actual
    if (-not (Test-Path ".\zwt.exe")) {
        [System.Windows.Forms.MessageBox]::Show("zwt.exe not found in the current directory.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    # Validar que las entradas solo contengan dígitos y tengan la longitud adecuada
    if ($textBoxStart.Text -notmatch '^\d{4,6}$') {
        [System.Windows.Forms.MessageBox]::Show("Start value must be a number between 4 and 6 digits.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    if ($textBoxEnd.Text -notmatch '^\d{4,6}$') {
        [System.Windows.Forms.MessageBox]::Show("End value must be a number between 4 and 6 digits.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    if ($UpDownSamples.Text -notmatch '^\d{1,3}$') {
        [System.Windows.Forms.MessageBox]::Show("Samples value must be a number between 1 and 3 digits.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    if ($UpDownInc.Text -notmatch '^\d{1,2}$') {
        [System.Windows.Forms.MessageBox]::Show("Increment value must be a number between 1 and 2 digits.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    $start = [int]$textBoxStart.Text
    $end = [int]$textBoxEnd.Text
    $samples = [int]$UpDownSamples.Text
    $increment = [int]$UpDownInc.Text

    if ($end -le $start) {
        [System.Windows.Forms.MessageBox]::Show("The final resolution must be greater than the initial resolution.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    $results = @()

    $totalIterations = [math]::Ceiling(($end - $start) / $increment)

    # Inicializar la barra de progreso
    $progressBar.Minimum = 0
    $progressBar.Maximum = $totalIterations
    $progressBar.Value = 0
    $progressBar.Visible = $true

    for ($res_act = $start; $end -ge $res_current; $res_act +=$increment) {

        # Ejecutar zwtimer con la resolucion
        Start-Process -FilePath ".\zwt.exe" -ArgumentList "$res_act" -NoNewWindow 

        Start-Sleep 1

        # Obtener la resolución actual
        $res_current = [TimerResolution]::GetCurrentResolution()

        # Sleep-Test
        $output = Invoke-Expression ".\zwt.exe test $samples"

        # Parsear la salida para extraer los valores de avg y delta
        $output -split [Environment]::NewLine | ForEach-Object {
            if ($_ -match 'avg: (\d+\.\d+) ms \| (\d+\.\d+) ms') {
                $avgSleep = [float] $matches[1]
                $delta = [float] $matches[2]

                # precisión de 4 decimales
                $formattedAvgSleep = "{0:N4}" -f $avgSleep
                $formattedDelta = "{0:N4}" -f $delta

                $results += "$formattedAvgSleep, $formattedDelta, $res_current"
            }
        }

        Write-Host "Completed test with resolution $res_current"

        # refrescar la barra de progreso
        $progressBar.PerformStep()

        # detener zwtimer
        Stop-Process -Name "zwt" -ErrorAction SilentlyContinue
    }

    # guardar resultados en un archivo de texto
    $header = "sleep(1), delta, zwres"
    $results = $header + [Environment]::NewLine + ($results -join [Environment]::NewLine)

    $results | Out-File -FilePath "Sleep-Test.txt" -Encoding ascii


    [System.Windows.Forms.MessageBox]::Show("The test has finished.", "Info", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
}

# ocultar consola, crear form
Console -Hide
[System.Windows.Forms.Application]::EnableVisualStyles();
$form = New-Object System.Windows.Forms.Form
$form.ClientSize = New-Object System.Drawing.Size(130, 220)
$form.Text = "Sleep-Tester"
$form.MaximizeBox = $false
$form.MinimizeBox = $false
$form.StartPosition = "CenterScreen"
$form.FormBorderStyle = [System.Windows.Forms.FormBorderStyle]::FixedSingle

# Start
$labelStart = New-Object System.Windows.Forms.Label
$labelStart.Text = "Start:"
$labelStart.Location = New-Object System.Drawing.Point(28, 15)
$labelStart.Size = New-Object System.Drawing.Size(32, 15)
$form.Controls.Add($labelStart)

$textBoxStart = New-Object System.Windows.Forms.TextBox
$textBoxStart.Location = New-Object System.Drawing.Point(65, 10)
$textBoxStart.Size = New-Object System.Drawing.Size(45, 20)
$textBoxStart.Text = $start_value
$textBoxStart.MaxLength = 6
$textBoxStart.Add_KeyPress({
    param($sender, $e)
    if (-not [char]::IsDigit($e.KeyChar) -and $e.KeyChar -ne [System.Windows.Forms.Keys]::Back) {
        $e.Handled = $true
    }
})
$form.Controls.Add($textBoxStart)

# End
$labelEnd = New-Object System.Windows.Forms.Label
$labelEnd.Text = "End:"
$labelEnd.Location = New-Object System.Drawing.Point(30, 55)
$labelEnd.Size = New-Object System.Drawing.Size(30, 15)
$form.Controls.Add($labelEnd)

$textBoxEnd = New-Object System.Windows.Forms.TextBox
$textBoxEnd.Location = New-Object System.Drawing.Point(65, 50)
$textBoxEnd.Size = New-Object System.Drawing.Size(45, 20)
$textBoxEnd.Text = $end_value
$textBoxEnd.MaxLength = 6
$textBoxEnd.Add_KeyPress({
    param($sender, $e)
    if (-not [char]::IsDigit($e.KeyChar) -and $e.KeyChar -ne [System.Windows.Forms.Keys]::Back) {
        $e.Handled = $true
    }
})
$form.Controls.Add($textBoxEnd)

# Samples
$labelSamples = New-Object System.Windows.Forms.Label
$labelSamples.Text = "Samples:"
$labelSamples.Location = New-Object System.Drawing.Point(17, 90)
$labelSamples.Size = New-Object System.Drawing.Size(52, 15)
$form.Controls.Add($labelSamples)

$UpDownSamples = New-Object System.Windows.Forms.NumericUpDown
$UpDownSamples.Location = New-Object System.Drawing.Point(70, 90)
$UpDownSamples.Size = New-Object System.Drawing.Size(40, 20)
$UpDownSamples.Value = $samples_value
$UpDownSamples.Maximum = 100
$UpDownSamples.Minimum = 0
$form.Controls.Add($UpDownSamples)

# Increment
$labelInc = New-Object System.Windows.Forms.Label
$labelInc.Text = "Increment:"
$labelInc.Location = New-Object System.Drawing.Point(10, 132)
$labelInc.Size = New-Object System.Drawing.Size(60, 15)
$form.Controls.Add($labelInc)

$UpDownInc = New-Object System.Windows.Forms.NumericUpDown
$UpDownInc.Location = New-Object System.Drawing.Point(70, 130)
$UpDownInc.Size = New-Object System.Drawing.Size(40, 20)
$UpDownInc.Value = $increment_value
$UpDownInc.Maximum = 100
$UpDownInc.Minimum = 0
$form.Controls.Add($UpDownInc)

# Test
$buttonTest = New-Object System.Windows.Forms.Button
$buttonTest.Size = New-Object System.Drawing.Size(80, 20)
$buttonTest.Location = New-Object System.Drawing.Point(25, 170)
$buttonTest.Text = "Start Test"
$buttonTest.Add_Click({
    test_precision
})
$form.Controls.Add($buttonTest)

# ProgressBar
$progressBar = New-Object System.Windows.Forms.ProgressBar
$progressBar.Size = New-Object System.Drawing.Size(110, 10)
$progressBar.Location = New-Object System.Drawing.Point(10, 200)
$progressBar.Visible = $true
$progressBar.Step = 1
$form.Controls.Add($progressBar)

$form.ShowDialog()
