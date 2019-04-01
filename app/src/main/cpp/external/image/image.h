#include <string>
#include <png.h>

class PNG
{
public:
	PNG(const std::string& file_name);
	~PNG();
	unsigned int get_width();	
	unsigned int get_height();	
	bool has_alpha();
	unsigned char* get_data();

private:
	const std::string file_name_;
	unsigned char* data_;
	png_uint_32 width_, height_;
	int bit_depth_, color_type_, interlace_type_;
	int compression_type_, filter_method_;
};

