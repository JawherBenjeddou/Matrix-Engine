#pragma once

#ifdef MX_PLATFORM_WINDOWS
	extern Matrix::core::Application* Matrix::core::CreateApplication();
	int main(int argc,char** argv)
	{
		auto launcher = Matrix::core::CreateApplication();
		delete launcher;
		return 0;
	}
#endif