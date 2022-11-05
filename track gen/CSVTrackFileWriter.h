#pragma once

#include "IFileWriter.h"
#include "Spline.h"

#include <fstream>
#include <vector>

class CSVTrackFileWriter : public IFileWriter
{
	public:
		void open(std::string file_name) override;
		void close() override;
		void write(Spline spline);
		//void writeMultiple(std::vector < Cone > data);
	protected:
		void write(void* data) override;
		void writeMultiple(std::vector < void* > data) override;
		std::fstream csv_file;
};