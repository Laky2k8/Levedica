#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include "utils.h"
#include "map/state.hpp"

std::vector<State> parse_state_csv(std::string csv)
{
	std::vector<State> states;
	std::vector<std::string> lines = split(csv, '\n');

	// Remove header
	lines.erase(lines.begin());

	for(std::string line : lines)
	{
		std::vector<std::string> parts = split(line, ';');

		if(parts.size() < 0 || parts.size() > 3)
		{
			throw std::runtime_error("Incorrect CSV line: " + line);
		}
		else
		{
			try
			{
				int state_id = stoi(parts.at(0));
				std::string state_name = parts.at(1);
				std::string original_owner = parts.at(2);

				State state = State(state_id, state_name, original_owner);
				states.emplace_back(state);
			}
			catch(const std::exception& e)
			{
				throw std::runtime_error("Incorrect CSV line: " + line + ", error: " + e.what());
			}
		}
	}

	return states;
}