#include "ISplineGenerator.h"
#include "Config.h"

#include <string>

using isg = ISplineGenerator;

isg::ISplineGenerator()
{
	//max_gradient = float(atof(Config::get("SplineMaxGradient").c_str()));
}