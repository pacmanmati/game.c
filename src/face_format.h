#pragma once

/* OBJ_FACE_FORMAT
 * the face element can follow any of the following formats:
 * (1) f v1 v2 v3 ,
 * (2) f v1/vt1 ... , including a texture coordinate vt
 * (3) f v1/vt1/vn1 ... , including a normal coordinate vn
 * (4) f v1//vn1 ... , where the texture coordinate vt is unspecified
 */

enum face_format {
  FACE_FORMAT_V = 1,  // 001
  FACE_FORMAT_VT = 2, // 010
  FACE_FORMAT_VN = 4, // 100
};
