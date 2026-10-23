#pragma once
///@file EntryPoint.h  Defines main() for applications built on the engine. Include it exactly once.

#include "Application.h"
#include "Logging.h"

#ifdef _WIN64

extern Matrix::core::Application* Matrix::core::CreateApplication();

int main(int argc, char** argv)
{
    Matrix::core::Application::SetCommandLineArgs(argc, argv);

    int exitCode = 1;
    try
    {
        Matrix::core::Application* application = Matrix::core::CreateApplication();
        if (!application)
        {
            MX_CORE_CRITICAL("CreateApplication() returned nullptr");
        }
        else
        {
            exitCode = application->Run();
            delete application;
        }
    }
    catch (const std::exception& e)
    {
        MX_CORE_CRITICAL("Unhandled exception: {}", e.what());
        exitCode = 3;
    }

    Matrix::core::Logging::Shutdown();
    return exitCode;
}

#else
    #error Matrix Engine currently supports 64-bit Windows only.
#endif
