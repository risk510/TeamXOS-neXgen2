/*============================================================================
 * filehdlr.c
 * Internal filesystem utilities.
 * M. Tim Jones <mtj@mtjones.com>
 *==========================================================================*/

#include "httpd-filehdlr.h"
#include "httpd-log.h"

unsigned char *filedata;
DWORD filedata_size;

/*----------------------------------------------------------------------------
 * lookupFilename() - Locate a file in the internal file system
 *--------------------------------------------------------------------------*/
int lookupFilename(char *filename, struct fileHdrStruct *filehdr)
{
  unsigned long offset = 0;
  unsigned long size;
  unsigned long i, found;
  int ret;

  while (offset < filedata_size) {

    ret = offset;
    found = 1;

    if ((filedata[offset] == 0xfa) && (filedata[offset+1] == 0xf3)) {

		/* Skip the header */
		offset+=2;

		/* Search the file name, but don't stop if not found... */
		for (i = 0 ; filedata[i+offset] != 0 && offset < filedata_size ; i++) {
			if(filename[i]>0 && filedata[i+offset]>0) {
				if (tolower(filename[i]) != tolower(filedata[i+offset])) {
					found = 0;
				}
			} else {
				found=0;
			}
		}

		if(offset >= filedata_size) return(0);
      /* Skip the filename and null terminator */
      offset += (i+1);

      /* Get the file size */
      size = (filedata[offset  ] << 24) | (filedata[offset+1] << 16) |
             (filedata[offset+2] <<  8) | (filedata[offset+3]);

      if (found) {
        filehdr->hdrStart = ret;
        filehdr->size = size;
        filehdr->fileStart = (offset+4);
//        LogString(LOG_EFS, "Open ", filename);
        return(1);
      }

      offset += (size+4);
    }

  }
  return(0);
}

