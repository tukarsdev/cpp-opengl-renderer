#!/usr/bin/env python3
import argparse
import os
import platform
import shutil
import subprocess
import sys
import stat

def run_command(cmd, shell=False, step_name=""):
    printable_cmd = " ".join(cmd) if isinstance(cmd, list) else cmd
    pretty_name = f" ({step_name})" if step_name else ""
    print(f"\n> {printable_cmd}{pretty_name}")
    
    try:
        subprocess.check_call(cmd, shell=shell)
    except subprocess.CalledProcessError as e:
        exit_code = hex(e.returncode).upper()
        if "build" in step_name.lower() or "cmake" in printable_cmd.lower():
            sys.exit(f"Build step failed with exit code {exit_code}.")
        elif "run" in step_name.lower() or "exe" in printable_cmd.lower():
            sys.exit(f"The program crashed (exit code {exit_code}).")
        else:
            sys.exit(f"Command failed: {printable_cmd} (exit code {exit_code})")
    except FileNotFoundError:
        sys.exit(f"Command not found: {cmd[0]}")
    except Exception as e:
        sys.exit(f"Unexpected error during {step_name or 'command'}: {e}")

def main():
    parser = argparse.ArgumentParser(description="Cross-platform CMake build script")
    parser.add_argument("--debug", action="store_true", help="Build in Debug mode")
    parser.add_argument("--clean", action="store_true", help="Clean build directory first")
    args = parser.parse_args()
    
    build_type = "Debug" if args.debug else "Release"
    build_dir = "build"
    
    system = platform.system().lower()
    is_windows = system == "windows"
    
    def remove_readonly(func, path, _):
        "Clear the readonly bit and reattempt the removal"
        os.chmod(path, stat.S_IWRITE)
        func(path)
    
    if args.clean and os.path.exists(build_dir):
        print("Cleaning build directory...")
        shutil.rmtree(build_dir, onexc=remove_readonly)
    
    cmake_generator = "Visual Studio 17 2022" if is_windows else "Unix Makefiles"
    
    run_command(["cmake", "-B", build_dir, "-G", cmake_generator], step_name="CMake configure")
    run_command(["cmake", "--build", build_dir, "--config", build_type], step_name="CMake build")
    
    exe_name = "renderer.exe" if is_windows else "renderer"
    exe_path = os.path.join(build_dir, build_type, exe_name)
    
    if not os.path.exists(exe_path):
        exe_path = os.path.join(build_dir, exe_name)
    
    if not is_windows and os.path.exists(exe_path):
        os.chmod(exe_path, 0o755)
    
    print(f"Running: {exe_path}")
    run_command([exe_path] if is_windows else [f"./{exe_path}"], shell=not is_windows, step_name="Run executable")

if __name__ == "__main__":
    main()

