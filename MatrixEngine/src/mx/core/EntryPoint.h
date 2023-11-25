#pragma once

#ifdef _WIN64
	extern Matrix::core::Application* Matrix::core::CreateApplication();
	int main(int argc,char** argv)
	{
		auto launcher = Matrix::core::CreateApplication();
		delete launcher;
		return 0;
	}
#else
#error Engine is only supported by windows check rep for more info !
#endif