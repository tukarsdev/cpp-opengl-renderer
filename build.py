#!/usr/bin/env python3
import argparse
import os
import platform
import shutil
import subprocess
import sys
import stat

BUILD_DIR = "build"

def print_stderr(stderr):
    print(stderr, file=sys.stderr, end="")

def run_command(cmd, shell=False, step_name="", log_stderr=True):
    printable_cmd = " ".join(cmd) if isinstance(cmd, list) else cmd
    pretty_name = f"[{step_name}]" if step_name else ""
    print(f"\n{pretty_name}:\n> {printable_cmd}")
    
    try: 
        result = subprocess.run(
            cmd, 
            shell=shell, 
            stderr=subprocess.PIPE,
            text=True
        )
        
        if result.returncode != 0:
            hex_returncode = hex(result.returncode)
            hex_returnpretty = hex_returncode[:2] + hex_returncode[2:].upper()
            print(f"The program exited with code: {hex_returnpretty}")
            
            if result.stderr != "" and log_stderr:
                print(result.stderr)
            
            return (result.returncode, result.stderr)
    except FileNotFoundError:
        sys.exit(f"Command not found: {cmd[0]}")
    except Exception as e:
        sys.exit(f"Unexpected error during {step_name or 'command'}: {e}")
    
    return (0, "")

def is_headless():
    """Check if running in a headless environment."""
    return not os.environ.get("DISPLAY") and not os.environ.get("WAYLAND_DISPLAY")

def detect_cmake_generator(is_windows):
    unix_fallback = "Unix Makefiles"
    windows_fallback = "Visual Studio 17 2022"
    
    # if shutil.which("ninja"):
    #     return "Ninja"

    if is_windows:
        try:
            output = subprocess.check_output(
                ["cmake", "--help"], 
                stderr=subprocess.DEVNULL, 
                text=True
            )
        except Exception as e:
            print("Failure to detect cmake generator: Exception occured:")
            print(e)
            print(f"Using fallback: {windows_fallback}")
            return windows_fallback
        
        ver = [
            "Visual Studio 18 2026",
            "Visual Studio 17 2022",
            "Visual Studio 16 2019",
            "Visual Studio 15 2017",
            "Visual Studio 14 2015"
        ]
        
        for gen in ver:
            if gen in output:
                return gen
        
        print("Failure to detect cmake generator: Not in cmake generator list")
        print(f"Using fallback: {windows_fallback}")
        return windows_fallback
    
    return unix_fallback

def clean_build():
    def remove_readonly(func, path, _):
        "Clear the readonly bit and reattempt the removal"
        os.chmod(path, stat.S_IWRITE)
        func(path)
    shutil.rmtree(BUILD_DIR, onexc=remove_readonly)

def build_configuration_files_command(cmake_generator):
    return run_command(
        ["cmake", "-B", BUILD_DIR, "-G", cmake_generator], 
        step_name="CMake configure", 
        log_stderr=False
    )

def build_configuration_files(cmake_generator):
    (exit_code, stderr_res) = build_configuration_files_command(cmake_generator)
    if exit_code != 0:
        print("There was an error generating configuration files:")
        print_stderr(stderr_res)
        print("Cleaning build directory and trying again.")
        clean_build()
        (exit_code, stderr_res) = build_configuration_files_command(cmake_generator)
        if exit_code != 0:
            print("There was an error generating the configuration files:")
            print_stderr(stderr_res)
            return

def build_step(build_type):
    run_command(
        ["cmake", "--build", BUILD_DIR, "--config", build_type], step_name="CMake build"
    )


def run_executable(is_windows, build_type):
    exe_name = "renderer.exe" if is_windows else "renderer"
    exe_path = os.path.join(BUILD_DIR, build_type, exe_name)
    
    if not os.path.exists(exe_path):
        exe_path = os.path.join(BUILD_DIR, exe_name)
    
    if not is_windows and os.path.exists(exe_path):
        os.chmod(exe_path, 0o755)
    
    if is_headless():
        print(f"\nHeadless environment detected. Skipping execution.")
        print(f"Build successful: {exe_path}")
        return

    print(f"Running: {exe_path}")
    run_command(
        [exe_path] if is_windows else [f"./{exe_path}"],
        shell=not is_windows,
        step_name="Run executable",
    )

# key note: need to not keep deleting dependencies folder

def main():
    parser = argparse.ArgumentParser(description="Cross-platform CMake build script")
    parser.add_argument("--debug", action="store_true", help="Build in Debug mode")
    parser.add_argument("--clean", action="store_true", help="Clean build directory first")
    parser.add_argument("--docker", action="store_true", help="Use docker to build and run the project")
    args = parser.parse_args()
    
    build_type = "Debug" if args.debug else "Release"
    
    system = platform.system().lower()
    is_windows = system == "windows"
    
    cmake_generator = detect_cmake_generator(is_windows)
    print(f"Using CMake generator: {cmake_generator}")
    
    if args.clean and os.path.exists(BUILD_DIR):
        print("Cleaning build directory...")
        clean_build()
    
    build_configuration_files(cmake_generator)
    build_step(build_type)
    run_executable(is_windows, build_type)

if __name__ == "__main__":
    main()


