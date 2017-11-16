package sz

/*
#include "glue.h"

// forward declaration for gateway functions
int readGo_cgo(void *data, int64_t size, int64_t *processed_size);
int seekGo_cgo(int64_t offset, int32_t whence, int64_t *new_position);
*/
import "C"
import (
	"fmt"
	"log"
	"os"
	"reflect"
	"time"
	"unsafe"

	"github.com/go-errors/errors"
)

var f *os.File

func Initialize() error {
	ret := C.libc7zip_initialize()
	if ret != 0 {
		return fmt.Errorf("could not initialize libc7zip")
	}

	lib := C.libc7zip_lib_new()
	if lib == nil {
		return fmt.Errorf("could not create new lib")
	}

	strm := C.libc7zip_in_stream_new()
	if strm == nil {
		return fmt.Errorf("could not create new in stream")
	}

	def := C.libc7zip_in_stream_get_def(strm)
	log.Printf("so uh, def is %v", def)

	var err error
	f, err = os.Open("main.7z")
	if err != nil {
		return errors.Wrap(err, 0)
	}

	stats, err := f.Stat()
	if err != nil {
		return errors.Wrap(err, 0)
	}

	def.size = C.int64_t(stats.Size())
	def.ext = C.CString(".7z")

	log.Printf("and now it's %v", def)

	def.read_cb = (C.read_cb_t)(unsafe.Pointer(C.readGo_cgo))
	def.seek_cb = (C.seek_cb_t)(unsafe.Pointer(C.seekGo_cgo))

	arch := C.libc7zip_archive_open(lib, strm)
	if arch == nil {
		return fmt.Errorf("could not open archive")
	}

	return nil
}

//export seekGo
func seekGo(offset int64, whence int32, newPosition unsafe.Pointer) int {
	log.Printf("in seekGo, should seek to %d, whence %d", offset, whence)
	ret, err := f.Seek(offset, int(whence))
	if err != nil {
		log.Printf("could not seek: %s", err.Error())
		return 1
	}

	newPosPtr := (*int64)(newPosition)
	*newPosPtr = ret
	log.Printf("seek ret = %d", ret)

	time.Sleep(1 * time.Second)
	return 0
}

//export readGo
func readGo(data unsafe.Pointer, size int64, processedSize unsafe.Pointer) int {
	log.Printf("in readGo, should read %d", size)

	// NOTE: this works
	// buf := (*(*[32 * 1024 * 1024]byte)(data))[:size:size]

	// NOTE: this is experimental:
	h := reflect.SliceHeader{
		Data: uintptr(data),
		Cap:  int(size),
		Len:  int(size),
	}
	buf := *(*[]byte)(unsafe.Pointer(&h))

	log.Printf("len(buf) = %d", len(buf))
	log.Printf("cap(buf) = %d", cap(buf))
	readBytes, err := f.Read(buf)
	if err != nil {
		log.Printf("could not read: %s", err.Error())
		return 1
	}

	log.Printf("read readBytes = %d", readBytes)

	processedSizePtr := (*int64)(processedSize)
	*processedSizePtr = int64(readBytes)

	time.Sleep(1 * time.Second)
	return 0
}