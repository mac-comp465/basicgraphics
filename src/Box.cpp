//
//  Box.cpp
//  
//
//  Created by Bret Jackson on 2/3/17.
//
//

#include "Box.h"

namespace basicgraphics {

	std::mutex Box::_mutex;

	Box::Box(const glm::vec3 &min, const glm::vec3 &max, const glm::vec4 &color) : _min(min), _max(max), _color(color)
	{
		_model = getModelInstance();

		glm::vec3 extents = (max - min) / 2.0f;
		glm::vec3 center = min + extents;
		glm::mat4 scale = glm::scale(glm::mat4(1.0), extents);

		glm::mat4 trans = glm::translate(glm::mat4(1.0), center);

		_localMat = trans * scale;


	}

	Box::~Box() {

	}

	std::shared_ptr<Model> Box::getModelInstance() {
		// lock mutex before accessing file
		std::lock_guard<std::mutex> lock(_mutex);
		static thread_local std::shared_ptr<Model> model(new Model("cube.obj", 1.0, glm::vec4(1.0)));
		return model;
	}

	void Box::setColor(const glm::vec4 &color)
	{
		_color = color;
	}

	void Box::draw(GLSLProgram &shader, const glm::mat4 &modelMatrix) {

		glm::mat4 model = modelMatrix * _localMat;
		shader.setUniform("model_mat", model);
		shader.setUniform("normal_mat", mat3(transpose(inverse(model))));
		_model->setMaterialColor(_color);
		_model->draw(shader);
		shader.setUniform("model_mat", modelMatrix);
		shader.setUniform("normal_mat", mat3(transpose(inverse(modelMatrix))));
	}

	bool Box::contains(const glm::vec3 &point) const
	{
		return  (point.x >= _min.x) &&
			(point.y >= _min.y) &&
			(point.z >= _min.z) &&
			(point.x <= _max.x) &&
			(point.y <= _max.y) &&
			(point.z <= _max.z);
	}

}