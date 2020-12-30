#include <face_data.h>
#include <stdlib.h>

struct face_data * create_face_data(int length, enum face_format format)
{
  struct face_data *fd = malloc(sizeof(struct face_data));
  fd->length = length;
  fd->format = format;
  
  fd->data_v = NULL;
  fd->data_vt = NULL;
  fd->data_vn = NULL;

  fd->data_v = malloc(sizeof(int) * length);
  if (format & FACE_FORMAT_VT)
    fd->data_vt = malloc(sizeof(int) * length);
  if (format & FACE_FORMAT_VN)
    fd->data_vn = malloc(sizeof(int) * length);

  return fd;
}

void free_face_data(struct face_data *f)
{
  free(f->data_v);
  free(f->data_vt);
  free(f->data_vn);
  free(f);
}
