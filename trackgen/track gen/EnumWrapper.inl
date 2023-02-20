#pragma once

#include <string>

#include "Spline.h"

template < typename T >
T toEnum(std::string name)
{
	return T(0);
}

template <>
AproximationAlgorithm toEnum(std::string name)
{
	if (name == "SegmentSplit") return AproximationAlgorithm::LineSplit;
	if (name == "VectorLenghtQubed") return AproximationAlgorithm::QubedVectorLenght;
	return AproximationAlgorithm::UnDefined;
}

template <>
SegmentingAlgorithm toEnum(std::string name)
{
	if (name == "LookAhead") return SegmentingAlgorithm::LookAhead;
	if (name == "Tree") return SegmentingAlgorithm::Tree;
	if (name == "Constant") return SegmentingAlgorithm::Constant;
	return SegmentingAlgorithm::UnDefined;
}

//template < enum class T >
//std::string toString(T en)
//{
//	return "";
//}



