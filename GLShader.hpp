// Copyright 2023 JohnCorn
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string>
#include <fstream>
#include <streambuf>
extern "C"
{
#include <GL/glew.h>
}

static inline std::string read_shader_file(const wchar_t *path)
{
    std::ifstream f(path);
    return std::string((std::istreambuf_iterator<char>(f)),
                       std::istreambuf_iterator<char>());
}

static inline GLint compile_shader(const wchar_t *path, GLenum type)
{
    std::string src = read_shader_file(path);
    const char *src_ptr = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src_ptr, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint info_log_size;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_size);
        std::vector<char> info_log(info_log_size);
        glGetShaderInfoLog(shader, info_log_size, NULL, info_log.data());
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}

static inline GLint build_shaders(const wchar_t *vertex_shader_path, const wchar_t *fragment_shaders_path)
{
    if (GLuint vertex_shader = compile_shader(vertex_shader_path, GL_VERTEX_SHADER))
        if (GLuint fragment_shader = compile_shader(fragment_shaders_path, GL_FRAGMENT_SHADER))
        {
            GLuint program = glCreateProgram();
            glAttachShader(program, vertex_shader);
            glAttachShader(program, fragment_shader);
            glLinkProgram(program);
            GLint success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success)
            {
                GLint info_log_size;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_size);
                std::vector<char> info_log(info_log_size);
                glGetProgramInfoLog(program, info_log_size, NULL, info_log.data());
                glDeleteProgram(program);
                program = 0;
            }

            glDeleteShader(vertex_shader);
            glDeleteProgram(fragment_shader);
            return program;
        }

    return 0;
}