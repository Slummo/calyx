#ifndef CX_RENDER_H
#define CX_RENDER_H

#include <ks/core.h>
#include <ks/math.h>
#include <ks/ds.h>
#include <signal.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cx/core.h>

CX_STRUCT(col3, {
    float r;
    float g;
    float b;
});

#define CX_COL3(r, g, b) \
    (cx_col3) {          \
        r, g, b          \
    }

CX_STRUCT(col4, {
    float r;
    float g;
    float b;
    float a;
});

#define CX_COL4(r, g, b, a) \
    (cx_col4) {             \
        r, g, b, a          \
    }

/* Shader */

CX_STRUCT(shader, { uint32_t id; });

CX_API cx_shader cx_shader_create(const char* vert_filename, const char* frag_filename);
CX_API void cx_shader_bind(cx_shader s);
CX_API void cx_shader_unbind(void);
CX_API void cx_shader_set_int(cx_shader s, const char* name, int* data);
CX_API void cx_shader_set_float(cx_shader s, const char* name, float* data);
CX_API void cx_shader_set_vec2(cx_shader s, const char* name, float* data);
CX_API void cx_shader_set_vec3(cx_shader s, const char* name, float* data);
CX_API void cx_shader_set_vec4(cx_shader s, const char* name, float* data);
CX_API void cx_shader_set_mat3(cx_shader s, const char* name, float* data);
CX_API void cx_shader_set_mat4(cx_shader s, const char* name, float* data);
CX_API void cx_shader_destroy(cx_shader s);

/* Buffer */

CX_STRUCT(buffer, {
    uint32_t id;
    uint32_t type;
});

CX_API cx_buffer cx_buffer_create(uint32_t type, size_t size, const void* data, uint32_t usage);
CX_API void cx_buffer_update(cx_buffer b, size_t off, size_t size, const void* data);
CX_API void cx_buffer_bind(cx_buffer b);
CX_API void cx_buffer_unbind(uint32_t type);
CX_API void cx_buffer_destroy(cx_buffer b);

/* Texture */

// TODO
CX_STRUCT(texture, { uint32_t id; });

/* Vertex attribute and format */

CX_STRUCT(vattr, {
    uint32_t loc;
    int32_t count;  // number of floats in data type
    uint32_t type;  // data type code
    uint32_t off;
    int32_t stride;
    uint32_t divisor;  // 0 = per-vertex, 1 = per-instance
});

#define CX_MAX_VATTRS_PER_VBO 8
CX_STRUCT(vfmt, {
    KS_SA_UNNAMED(cx_vattr, CX_MAX_VATTRS_PER_VBO) attrs;
    uint32_t vsize;
});

CX_API extern const cx_vfmt CX_VFMT_POS2;
CX_API extern const cx_vfmt CX_VFMT_POS3;
CX_API extern const cx_vfmt CX_VFMT_POS2_COL3;
CX_API extern const cx_vfmt CX_VFMT_POS3_COL3;
CX_API extern const cx_vfmt CX_VFMT_INST_FLOAT;
CX_API extern const cx_vfmt CX_VFMT_INST_VEC2;
CX_API extern const cx_vfmt CX_VFMT_INST_VEC3;
CX_API extern const cx_vfmt CX_VFMT_INST_MAT3;
CX_API extern const cx_vfmt CX_VFMT_INST_MAT4;

/* Index format */
CX_STRUCT(ifmt, {
    uint32_t type;    // data type code
    uint32_t ixsize;  // size of data type
});

CX_API extern const cx_ifmt KS_IFMT_U8;
CX_API extern const cx_ifmt KS_IFMT_U16;
CX_API extern const cx_ifmt KS_IFMT_U32;

/* Mesh */

#define KS_MAX_VBOS_PER_MESH 8
CX_STRUCT(mesh, {
    uint32_t vao;
    KS_SA_UNNAMED(cx_buffer, KS_MAX_VBOS_PER_MESH) vbos;
    cx_buffer ebo;
    bool has_indices;
    cx_buffer inst_vbo;
    bool has_instances;
    uint32_t ixtype;
    int32_t vcount;
    int32_t ixcount;
    int32_t iecount;
    uint32_t next_loc;
});

CX_API cx_mesh cx_mesh_create(void);
CX_API void cx_mesh_load_vertices(cx_mesh* m, int32_t vcount, const void* verts, const cx_vfmt* vfmt);
CX_API void cx_mesh_load_indices(cx_mesh* m, int32_t ixcount, const void* inds, const cx_ifmt* ifmt);
CX_API void cx_mesh_load_instances(cx_mesh* m, int32_t iecount, const void* insts, const cx_vfmt* vfmt);
CX_API void cx_mesh_update_instances(cx_mesh* m, int32_t iecount, const void* insts, const cx_vfmt* vfmt);
CX_API void cx_mesh_draw(cx_mesh* m, cx_shader s);
CX_API void cx_mesh_destroy(cx_mesh* m);

/* Batcher */

CX_STRUCT(batcher, {
    void* vertices;
    void* indices;
    int32_t vcount;
    int32_t ixcount;
    int32_t max_verts;
    int32_t max_inds;
    int32_t stride;
    cx_mesh gpu_handle;
});

CX_API void cx_batcher_init(int32_t max_verts, int32_t max_inds, int32_t stride, const cx_vattr* attributes,
                            int32_t attr_count);
CX_API void cx_batcher_next_vertex(void);
CX_API void cx_batcher_push_index(uint32_t i);
CX_API void cx_batcher_flush(void);

/* Renderer */

CX_FUNC(void, win_resize, int32_t width, int32_t height);

CX_STRUCT(render_ctx, {
    int32_t width, height;
    cx_shader shader;
    bool is_drawing;
    GLFWwindow* win;
    cx_batcher batcher;
    cx_win_resize resize_cb;
    uint32_t primitive;
});

CX_API extern cx_render_ctx g_render;

CX_API void cx_render_init(int32_t width, int32_t height, const char* title);
CX_API bool cx_win_should_close(void);
CX_API void cx_win_resize_cb(cx_win_resize cb);
CX_API ks_vec2 cx_win_size(void);
CX_API void cx_primitive(uint32_t type);
CX_API void cx_drawbox(int32_t x, int32_t y, int32_t w, int32_t h);
CX_API void cx_drawbox_reset(void);
CX_API void cx_background(cx_col4 col);
CX_API void cx_wiremode(bool state);
CX_API void cx_line_width(float width);
CX_API void cx_begin_drawing(void);
CX_API void cx_draw_line(ks_vec3 p1, ks_vec3 p2, float size, cx_col4 c);
CX_API void cx_draw_rect(ks_vec3 left, ks_vec3 dim, float line_size, cx_col4 c);
CX_API void cx_end_drawing(void);
CX_API void cx_render_terminate(void);

#endif  // KS_RENDER_H