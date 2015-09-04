#!/usr/bin/env python

import os, subprocess
import sys

UNIT_TESTS=None

c_api_only = "--c_api_only" in sys.argv
win_app_only = "--winapp_only" in sys.argv

if not c_api_only and "--test" in sys.argv:
  UNIT_TESTS = ["fixture_log_observer.cpp", "main.cpp", "mock_file_source.cpp", "util.cpp", "gtest-all.cc", "storage.cpp"]
  UNIT_TESTS += ["sprite_atlas.cpp", "sprite_image.cpp", "sprite_parser.cpp", "sprite_store.cpp"]
  UNIT_TESTS += ["annotations.cpp", "api_misuse.cpp", "repeated_render.cpp", "set_style.cpp"]
  UNIT_TESTS += ["custom_sprites.cpp", "headless.cpp"]
  UNIT_TESTS += ["assert.cpp", "bilinear.cpp", "binpack.cpp", "clip_ids.cpp", "comparisons.cpp", "enums.cpp", "functions.cpp", "geo.cpp", "map.cpp", "map_context.cpp", "mapbox.cpp", "merge_lines.cpp", "style_parser.cpp", "test_conversions.cpp", "thread.cpp", "tile.cpp", "transform.cpp", "variant.cpp", "work_queue.cpp"]
  # needs server #
  UNIT_TESTS += ["cache_response.cpp", "cache_revalidate.cpp", "database.cpp", "directory_reading.cpp", "file_reading.cpp", "http_cancel.cpp", "http_coalescing.cpp", "http_error.cpp", "http_header_parsing.cpp", "http_issue_1369.cpp", "http_load.cpp", "http_other_loop.cpp", "http_reading.cpp"]
  UNIT_TESTS += ["glyph_store.cpp", "pending_resources.cpp", "resources_loading.cpp", "sprite.cpp"]

BUILD_RENDER_EXE = UNIT_TESTS is None and not c_api_only and not win_app_only
BUILD_WINAPP_EXE = UNIT_TESTS is None or win_app_only
LINK_MAPBOX_GL_DLL = UNIT_TESTS is None and not win_app_only

RELEASE_FLAGS = ["-DNDEBUG", "-O3"]   # -g ?
DEBUG_FLAGS = ["-DDEBUG", "-Og", "-g"]
C_FLAGS = DEBUG_FLAGS + ["-fexceptions"] 
CPP_FLAGS = C_FLAGS + ["-std=c++14", "-frtti"]

FILES_TO_SKIP=0

scriptPath = os.path.dirname(os.path.abspath(__file__))
buildPath = os.path.join(scriptPath, "build", "mingw")
buildGenPath = os.path.join(buildPath, "gen")
file_index = 0

# Generate config.hpp
#
#a = subprocess.call(["git", "describe", "--tags", "--always", "--abbrev=0"])
#b = subprocess.call(["git", "rev-parse", "HEAD"])
subprocess.call(["python", "scripts/build-version.py", buildGenPath, "v0.5.2", "0fcfafe7b11df0555cf9ac728e7488b1f4de5428"])

# Find shaders
#
shaderList = []
for root, dirs, files in os.walk(r"src"):
  for fname in files:
    name, ext = os.path.splitext(fname)
  
    if not ext.lower() in [".glsl"]:
      continue
    else:
      fpath = os.path.join(scriptPath, root, fname)
      shaderList.append(fpath)

# Generate shaders
#
build_shaders_cmd = ["python", "scripts/build-shaders.py", buildGenPath] + shaderList
subprocess.call(build_shaders_cmd)

# Compile source files
#
OBJs = []
src_folders = ["src", os.path.join("platform", "default"), buildGenPath]

EXCLUDED_FILES = [ "asset_request_zip.cpp" ]

