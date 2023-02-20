#include "CSVConeFileWriter.h"

#include <fstream>

using fw = CSVConeFileWriter;

void fw::open(std::string file_name)
{
	csv_file.open(file_name, std::ios::out);
	csv_file << "x;y;type\n";
}

void fw::close()
{
	csv_file.close();
}

void fw::write(Cone data)
{
	std::string type = data.type == Type::Right ? "yellow" : "blue";
	csv_file << data.pos.x << ";" << data.pos.y << ";" << type << "\n";
}

void fw::writeMultiple(std::vector<Cone> data)
{
	for (int i = 0; i < data.size(); i++) write(data[i]);
}

void fw::write(void* data)
{
	write(*(Cone*)data);
}

void fw::writeMultiple(std::vector<void*> data)
{
	for (auto d : data) write(d);
}