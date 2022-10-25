#pragma once

#include <string>

#include "Spline.h"

class EnumWrapper
{
	template < T >
	static T toEnum(std::string name)
	{
		return T(0);
	}

	template <>
	AproximationAlgorithm toEnum(std::string name)
	{
		if (name == "LineSplit") return AproximationAlgorithm::LineSplit;
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

	template < enum class T >
	static std::string toString(T en)
	{
		return "";
	}
};



