#pragma once

#include <string>
#include <vector>

#include "Cone.h"

class IFileWriter
{
	public:

		virtual void open(std::string file_name) = 0;
		//virtual void write(std::string content) = 0;
		virtual void close() = 0;
		virtual void write(void* data) = 0;
		virtual void writeMultiple(std::vector < void* > data) = 0;
		virtual ~IFileWriter() = default;
};