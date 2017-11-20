
#ifndef LIBC7ZIP_H
#define LIBC7ZIP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct lib;
typedef struct lib lib;
lib *lib_new();
void lib_free(lib *l);

typedef int (*read_cb_t)(int64_t id, void *data, int64_t size, int64_t *processed_size);
typedef int (*seek_cb_t)(int64_t id, int64_t offset, int32_t whence, int64_t *new_position);
typedef int (*write_cb_t)(int64_t id, const void *data, int64_t size, int64_t *processed_size);

typedef struct in_stream_def {
  int64_t id;
	seek_cb_t seek_cb;
	read_cb_t read_cb;
  char *ext;
  int64_t size;
} in_stream_def;

typedef struct out_stream_def {
  int64_t id;
  write_cb_t write_cb;
} out_stream_def;

struct in_stream;
typedef struct in_stream in_stream;
in_stream *in_stream_new();
in_stream_def *in_stream_get_def(in_stream *is);
void in_stream_commit_def(in_stream *is);
void in_stream_free(in_stream *is);

struct out_stream;
typedef struct out_stream out_stream;
out_stream *out_stream_new();
out_stream_def *out_stream_get_def(out_stream *s);
void out_stream_free(out_stream *s);

struct archive;
typedef struct archive archive;
archive *archive_open(lib *l, in_stream *is);
int64_t archive_get_item_count(archive *a);

// copied from lib7zip.h so we don't have to include it
enum property_index {
  PROP_INDEX_BEGIN,

  kpidPackSize = PROP_INDEX_BEGIN, //(Packed Size)
  kpidAttrib, //(Attributes)
  kpidCTime, //(Created)
  kpidATime, //(Accessed)
  kpidMTime, //(Modified)
  kpidSolid, //(Solid)
  kpidEncrypted, //(Encrypted)
  kpidUser, //(User)
  kpidGroup, //(Group)
  kpidComment, //(Comment)
  kpidPhySize, //(Physical Size)
  kpidHeadersSize, //(Headers Size)
  kpidChecksum, //(Checksum)
  kpidCharacts, //(Characteristics)
  kpidCreatorApp, //(Creator Application)
  kpidTotalSize, //(Total Size)
  kpidFreeSpace, //(Free Space)
  kpidClusterSize, //(Cluster Size)
  kpidVolumeName, //(Label)
  kpidPath, //(FullPath)
  kpidIsDir, //(IsDir)
  kpidSize, //(Uncompressed Size)

  PROP_INDEX_END
};

struct item;
typedef struct item item;
item *archive_get_item(archive *a, int64_t index);
char *item_get_string_property(item *i, int32_t property_index);
uint64_t item_get_uint64_property(item *i, int32_t property_index);
int32_t item_get_bool_property(item *i, int32_t property_index);
void item_free(item *i);
int archive_extract_item(archive *a, item *i, out_stream *os);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // LIBC7ZIP_H