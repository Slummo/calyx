#include <cx/render.h>
#include <ks/time.h>
#include <ks/io.h>

/* Shader */

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &source, NULL);
    glCompileShader(s);

    int ret;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        char buf[1024];
        glGetShaderInfoLog(s, 1024, NULL, buf);
        fprintf(stderr, "%s", buf);
        return 0;
    }

    return s;
}

CX_API cx_shader cx_shader_create(const char* vert_filename, const char* frag_filename) {
    cx_shader s = {0};

    char* vert_src = ks_fread(vert_filename);
    char* frag_src = ks_fread(frag_filename);

    if (!vert_src || !frag_src) {
        ks_log(KSERR, "Error while reading from shader sources");
        free(vert_src);
        free(frag_src);
        return s;
    }

    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vert_src);
    if (!vertex) {
        ks_log(KSERR, "Error while compiling vertex shader");
        free(vert_src);
        free(frag_src);
        return s;
    }

    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, frag_src);
    if (!fragment) {
        ks_log(KSERR, "Error while compiling fragment shader");
        free(vert_src);
        free(frag_src);
        return s;
    }

    free(vert_src);
    free(frag_src);

    GLuint p = glCreateProgram();
    glAttachShader(p, vertex);
    glAttachShader(p, fragment);
    glLinkProgram(p);

    int ret;
    glGetProgramiv(p, GL_LINK_STATUS, &ret);
    if (!ret) {
        char buf[1024];
        glGetProgramInfoLog(p, 1024, NULL, buf);
        fprintf(stderr, "%s", buf);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(p);
        return s;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    s.id = p;
    return s;
}

CX_API cx_shader cx_shader_create_compute(const char* comp_filename) {
    cx_shader s = {0};

    char* comp_src = ks_fread(comp_filename);

    if (!comp_src) {
        ks_log(KSERR, "Error while reading from shader source");
        free(comp_src);
        return s;
    }

    GLuint compute = compile_shader(GL_COMPUTE_SHADER, comp_src);
    if (!compute) {
        ks_log(KSERR, "Error while compiling compute shader");
        free(comp_src);
        return s;
    }

    free(comp_src);

    GLuint p = glCreateProgram();
    glAttachShader(p, compute);
    glLinkProgram(p);

    int ret;
    glGetProgramiv(p, GL_LINK_STATUS, &ret);
    if (!ret) {
        char buf[4096];
        glGetProgramInfoLog(p, 4096, NULL, buf);
        fprintf(stderr, "%s", buf);
        glDeleteShader(compute);
        glDeleteProgram(p);
        return s;
    }

    glDeleteShader(compute);

    s.id = p;
    return s;
}

CX_API void cx_shader_bind(cx_shader s) {
    glUseProgram(s.id);
}

CX_API void cx_shader_unbind(void) {
    glUseProgram(0);
}

CX_API void cx_shader_set_int32(cx_shader s, const char* name, int32_t data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniform1i(loc, data);
}

CX_API void cx_shader_set_uint32(cx_shader s, const char* name, uint32_t data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniform1ui(loc, data);
}

CX_API void cx_shader_set_float(cx_shader s, const char* name, float data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniform1f(loc, data);
}

CX_API void cx_shader_set_vec2(cx_shader s, const char* name, float* data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniform2fv(loc, 1, data);
}

CX_API void cx_shader_set_vec3(cx_shader s, const char* name, float* data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniform3fv(loc, 1, data);
}

CX_API void cx_shader_set_vec4(cx_shader s, const char* name, float* data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniform4fv(loc, 1, data);
}

CX_API void cx_shader_set_mat3(cx_shader s, const char* name, float* data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniformMatrix3fv(loc, 1, GL_FALSE, data);
}

CX_API void cx_shader_set_mat4(cx_shader s, const char* name, float* data) {
    GLint loc = glGetUniformLocation(s.id, name);
    KS_ASSERT(loc != -1, "Uniform not found");
    glUniformMatrix4fv(loc, 1, GL_FALSE, data);
}

CX_API void cx_shader_destroy(cx_shader s) {
    glDeleteProgram(s.id);
}

/* Buffer */

CX_API cx_buffer cx_buffer_create(uint32_t type, size_t size, const void* data, uint32_t usage) {
    cx_buffer b = {.id = 0, .type = type};
    glGenBuffers(1, &b.id);
    cx_buffer_bind(b);
    glBufferData(b.type, (GLsizeiptr)size, data, usage);
    return b;
}

