#pragma once

#include "pch.h"
#include "Common.h"

#include "Shader.h"

namespace Matrix
{
    namespace graphics
    {
        class MATRIX_DLL ShaderFactory
        {
        public:
            static inline ShaderFactory& GetInstance() {
                static ShaderFactory instance;
                return instance;
            }

            void CreateShader(const std::string& shadername,std::string_view fragpath, std::string_view vertpath);

            Shader* GetShader(std::string_view shadername);

            //clean up the memory when no longer needed.
            void Cleanup();

        private:
            ShaderFactory() {}

            //Prevent copy/move constructor and assignment
            ShaderFactory(const ShaderFactory&) = delete;
            ShaderFactory(ShaderFactory&&) = delete;
            ShaderFactory& operator=(const ShaderFactory&) = delete;
            ShaderFactory& operator=(ShaderFactory&&) = delete;
        private:
            std::unordered_map<std::string,Shader*> m_Shaders;
        };
    }
}
