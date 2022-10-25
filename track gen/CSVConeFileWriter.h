#pragma once

#include "IFileWriter.h"
#include "Cone.h"

#include <string>
#include <vector>
#include <fstream>

class CSVConeFileWriter : public IFileWriter
{
	public:
		void open(std::string file_name) override;
		void close() override;
		void write(Cone* data);
		void writeMultiple(std::vector < Cone* > data);
	protected:
		std::fstream csv_file;

		void write(void* data) override;
		void writeMultiple(std::vector < void* > data) override;
};