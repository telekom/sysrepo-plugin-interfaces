#ifndef READ_H
#define READ_H

int read_from_proc_file(const char *dir_path, char *interface, const char *fn, int *val);

int read_from_sys_file(const char* dir_path, char* interface, int* val);

#endif /* READ_H */
