#include <Graphic/Shader.hpp>
#include <iostream>
#include <fstream>

static std::string LoadFileContent(const std::string& File)
{
    std::string Content;

    std::ifstream Input(File.c_str(),std::ios::binary);

    if (!Input.is_open())
    {
        // Exception..
        std::cout<<"Unable to open file '"<<File<<'\''<<std::endl;
        return "";
    }

    Input.seekg(0,std::ios::end);

    size_t Len = Input.tellg();

    Input.seekg(0);

    Content.resize(Len);

    Input.read(&Content[0],Len);

    return Content;

}

static bool CheckProgramError(GLuint ProgramID, GLuint VertexShader, GLuint FragmentShader)
{
    int InfoLogLength=0;
    int CharWritten=0;
    char* InfoLog = NULL;

    GLint Vert_CS = GL_FALSE;
    GLint Frag_CS = GL_FALSE;

    glGetShaderiv(VertexShader,GL_COMPILE_STATUS,&Vert_CS);
    glGetShaderiv(FragmentShader,GL_COMPILE_STATUS,&Frag_CS);

    if (Vert_CS == GL_FALSE)
    {
        // An error has occured during compilation :
        glGetShaderiv(VertexShader,GL_INFO_LOG_LENGTH,&InfoLogLength);

        if (InfoLogLength>0)
        {
            InfoLog = new char [InfoLogLength];
            glGetShaderInfoLog(VertexShader,InfoLogLength,&CharWritten,InfoLog);
            std::cout<<"*** GLSL : COMPILATION ERROR ***"<<std::endl;
            std::cout<<InfoLog<<std::endl;
            std::cout<<"********************************"<<std::endl;
            delete [] InfoLog;
        }
    }

    if (Frag_CS == GL_FALSE)
    {
        // An error has occured during compilation :
        glGetShaderiv(FragmentShader,GL_INFO_LOG_LENGTH,&InfoLogLength);

        if (InfoLogLength>0)
        {
            InfoLog = new char [InfoLogLength];
            glGetShaderInfoLog(FragmentShader,InfoLogLength,&CharWritten,InfoLog);
            std::cout<<"*** GLSL : COMPILATION ERROR ***"<<std::endl;
            std::cout<<InfoLog<<std::endl;
            std::cout<<"********************************"<<std::endl;
            delete [] InfoLog;
        }
    }

    return (Frag_CS == GL_TRUE && Vert_CS == GL_TRUE);
}

Graphic::Shader::Shader() : ProgramID(0),VertexShader(0), FragmentShader(0)
{}

bool Graphic::Shader::LoadFromFile(const std::string& VertFile, const std::string& FragFile)
{
    // Todo :
    // Make sure programID is 0 !

    std::string VertFile_Content = LoadFileContent(VertFile);
    std::string FragFile_Content = LoadFileContent(FragFile);

    VertexShader = glCreateShader(GL_VERTEX_SHADER);
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* VFile_CPtr = VertFile_Content.c_str();
    const char* FFile_CPtr = FragFile_Content.c_str();

    glShaderSource(VertexShader, 1, &VFile_CPtr, NULL);
    glShaderSource(FragmentShader, 1, &FFile_CPtr, NULL);

    glCompileShader(VertexShader);
    glCompileShader(FragmentShader);

    ProgramID = glCreateProgram();

    glAttachShader(ProgramID, VertexShader);
    glAttachShader(ProgramID, FragmentShader);

    glLinkProgram(ProgramID);

    if (!CheckProgramError(ProgramID, VertexShader, FragmentShader))
    {
        DeleteShader();
        return false;
    }

    UniformLocations.clear();
    GLint NumUniforms = 0;
    glGetProgramiv(ProgramID, GL_ACTIVE_UNIFORMS, &NumUniforms);

    char NameBuffer[ 256 ];

    for (GLint i=0; i< NumUniforms; ++i)
    {
        std::string UniformName;
        UniformName.reserve(256);
        GLenum Type = 0;
        GLsizei Length = 0;
        GLsizei Size = 0;

        glGetActiveUniform(ProgramID, i, 256, &Length, &Size, &Type, NameBuffer);

        UniformLocations[ NameBuffer ] = i;

    }

    return true;
}

GLint Graphic::Shader::GetUniformLocation(const std::string& UnifName)
{
    auto it = UniformLocations.find(UnifName);

    if (it == UniformLocations.end())
        return -1;

    return it->second;
}

void Graphic::Shader::DeleteShader()
{
    if (ProgramID)
    {
        glDetachShader(ProgramID,VertexShader);
        glDetachShader(ProgramID,FragmentShader);

        glDeleteShader(VertexShader);
        glDeleteShader(FragmentShader);

        glDeleteProgram(ProgramID);

        ProgramID = 0;
        VertexShader = 0;
        FragmentShader = 0;
    }
}

Graphic::Shader::~Shader()
{
    if (ProgramID != 0)
        DeleteShader();
}

