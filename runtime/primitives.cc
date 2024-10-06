#include "primitives.h"

namespace primitives {
	namespace plane {
		float vertices[] = {
			1, 0, 1,
			1, 0, 0,
			0, 0, 1,
			0, 0, 0
		}; 

		unsigned int indices[] = {
			0, 1, 3,
			0, 3, 2
		};
	}

	namespace quad {
		float vertices[] = {
			1, 1, 0.5,
			1, 0, 0.5,
			0, 1, 0.5,
			0, 0, 0.5, // square 1
	
			0.5, 1, 1,
			0.5, 1, 0,
			0.5, 0, 1,
			0.5, 0, 0 // square 2
		};

		unsigned int indices[] = {
			3, 1, 0,
			2, 3, 0,

			7, 5, 4,
			6, 7, 4
		};
	}
}
