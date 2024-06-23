# Chequear privilegios de administrador
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
$delay = 2
if (-not $isAdmin) {
    [System.Windows.Forms.MessageBox]::Show("Run the script as an administrator", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    Start-Sleep -Seconds $delay
    Exit
}

# Valores de inicio, final y conteo
$start_value = 5000
$end_value = 5200
$count_value = 20

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

function test_precision {
    # Verificar si zwtimer.exe existe en el directorio actual
    if (-not (Test-Path ".\zwtimer.exe")) {
        [System.Windows.Forms.MessageBox]::Show("zwtimer.exe not found in the current directory.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
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

    if ($textBoxCount.Text -notmatch '^\d{1,3}$') {
        [System.Windows.Forms.MessageBox]::Show("Count value must be a number between 1 and 3 digits.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    $start_res = [int]$textBoxStart.Text
    $end_res = [int]$textBoxEnd.Text
    $count = [int]$textBoxCount.Text

    if ($end_res -le $start_res) {
        [System.Windows.Forms.MessageBox]::Show("The final resolution must be greater than the initial resolution.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
        return
    }

    $results = @()

    $totalIterations = [math]::Ceiling(($end_res - $start_res) / 10)

    # Inicializar la barra de progreso
    $progressBar.Minimum = 0
    $progressBar.Maximum = $totalIterations
    $progressBar.Value = 0
    $progressBar.Visible = $true

    for ($res_act = $start_res; $end_res -ge $res_current; $res_act +=13) {

        # Ejecutar zwtimer con la resolucion
        Start-Process -FilePath ".\zwtimer.exe" -ArgumentList "$res_act" -NoNewWindow 

        Start-Sleep 1

        # Obtener la resolución actual
        $res_current = [TimerResolution]::GetCurrentResolution()

        # Sleep-Test
        $output = Invoke-Expression ".\zwtimer.exe test $count"

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
        Stop-Process -Name "zwtimer" -ErrorAction SilentlyContinue
    }

    # guardar resultados en un archivo de texto
    $header = "sleep(1), delta, zwres"
    $results = $header + [Environment]::NewLine + ($results -join [Environment]::NewLine)

    # timestamp
    $timestamp = Get-Random -Minimum 1000 -Maximum 5000

    $results | Out-File -FilePath "Sleep-Test-$timestamp.txt" -Encoding ascii


    [System.Windows.Forms.MessageBox]::Show("The test has finished.", "Info", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
}

# Crear form, ocultar consola
Console -Hide
[System.Windows.Forms.Application]::EnableVisualStyles();
$form = New-Object System.Windows.Forms.Form
$form.Size = New-Object System.Drawing.Size(350, 160)
$form.Text = "Sleep-Tester"
$form.MaximizeBox = $false
$form.StartPosition = "CenterScreen"
$form.FormBorderStyle = [System.Windows.Forms.FormBorderStyle]::FixedSingle

$labelStart = New-Object System.Windows.Forms.Label
$labelStart.Text = "Start"
$labelStart.Location = New-Object System.Drawing.Point(65, 10)
$labelStart.Size = New-Object System.Drawing.Size(30, 15)
$form.Controls.Add($labelStart)

$labelEnd = New-Object System.Windows.Forms.Label
$labelEnd.Text = "End"
$labelEnd.Location = New-Object System.Drawing.Point(160, 10)
$labelEnd.Size = New-Object System.Drawing.Size(30, 15)
$form.Controls.Add($labelEnd)

$labelCount = New-Object System.Windows.Forms.Label
$labelCount.Text = "Count"
$labelCount.Location = New-Object System.Drawing.Point(240, 10)
$labelCount.Size = New-Object System.Drawing.Size(50, 15)
$form.Controls.Add($labelCount)

$textBoxStart = New-Object System.Windows.Forms.TextBox
$textBoxStart.Size = New-Object System.Drawing.Size(60, 20)
$textBoxStart.Text = $start_value
$textBoxStart.MaxLength = 6
$textBoxStart.Location = New-Object System.Drawing.Point(50, 30)
$textBoxStart.Add_KeyPress({
    param($sender, $e)
    if (-not [char]::IsDigit($e.KeyChar) -and $e.KeyChar -ne [System.Windows.Forms.Keys]::Back) {
        $e.Handled = $true
    }
})
$form.Controls.Add($textBoxStart)

$textBoxEnd = New-Object System.Windows.Forms.TextBox
$textBoxEnd.Size = New-Object System.Drawing.Size(60, 20)
$textBoxEnd.Text = $end_value
$textBoxEnd.MaxLength = 6
$textBoxEnd.Location = New-Object System.Drawing.Point(140, 30)
$textBoxEnd.Add_KeyPress({
    param($sender, $e)
    if (-not [char]::IsDigit($e.KeyChar) -and $e.KeyChar -ne [System.Windows.Forms.Keys]::Back) {
        $e.Handled = $true
    }
})
$form.Controls.Add($textBoxEnd)

$textBoxCount = New-Object System.Windows.Forms.TextBox
$textBoxCount.Size = New-Object System.Drawing.Size(60, 20)
$textBoxCount.Text = $count_value
$textBoxCount.MaxLength = 3
$textBoxCount.Location = New-Object System.Drawing.Point(230, 30)
$textBoxCount.Add_KeyPress({
    param($sender, $e)
    if (-not [char]::IsDigit($e.KeyChar) -and $e.KeyChar -ne [System.Windows.Forms.Keys]::Back) {
        $e.Handled = $true
    }
})
$form.Controls.Add($textBoxCount)

$buttonTest = New-Object System.Windows.Forms.Button
$buttonTest.Size = New-Object System.Drawing.Size(80, 20)
$buttonTest.Location = New-Object System.Drawing.Point(130, 60)
$buttonTest.Text = "Start Test"
$buttonTest.Add_Click({
    test_precision
})
$form.Controls.Add($buttonTest)

$progressBar = New-Object System.Windows.Forms.ProgressBar
$progressBar.Size = New-Object System.Drawing.Size(300, 20)
$progressBar.Location = New-Object System.Drawing.Point(20, 90)
$progressBar.Visible = $true
$progressBar.Step = 1
$form.Controls.Add($progressBar)

$form.ShowDialog()
