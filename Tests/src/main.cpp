#include "TestFramework.h"

/// Runs every registered test. Exit code is the number of failed tests (0 = success).
/// Usage: Tests.exe [filter]   - only runs tests whose "Suite.Name" contains the filter.
int main(int argc, char** argv)
{
    std::string filter = argc > 1 ? argv[1] : "";

    Matrix::util::FileSystem::Init();
    Matrix::core::Logging::Init();
    Matrix::core::Logging::GetCoreLogger()->set_level(spdlog::level::warn);
    Matrix::core::Logging::GetClientLogger()->set_level(spdlog::level::warn);
    Matrix::scripting::RegisterBuiltinScripts();

    int passed = 0, failed = 0, skipped = 0;
    for (const auto& test : tests::Registry())
    {
        std::string fullName = test.Suite + "." + test.Name;
        if (!filter.empty() && fullName.find(filter) == std::string::npos)
        {
            ++skipped;
            continue;
        }
        try
        {
            test.Body();
            std::printf("[ PASS ] %s\n", fullName.c_str());
            ++passed;
        }
        catch (const tests::Failure& failure)
        {
            std::printf("[ FAIL ] %s\n         %s\n", fullName.c_str(), failure.Message.c_str());
            ++failed;
        }
        catch (const std::exception& e)
        {
            std::printf("[ FAIL ] %s\n         exception: %s\n", fullName.c_str(), e.what());
            ++failed;
        }
    }

    std::printf("\n%d passed, %d failed, %d skipped\n", passed, failed, skipped);
    Matrix::core::Logging::Shutdown();
    return failed;
}
