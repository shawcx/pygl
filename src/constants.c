void add_constants(PyObject *mod) {
// basics
    PyModule_AddIntConstant( mod, "TRUE",  GL_TRUE  );
    PyModule_AddIntConstant( mod, "FALSE", GL_FALSE );

// types
    PyModule_AddIntConstant( mod, "BYTE",           GL_BYTE           );
    PyModule_AddIntConstant( mod, "UNSIGNED_BYTE",  GL_UNSIGNED_BYTE  );
    PyModule_AddIntConstant( mod, "SHORT",          GL_SHORT          );
    PyModule_AddIntConstant( mod, "UNSIGNED_SHORT", GL_UNSIGNED_SHORT );
    PyModule_AddIntConstant( mod, "INT",            GL_INT            );
    PyModule_AddIntConstant( mod, "UNSIGNED_INT",   GL_UNSIGNED_INT   );
    PyModule_AddIntConstant( mod, "FLOAT",          GL_FLOAT          );
    PyModule_AddIntConstant( mod, "DOUBLE",         GL_DOUBLE         );

    PyModule_AddIntConstant( mod, "BITMAP",         GL_BITMAP         );

// logic operators
    PyModule_AddIntConstant( mod, "EQUAL",    GL_EQUAL    );
    PyModule_AddIntConstant( mod, "NOTEQUAL", GL_NOTEQUAL );
    PyModule_AddIntConstant( mod, "GREATER",  GL_GREATER  );
    PyModule_AddIntConstant( mod, "GEQUAL",   GL_GEQUAL   );
    PyModule_AddIntConstant( mod, "LESS",     GL_LESS     );
    PyModule_AddIntConstant( mod, "LEQUAL",   GL_LEQUAL   );
    PyModule_AddIntConstant( mod, "ALWAYS",   GL_ALWAYS   );
    PyModule_AddIntConstant( mod, "NEVER",    GL_NEVER    );

// driver info
    PyModule_AddIntConstant( mod, "VENDOR",     GL_VENDOR     );
    PyModule_AddIntConstant( mod, "RENDERER",   GL_RENDERER   );
    PyModule_AddIntConstant( mod, "VERSION",    GL_VERSION    );
    PyModule_AddIntConstant( mod, "EXTENSIONS", GL_EXTENSIONS );

// faces
    PyModule_AddIntConstant( mod, "FRONT",          GL_FRONT          );
    PyModule_AddIntConstant( mod, "BACK",           GL_BACK           );
    PyModule_AddIntConstant( mod, "FRONT_AND_BACK", GL_FRONT_AND_BACK );

    PyModule_AddIntConstant( mod, "CW",  GL_CW  );
    PyModule_AddIntConstant( mod, "CCW", GL_CCW );

// shapes
    PyModule_AddIntConstant( mod, "POINTS",         GL_POINTS         );
    PyModule_AddIntConstant( mod, "LINES",          GL_LINES          );
    PyModule_AddIntConstant( mod, "LINE_STRIP",     GL_LINE_STRIP     );
    PyModule_AddIntConstant( mod, "LINE_LOOP",      GL_LINE_LOOP      );
    PyModule_AddIntConstant( mod, "TRIANGLES",      GL_TRIANGLES      );
    PyModule_AddIntConstant( mod, "TRIANGLE_STRIP", GL_TRIANGLE_STRIP );
    PyModule_AddIntConstant( mod, "TRIANGLE_FAN",   GL_TRIANGLE_FAN   );
    PyModule_AddIntConstant( mod, "QUADS",          GL_QUADS          );
    PyModule_AddIntConstant( mod, "QUAD_STRIP",     GL_QUAD_STRIP     );
    PyModule_AddIntConstant( mod, "POLYGON",        GL_POLYGON        );

// call lists
    PyModule_AddIntConstant( mod, "COMPILE",             GL_COMPILE             );
    PyModule_AddIntConstant( mod, "COMPILE_AND_EXECUTE", GL_COMPILE_AND_EXECUTE );

// lighting
    PyModule_AddIntConstant( mod, "LIGHTING",   GL_LIGHTING   );
    PyModule_AddIntConstant( mod, "MAX_LIGHTS", GL_MAX_LIGHTS );
    PyModule_AddIntConstant( mod, "LIGHT0",     GL_LIGHT0     );
    PyModule_AddIntConstant( mod, "LIGHT1",     GL_LIGHT1     );
    PyModule_AddIntConstant( mod, "LIGHT2",     GL_LIGHT2     );
    PyModule_AddIntConstant( mod, "LIGHT3",     GL_LIGHT3     );
    PyModule_AddIntConstant( mod, "LIGHT4",     GL_LIGHT4     );
    PyModule_AddIntConstant( mod, "LIGHT5",     GL_LIGHT5     );
    PyModule_AddIntConstant( mod, "LIGHT6",     GL_LIGHT6     );
    PyModule_AddIntConstant( mod, "LIGHT7",     GL_LIGHT7     );

    PyModule_AddIntConstant( mod, "AMBIENT",    GL_AMBIENT    );
    PyModule_AddIntConstant( mod, "DIFFUSE",    GL_DIFFUSE    );
    PyModule_AddIntConstant( mod, "AMBIENT_AND_DIFFUSE", GL_AMBIENT_AND_DIFFUSE );
    PyModule_AddIntConstant( mod, "SPECULAR",   GL_SPECULAR   );
    PyModule_AddIntConstant( mod, "POSITION",   GL_POSITION   );
    PyModule_AddIntConstant( mod, "SHININESS",  GL_SHININESS  );
    PyModule_AddIntConstant( mod, "EMISSION",   GL_EMISSION   );

// pixel formats
    PyModule_AddIntConstant( mod, "R3_G3_B2", GL_R3_G3_B2 );
    PyModule_AddIntConstant( mod, "RGB",      GL_RGB      );
    PyModule_AddIntConstant( mod, "RGB4",     GL_RGB4     );
    PyModule_AddIntConstant( mod, "RGB5",     GL_RGB5     );
    PyModule_AddIntConstant( mod, "RGB8",     GL_RGB8     );
    PyModule_AddIntConstant( mod, "RGB10",    GL_RGB10    );
    PyModule_AddIntConstant( mod, "RGB12",    GL_RGB12    );
    PyModule_AddIntConstant( mod, "RGB16",    GL_RGB16    );
    PyModule_AddIntConstant( mod, "RGBA",     GL_RGBA     );
    PyModule_AddIntConstant( mod, "RGBA2",    GL_RGBA2    );
    PyModule_AddIntConstant( mod, "RGBA4",    GL_RGBA4    );
    PyModule_AddIntConstant( mod, "RGB5_A1",  GL_RGB5_A1  );
    PyModule_AddIntConstant( mod, "RGBA8",    GL_RGBA8    );
    PyModule_AddIntConstant( mod, "RGB10_A2", GL_RGB10_A2 );
    PyModule_AddIntConstant( mod, "RGBA12",   GL_RGBA12   );
    PyModule_AddIntConstant( mod, "RGBA16",   GL_RGBA16   );
#ifdef GL_BGR
    PyModule_AddIntConstant( mod, "BGR",      GL_BGR      );
#endif
#ifdef GL_BGRA
    PyModule_AddIntConstant( mod, "BGRA",     GL_BGRA     );
#endif

// buffer bits
    PyModule_AddIntConstant( mod, "COLOR_BUFFER_BIT",   GL_COLOR_BUFFER_BIT   );
    PyModule_AddIntConstant( mod, "DEPTH_BUFFER_BIT",   GL_DEPTH_BUFFER_BIT   );
    PyModule_AddIntConstant( mod, "STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT );
    PyModule_AddIntConstant( mod, "ACCUM_BUFFER_BIT",   GL_ACCUM_BUFFER_BIT   );

    PyModule_AddIntConstant( mod, "ALPHA_TEST",                  GL_ALPHA_TEST                  );
    PyModule_AddIntConstant( mod, "DEPTH_TEST",                  GL_DEPTH_TEST                  );
    PyModule_AddIntConstant( mod, "SCISSOR_TEST",                GL_SCISSOR_TEST                );
    PyModule_AddIntConstant( mod, "STENCIL_TEST",                GL_STENCIL_TEST                );

    PyModule_AddIntConstant( mod, "ALPHA",                       GL_ALPHA                       );
    PyModule_AddIntConstant( mod, "ALPHA4",                      GL_ALPHA4                      );
    PyModule_AddIntConstant( mod, "ALPHA8",                      GL_ALPHA8                      );
    PyModule_AddIntConstant( mod, "ALPHA12",                     GL_ALPHA12                     );
    PyModule_AddIntConstant( mod, "ALPHA16",                     GL_ALPHA16                     );
    PyModule_AddIntConstant( mod, "AUTO_NORMAL",                 GL_AUTO_NORMAL                 );
    PyModule_AddIntConstant( mod, "BLEND",                       GL_BLEND                       );
    PyModule_AddIntConstant( mod, "CLIP_PLANE0",                 GL_CLIP_PLANE0                 );
    PyModule_AddIntConstant( mod, "CLIP_PLANE1",                 GL_CLIP_PLANE1                 );
    PyModule_AddIntConstant( mod, "CLIP_PLANE2",                 GL_CLIP_PLANE2                 );
    PyModule_AddIntConstant( mod, "CLIP_PLANE3",                 GL_CLIP_PLANE3                 );
    PyModule_AddIntConstant( mod, "CLIP_PLANE4",                 GL_CLIP_PLANE4                 );
    PyModule_AddIntConstant( mod, "CLIP_PLANE5",                 GL_CLIP_PLANE5                 );
    PyModule_AddIntConstant( mod, "COLOR_LOGIC_OP",              GL_COLOR_LOGIC_OP              );
    PyModule_AddIntConstant( mod, "COLOR_MATERIAL",              GL_COLOR_MATERIAL              );

    PyModule_AddIntConstant( mod, "DITHER",                      GL_DITHER                      );
    PyModule_AddIntConstant( mod, "DONT_CARE",                   GL_DONT_CARE                   );
    PyModule_AddIntConstant( mod, "FASTEST",                     GL_FASTEST                     );
    PyModule_AddIntConstant( mod, "FLAT",                        GL_FLAT                        );
    PyModule_AddIntConstant( mod, "FOG",                         GL_FOG                         );
    PyModule_AddIntConstant( mod, "FOG_HINT",                    GL_FOG_HINT                    );
    PyModule_AddIntConstant( mod, "INDEX_LOGIC_OP",              GL_INDEX_LOGIC_OP              );
    PyModule_AddIntConstant( mod, "LINEAR",                      GL_LINEAR                      );
    PyModule_AddIntConstant( mod, "LINE_SMOOTH",                 GL_LINE_SMOOTH                 );
    PyModule_AddIntConstant( mod, "LINE_SMOOTH_HINT",            GL_LINE_SMOOTH_HINT            );
    PyModule_AddIntConstant( mod, "LINE_STIPPLE",                GL_LINE_STIPPLE                );
    PyModule_AddIntConstant( mod, "MAP1_COLOR_4",                GL_MAP1_COLOR_4                );
    PyModule_AddIntConstant( mod, "MAP1_INDEX",                  GL_MAP1_INDEX                  );
    PyModule_AddIntConstant( mod, "MAP1_NORMAL",                 GL_MAP1_NORMAL                 );
    PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_1",        GL_MAP1_TEXTURE_COORD_1        );
    PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_2",        GL_MAP1_TEXTURE_COORD_2        );
    PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_3",        GL_MAP1_TEXTURE_COORD_3        );
    PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_4",        GL_MAP1_TEXTURE_COORD_4        );
    PyModule_AddIntConstant( mod, "MAP1_VERTEX_3",               GL_MAP1_VERTEX_3               );
    PyModule_AddIntConstant( mod, "MAP1_VERTEX_4",               GL_MAP1_VERTEX_4               );
    PyModule_AddIntConstant( mod, "MAP2_COLOR_4",                GL_MAP2_COLOR_4                );
    PyModule_AddIntConstant( mod, "MAP2_INDEX",                  GL_MAP2_INDEX                  );
    PyModule_AddIntConstant( mod, "MAP2_NORMAL",                 GL_MAP2_NORMAL                 );
    PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_1",        GL_MAP2_TEXTURE_COORD_1        );
    PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_2",        GL_MAP2_TEXTURE_COORD_2        );
    PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_3",        GL_MAP2_TEXTURE_COORD_3        );
    PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_4",        GL_MAP2_TEXTURE_COORD_4        );
    PyModule_AddIntConstant( mod, "MAP2_VERTEX_3",               GL_MAP2_VERTEX_3               );
    PyModule_AddIntConstant( mod, "MAP2_VERTEX_4",               GL_MAP2_VERTEX_4               );
    PyModule_AddIntConstant( mod, "MODELVIEW",                   GL_MODELVIEW                   );
    PyModule_AddIntConstant( mod, "NICEST",                      GL_NICEST                      );
    PyModule_AddIntConstant( mod, "NORMALIZE",                   GL_NORMALIZE                   );
    PyModule_AddIntConstant( mod, "PERSPECTIVE_CORRECTION_HINT", GL_PERSPECTIVE_CORRECTION_HINT );
    PyModule_AddIntConstant( mod, "POINT_SMOOTH",                GL_POINT_SMOOTH                );
    PyModule_AddIntConstant( mod, "POINT_SMOOTH_HINT",           GL_POINT_SMOOTH_HINT           );
    PyModule_AddIntConstant( mod, "POLYGON_OFFSET_FILL",         GL_POLYGON_OFFSET_FILL         );
    PyModule_AddIntConstant( mod, "POLYGON_OFFSET_LINE",         GL_POLYGON_OFFSET_LINE         );
    PyModule_AddIntConstant( mod, "POLYGON_OFFSET_POINT",        GL_POLYGON_OFFSET_POINT        );
    PyModule_AddIntConstant( mod, "POLYGON_SMOOTH",              GL_POLYGON_SMOOTH              );
    PyModule_AddIntConstant( mod, "POLYGON_SMOOTH_HINT",         GL_POLYGON_SMOOTH_HINT         );
    PyModule_AddIntConstant( mod, "POLYGON_STIPPLE",             GL_POLYGON_STIPPLE             );
    PyModule_AddIntConstant( mod, "PROJECTION",                  GL_PROJECTION                  );
    PyModule_AddIntConstant( mod, "PROXY_TEXTURE_2D",            GL_PROXY_TEXTURE_2D            );

    PyModule_AddIntConstant( mod, "SMOOTH",                      GL_SMOOTH                      );

    PyModule_AddIntConstant( mod, "TEXTURE",                     GL_TEXTURE                     );
    PyModule_AddIntConstant( mod, "TEXTURE_1D",                  GL_TEXTURE_1D                  );
    PyModule_AddIntConstant( mod, "TEXTURE_2D",                  GL_TEXTURE_2D                  );
#ifdef GL_TEXTURE_3D
    PyModule_AddIntConstant( mod, "TEXTURE_3D",                  GL_TEXTURE_3D                  );
#endif
    PyModule_AddIntConstant( mod, "TEXTURE_GEN_Q",               GL_TEXTURE_GEN_Q               );
    PyModule_AddIntConstant( mod, "TEXTURE_GEN_R",               GL_TEXTURE_GEN_R               );
    PyModule_AddIntConstant( mod, "TEXTURE_GEN_S",               GL_TEXTURE_GEN_S               );
    PyModule_AddIntConstant( mod, "TEXTURE_GEN_T",               GL_TEXTURE_GEN_T               );
    PyModule_AddIntConstant( mod, "TEXTURE_MIN_FILTER",          GL_TEXTURE_MIN_FILTER          );
    PyModule_AddIntConstant( mod, "TEXTURE_MAG_FILTER",          GL_TEXTURE_MAG_FILTER          );

    PyModule_AddIntConstant( mod, "LUMINANCE",                   GL_LUMINANCE                   );
    PyModule_AddIntConstant( mod, "LUMINANCE4",                  GL_LUMINANCE4                  );
    PyModule_AddIntConstant( mod, "LUMINANCE8",                  GL_LUMINANCE8                  );
    PyModule_AddIntConstant( mod, "LUMINANCE12",                 GL_LUMINANCE12                 );
    PyModule_AddIntConstant( mod, "LUMINANCE16",                 GL_LUMINANCE16                 );
    PyModule_AddIntConstant( mod, "LUMINANCE_ALPHA",             GL_LUMINANCE_ALPHA             );
    PyModule_AddIntConstant( mod, "LUMINANCE4_ALPHA4",           GL_LUMINANCE4_ALPHA4           );
    PyModule_AddIntConstant( mod, "LUMINANCE6_ALPHA2",           GL_LUMINANCE6_ALPHA2           );
    PyModule_AddIntConstant( mod, "LUMINANCE8_ALPHA8",           GL_LUMINANCE8_ALPHA8           );
    PyModule_AddIntConstant( mod, "LUMINANCE12_ALPHA4",          GL_LUMINANCE12_ALPHA4          );
    PyModule_AddIntConstant( mod, "LUMINANCE12_ALPHA12",         GL_LUMINANCE12_ALPHA12         );
    PyModule_AddIntConstant( mod, "LUMINANCE16_ALPHA16",         GL_LUMINANCE16_ALPHA16         );
    PyModule_AddIntConstant( mod, "INTENSITY",                   GL_INTENSITY                   );
    PyModule_AddIntConstant( mod, "INTENSITY4",                  GL_INTENSITY4                  );
    PyModule_AddIntConstant( mod, "INTENSITY8",                  GL_INTENSITY8                  );
    PyModule_AddIntConstant( mod, "INTENSITY12",                 GL_INTENSITY12                 );
    PyModule_AddIntConstant( mod, "INTENSITY16",                 GL_INTENSITY16                 );

    PyModule_AddIntConstant( mod, "COLOR_INDEX",                 GL_COLOR_INDEX                 );
    PyModule_AddIntConstant( mod, "RED",                         GL_RED                         );
    PyModule_AddIntConstant( mod, "GREEN",                       GL_GREEN                       );
    PyModule_AddIntConstant( mod, "BLUE",                        GL_BLUE                        );

    PyModule_AddIntConstant( mod, "CULL_FACE",                   GL_CULL_FACE                   );

    PyModule_AddIntConstant( mod, "POINT",                    GL_POINT                    );
    PyModule_AddIntConstant( mod, "LINE",                     GL_LINE                     );
    PyModule_AddIntConstant( mod, "FILL",                     GL_FILL                     );

    PyModule_AddIntConstant( mod, "UNPACK_ALIGNMENT",         GL_UNPACK_ALIGNMENT         );

    PyModule_AddIntConstant( mod, "ZERO",                     GL_ZERO                     );
    PyModule_AddIntConstant( mod, "ONE",                      GL_ONE                      );
    PyModule_AddIntConstant( mod, "SRC_COLOR",                GL_SRC_COLOR                );
    PyModule_AddIntConstant( mod, "ONE_MINUS_SRC_COLOR",      GL_ONE_MINUS_SRC_COLOR      );
    PyModule_AddIntConstant( mod, "DST_COLOR",                GL_DST_COLOR                );
    PyModule_AddIntConstant( mod, "ONE_MINUS_DST_COLOR",      GL_ONE_MINUS_DST_COLOR      );
    PyModule_AddIntConstant( mod, "SRC_ALPHA",                GL_SRC_ALPHA                );
    PyModule_AddIntConstant( mod, "ONE_MINUS_SRC_ALPHA",      GL_ONE_MINUS_SRC_ALPHA      );
    PyModule_AddIntConstant( mod, "DST_ALPHA",                GL_DST_ALPHA                );
    PyModule_AddIntConstant( mod, "ONE_MINUS_DST_ALPHA",      GL_ONE_MINUS_DST_ALPHA      );
    PyModule_AddIntConstant( mod, "SRC_ALPHA_SATURATE",       GL_SRC_ALPHA_SATURATE       );

    PyModule_AddIntConstant( mod, "COLOR_ATTACHMENT0",        GL_COLOR_ATTACHMENT0        );
    PyModule_AddIntConstant( mod, "DEPTH_ATTACHMENT",         GL_DEPTH_ATTACHMENT         );
    PyModule_AddIntConstant( mod, "STENCIL_ATTACHMENT",       GL_STENCIL_ATTACHMENT       );
    PyModule_AddIntConstant( mod, "DEPTH_STENCIL_ATTACHMENT", GL_DEPTH_STENCIL_ATTACHMENT );

#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    PyModule_AddIntConstant( mod, "MAX_TEXTURE_MAX_ANISOTROPY_EXT", GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT );
#endif
    PyModule_AddIntConstant( mod, "TEXTURE_CUBE_MAP_SEAMLESS", GL_TEXTURE_CUBE_MAP_SEAMLESS );

    PyModule_AddIntConstant( mod, "ARRAY_BUFFER",         GL_ARRAY_BUFFER         );
    PyModule_AddIntConstant( mod, "ELEMENT_ARRAY_BUFFER", GL_ELEMENT_ARRAY_BUFFER );
}