CX_API void cx_buffer_update(cx_buffer b, size_t off, size_t size, const void* data) {
    cx_buffer_bind(b);
    glBufferSubData(b.type, (GLintptr)off, (GLsizeiptr)size, data);
}

CX_API void cx_buffer_bind(cx_buffer b) {
    glBindBuffer(b.type, b.id);
}

CX_API void cx_buffer_base(cx_buffer b, uint32_t index) {
    glBindBufferBase(b.type, index, b.id);
}

CX_API void cx_buffer_range(cx_buffer b, uint32_t index, size_t off, size_t size) {
    glBindBufferRange(b.type, index, b.id, (GLintptr)off, (GLintptr)size);
}

CX_API void cx_buffer_unbind(uint32_t type) {
    glBindBuffer(type, 0);
}

CX_API void cx_buffer_destroy(cx_buffer b) {
    glDeleteBuffers(1, &b.id);
}

/* Texture*/

/* Vertex format */

// clang-format off
CX_API const cx_vfmt CX_VFMT_POS2 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 2, GL_FLOAT, 0, sizeof(ks_vec2), 0},
        },
        .len = 1
    },
    .vsize = sizeof(ks_vec2)
};

CX_API const cx_vfmt CX_VFMT_POS3 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 3, GL_FLOAT, 0, sizeof(ks_vec3), 0},
        },
        .len = 1
    },
    .vsize = sizeof(ks_vec3)
};

CX_API const cx_vfmt CX_VFMT_POS2_COL3 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 2, GL_FLOAT, 0               , sizeof(ks_vec2) + sizeof(cx_col3), 0},
            {1, 3, GL_FLOAT, sizeof(ks_vec2) , sizeof(ks_vec2) + sizeof(cx_col3), 0}
        },
        .len = 2
    },
    .vsize = sizeof(ks_vec2) + sizeof(cx_col3)
};

CX_API const cx_vfmt CX_VFMT_POS3_COL3 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 3, GL_FLOAT, 0               , sizeof(ks_vec3) + sizeof(cx_col3), 0},
            {1, 3, GL_FLOAT, sizeof(ks_vec3) , sizeof(ks_vec3) + sizeof(cx_col3), 0}
        },
        .len = 2
    },
    .vsize = sizeof(ks_vec3) + sizeof(cx_col3)
};

CX_API const cx_vfmt CX_VFMT_INST_FLOAT = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 1, GL_FLOAT, 0, sizeof(float), 1}
        },
        .len = 1
    },
    .vsize = sizeof(float)
};

CX_API const cx_vfmt CX_VFMT_INST_VEC2 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 2, GL_FLOAT, 0, sizeof(ks_vec2), 1}
        },
        .len = 1
    },
    .vsize = sizeof(ks_vec2)
};

CX_API const cx_vfmt CX_VFMT_INST_VEC3 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 3, GL_FLOAT, 0, sizeof(ks_vec3), 1}
        },
        .len = 1
    },
    .vsize = sizeof(ks_vec3)
};

CX_API const cx_vfmt CX_VFMT_INST_VEC4 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 4, GL_FLOAT, 0, sizeof(ks_vec4), 1}
        },
        .len = 1
    },
    .vsize = sizeof(ks_vec4)
};

CX_API const cx_vfmt CX_VFMT_INST_MAT3 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 3, GL_FLOAT, 0,                   sizeof(ks_mat3), 1},
            {1, 3, GL_FLOAT, 1 * sizeof(ks_vec3), sizeof(ks_mat3), 1},
            {2, 3, GL_FLOAT, 2 * sizeof(ks_vec3), sizeof(ks_mat3), 1}
        },
        .len = 3
    },
    .vsize = sizeof(ks_mat3)
};

CX_API const cx_vfmt CX_VFMT_INST_MAT4 = {
    .attrs = {
        .data = {
            // loc, count, type, offset, stride, divisor
            {0, 4, GL_FLOAT, 0,                   sizeof(ks_mat4), 1},
            {1, 4, GL_FLOAT, 1 * sizeof(ks_vec4), sizeof(ks_mat4), 1},
            {2, 4, GL_FLOAT, 2 * sizeof(ks_vec4), sizeof(ks_mat4), 1},
            {3, 4, GL_FLOAT, 3 * sizeof(ks_vec4), sizeof(ks_mat4), 1} 
        },
        .len = 4
    },
    .vsize = sizeof(ks_mat4)
};

/* Index format */
                            // type, ixsize
