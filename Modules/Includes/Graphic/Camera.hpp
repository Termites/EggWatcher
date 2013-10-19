#ifndef _MODULE_SCENE_CAMERA_HPP_
#define _MODULE_SCENE_CAMERA_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Graphic
{
	class Camera
	{
		private:
			glm::vec3 Location;
			glm::vec3 LookAt;

			glm::mat4 Matrix;

		public:

			//-----------------------------------------
			// Getter / Setter
			void SetLocation( const glm::vec3& Location)
			{ this->Location = Location; }

			const glm::vec3& GetLocation() const
			{ return Location; }

			void SetLookAt( const glm::vec3& LookAt)
			{ this->LookAt = LookAt; }

			const glm::vec3& GetLookAt() const
			{ return LookAt; }

			const glm::mat4& GetMatrix() const
			{ return Matrix; }
			//-----------------------------------------

			void UpdateMatrix()
			{ Matrix = glm::lookAt(Location, LookAt, glm::vec3(0,0,1)); }

	};
}

#endif
