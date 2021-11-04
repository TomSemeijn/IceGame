#include "Functions.h"

namespace reb {

	//converts degrees to radians
	float getRadians(float degrees) {
		return degrees / (180 / PI);
	}

	//converts radians to degrees
	float getDegrees(float radians) {
		return radians * (180 / PI);
	}

	//snaps to window pixel space
	void windowSnap(sf::RenderTarget* window, Vector2& vec) {
		vec.x *= window->getSize().x;
		vec.y *= window->getSize().y;
		vec.x = std::ceilf(vec.x);
		vec.y = std::ceilf(vec.y);
		vec.x /= window->getSize().x;
		vec.y /= window->getSize().y;
	}

	//translates the given point by a rotation of the given amount of radians around the given origin
	void translateRotation(Vector2& point, const Vector2& origin, const float radians) {
		float newX = origin.x + (point.x - origin.x)*std::cosf(radians) - (point.y - origin.y)*std::sinf(radians);
		point.y = origin.y + (point.x - origin.x)*std::sinf(radians) + (point.y - origin.y)*std::cosf(radians);
		point.x = newX;
	}

	//erase all Occurrences of given substring from main string. from "https://thispointer.com/how-to-remove-substrings-from-a-string-in-c/"
	std::string eraseAllSubStr(std::string & mainStr, const std::string & toErase) {
		size_t pos = std::string::npos;
		// Search for the substring in string in a loop untill nothing is found
		while ((pos = mainStr.find(toErase)) != std::string::npos) {
			// If found then erase it from string
			mainStr.erase(pos, toErase.length());
		}
		return mainStr;
	}

	//returns the amount of occurrences the given substring has in the given string (https://stackoverflow.com/questions/22489073/counting-the-number-of-occurrences-of-a-string-within-a-string)
	int countSubstr(std::string str, std::string sub) {
		int occurrences = 0;
		std::string::size_type pos = 0;
		while ((pos = str.find(sub, pos)) != std::string::npos) {
			++occurrences;
			pos += sub.length();
		}
		return occurrences;
	}

	//splits the given string with the given delimiter (https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/)
	std::vector<std::string> split(const std::string& s, char delimiter) {
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter)){
			tokens.push_back(token);
		}
		return tokens;
	}

	//replaces all occurrences the given substring in the referenced string (https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c)
	void replaceSubstr(std::string& str, std::string substr, std::string replacement) {
		size_t index = 0;
		while (true) {
			/* Locate the substring to replace. */
			index = str.find(substr, index);
			if (index == std::string::npos) break;

			/* Make the replacement. */
			str.replace(index, substr.length(), replacement);

			/* Advance index forward so the next iteration doesn't pick it up as well. */
			index += replacement.length();
		}
	}

	//returns a boolean for if the given lines intersect (https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect)
	bool linesIntersect(Vector2 a, Vector2 b, Vector2 x, Vector2 y) {
		Vector2 r = b - a;
		Vector2 s = y - x;
		float denom = r.crossProd(s);
		if (denom == 0) { return false; }
		//q = x, p = a
		Vector2 temp = x - a;
		float u = temp.crossProd(r) / denom;
		if (u < 0 || u > 1) { return false; }
		float t = temp.crossProd(s) / denom;
		if (t < 0 || t > 1) { return false; }
		return true;
	}

	//returns a boolean for if the given lines intersect and sets the intersection point to the referenced vector (https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect)
	bool linesIntersectAt(Vector2 a, Vector2 b, Vector2 x, Vector2 y, Vector2& intersection) {
		Vector2 r = b - a;
		Vector2 s = y - x;
		float denom = r.crossProd(s);
		if (denom == 0) { return false; }
		//q = x, p = a
		Vector2 temp = x - a;
		float u = temp.crossProd(r) / denom;
		if (u < 0 || u > 1) { return false; }
		float t = temp.crossProd(s) / denom;
		if (t < 0 || t > 1) { return false; }
		intersection = a + r * t;
		return true;
	}

	// trim from start (in place) (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605)
	void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place) (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605)
	void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	// trim from both ends (in place) (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605)
	void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

	//seeds the random number generator
	std::random_device rd;
	std::mt19937 random_numbers(rd());

	//generates a random float
	float genRandf(float min, float max) {
		std::uniform_real_distribution<> dist(std::min(min, max), std::max(min, max));
		return dist(random_numbers);
	}

	//generates a random integer
	int genRandi(int min, int max) {
		std::uniform_int_distribution<> dist(std::min(min, max), std::max(min, max));
		return dist(random_numbers);
	}

	//turns a relative path string into a global path string
	std::string getGlobalPath(std::string relativePath, std::filesystem::path relativeTo) {
		//counts the amount of ..\ characters in the string
		int backAmount = countSubstr(relativePath, "..\\");

		//removes the ..\ characters
		relativePath = eraseAllSubStr(relativePath, "..\\");

		//goes back the amount of ..\ characters in the path the relative path is relative to
		for (int k = 0; k < backAmount; k++) {
			relativeTo = relativeTo.parent_path();
		}

		//adds the paths together
		return relativeTo.string() + "/" + relativePath;
	}

	//returns the distance from a point to a line segment (https://stackoverflow.com/questions/10983872/distance-from-a-point-to-a-polygon)
	float distancePointLine(Vector2 a, Vector2 b, Vector2 p) {
		Vector2 line = b - a;
		float lineLength = line.getLength();
		float r = line.dotProduct(p - a);
		r /= lineLength;
		float distance = 0;
		if (r < 0) {
			distance = (p - a).getLength();
		}
		else if (r > 1) {
			distance = (b - p).getLength();
		}
		else {
			distance = std::sqrtf(std::powf((p - a).getLength(), 2) - std::powf(r * lineLength, 2));
		}
		return distance;
	}

	//converts a directional vector to an angle value (https://stackoverflow.com/questions/6247153/angle-from-2d-unit-vector)
	float directionToAngle(Vector2 dir) {
		return std::atan2(dir.y, dir.x);
	}

	//converts an angle value to a directional vector
	Vector2 angleToDirection(float angle) {
		return Vector2(std::cosf(angle), std::sinf(angle));
	}

}