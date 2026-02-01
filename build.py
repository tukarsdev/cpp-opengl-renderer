#!/usr/bin/env python3
import argparse
import os
import platform
import shutil
import subprocess
import sys
import stat

# Package names for each package manager
LINUX_DEPENDENCIES = {
    "apt": [
        "pkg-config",
        "libgl1-mesa-dev",
        "libwayland-dev",
        "wayland-protocols",
        "libxkbcommon-dev",
        "libx11-dev",
        "libxrandr-dev",
        "libxinerama-dev",
        "libxcursor-dev",
        "libxi-dev",
    ],
    "dnf": [
        "pkgconfig",
        "mesa-libGL-devel",
        "wayland-devel",
        "wayland-protocols-devel",
        "libxkbcommon-devel",
        "libX11-devel",
        "libXrandr-devel",
        "libXinerama-devel",
        "libXcursor-devel",
        "libXi-devel",
    ],
    "pacman": [
        "pkg-config",
        "libglvnd",
        "wayland",
        "wayland-protocols",
        "libxkbcommon",
        "libx11",
        "libxrandr",
        "libxinerama",
        "libxcursor",
        "libxi",
    ],
}


def detect_package_manager():
    """Detect which package manager is available."""
    managers = [
        ("apt", ["apt", "--version"]),
        ("dnf", ["dnf", "--version"]),
        ("pacman", ["pacman", "--version"]),
    ]
    for name, cmd in managers:
        try:
            subprocess.run(
                cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True
            )
            return name
        except (subprocess.CalledProcessError, FileNotFoundError):
            continue
    return None


def check_package_installed(package_manager, package):
    """Check if a package is installed."""
    try:
        if package_manager == "apt":
            result = subprocess.run(
                ["dpkg", "-s", package],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )
            return result.returncode == 0
        elif package_manager == "dnf":
            result = subprocess.run(
                ["rpm", "-q", package],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )
            return result.returncode == 0
        elif package_manager == "pacman":
            result = subprocess.run(
                ["pacman", "-Q", package],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )
            return result.returncode == 0
    except FileNotFoundError:
        pass
    return False


def get_missing_dependencies(package_manager):
    """Return list of missing dependencies."""
    if package_manager not in LINUX_DEPENDENCIES:
        return []

    missing = []
    for package in LINUX_DEPENDENCIES[package_manager]:
        if not check_package_installed(package_manager, package):
            missing.append(package)
    return missing


def get_install_command(package_manager, packages):
    """Get the command to install packages."""
    # Check if running as root
    is_root = os.geteuid() == 0 if hasattr(os, 'geteuid') else False

    if package_manager == "apt":
        cmd = ["apt", "install", "-y"] + packages
    elif package_manager == "dnf":
        cmd = ["dnf", "install", "-y"] + packages
    elif package_manager == "pacman":
        cmd = ["pacman", "-S", "--noconfirm"] + packages
    else:
        return None

    # Only add sudo if not root
    if not is_root:
        cmd = ["sudo"] + cmd

    return cmd


def check_and_install_dependencies():
    """Check for missing dependencies and offer to install them."""
    if platform.system().lower() != "linux":
        return True

    package_manager = detect_package_manager()
    if not package_manager:
        print("Warning: Could not detect package manager. Skipping dependency check.")
        return True

    missing = get_missing_dependencies(package_manager)
    if not missing:
        return True

    print(f"\nMissing dependencies detected ({len(missing)} package(s)):")
    for pkg in missing:
        print(f"  - {pkg}")

    install_cmd = get_install_command(package_manager, missing)
    print(f"\nInstall command: {' '.join(install_cmd)}")

    try:
        response = input("\nWould you like to install them now? [Y/n] ").strip().lower()
    except EOFError:
        response = "n"

    if response in ("", "y", "yes"):
        try:
            subprocess.check_call(install_cmd)
            print("Dependencies installed successfully.\n")
            return True
        except subprocess.CalledProcessError:
            print("Failed to install dependencies.", file=sys.stderr)
            return False
        except KeyboardInterrupt:
            print("\nInstallation cancelled.")
            return False
    else:
        print("Skipping dependency installation. Build may fail.")
        return True


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

def is_headless():
    """Check if running in a headless environment."""
    return not os.environ.get("DISPLAY") and not os.environ.get("WAYLAND_DISPLAY")

def main():
    parser = argparse.ArgumentParser(description="Cross-platform CMake build script")
    parser.add_argument("--debug", action="store_true", help="Build in Debug mode")
    parser.add_argument("--clean", action="store_true", help="Clean build directory first")
    parser.add_argument(
        "--skip-deps",
        action="store_true",
        help="Skip dependency check on Linux",
    )
    args = parser.parse_args()

    # Check dependencies on Linux
    if not args.skip_deps:
        if not check_and_install_dependencies():
            sys.exit(1)

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

    run_command(
        ["cmake", "-B", build_dir, "-G", cmake_generator], step_name="CMake configure"
    )
    run_command(
        ["cmake", "--build", build_dir, "--config", build_type], step_name="CMake build"
    )

    exe_name = "renderer.exe" if is_windows else "renderer"
    exe_path = os.path.join(build_dir, build_type, exe_name)

    if not os.path.exists(exe_path):
        exe_path = os.path.join(build_dir, exe_name)

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


if __name__ == "__main__":
    main()
