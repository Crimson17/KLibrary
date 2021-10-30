#pragma once


namespace kl {
	class math {
	public:
		// Returns a dot product of 2 given vectors
		static double VectorDotProd(vec2 a, vec2 b) {
			return a.x * b.x + a.y * b.y;
		}
		static double VectorDotProd(vec3 a, vec3 b) {
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		// Returns a cross product of 2 given vectors
		static vec3 VectorCrossProd(vec3 a, vec3 b) {
			return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		}

		// Checks if the given number is prime
		static bool IsPrime(uint64 num) {
			if (num == 0 || num == 1) {
				return false;
			}

			uint64 half = num / 2;
			for (uint64 i = 2; i <= half; i++) {
				if (num % i == 0) {
					return false;
				}
			}
			return true;
		}
	};
}