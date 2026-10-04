#pragma once

#include <string>
#include "country.hpp"

class State
{
	private:
		int state_id;
		std::string name;
		std::string original_country_code;
		
	public:
		std::string owner_country;

		State(int state_id, std::string name, std::string original_country_code, std::string owner_country)
		{
			this->state_id = state_id;
			this->name = name;
			this->original_country_code = original_country_code;
			this->owner_country = owner_country;
		}

		State()
		{
			this->state_id = -1;
			this->name = "";
			this->original_country_code = "";
			this->owner_country = "";
		}

		int getID() const
		{
			return this->state_id;
		}

		std::string getName() const
		{
			return this->name;
		}

		std::string getCountryCode()
		{
			return this->original_country_code;
		}
};

State* getStateByID(std::vector<State>& states, int state_id)
{
	for (State& state : states)
	{
		if (state.getID() == state_id)
		{
			return &state;
		}
	}
	return nullptr; // not found
}