#pragma once

#include <vector>
#include <string>
#include <sstream>

// Modified from vec3 implementation of https://raytracing.github.io/books/RayTracingInOneWeekend.html
class vec2 
{
	public:
		union 
		{
			struct { double x, y; };
			double e[2];
		};

		vec2() : e{0,0} {}
		vec2(double e0, double e1) : e{e0, e1} {}

		/*double x() const { return e[0]; }
		double y() const { return e[1]; }*/

		vec2 operator-() const { return vec2(-e[0], -e[1]); }
		double operator[](int i) const { return e[i]; }
		double& operator[](int i) { return e[i]; }

		vec2& operator+=(const vec2& v) 
		{
			e[0] += v.e[0];
			e[1] += v.e[1];
			return *this;
		}

		vec2& operator*=(double t) 
		{
			e[0] *= t;
			e[1] *= t;
			return *this;
		}

		vec2& operator/=(double t) 
		{
			return *this *= 1/t;
		}

		// return e[0] on vec2.x and e[1] on vec2.y
};

std::vector<std::string> split(std::string str, char delimiter)
{
    std::stringstream ss(str);
    std::vector<std::string> res;
    std::string token;
    while (std::getline(ss, token, delimiter)) 
	{
        res.push_back(token);
    }
    return res;
}