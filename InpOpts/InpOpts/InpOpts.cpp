#include "InpOpts.hpp"

using namespace std;

namespace inpopts{
	string Version(){
		return to_string(STURDY_ROBOT_VERSION_MAJOR) + "." + to_string(STURDY_ROBOT_VERSION_MINOR);
	}
}