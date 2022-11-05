#include "CSVTrackFileWriter.h"

#include <fstream>

using fw = CSVTrackFileWriter;

void fw::open(std::string file_name)
{
	csv_file.open(file_name, std::ios::out);
	csv_file << "x;y;index\n";
}

void fw::close()
{
	csv_file.close();
}

void fw::write(Spline data)
{
	//std::string type = data.type == Type::Right ? "yellow" : "blue";
	int i = 0;
	for (auto idk : data.getPointRepresenation())
	{
		csv_file << idk.position.x << ";" << idk.position.y << ";" << i << "\n";
		i++;
	}
}

//void fw::writeMultiple(std::vector<Cone> data)
//{
//	for (int i = 0; i < data.size(); i++) write(data[i]);
//}

void fw::write(void* data)
{
	write(*(Spline*)data);
}

void fw::writeMultiple(std::vector<void*> data)
{
	for (auto d : data) write(d);
}