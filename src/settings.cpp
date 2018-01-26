#include "settings.hpp"
#include "utils.hpp"

settings::settings(int argc, char* argv[])
{
	if (argc > 1) {
		m_floors_count = utils::value_or<int>(argv[1], default_floors_count);
	}

	if (argc > 2) {
		m_floor_height = utils::value_or<float>(argv[2], default_floor_height);
	}

	if (argc > 3) {
		m_speed = utils::value_or<float>(argv[3], default_speed);
	}

	if (argc > 4) {
		m_open_time = utils::value_or<float>(argv[4], default_open_time);
	}

}