//
//  Cone.hpp
//  
//
//  Created by Bret Jackson on 2/3/17.
//
//

#ifndef Cone_hpp
#define Cone_hpp

#include <stdio.h>
#include "Model.h"
#include "GLSLProgram.h"
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace basicgraphics {

	class Cone {
	public:
		/*!
		 * Creates a cone with the base at point1 and the point at point2.
		 */
		Cone(const glm::vec3 &point1, const glm::vec3 &point2, const float radius, const glm::vec4 &color);
		virtual ~Cone();

		virtual void draw(GLSLProgram &shader, const glm::mat4 &modelMatrix);

	protected:
		std::shared_ptr<Model> _model;
		const glm::vec3 _point1;
		const glm::vec3 _point2;
		glm::mat4 _localMat;
		const float _radius;
		const glm::vec4 _color;

		std::shared_ptr<Model> getModelInstance();
	};

}

#endif /* Cone_hpp */
