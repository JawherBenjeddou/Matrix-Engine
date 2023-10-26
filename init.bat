@echo off
rem Create a "Dependencies" directory if it doesn't exist
mkdir Dependencies

rem Set the directory where dependencies will be installed
set "install_dir=Dependencies"

rem Clone or update ImGui
if not exist "%install_dir%\imgui" (
    git clone https://github.com/ocornut/imgui.git "%install_dir%\imgui"
) else (
    cd "%install_dir%\imgui"
    git pull
)

rem Clone or update spdlog
if not exist "%install_dir%\spdlog" (
    git clone https://github.com/gabime/spdlog.git "%install_dir%\spdlog"
) else (
    cd "%install_dir%\spdlog"
    git pull
)

rem Clone or update stb
if not exist "%install_dir%\stb" (
    git clone https://github.com/nothings/stb.git "%install_dir%\stb"
) else (
    cd "%install_dir%\stb"
    git pull
)

rem Clone or update GLM (inside the "GLM" folder)
if not exist "%install_dir%\GLM" (
    git clone https://github.com/g-truc/glm.git "%install_dir%\GLM"
) else (
    cd "%install_dir%\GLM"
    git pull
)

echo Dependencies have been successfully installed in %install_dir%
