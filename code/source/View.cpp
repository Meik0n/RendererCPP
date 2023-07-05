
// Este código es de dominio público.
// angel.rodriguez@esne.edu
// 2013.12 - 2021.04

#include <cassert>
#include <cmath>
#include <math.hpp>
#include <View.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace example
{

	View::View(unsigned width, unsigned height, Point3f light_vector)
		:
		width(width),
		height(height),
		number_of_models(0),
		light(light_vector)
	{
	}
	void View::add_model(std::shared_ptr<Model> model_name)
	{
		models.push_back(model_name);
		number_of_models++;
	}
}

