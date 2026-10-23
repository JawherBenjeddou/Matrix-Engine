#!/usr/bin/env python3
"""
Matrix Engine bootstrap script.

Usage:
    python scripts/setup.py              # fetch dependencies + generate project files
    python scripts/setup.py --build      # ...and build the Debug configuration
    python scripts/setup.py --build --config Release
    python scripts/setup.py --action vs2022   # force a premake action

The script is idempotent: run it as often as you like.
"""
import argparse
import os
import platform
import shutil
import subprocess
import sys
import urllib.request
import zipfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
DEPS = os.path.join(ROOT, "Dependencies")
PREMAKE_DIR = os.path.join(ROOT, "vendor", "premake")
PREMAKE_EXE = os.path.join(PREMAKE_DIR, "premake5.exe")
PREMAKE_URL = "https://github.com/premake/premake-core/releases/download/v5.0.0/premake-5.0.0-windows.zip"

# (path under Dependencies, git url, tag) - mirrors .gitmodules so a non-git download still works.
SUBMODULES = [
    ("entt",   "https://github.com/skypjack/entt", "v3.12.2"),
    ("glm",    "https://github.com/g-truc/glm",    "0.9.9.8"),
    ("imgui",  "https://github.com/ocornut/imgui", "v1.89.9-docking"),
    ("spdlog", "https://github.com/gabime/spdlog", "v1.15.3"),
    ("stb",    "https://github.com/nothings/stb",  "master"),
]


def run(cmd, cwd=ROOT, check=True):
    print("> " + " ".join(cmd))
    return subprocess.run(cmd, cwd=cwd, check=check)


def dependency_present(name):
    path = os.path.join(DEPS, name)
    return os.path.isdir(path) and len(os.listdir(path)) > 0


def fetch_dependencies():
    missing = [m for m in SUBMODULES if not dependency_present(m[0])]
    if not missing:
        print("Dependencies already present.")
        return

    inside_git_repo = os.path.isdir(os.path.join(ROOT, ".git"))
    if inside_git_repo:
        try:
            run(["git", "submodule", "update", "--init", "--recursive", "--depth", "1"])
        except (subprocess.CalledProcessError, FileNotFoundError) as exc:
            print(f"git submodule update failed ({exc}); falling back to direct clones.")

    for name, url, tag in SUBMODULES:
        if dependency_present(name):
            continue
        target = os.path.join(DEPS, name)
        if os.path.isdir(target):
            shutil.rmtree(target)
        run(["git", "clone", "--depth", "1", "--branch", tag, url, target])


def fetch_premake():
    if os.path.isfile(PREMAKE_EXE):
        return
    os.makedirs(PREMAKE_DIR, exist_ok=True)
    archive = os.path.join(PREMAKE_DIR, "premake.zip")
    print(f"Downloading premake5 from {PREMAKE_URL}")
    urllib.request.urlretrieve(PREMAKE_URL, archive)
    with zipfile.ZipFile(archive) as zf:
        zf.extract("premake5.exe", PREMAKE_DIR)
    os.remove(archive)


def find_vswhere():
    candidates = [
        os.path.join(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
                     "Microsoft Visual Studio", "Installer", "vswhere.exe"),
        os.path.join(os.environ.get("ProgramFiles", r"C:\Program Files"),
                     "Microsoft Visual Studio", "Installer", "vswhere.exe"),
    ]
    for c in candidates:
        if os.path.isfile(c):
            return c
    return None


def detect_visual_studio():
    """Returns (premake_action, msbuild_path) for the newest installed Visual Studio."""
    vswhere = find_vswhere()
    if not vswhere:
        return "vs2022", None
    try:
        out = subprocess.check_output(
            [vswhere, "-latest", "-products", "*", "-requires", "Microsoft.Component.MSBuild",
             "-property", "installationPath"], text=True).strip()
        version = subprocess.check_output(
            [vswhere, "-latest", "-products", "*", "-requires", "Microsoft.Component.MSBuild",
             "-property", "installationVersion"], text=True).strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "vs2022", None
    major = int(version.split(".")[0]) if version else 17
    action = {16: "vs2019", 17: "vs2022"}.get(major, "vs2026" if major >= 18 else "vs2022")
    msbuild = os.path.join(out, "MSBuild", "Current", "Bin", "MSBuild.exe")
    return action, (msbuild if os.path.isfile(msbuild) else None)


def generate_projects(action):
    run([PREMAKE_EXE, action])


def build(msbuild, config):
    if not msbuild:
        print("MSBuild not found; open the generated solution in Visual Studio instead.")
        return 1
    solution = "Matrix.slnx" if os.path.isfile(os.path.join(ROOT, "Matrix.slnx")) else "Matrix.sln"
    result = run([msbuild, solution, f"-p:Configuration={config}", "-p:Platform=x64",
                  "-m", "-nologo", "-v:m"], check=False)
    return result.returncode


def main():
    parser = argparse.ArgumentParser(description="Set up and optionally build Matrix Engine.")
    parser.add_argument("--build", action="store_true", help="build after generating project files")
    parser.add_argument("--config", default="Debug", choices=["Debug", "Release"])
    parser.add_argument("--action", default=None, help="premake action override (vs2022, vs2026, ...)")
    parser.add_argument("--no-deps", action="store_true", help="skip dependency fetching")
    args = parser.parse_args()

    if platform.system() != "Windows":
        print("Matrix Engine currently targets Windows only.")
        return 1

    if not args.no_deps:
        fetch_dependencies()
    fetch_premake()

    action, msbuild = detect_visual_studio()
    if args.action:
        action = args.action
    generate_projects(action)

    if args.build:
        return build(msbuild, args.config)
    print("\nProject files generated. Open Matrix.slnx / Matrix.sln in Visual Studio, "
          "or run 'python scripts/setup.py --build'.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
