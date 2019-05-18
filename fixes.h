static char* strdup(const char* s) {
  size_t size = strlen(s);
  char* r = (char*)malloc(size + 1);
  memcpy(r, s, size + 1);
  return r;
}
