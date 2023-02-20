#pragma once

#include "IFileWriter.h"

#include <string>
#include <vector>

class XMLConeFileWriter : public IFileWriter
{
	public:
		void open(std::string file_name) override;
		void close() override;
		void write(Cone data);
		void writeMultiple(std::vector < Cone > data);
	protected:
		void write(void* data) override;
		void writeMultiple(std::vector < void* > data) override;
		std::string file_name;
		
		std::vector < Cone > left_cones;
		std::vector < Cone > right_cones;
};