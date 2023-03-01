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
-- TODO: figure out build instructions for macosx
supported_platforms = { "linux", "windows" }
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
      patches = {
        ["remove-winmain.diff"] = [==[
diff -Naur old/tigr/tigr.c new/tigr/tigr.c
--- old/tigr/tigr.c	2023-02-03 18:08:58.881163919 +0100
+++ new/tigr/tigr.c	2023-02-03 18:09:28.610295470 +0100
@@ -2609,31 +2609,6 @@
     win->lastChar = 0;
     return c;
 }
-
-// We supply our own WinMain and just chain through to the user's
-// real entry point.
-#ifdef UNICODE
-int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
-#else
-int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
-#endif
-{
-    int n, argc;
-    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
-    char** argv = (char**)calloc(argc + 1, sizeof(int));
-
-    (void)hInstance;
-    (void)hPrevInstance;
-    (void)lpCmdLine;
-    (void)nCmdShow;
-
-    for (n = 0; n < argc; n++) {
-        int len = WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, 0, 0, NULL, NULL);
-        argv[n] = (char*)malloc(len);
-        WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, argv[n], len, NULL, NULL);
-    }
-    return main(argc, argv);
-}
 #endif
 
 //////// End of inlined file: tigr_win.c ////////
]==]
      },
    },
  },
}