for src_folder in src_folders:

  for root, dirs, files in os.walk(src_folder):
    
    for fname in files:
      name, ext = os.path.splitext(fname)

      if fname in EXCLUDED_FILES:
        print "skipping '%s'" % os.path.join(root, fname)
        continue

      if not ext.lower() in [".c", ".cpp"]:
        continue

      in_path = os.path.join(root, fname)
      out_path = os.path.join(buildPath, "%s.o" % name)
     
      if file_index < FILES_TO_SKIP:
        file_index += 1
        continue

      if os.path.exists(out_path):
        OBJs.append(out_path)
        continue

      is_cpp = ext.lower() in [".cpp"]

      clang_cmd = [ "gcc" ]
      if is_cpp:
        clang_cmd += CPP_FLAGS
      else:
        clang_cmd += C_FLAGS

      #clang_cmd += ["-S"]
      #clang_cmd += ["-E"]
      #clang_cmd += ["-fPIC"]                                                                               # NOTE(nico) - from osx build log - useless for win32
      #clang_cmd += ["-femulated-tls"]                                                                      # NOTE(nico) - ?test? for 'undefined reference to `std::__once_call'
      clang_cmd += ["-pthread"]                                                                             # NOTE(nico) - otherwise linking fails
      clang_cmd += ["-D_USE_MATH_DEFINES"]                                                                  # NOTE(nico) - to define M_PI
      clang_cmd += ["-DMBGL_C_EXPORTS"]
      #clang_cmd += ["-DCURL_STATICLIB", "-ZIP_EXTERN="]                                                    # NOTE(nico) - for static linking
      clang_cmd += ["-DNU_BUILD_STATIC="]                                                                   # NOTE(nico) - to prevent `nunicode` to export its symbols...?
      clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
      clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-0.10.36", "include")]
      clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1")]
      clang_cmd += ["-I" + r"C:\mingw\lib\libzip\include"]                                                  # NOTE(nico) - ? for <zipconf.h>
      clang_cmd += ["-I" + r"C:\mingw\include"]                                                             # NOTE(nico) - ? for gcc only, which is suppose to look here
      clang_cmd += ["-I" + os.path.join(buildGenPath, "include")]                                           # NOTE(jeff) - for mbgl generated files
      clang_cmd += ["-c", "-o", out_path]
      clang_cmd += [in_path]

      print file_index, "**", clang_cmd
      code = subprocess.call(clang_cmd)
      if code == 0:
        OBJs.append(out_path)

      file_index += 1

assert len(OBJs) == len(set(OBJs))

# Link DLL
#
if LINK_MAPBOX_GL_DLL:
  clang_cmd = [ "g++", "-shared", "-pthread" ]
  clang_cmd += CPP_FLAGS
  if c_api_only:
    #clang_cmd += [ "-static" ]
    pass
  else:
    clang_cmd += [ "-Wl,--export-all-symbols" ]                                                               # NOTE(nico) - should be 'hidden' but...
  clang_cmd += [ "-Wl,--out-implib=libmapbox-gl.dll.a" ]
  clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-0.10.36") ]
  clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1", "_build", "libnu") ]
  clang_cmd += [ "-o", "libmapbox-gl.dll" ]
  clang_cmd += OBJs
  # IMPORTANT(nico) - must come *after* the input files
  # IMPORTANT(nico) - order is important for the linker
  clang_cmd += [ "-luv" ]
  clang_cmd += [ "-lnu" ]
  clang_cmd += [ "-lglew32" ]
  clang_cmd += [ "-lz", "-lzip", "-lcurl", "-ljpeg", "-lpng", "-lsqlite3", "-lglfw3" ]
  clang_cmd += [ "-lwldap32"]                                                                                # NOTE(nico) - for static lcurl
  clang_cmd += [ "-lwsock32", "-lws2_32", "-liphlpapi", "-lpsapi" ]                                          # NOTE(nico) - libuv-0.10.36
  clang_cmd += [ "-lopengl32", "-lgdi32" ]
  print clang_cmd
  subprocess.call(clang_cmd)

# Build `render.exe`
#
if BUILD_RENDER_EXE:
  clang_cmd = [ "g++", "-pthread" ]
  clang_cmd += CPP_FLAGS
  clang_cmd += [ "bin\\render.cpp" ]
  clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
  clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-0.10.36", "include")]
  clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1")]
  clang_cmd += ["-I" + r"C:\mingw\include"]                                                             # NOTE(nico) - ? for gcc only, which is suppose to look here
  clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-0.10.36") ]
  clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1", "_build", "libnu") ]
  clang_cmd += [ "-L." ]
  clang_cmd += [ "-o", "render.exe" ]
  # IMPORTANT(nico) - must come *after* the input files
  # IMPORTANT(nico) - order is important for the linker
  #clang_cmd += [ "-luv" ]
  clang_cmd += [ "-lwsock32", "-lws2_32", "-liphlpapi", "-lpsapi" ]                                          # NOTE(nico) - libuv-0.10.36
  clang_cmd += [ "-lmapbox-gl" ]
  #clang_cmd += [ "-Wl,-Bstatic", "-lboost_program_options" ]
  clang_cmd += [ "-lboost_program_options" ]
  print clang_cmd
  subprocess.call(clang_cmd)