CX_API const cx_ifmt CX_IFMT_U8 = {GL_UNSIGNED_BYTE , sizeof(uint8_t)};
CX_API const cx_ifmt CX_IFMT_U16 = {GL_UNSIGNED_SHORT, sizeof(uint16_t)};
CX_API const cx_ifmt CX_IFMT_U32 = {GL_UNSIGNED_INT  , sizeof(uint32_t)};

// clang-format on

/* Mesh */

static void _cx_mesh_init_vbo_internal(cx_mesh* m, cx_buffer b, const cx_vfmt* vfmt) {
    KS_ASSERT_NONNULL_ARGS(m && vfmt);

    glBindVertexArray(m->vao);
    // cx_buffer_bind(b);
    glBindBuffer(GL_ARRAY_BUFFER, b.id);

    ks_array_foreach(attr, &vfmt->attrs) {
        uint32_t loc = m->next_loc + attr->loc;
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, attr->count, attr->type, GL_FALSE, attr->stride,
                              (const void*)(uintptr_t)(attr->off));
        if (attr->divisor > 0) {
            glVertexAttribDivisor(loc, attr->divisor);
        }
    }

    m->next_loc += vfmt->attrs.len;

    glBindVertexArray(0);
}

CX_API cx_mesh cx_mesh_create(void) {
    cx_mesh m = {0};

    glGenVertexArrays(1, &m.vao);
    m.ixtype = GL_UNSIGNED_INT;

    return m;
}

CX_API void cx_mesh_load_vertices(cx_mesh* m, int32_t vcount, const void* verts, const cx_vfmt* vfmt) {
    KS_ASSERT_NONNULL_ARGS(m && verts && vfmt);
    if (ks_sa_isfull(&m->vbos)) {
        return;
    }

    cx_buffer vbo = cx_buffer_create(GL_ARRAY_BUFFER, (size_t)vcount * vfmt->vsize, verts, GL_STATIC_DRAW);
    _cx_mesh_init_vbo_internal(m, vbo, vfmt);
    ks_sa_push(&m->vbos, vbo);

    m->vcount = vcount;
    m->iecount = 1;
}

CX_API void cx_mesh_load_indices(cx_mesh* m, int32_t ixcount, const void* inds, const cx_ifmt* ifmt) {
    KS_ASSERT_NONNULL_ARGS(m && inds && ifmt);

    cx_buffer ebo = cx_buffer_create(GL_ELEMENT_ARRAY_BUFFER, (size_t)ixcount * ifmt->ixsize, inds, GL_STATIC_DRAW);

    glBindVertexArray(m->vao);
    cx_buffer_bind(ebo);

    m->ebo = ebo;
    m->has_indices = true;
    m->ixtype = ifmt->type;
    m->ixcount = ixcount;

    glBindVertexArray(0);
}

CX_API void cx_mesh_load_instances(cx_mesh* m, int32_t iecount, const void* insts, const cx_vfmt* vfmt) {
    KS_ASSERT_NONNULL_ARGS(m && insts && vfmt);

    cx_buffer vbo = cx_buffer_create(GL_ARRAY_BUFFER, (size_t)iecount * vfmt->vsize, insts, GL_DYNAMIC_DRAW);
    _cx_mesh_init_vbo_internal(m, vbo, vfmt);

    m->inst_vbo = vbo;
    m->has_instances = true;
    m->iecount = iecount;
}

CX_API void cx_mesh_load_instances_buf(cx_mesh* m, int32_t iecount, cx_buffer b, const cx_vfmt* vfmt) {
    KS_ASSERT_NONNULL_ARGS(m);

    _cx_mesh_init_vbo_internal(m, b, vfmt);

    m->inst_vbo = b;
    m->has_instances = true;
    m->iecount = iecount;
}

CX_API void cx_mesh_update_instances(cx_mesh* m, int32_t iecount, const void* insts, const cx_vfmt* vfmt) {
    KS_ASSERT_NONNULL_ARGS(m && insts && vfmt);
    KS_ASSERT(m->has_instances, "No instance buffer initialized for this mesh");
    glBindVertexArray(m->vao);
    cx_buffer_update(m->inst_vbo, 0, (size_t)iecount * vfmt->vsize, insts);
    glBindVertexArray(0);
}

CX_API void cx_mesh_draw(cx_mesh* m, cx_shader s) {
    KS_ASSERT_NONNULL_ARGS(m);

    g_render.is_drawing = true;

    cx_shader_bind(s);
    glBindVertexArray(m->vao);

    if (m->has_indices) {
        if (m->has_instances) {
            glDrawElementsInstanced(g_render.primitive, m->ixcount, m->ixtype, NULL, m->iecount);
        } else {
            glDrawElements(g_render.primitive, m->ixcount, m->ixtype, NULL);
        }
    } else {
        if (m->has_instances) {
            glDrawArraysInstanced(g_render.primitive, 0, m->vcount, m->iecount);
        } else {
            glDrawArrays(g_render.primitive, 0, m->vcount);
        }
    }

    glBindVertexArray(0);

    g_render.is_drawing = false;
}

