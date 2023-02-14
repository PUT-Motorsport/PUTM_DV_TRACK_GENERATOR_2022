#include "ISplineGenerator.h"
#include "Config.h"

#include <string>

using isg = ISplineGenerator;

isg::ISplineGenerator(GeneratorType type) : type(type)
{
	//max_gradient = float(atof(Config::get("SplineMaxGradient").c_str()));
}