#include <mesh.h>
#include <io.h>
#include <obj.h>

mesh * mesh_create(const char *path)
{
  mesh *m = malloc(sizeof(mesh));
  char *data = read_file(path);
  fprintf(stderr, "%s", data); // print data
  m->vertices =  obj_parse_vertices(data);
  m->faces = obj_parse_faces(data);
  return m;
}

void mesh_delete(mesh *m)
{
  free_vertex_data(m->vertices);
  free_face_data(m->faces);
  free(m);  
}
