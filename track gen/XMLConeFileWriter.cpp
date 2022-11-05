#include "XMLConeFileWriter.h"

#include <fstream>

using fw = XMLConeFileWriter;

void fw::open(std::string file_name)
{
	this->file_name = file_name;
}

void fw::close()
{	
	std::fstream xml_file;

	xml_file.open(file_name, std::ios::out);

	xml_file << "<Cones>\n";

	xml_file << "\t<Blue>\n";
	for (auto cone : left_cones)
	{
		xml_file << "\t\t<Cone x = \"" << std::to_string(cone.pos.x) << "\" y = \"" << std::to_string(cone.pos.y) << "\"/>\n";
	}
	xml_file << "\t<Right>\n";
	xml_file << "\t<Yellow>\n";
	for (auto cone : right_cones)
	{
		xml_file << "\t\t<Cone x = \"" << std::to_string(cone.pos.x) << "\" y = \"" << std::to_string(cone.pos.y) << "\"/>\n";
	}
	xml_file << "\t<Left>\n";

	xml_file << "</Cones>";

	xml_file.close();
}

void fw::write(Cone data)
{
	switch (data.type)
	{
		case Type::Right:
			right_cones.push_back(data);
			break;
		case Type::Left:
			left_cones.push_back(data);
			break;
	}
}

void fw::writeMultiple(std::vector < Cone > data)
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