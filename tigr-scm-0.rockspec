package="tigr"
version="scm-0"
source = {
  url = "gitrec+https://github.com/siffiejoe/lua-tigr.git",
}
description = {
  summary = "Binding to Tigr.",
  detailed = [[
  ]],
  homepage = "https://github.com/siffiejoe/lua-tigr/",
  license = "MIT",
}
-- TODO: figure out build instructions for windows and macosx
supported_platforms = { "linux" }
dependencies = {
  "lua >= 5.1, < 5.5",
  "luarocks-fetch-gitrec",
}
external_dependencies = {
  platforms = {
    linux = {
      GL = {
        header = "GL/gl.h",
        library = "GL",
      },
      X11 = {
        header = "X11/X.h",
        library = "X11",
      },
    },
  },
}
build = {
  type = "builtin",
  modules = {
    tigr = {
      sources = {
        "src/ltigr.c",
        "tigr/tigr.c",
        "moon/moon.c",
        "compat-5.3/c-api/compat-5.3.c",
      },
      defines = {
        "MOON_PREFIX=ltigr",
        "COMPAT53_PREFIX=ltigr",
      },
    },
  },
  platforms = {
    linux = {
      modules = {
        tigr = {
          incdirs = {
            "tigr",
            "moon",
            "compat-5.3/c-api",
            "$(GL_INCDIR)",
            "$(X11_INCDIR)",
          },
          libdirs = {
            "$(GL_LIBDIR)",
            "$(X11_LIBDIR)",
          },
          libraries = {
            "GLU",
            "GL",
            "X11",
          },
        },
      },
    },
    windows = {
      modules = {
        tigr = {
          incdirs = {
            "tigr",
            "moon",
            "compat-5.3/c-api",
          },
          libraries = {
            "opengl32",
            "gdi32",
          },
        },
      },
    },
  },
}