# Build `winapp.exe`
#
if BUILD_WINAPP_EXE:
  clang_cmd = [ "g++", "-pthread" ]
  clang_cmd += CPP_FLAGS
  clang_cmd += [ "mingw\\main_without_MapThread.cpp" ]
  clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
  clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-0.10.36", "include")]
  clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1")]
  clang_cmd += ["-I" + r"C:\mingw\include"]                                                             # NOTE(nico) - ? for gcc only, which is suppose to look here
  clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-0.10.36") ]
  clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1", "_build", "libnu") ]
  clang_cmd += [ "-L." ]
  clang_cmd += [ "-o", "winapp.exe" ]
  # IMPORTANT(nico) - must come *after* the input files
  # IMPORTANT(nico) - order is important for the linker
  #clang_cmd += [ "-luv" ]
  clang_cmd += [ "-lwsock32", "-lws2_32", "-liphlpapi", "-lpsapi" ]                                          # NOTE(nico) - libuv-0.10.36
  clang_cmd += [ "-lmapbox-gl" ]
  #clang_cmd += [ "-Wl,-Bstatic", "-lboost_program_options" ]
  clang_cmd += [ "-lboost_program_options" ]
  print clang_cmd
  subprocess.call(clang_cmd)

##############################################################################
# Unit tests
#

OBJs = []
if UNIT_TESTS is None:
  src_folders = []
else:
  src_folders = ["test", os.path.join("deps", "gtest")]
EXCLUDED_FILES = []

for src_folder in src_folders:

  for root, dirs, files in os.walk(src_folder):
    
    for fname in files:
      name, ext = os.path.splitext(fname)

      if fname in EXCLUDED_FILES or (UNIT_TESTS is not None and fname not in UNIT_TESTS):
        print "skipping '%s'" % os.path.join(root, fname)
        continue

      if not ext.lower() in [".c", ".cpp", ".cc"]:
        continue

      in_path = os.path.join(root, fname)
      out_path = os.path.join(buildPath, "test", "%s.o" % name)

      if os.path.exists(out_path):
        OBJs.append(out_path)
        continue

      clang_cmd = [ "g++", "-pthread" ]
      clang_cmd += CPP_FLAGS
      clang_cmd += [ in_path ]
      
      #clang_cmd += [ r"deps\gtest\gtest-all.cc" ]
      #for fixtname in ["fixture_log_observer.cpp", "main.cpp", "mock_file_source.cpp", "util.cpp"]:
      #  clang_cmd += [ os.path.join("test", "fixtures", fixtname) ]

      clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
      clang_cmd += ["-I" + os.path.join(scriptPath, "deps", "gtest")]
      clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-0.10.36", "include")]
      clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1")]
      clang_cmd += ["-I" + r"C:\mingw\include"]                                                             # NOTE(nico) - ? for gcc only, which is suppose to look here
      clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-0.10.36") ]
      clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1", "_build", "libnu") ]
      clang_cmd += [ "-L." ]
      clang_cmd += ["-c", "-o", out_path]
      
      print clang_cmd
      code = subprocess.call(clang_cmd)
      if code == 0:
        OBJs.append(out_path)
  
clang_cmd = [ "g++", "-pthread" ]
clang_cmd += CPP_FLAGS
clang_cmd += [ "-o", "test.exe" ]
clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-0.10.36", "include")]
clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1")]
clang_cmd += ["-I" + r"C:\mingw\include"]                                                             # NOTE(nico) - ? for gcc only, which is suppose to look here
clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-0.10.36") ]
clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1", "_build", "libnu") ]
clang_cmd += [ "-L." ]
clang_cmd += OBJs
# IMPORTANT(nico) - must come *after* the input files
# IMPORTANT(nico) - order is important for the linker
#clang_cmd += [ "-luv" ]
clang_cmd += [ "-lwsock32", "-lws2_32", "-liphlpapi", "-lpsapi" ]                                          # NOTE(nico) - libuv-0.10.36
clang_cmd += [ "-lmapbox-gl" ]
#clang_cmd += [ "-Wl,-Bstatic", "-lboost_program_options" ]
clang_cmd += [ "-lboost_program_options" ]
print clang_cmd
subprocess.call(clang_cmd)
