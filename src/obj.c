#include <obj.h>
#include <mesh.h>
#include <str-utils.h>
#include <stdlib.h>
#include <string.h> /* strncpy */

/*
 * some considerations when interpreting face data/indices...
 *
 * indices may be negative (appears uncommon, but can happen).
 *
 * the face element can follow any of the following formats:
 * (1) f v1 v2 v3 ,
 * (2) f v1/vt1 ... , including a texture coordinate vt
 * (3) f v1/vt1/vn1 ... , including a normal coordinate vn
 * (4) f v1//vn1 ... , where the texture coordinate vt is unspecified
 *
 * the format is assumed to be consistent when reading a given object.
 * i.e. suppose your file has two objects defined, one is a cube and the
 * other an ico-sphere. suppose the cube defines faces using the first format (1)
 * consisting of vertex indices only - it would be unreasonable for the cube's 
 * face definitions to suddenly begin using, say, format (2); regardless of 
 * whether texture/normal coordinates are defined. however it wouldn't be unimaginable
 * for the second object defined in another group (ico-sphere) to use another format.
 * 
 * the final consideration is: the obj format may define any n sided face where n > 2.
 * unfortunately, this is a little inconvenient since we want all faces to be triangles.
 * it may be suitable for this function to return a n > 3 face and not worry about it for now,
 * later we can delegate to some 'triangulation' function.
 */

struct vertex_data * obj_parse_vertices(const char *data)
{
  const char *start = data;
  int num_vertices = 0;

  while (*(data = match_substring(data+1, "\nv "))) {
    num_vertices++;
  }

  struct vertex_data *vertices = create_vertex_data(num_vertices * 3);

  data = start;
  float *v_ptr = vertices->data_v;

  // FIX: if file begins with a vertex entry "v x y z \nv ..." then this first
  // entry will be ignored. solution: manually check data[0] for a "v "
  while (*(data = match_substring(data, "\nv "))) { // +1
    for (int i = 0; i < 3; i++) {
      data = match_any_char(data, "-0123456789");
      *(v_ptr++) = strtof(data, NULL);
      data = match_any_char(data, " \n"); // find the next whitespace or newline
    }
  }
  return vertices;
}

/*
 * convert n-gon to triangles
 * *--*
 * |\ |
 * | \|
 * *--*
 * if no triangulation necessary, just write the data to out.
 */
static void obj_triangulate(int *elems, int num_elems, int **out)
{
  if (num_elems < 4) {
    for (int i = 0; i < num_elems; i++) {
      *((*out)++) = elems[i];
    }
    fprintf(stderr, "\n");
  } else { // TODO: test this case
    int *write = *out;
    for (int i = 2; i < num_elems; i++) {
      // new triangle is faces[0] faces[i-1] faces[i]
      *(write++) = elems[0];
      *(write++) = elems[i-1];
      *(write++) = elems[i];
    }
  }
}

// TODO: handle errors
struct face_data * obj_parse_faces(const char *data)
{
  int length = obj_calc_final_face_element_amount(data);
  enum face_format face_format = obj_determine_face_format(data);
  struct face_data *face_data = create_face_data(length, face_format);
  const char *save = NULL;
  int *v_save = face_data->data_v;
  int *vt_save = face_data->data_vt;
  int *vn_save = face_data->data_vn;
  int num_elems = 3;
  int *elems = malloc(sizeof(int) * num_elems);
  int *elems_save;
  while (*(data = match_substring(data+1, "\nf "))) {
    elems_save = elems;
    save = data;
    int new_num_elems = obj_get_num_delimited_entries_on_line(data);
    if (new_num_elems > num_elems)
      elems = realloc(elems, sizeof(int) * new_num_elems);
    num_elems = new_num_elems;

    {
      for (int i = 0; i < num_elems; i++) {
	// get to the number
	data = match_any_char(data, "-1234567890");
	elems[i] = (int)strtol(data, NULL, 10)-1;
	fprintf(stderr, "%d, ", elems[i]);
	data = match_char(data, ' ');
      }
      obj_triangulate(elems, num_elems, &(face_data->data_v));
      elems = elems_save;
    }
    
    // if (face_format & FACE_FORMAT_VT) {
    //   data = save;
    //   for (int i = 0; i < num_elems; i++) {
    // 	// get to the number
    // 	data = match_char(data, '/');
    // 	data = match_any_char(data, "-1234567890");
    // 	elems[i] = (int)strtol(data, NULL, 10);
    // 	data = match_char(data, ' ');
    //   }
    //   obj_triangulate(elems, num_elems, &(face_data->data_vt));
    //   elems = elems_save;
    // }

    // if (face_format & FACE_FORMAT_VN) {
    //   data = save;
    //   for (int i = 0; i < num_elems; i++) {
    // 	// get to the number
    // 	data = match_char(data, '/');
    // 	data = match_char(data+1, '/');
    // 	data = match_any_char(data, "-1234567890");
    // 	elems[i] = (int)strtol(data, NULL, 10);
    // 	data = match_char(data, ' ');
    //   }
    //   obj_triangulate(elems, num_elems, &(face_data->data_vn));
    //   elems = elems_save;
    // }
    data = save;
  }
  face_data->data_v = v_save;
  // for (int i = 0; i < length; i++) fprintf(stderr, "%d\n", face_data->data_v[i]);
  // face_data->data_v = v_save;
  face_data->data_vt = vt_save;
  face_data->data_vn = vn_save;
  return face_data;
}

static enum face_format obj_determine_face_format(const char *data)
{
  data = match_substring(data, "\nf ");
  data = match_any_char(data, "-0123456789");
  const char *end = match_char(data, ' ');
  // unholy abomination... should i refactor?
  return (data = match_char_up_to(data+1, '/', end)) == end ?
    FACE_FORMAT_V : // scenario 1
    (data = match_char_up_to(data+1, '/', end)) == end ?
    FACE_FORMAT_V + FACE_FORMAT_VT : // scenario 2
    *(data-1) == '/' ? FACE_FORMAT_V + FACE_FORMAT_VN : FACE_FORMAT_V + FACE_FORMAT_VN + FACE_FORMAT_VT; // scenario 3 : 4
}

static int obj_calc_final_face_element_amount(const char *data)
{
  /* this function has to perform a calculation to determine
    the number of face indices post triangulation */
  // const char *start = data;
  int count = 0;
  while (*(data = match_substring(data+1, "\nf "))) {
    // count how many space delimited entries there are
    int d = obj_get_num_delimited_entries_on_line(data);
    count += (d - 2) * 3;
  }
  return count;
}

static int obj_get_num_delimited_entries_on_line(const char *data)
{
  int count = 0;
  // have to search for [-]digit before every delimiter search
  const char *end = match_char(data+1, '\n');
  while ((data = match_any_char_up_to(data, "-0123456789", end)) != end) {
    data = match_char_up_to(data, ' ', end);
    count++;
  }
  return count;
}
