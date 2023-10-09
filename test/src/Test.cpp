#include "src/Matrix.h"

class EngineCoreTester : public Matrix::core::Application
{
public:
	EngineCoreTester() {}
	
	~EngineCoreTester() {}
};

Matrix::core::Application* Matrix::core::CreateApplication()
{
	return new EngineCoreTester();
}