CX_API void cx_mesh_destroy(cx_mesh* m) {
    KS_ASSERT_NONNULL_ARGS(m);
    glDeleteVertexArrays(1, &m->vao);
    // Don't destroy buffers to allow sharing between renderables
}

/* Batcher */

CX_API void cx_batcher_init(int32_t max_verts, int32_t max_inds, int32_t stride, const cx_vattr* attrs,
                            int32_t attr_count);
CX_API void cx_batcher_next_vertex(void);
CX_API void cx_batcher_push_index(uint32_t i);

CX_API void cx_batcher_flush(void) {
    return;
}

/* Renderer */

static void GLAPIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, KS_UNUSED GLsizei length,
                                      const GLchar* message, KS_UNUSED const void* user_param) {
    fprintf(stderr, "[GL_DEBUG] source=0x%x type=0x%x id=%u severity=0x%x\n\t%s\n", source, type, id, severity,
            message);

    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        raise(SIGTRAP);
    }
}

static void framebuffer_size_callback(KS_UNUSED GLFWwindow* window, int32_t width, int32_t height) {
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);
    g_render.width = width;
    g_render.height = height;
    if (g_render.resize_cb) {
        g_render.resize_cb(width, height);
    }
}

CX_API cx_render_ctx g_render = {0};

CX_API void cx_render_init(int32_t width, int32_t height, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        ks_log(KSERR, "Error with glfwInit");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        ks_log(KSERR, "Error with glfwCreateWindow");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwShowWindow(window);
    glfwSwapInterval(1);

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ks_log(KSERR, "Error with gladLoadGLLoader");
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }

    // Set debug callback
    if (GLAD_GL_KHR_debug || GLAD_GL_VERSION_4_3) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    g_render.width = width;
    g_render.height = height;
    g_render.is_drawing = false;
    g_render.win = window;
    g_render.primitive = GL_TRIANGLES;
    // TODO: init batcher
}

CX_API float cx_win_refresh_rate(void) {
    double start = glfwGetTime();
    for (int i = 0; i < 60; ++i) {
        glfwPollEvents();
        glfwSwapBuffers(g_render.win);
    }
    double end = glfwGetTime();
    return 60.0f / (float)(end - start);
}

CX_API bool cx_win_should_close(void) {
    return glfwWindowShouldClose(g_render.win);
}

CX_API void cx_win_resize_cb(cx_win_resize cb) {
    g_render.resize_cb = cb;
}

CX_API ks_vec2 cx_win_size(void) {
    return KS_VEC2(g_render.width, g_render.height);
}

CX_API void cx_primitive(uint32_t type) {
    g_render.primitive = type;
}

CX_API void cx_drawbox(int32_t x, int32_t y, int32_t w, int32_t h) {
    glViewport(x, y, w, h);
    glScissor(x, y, w, h);
}

CX_API void cx_drawbox_reset(void) {
    cx_drawbox(0, 0, g_render.width, g_render.height);
}

CX_API void cx_background(cx_col4 col) {
    glClearColor(col.r, col.g, col.b, col.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

CX_API void cx_wiremode(bool state) {
    glPolygonMode(GL_FRONT_AND_BACK, state ? GL_LINE : GL_FILL);
}

CX_API void cx_line_width(float width) {
    glLineWidth(width);
}

CX_API void cx_dispatch_compute(uint32_t groupsx, uint32_t groupsy, uint32_t groupsz) {
    glDispatchCompute(groupsx, groupsy, groupsz);
}

CX_API void cx_mem_barrier(uint32_t flags) {
    glMemoryBarrier(flags);
}

CX_API void cx_begin_drawing(void) {
    g_render.is_drawing = true;
}

CX_API void cx_draw_line(ks_vec3 p1, ks_vec3 p2, float size, cx_col4 c);
CX_API void cx_draw_rect(ks_vec3 left, ks_vec3 dim, float line_size, cx_col4 c);

CX_API void cx_end_drawing(void) {
    cx_batcher_flush();
    glfwSwapBuffers(g_render.win);
    glfwPollEvents();
    g_render.is_drawing = false;
}

CX_API void cx_render_terminate(void) {
    glfwDestroyWindow(g_render.win);
    glfwTerminate();
}