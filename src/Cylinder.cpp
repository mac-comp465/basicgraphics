//
//  Cylinder.cpp
//  
//
//  Created by Bret Jackson on 2/3/17.
//
//

#include "Cylinder.h"

namespace basicgraphics {

	Cylinder::Cylinder(const glm::vec3 &point1, const glm::vec3 &point2, const float radius, const glm::vec4 &color) : _point1(point1), _point2(point2), _radius(radius), _color(color)
	{
		_model = getModelInstance();

		glm::vec3 direction = point2 - point1;
		float length = glm::length(direction);
		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(radius, length, radius));

		//Calc rotation matrix
		glm::vec3 start = glm::normalize(glm::vec3(0,1,0));
		glm::vec3 dest = glm::normalize(direction);

		float cosTheta = glm::dot(start, dest);
		glm::vec3 rotationAxis;
		glm::mat4 rotate(1.0);
		if (cosTheta < -1 + 0.001f) {
			// special case when vectors in opposite directions:
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
			if (glm::length(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
				rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

			rotationAxis = glm::normalize(rotationAxis);
			rotate = glm::toMat4(glm::angleAxis(glm::radians(180.0f), rotationAxis));
		}
		else {
			rotationAxis = glm::cross(start, dest);

			float s = sqrt((1 + cosTheta) * 2);
			float invs = 1 / s;

			rotate = glm::toMat4(glm::quat(
				s * 0.5f,
				rotationAxis.x * invs,
				rotationAxis.y * invs,
				rotationAxis.z * invs
			));
		}


		glm::mat4 trans = glm::translate(glm::mat4(1.0), point1+0.5f*direction);

		_localMat = trans * rotate * scale;


	}

	Cylinder::~Cylinder() {

	}

	std::shared_ptr<Model> Cylinder::getModelInstance() {
		static std::shared_ptr<Model> model(new Model("cylinder.obj", 1.0, glm::vec4(1.0)));
		return model;
	}

	void Cylinder::draw(GLSLProgram &shader, const glm::mat4 &modelMatrix) {

		glm::mat4 model = modelMatrix * _localMat;
		shader.setUniform("model_mat", model);
		shader.setUniform("normal_mat", mat3(transpose(inverse(model))));
		_model->setMaterialColor(_color);
		_model->draw(shader);
		shader.setUniform("model_mat", modelMatrix);
		shader.setUniform("normal_mat", mat3(transpose(inverse(modelMatrix))));
	}

}