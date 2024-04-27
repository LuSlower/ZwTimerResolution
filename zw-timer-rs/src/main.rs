use std::env;
use std::ffi::c_void;
use std::thread::park;

extern "system" {
    fn ZwSetTimerResolution(desired_resolution: u32, set_resolution: bool,
                            current_resolution: *mut u32) -> i16;
    fn SetProcessWorkingSetSize(hProcess: *mut c_void,
                                dwMinimumWorkingSetSize: usize,
                                dwMaximumWorkingSetSize: usize
                                ) -> i16;
        fn SetPriorityClass(hProcess: *mut c_void, dwPriorityClass: u32) -> i32;
        fn OpenProcess(dwDesiredAccess: u32, bInheritHandle: bool, dwProcessId: u32) -> *mut c_void;
        fn SetProcessInformation(ProcessHandle: *mut c_void, ProcessInformationClass: u32,
            ProcessInformation: *mut c_void,
            ProcessInformationSize: u32,
        ) -> i16;
        fn GetCurrentProcessId() -> u32;
        fn FreeConsole();
}

const PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION: u32 = 0x4;
const PROCESS_QUERY_INFORMATION: u32 = 0x0400;
const PROCESS_QUERY_LIMITED_INFORMATION: u32 = 0x1000;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 || args[1] == "help" {
        eprintln!("Uso: zw-timer-rs.exe <int>");
        println!("especifique su resolucion de temporizador personalizada");
        std::process::exit(1);
    }

    let resolution: u32 = match args[1].parse() {
        Ok(num) => num,
        Err(_) => {
            println!("error: resolucion invalida");
            std::process::exit(1);
        }
    };

    let mut current_resolution: u32 = 0;
    let status = unsafe { ZwSetTimerResolution(resolution, true, &mut current_resolution) };
    if status != 0 {
        eprintln!("error: ZwSetTimerResolution fallo, {}", status);
        std::process::exit(1);
    }

    let process_handle = unsafe { OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
        false,
        GetCurrentProcessId()) };

    if process_handle.is_null() {
        eprintln!("error: OpenProcess fallo");
        std::process::exit(1);
    }

    let status = unsafe {
        SetProcessInformation(
            process_handle,
            PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION,
            &mut current_resolution as *mut u32 as *mut std::ffi::c_void,
            std::mem::size_of::<u32>() as u32,
        )
    };
    if status != 0 {
        eprintln!("error: SetProcessInformation fallo, {}", status);
        std::process::exit(1);
    }

    println!("resolucion establecida correctamente a {} ns", current_resolution);

    unsafe {
        SetPriorityClass(process_handle, 0x00100000); // PROCESS_MODE_BACKGROUND_BEGIN
        SetProcessWorkingSetSize(process_handle, usize::MIN , usize::MAX);
        FreeConsole(); // Ocultar consola

    }

    park();
}
