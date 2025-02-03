
#include "egl_droid.hpp"
#include <cstring>
#include <cstdlib>

namespace gl {

    ShaderProgram::ShaderProgram() : shader_id(0) {}

    ShaderProgram::ShaderProgram(GLuint id) : shader_id(id) {}

    ShaderProgram::~ShaderProgram() {
        release();
    }

    void ShaderProgram::release() {
        if (vertex_shader) {
            glDeleteShader(vertex_shader);
            vertex_shader = 0;
        }
        if (fragment_shader) {
            glDeleteShader(fragment_shader);
            fragment_shader = 0;
        }
        if (shader_id) {
            glDeleteProgram(shader_id);
            shader_id = 0;
        }
    }

    ShaderProgram &ShaderProgram::operator=(const ShaderProgram &p) {
        shader_id = p.shader_id;
        name_ = p.name_;
        return *this;
    }

    int ShaderProgram::printShaderLog(GLuint shader) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 0) {
            char *log = new char[len + 1];
            glGetShaderInfoLog(shader, len, nullptr, log);
                    LOG_ERROR("Shader log: \n");
            delete[] log;
        }
        return 0;
    }

    void ShaderProgram::printProgramLog(GLuint program) {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        if (len > 0) {
            char *log = new char[len + 1];
            glGetProgramInfoLog(program, len, nullptr, log);
                    LOG_ERROR("Program log:\n log \n");
            delete[] log;
        }
    }

    bool ShaderProgram::checkError() {
        bool errorFound = false;
        GLenum glErr = glGetError();
        while (glErr != GL_NO_ERROR) {
                    LOG_ERROR("OpenGL Error: glErr \n");
            errorFound = true;
            glErr = glGetError();
        }
        return errorFound;
    }

    GLuint ShaderProgram::createProgram(const char *vshaderSource, const char *fshaderSource) {
        GLint vertCompiled = GL_FALSE;
        GLint fragCompiled = GL_FALSE;
        GLint linked = GL_FALSE;
        GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vShader, 1, &vshaderSource, nullptr);
        glShaderSource(fShader, 1, &fshaderSource, nullptr);
        glCompileShader(vShader);
        checkError();
        glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
        if (vertCompiled != GL_TRUE) {
                    LOG_ERROR("Error compiling vertex shader.\n");
            printShaderLog(vShader);
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            return 0;
        }
        glCompileShader(fShader);
        checkError();
        glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
        if (fragCompiled != GL_TRUE) {
                    LOG_ERROR("Error compiling fragment shader.\n");
            printShaderLog(fShader);
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            return 0;
        }
        GLuint vfProgram = glCreateProgram();
        glAttachShader(vfProgram, vShader);
        glAttachShader(vfProgram, fShader);
        glLinkProgram(vfProgram);
        checkError();
        glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
        if (linked != GL_TRUE) {
                    LOG_ERROR("Error linking shader program.\n");
            printProgramLog(vfProgram);
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            glDeleteProgram(vfProgram);
            return 0;
        }
        vertex_shader = vShader;
        fragment_shader = fShader;
        shader_id = vfProgram;
        return vfProgram;
    }

    GLuint ShaderProgram::createProgramFromFile(const std::string &vert, const std::string &frag) {
        std::ifstream vFile(vert);
        if (!vFile.is_open()) {
            return 0;
        }
        std::ifstream fFile(frag);
        if (!fFile.is_open())
            return 0;
        std::string vSource;
        std::string fSource;
        std::string line;
        while(std::getline(vFile, line)) {
            vSource += line + "\n";
        }
        while(std::getline( fFile, line)) {
            fSource += line + "\n";
        }
        return createProgram(vSource.c_str(), fSource.c_str());
    }

    bool ShaderProgram::loadProgram(const std::string &v, const std::string &f) {
        shader_id = createProgramFromFile(v, f);
        return shader_id != 0;
    }

    bool ShaderProgram::loadProgramFromText(const std::string &v, const std::string &f) {
        shader_id = createProgram(v.c_str(), f.c_str());
        return shader_id != 0;
    }

    void ShaderProgram::setName(const std::string &n) {
        name_ = n;
    }

    void ShaderProgram::useProgram() {
        glUseProgram(shader_id);
    }

    void ShaderProgram::setUniform(const std::string &name, int value) {
        GLint location = glGetUniformLocation(shader_id, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform1i(location, value);
    }

    void ShaderProgram::setUniform(const std::string &name, float value) {
        GLint location = glGetUniformLocation(shader_id, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform1f(location, value);
    }

    void ShaderProgram::setUniform(const std::string &name, const glm::vec2 &value) {
        GLint location = glGetUniformLocation(shader_id, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const std::string &name, const glm::vec3 &value) {
        GLint location = glGetUniformLocation(shader_id, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const std::string &name, const glm::vec4 &value) {
        GLint location = glGetUniformLocation(shader_id, name.c_str());
        if (location == -1) {
            return;
        }
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const std::string &name, const glm::mat4 &value) {
        GLint location = glGetUniformLocation(shader_id, name.c_str());
        if (location == -1) {
            return;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    bool GLWindow::initGL() {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            GL_LOGW("Unable to get EGL display");
            return false;
        }
        if (!eglInitialize(display, nullptr, nullptr)) {
            GL_LOGW("Unable to initialize EGL");
            return false;
        }
        const EGLint attribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
                EGL_BLUE_SIZE,   8,
                EGL_GREEN_SIZE,  8,
                EGL_RED_SIZE,    8,
                EGL_DEPTH_SIZE, 16,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };

        EGLConfig config;
        EGLint numConfigs;
        if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
            GL_LOGW("Unable to choose EGL config");
            return false;
        }
        EGLint format;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
            GL_LOGW("Unable to get EGL config attribute");
            return false;
        }
        ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);
        const EGLint contextAttribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
        };
        surface = eglCreateWindowSurface(display, config, app->window, nullptr);
        if (surface == EGL_NO_SURFACE) {
            GL_LOGW("Unable to create EGL surface");
            return false;
        }
        context = eglCreateContext(display, config, nullptr, contextAttribs);
        if (context == EGL_NO_CONTEXT) {
            GL_LOGW("Unable to create EGL context");
            return false;
        }
        if (!eglMakeCurrent(display, surface, surface, context)) {
            GL_LOGW("Unable to make EGL context current");
            return false;
        }
        eglQuerySurface(display, surface, EGL_WIDTH, &width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &height);
        return true;
    }

    GLWindow::GLWindow(struct android_app* app)
            : app(app),
              display(EGL_NO_DISPLAY),
              surface(EGL_NO_SURFACE),
              context(EGL_NO_CONTEXT),
              width(0),
              height(0),
              currentObject(nullptr) {
    }

    GLWindow::~GLWindow() {
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context != EGL_NO_CONTEXT) {
                eglDestroyContext(display, context);
            }
            if (surface != EGL_NO_SURFACE) {
                eglDestroySurface(display, surface);
            }
            eglTerminate(display);
        }
    }

    void GLWindow::swapBuffers() {
        if (display != EGL_NO_DISPLAY && surface != EGL_NO_SURFACE) {
            eglSwapBuffers(display, surface);
        }
    }

    void GLWindow::processEvents() {
        int ident, events;
        struct android_poll_source* source;
        while ((ident = ALooper_pollOnce(0, nullptr, &events, (void**)&source)) >= 0) {
            if (source) {
                source->process(app, source);
            }
            if (app->destroyRequested != 0) {
                exit(0);
            }
        }
    }

    void GLWindow::setObject(GLObject* obj) {
        currentObject = obj;
        if (currentObject) {
            currentObject->load();
        }
    }

    void GLWindow::mainLoop() {
        while (1) {
            processEvents();
            if (currentObject) {
                currentObject->update();
                currentObject->draw();
            }
            swapBuffers();
        }
    }

}
