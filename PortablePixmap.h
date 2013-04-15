
struct RGBTupel
{
	RGBTupel (unsigned char r, unsigned char g, unsigned char b)
	: R(r), G(g), B(b)
	{}

	unsigned char R, G, B;
}

class PortablePixmap 
{
public:
    PortablePixmap(unsigned int width, unsigned int height);
    virtual ~PortablePixmap();




private:
    unsigned int* mData;
	unsigned char mWidth;
	unsigned char mHeight;
};
