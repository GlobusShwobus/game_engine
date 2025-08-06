#pragma once

namespace badEngine{

	//state machine system
	namespace SMS {

		enum class Facing {
			UNKNOWN,
			LEFT,
			RIGHT
		};
		struct State {
			bool isMoving     = false;
			Facing facing     = Facing::UNKNOWN;
		};

	}

}