#include "src/Matrix.h"

class EngineCoreTester : public Matrix::core::Application
{

};

Matrix::core::Application* Matrix::core::CreateApplication()
{
	return new EngineCoreTester();
}