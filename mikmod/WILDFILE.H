#ifndef WILDFILE_H
#define WILDFILE_H

#ifdef __cplusplus
extern "C" {
#endif

char *GetFirstName(char *wildname,int attrib);
char *GetNextName(void);
void MyGlob(int *argc,char **argv[],int attrib);

#ifdef __cplusplus
}
#endif

#endif
