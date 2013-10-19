#include <Graphic/FramebufferObject.hpp>

#include <iostream>

Graphic::FramebufferObject::FramebufferObject(GLsizei Width, GLsizei Height, size_t AttachmentsCount,FBOFlags Flags)
{
    GLenum InternalAttachmentFormat;
    GLenum AttachmentFormat;
    GLenum AttachmentType;

    bool bFloatingPoint = Flags & FBO_Float;
    bool bDepth = Flags & FBO_Depth;
    bool bDepthTexture = Flags & FBO_DepthTexture;

    DepthbufferID = 0;
    DepthbufferTextureID = 0;
    this->Width = Width;
    this->Height = Height;

    AttachmentType = bFloatingPoint ? GL_FLOAT : GL_UNSIGNED_BYTE;

	//--------------------------------------------------------
    // Get internal format & attachment format
    if ( (Flags&FBO_Red) && (Flags&FBO_Green) && (Flags&FBO_Blue) && (Flags&FBO_Alpha))
    {
        AttachmentFormat = GL_RGB;
        InternalAttachmentFormat = bFloatingPoint ? GL_RGBA32F : GL_RGBA;
    }
    else if ( (Flags&FBO_Red) && (Flags&FBO_Green) && (Flags&FBO_Blue))
    {
        AttachmentFormat = GL_RGB;
        InternalAttachmentFormat = bFloatingPoint ? GL_RGB32F : GL_RGB;
    }
    else if ( (Flags&FBO_Red) && (Flags&FBO_Green))
    {
        AttachmentFormat = GL_RG;
        InternalAttachmentFormat = bFloatingPoint ? GL_RG32F : GL_RG;
    }
    else if ( (Flags&FBO_Red))
    {
        AttachmentFormat = GL_RED;
        InternalAttachmentFormat = bFloatingPoint ? GL_R32F : GL_RED;
    }
	//--------------------------------------------------------

	//--------------------------------------------------------
    // Generate textures
    if (AttachmentsCount > 0)
    {
        ColorAttachments.resize(AttachmentsCount);
        glGenTextures(AttachmentsCount,ColorAttachments.data());
    }
	//--------------------------------------------------------

	//--------------------------------------------------------
    // Generate depth texture
    if (bDepth && bDepthTexture)
    {
        glGenTextures(1,&DepthbufferTextureID);
        glBindTexture(GL_TEXTURE_2D,DepthbufferTextureID);
        glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,Width,Height,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,NULL);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }
    else
        DepthbufferTextureID = 0;
	//--------------------------------------------------------

	//--------------------------------------------------------
    // Generate attachment textures
    for (size_t i=0;i<AttachmentsCount;++i)
    {
        glBindTexture(GL_TEXTURE_2D, ColorAttachments[i]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,InternalAttachmentFormat,Width,Height,0,AttachmentFormat,AttachmentType,NULL);
    }

    glBindTexture(GL_TEXTURE_2D,0);
	//--------------------------------------------------------

    if (bDepth)
    {
        glGenRenderbuffers(1,&DepthbufferID);
        glBindRenderbuffer(GL_RENDERBUFFER,DepthbufferID);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,Width,Height);
        glBindRenderbuffer(GL_RENDERBUFFER,0);
    }

    glGenFramebuffers(1,&FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER,FramebufferID);

    if (AttachmentsCount == 0)
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    for (size_t i=0;i<AttachmentsCount;++i)
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColorAttachments[i],0);

    if (bDepth)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthbufferID);

        if (bDepthTexture)
            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthbufferTextureID,0);
    }

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		// Todo : Error
        std::cout<<"Error occured while creating Graphic::FramebufferObject"<<std::endl;
	}

    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Graphic::FramebufferObject::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER,FramebufferID);

    size_t BuffersCount = ColorAttachments.size();

    std::vector<GLenum> Buffers(BuffersCount);
    for (size_t i=0;i<BuffersCount;++i)
        Buffers[i] = GL_COLOR_ATTACHMENT0 + i;

    if (BuffersCount >0)
        glDrawBuffers(BuffersCount,Buffers.data());
    else
        glDrawBuffer(GL_NONE);

    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0,0,Width,Height);
}

GLuint Graphic::FramebufferObject::GetTexture(size_t ID) const
{
    if (ID == (size_t)-1)
        return DepthbufferTextureID;

    return ColorAttachments[ID];
}

void Graphic::FramebufferObject::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glPopAttrib();
}

Graphic::FramebufferObject::~FramebufferObject()
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	if (ColorAttachments.size() > 0)
		glDeleteTextures(ColorAttachments.size(),ColorAttachments.data());

	if (DepthbufferTextureID != 0)
		glDeleteTextures(1,&DepthbufferTextureID);

	ColorAttachments.clear();

	if (DepthbufferID !=0)
		glDeleteRenderbuffers(1,&DepthbufferID);

	glDeleteFramebuffers(1,&FramebufferID);
}
