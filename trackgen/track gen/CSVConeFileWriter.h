#pragma once

#include "IFileWriter.h"

#include <string>
#include <vector>
#include <fstream>

class CSVConeFileWriter : public IFileWriter
{
	public:
		void open(std::string file_name) override;
		void close() override;
		void write(Cone cone);
		void writeMultiple(std::vector < Cone > data);
	protected:
		void write(void* data) override;
		void writeMultiple(std::vector < void* > data) override;
		std::fstream csv_file;
};