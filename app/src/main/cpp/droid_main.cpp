#include "egl_droid.hpp"

static const GLfloat cubeVertices[] = {
        // Front face
        -1.0f, -1.0f,  1.0f,   1, 0, 0,
        1.0f, -1.0f,  1.0f,   0, 1, 0,
        1.0f,  1.0f,  1.0f,   0, 0, 1,
        -1.0f, -1.0f,  1.0f,   1, 0, 0,
        1.0f,  1.0f,  1.0f,   0, 0, 1,
        -1.0f,  1.0f,  1.0f,   1, 1, 0,
        // Back face
        -1.0f, -1.0f, -1.0f,   1, 0, 1,
        1.0f,  1.0f, -1.0f,   0, 1, 1,
        1.0f, -1.0f, -1.0f,   0, 1, 0,
        -1.0f, -1.0f, -1.0f,   1, 0, 1,
        -1.0f,  1.0f, -1.0f,   0, 0, 1,
        1.0f,  1.0f, -1.0f,   0, 1, 1,
        // Left face
        -1.0f, -1.0f, -1.0f,   1, 0, 1,
        -1.0f, -1.0f,  1.0f,   1, 0, 0,
        -1.0f,  1.0f,  1.0f,   1, 1, 0,
        -1.0f, -1.0f, -1.0f,   1, 0, 1,
        -1.0f,  1.0f,  1.0f,   1, 1, 0,
        -1.0f,  1.0f, -1.0f,   0, 0, 1,
        // Right face
        1.0f, -1.0f, -1.0f,   0, 1, 0,
        1.0f,  1.0f,  1.0f,   0, 0, 1,
        1.0f, -1.0f,  1.0f,   0, 1, 1,
        1.0f, -1.0f, -1.0f,   0, 1, 0,
        1.0f,  1.0f, -1.0f,   1, 0, 1,
        1.0f,  1.0f,  1.0f,   0, 0, 1,
        // Top face
        -1.0f,  1.0f,  1.0f,   1, 1, 0,
        1.0f,  1.0f,  1.0f,   0, 0, 1,
        1.0f,  1.0f, -1.0f,   1, 0, 1,
        -1.0f,  1.0f,  1.0f,   1, 1, 0,
        1.0f,  1.0f, -1.0f,   1, 0, 1,
        -1.0f,  1.0f, -1.0f,   0, 0, 1,
        // Bottom face
        -1.0f, -1.0f,  1.0f,   1, 0, 0,
        1.0f, -1.0f, -1.0f,   0, 1, 0,
        1.0f, -1.0f,  1.0f,   0, 1, 0,
        -1.0f, -1.0f,  1.0f,   1, 0, 0,
        -1.0f, -1.0f, -1.0f,   1, 0, 1,
        1.0f, -1.0f, -1.0f,   0, 1, 0,
};

class GameObject : public gl::GLObject {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLint uMVPMatrixLocation = -1;
    float angle = 0.0f;
    int w, h;
public:

    GameObject(int width, int height) {
        w = width;
        h = height;
    }

    void init_cube() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    virtual void load() override {
        static const char *vertexShaderSource =
                "#version 300 es\n"
                "uniform mat4 uMVPMatrix;\n"
                "layout(location = 0) in vec3 aPosition;\n"
                "layout(location = 1) in vec3 aColor;\n"
                "out vec3 vColor;\n"
                "void main() {\n"
                "    vColor = aColor;\n"
                "    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);\n"
                "}\n";

        static const char *fragmentShaderSource =
                "#version 300 es\n"
                "precision mediump float;\n"
                "in vec3 vColor;\n"
                "out vec4 fragColor;\n"
                "void main() {\n"
                "    fragColor = vec4(vColor, 1.0);\n"
                "}\n";

        if(program.loadProgramFromText(vertexShaderSource, fragmentShaderSource)) {
            GL_LOGI("Loaded Succesfully");
        }
        init_cube();
    }
    virtual void draw() override {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, w, h);
        glEnable(GL_DEPTH_TEST);
        program.useProgram();
        float aspect = static_cast<float>(w) / static_cast<float>(h);
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 1.0f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        view = glm::rotate(view, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        angle += 1.0f;
        if (angle > 360.0f)
            angle -= 360.0f;
        glm::mat4 model(1.0f);
        glm::mat4 mvp = proj * view * model;
        program.setUniform("uMVPMatrix", mvp);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    virtual void update() override {

    }
    virtual void handleCommand(int32_t cmd) override {

    }
    gl::ShaderProgram program;
};

extern "C" void android_main(struct android_app* state) {
    app_dummy();
    while (state->window == nullptr) {
        int ident;
        int events;
        struct android_poll_source* source;
        if ((ident = ALooper_pollOnce(100, nullptr, &events, (void**)&source)) >= 0) {
            if (source) {
                source->process(state, source);
            }
        }
    }
    gl::GLWindow window(state);
    if (!window.initGL()) {
       GL_LOGW("Failed to initialize GLWindow");
        return;
    }
    GameObject game(window.getWidth(), window.getHeight());
    window.setObject(&game);
    window.mainLoop();
}
