#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include "utils.h"
#include "map/state.hpp"
#include "map/country.hpp"

inline std::string trim(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

inline std::unordered_map<int, std::string> parse_state_owner_csv(const std::string& csv)
{
    std::unordered_map<int, std::string> state_owners;
    std::vector<std::string> lines = split(csv, '\n');

    if (lines.empty()) return state_owners;

    // Remove header
    lines.erase(lines.begin());

    for (const std::string& raw_line : lines)
    {
        std::string line = trim(raw_line);
        if (line.empty()) continue;

        std::vector<std::string> parts = split(line, ';');

        if (parts.size() >= 2)
        {
            std::string id_str = trim(parts.at(0));
            std::string country_id = trim(parts.at(1));

            if (!id_str.empty() && std::all_of(id_str.begin(), id_str.end(), ::isdigit))
            {
                int state_id = std::stoi(id_str);
                state_owners[state_id] = country_id;
            }
        }
    }

    return state_owners;
}

inline std::vector<State> parse_state_csv(const std::string& csv, const std::string& state_owners_csv)
{
    std::vector<State> states;
    std::vector<std::string> lines = split(csv, '\n');

    std::unordered_map<int, std::string> state_owners = parse_state_owner_csv(state_owners_csv);

    if (lines.empty()) return states;

    // Remove header
    lines.erase(lines.begin());

    for (const std::string& raw_line : lines)
    {
        std::string line = trim(raw_line);
        if (line.empty()) continue;

        std::vector<std::string> parts = split(line, ';');

        if (parts.size() >= 3)
        {
            std::string id_str = trim(parts.at(0));
            std::string state_name = trim(parts.at(1));
            std::string original_owner = trim(parts.at(2));

            if (!id_str.empty() && std::all_of(id_str.begin(), id_str.end(), ::isdigit))
            {
                int state_id = std::stoi(id_str);
                std::string current_owner = "";

                auto it = state_owners.find(state_id);
                if (it != state_owners.end())
                {
                    current_owner = state_owners[state_id];
                }
				else
				{
					current_owner = "No owner";
				}

				State state = State(state_id, state_name, original_owner, current_owner);

                states.emplace_back(state);
            }
        }
    }

    return states;
}

inline std::vector<Country> parse_country_csv(const std::string& csv)
{
    std::vector<Country> countries;
    std::vector<std::string> lines = split(csv, '\n');

    if (lines.empty()) return countries;

    // Remove header
    lines.erase(lines.begin());

    for (const std::string& raw_line : lines)
    {
        std::string line = trim(raw_line);
        if (line.empty()) continue;

        std::vector<std::string> parts = split(line, ';');

        if (parts.size() >= 10)
        {
            std::string country_id = trim(parts.at(0));
            u32 color = hex_to_u32(trim(parts.at(9)).c_str());

            Country country(country_id, color);
            country.setNames(
                trim(parts.at(1)),
                trim(parts.at(2)),
                trim(parts.at(3)),
                trim(parts.at(4)),
                trim(parts.at(5)),
                trim(parts.at(6)),
                trim(parts.at(7))
            );

            countries.emplace_back(country);
        }
    }

    return countries;
}