
#define GLUE_IMPLEMENT 1
#include "glue.h"

#include "_cgo_export.h"

#include <stdio.h>

#ifdef __WIN32
#include <windows.h>
#endif

#define LOADSYM(sym) { \
  sym ## _ = (void*) GetProcAddress(dynlib, #sym); \
  if (! sym ## _) { \
    fprintf(stderr, "Could not load symbol %s\n", #sym); \
    fflush(stderr); \
    return 1; \
  } \
}

HMODULE dynlib;

int libc7zip_initialize() {
  dynlib = LoadLibrary("libc7zip.dll");
  if (!dynlib) {
    fprintf(stderr, "Could not load libc7zip.dll\n");
    fflush(stderr);
    return 1;
  }

  LOADSYM(lib_new)
  LOADSYM(in_stream_new)
  LOADSYM(in_stream_get_def)
  LOADSYM(archive_open)
  LOADSYM(archive_get_item_count)

  return 0;
}

lib *libc7zip_lib_new() {
  return lib_new_();
}

in_stream *libc7zip_in_stream_new() {
  return in_stream_new_();
}

in_stream_def *libc7zip_in_stream_get_def(in_stream *s) {
  return in_stream_get_def_(s);
}

archive *libc7zip_archive_open(lib *l, in_stream *s) {
  return archive_open_(l, s);
}

int64_t libc7zip_archive_get_item_count(archive *a) {
  return archive_get_item_count_(a);
}

// Gateway functions

int seekGo_cgo(int64_t offset, int32_t whence, int64_t *new_position) {
  return seekGo(offset, whence, new_position);
}

int readGo_cgo(void *data, int64_t size, int64_t *processed_size) {
  return readGo(data, size, processed_size);
}
