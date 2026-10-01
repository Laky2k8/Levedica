#include <string>

class State
{
	private:
		int state_id;
		std::string name;
		std::string original_country_code;

	public:
		State(int state_id, std::string name, std::string original_country_code)
		{
			this->state_id = state_id;
			this->name = name;
			this->original_country_code = original_country_code;
		}

		int getID()
		{
			return this->state_id;
		}

		std::string getName()
		{
			return this->name;
		}

		std::string getCountryCode()
		{
			return this->original_country_code;
		}
};