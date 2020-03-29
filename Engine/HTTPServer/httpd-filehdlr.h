/*============================================================================
 * filehdlr.h 
 * Internal file system structures and function prototypes.
 * M. Tim Jones <mtj@mtjones.com>
 *==========================================================================*/

#ifndef _FILEHDLR_H
#define _FILEHDLR_H

#ifdef __cplusplus
extern "C" {
#endif

struct fileHdrStruct {
  unsigned long  hdrStart;
  unsigned long  size;
  unsigned long  fileStart;
};

int lookupFilename(char *, struct fileHdrStruct *);

#ifdef __cplusplus
}
#endif

#endif /* _FILEHDLR_H */

