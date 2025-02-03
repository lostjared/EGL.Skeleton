#ifndef GL_HPP
#define GL_HPP

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <string>
#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#define GL_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "GL", __VA_ARGS__))
#define GL_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "GL", __VA_ARGS__))
#define LOG_ERROR GL_LOGW

namespace gl {

    class ShaderProgram {
    public:
        ShaderProgram();
        explicit ShaderProgram(GLuint id);
        ~ShaderProgram();
        void release();
        ShaderProgram &operator=(const ShaderProgram &p);
        GLuint createProgram(const char *vshaderSource, const char *fshaderSource);
        GLuint createProgramFromFile(const std::string &vert, const std::string &frag);
        bool loadProgram(const std::string &v, const std::string &f);
        bool loadProgramFromText(const std::string &v, const std::string &f);
        void useProgram();
        void setName(const std::string &n);
void setUniform(const std::string &name, int value);
        void setUniform(const std::string &name, float value);
        void setUniform(const std::string &name, const glm::vec2 &value);
        void setUniform(const std::string &name, const glm::vec3 &value);
        void setUniform(const std::string &name, const glm::vec4 &value);
        void setUniform(const std::string &name, const glm::mat4 &value);
        bool checkError();
        int printShaderLog(GLuint shader);
        void printProgramLog(GLuint program);
        GLuint id() const { return shader_id; }
    private:
        GLuint shader_id;
        GLuint vertex_shader = 0;
        GLuint fragment_shader = 0;
        std::string name_;
    };

    class GLObject {
    public:
        virtual ~GLObject() {}
        virtual void load() = 0;
        virtual void draw() = 0;
        virtual void update() {}
        virtual void handleCommand(int32_t cmd) {}
    };

    class GLWindow {
    public:
        GLWindow(struct android_app* app);
        ~GLWindow();
        bool initGL();
        void swapBuffers();
        void processEvents();
        void setObject(GLObject* obj);
        void mainLoop();
        int getWidth() const { return width; }
        int getHeight() const { return height; }
    private:
        struct android_app* app;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
        int width;
        int height;
        GLObject* currentObject;
    };
}

#endif

