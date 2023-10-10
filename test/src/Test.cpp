#include "src/Matrix.h"

class EngineCoreTester : public Matrix::core::Application
{
public:

	//Config struct takes {Title,width,height} and a default struct exists
	EngineCoreTester(Matrix::ui::WindowSpec config) : Application(config) {}

	EngineCoreTester() {}
	
	~EngineCoreTester() {}
};

Matrix::core::Application* Matrix::core::CreateApplication()
{
	return new EngineCoreTester();
}