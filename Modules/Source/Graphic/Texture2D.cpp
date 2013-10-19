#include <Graphic/Texture2D.hpp>

#include <fstream>
#include <png.h>

Graphic::Texture2D::Texture2D()
{
	glGenTextures(1,&TextureID);
}

//---------------------------------------------------------------------------------------
// PNG-File helper function
static void ReadPNGData(png_structp PNGPtr, png_bytep Data, png_size_t Length)
{
    png_voidp IOPtr = png_get_io_ptr(PNGPtr);

    std::ifstream * File = reinterpret_cast<std::ifstream*>(IOPtr);

    File->read(reinterpret_cast<char*>(Data),Length);

}
//---------------------------------------------------------------------------------------

bool Graphic::Texture2D::LoadFromFile(const std::string& File)
{
    std::ifstream Input(File.c_str(),std::ios::binary);
    if (!Input.is_open())
    {
        return false;
    }
    png_byte Sig[8];

    Input.read((char*)Sig,8);

    bool bPNG = !png_sig_cmp(Sig,0,8);


    if (bPNG==0)
    {
        //DebugErrorMessage("ResourceManager::LoadPNGFile\n\nUnable to load file '"+File+"'.\nNot a valid PNG file.",Error_Warning);
        return false;
    }

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);

    if (!pngPtr)
    {
        //DebugErrorMessage("ResourceManager::LoadPNGFile\n\nUnable to load file '"+File+"'.\nUnable to create PNG read structure.",Error_Warning);
        return false;
    }

    png_infop pngInfoPtr = png_create_info_struct(pngPtr);

    if (!pngInfoPtr)
    {
        //DebugErrorMessage("ResourceManager::LoadPNGFile\n\nUnable to load file '"+File+"'.\nUnable to create PNG read structure.",Error_Warning);
        png_destroy_read_struct(&pngPtr,(png_infopp)0,(png_infopp)0);
        return false;
    }

    png_bytep* rowPtr = NULL;
    uint8_t* Data=NULL;

    if (setjmp(png_jmpbuf(pngPtr)))
    {
        png_destroy_read_struct(&pngPtr,&pngInfoPtr,(png_infopp)0);
        if (rowPtr)
            delete [] rowPtr;
        if (Data)
            delete [] Data;

        //DebugErrorMessage("ResourceManager::LoadPNGFile\n\nUnable to load file '"+File+"'.\nAn error has occured during loading.",Error_Warning);
        return false;

    }

    png_set_read_fn(pngPtr,reinterpret_cast<png_voidp>(&Input),ReadPNGData);

    png_set_sig_bytes(pngPtr,8);

    png_read_info(pngPtr,pngInfoPtr);

    Width = png_get_image_width(pngPtr, pngInfoPtr);
    Height = png_get_image_height(pngPtr, pngInfoPtr);
    png_uint_32 BitDepth = png_get_bit_depth(pngPtr,pngInfoPtr);
    png_uint_32 Channels = png_get_channels(pngPtr,pngInfoPtr);
    png_uint_32 ColorType = png_get_color_type(pngPtr,pngInfoPtr);

    switch (ColorType)
    {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(pngPtr);
            Channels = 3;
        break;

        case PNG_COLOR_TYPE_GRAY:
            // Convert to 8 bpp :
            if (BitDepth<8)
                png_set_expand_gray_1_2_4_to_8(pngPtr);
            BitDepth=8;
        break;
    }

    if (png_get_valid(pngPtr,pngInfoPtr, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(pngPtr);
        ++Channels;
    }

    if (BitDepth == 16)
        png_set_strip_16(pngPtr);

    rowPtr = new png_bytep[Height];

    Data = new uint8_t[Width * Height * (BitDepth * Channels)/8];
    const size_t Stride = Width * (BitDepth * Channels)/8;

    for (int i=0;i<Height;++i)
    {
        png_uint_32 Offset =  i * Stride;
        rowPtr[i] = reinterpret_cast<png_bytep> (Data) + Offset;
    }

    png_read_image(pngPtr,rowPtr);


    GLenum Component = GL_RGBA;
    GLenum Format=GL_RGB8UI;
    switch (Channels)
    {
        case 1:
            Format = GL_RED;
            Component = GL_RED;
        break;

        case 2:
            Format = GL_RG;
            Component = GL_RG;
        break;

        case 3:
            Format = GL_RGB;
            Component = GL_RGB;
        break;

        case 4:
            Format = GL_RGBA;
            Component = GL_RGBA;
        break;
    }

    glBindTexture(GL_TEXTURE_2D,TextureID);



    //Format = GL_RGBA;
    //Component = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D,0,Format,Width,Height,0,Component,GL_UNSIGNED_BYTE,Data);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D,0);

    delete [] rowPtr;
    png_destroy_read_struct(&pngPtr,&pngInfoPtr,(png_infopp)0);

    delete [] Data;

    return true;
}

Graphic::Texture2D::~Texture2D()
{
	glDeleteTextures(1, &TextureID);
}
