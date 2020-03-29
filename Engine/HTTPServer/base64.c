#define OK                  (0)
#define FAIL                (-1)
#define BUFOVER             (-2)
#define CHAR64(c)           (((c) < 0 || (c) > 127) ? -1 : index_64[(c)])
static char     basis_64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????";
static char     index_64[128] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
  -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

int encode64(const char *_in, unsigned int inlen, char *_out, unsigned int outmax, unsigned int *outlen) {
  const unsigned char *in = (const unsigned char *) _in;
  unsigned char  *out = (unsigned char *) _out;
  unsigned char   oval;
  char           *blah;
  unsigned int    olen;

  olen = (inlen + 2) / 3 * 4;
  if (outlen) *outlen = olen;
  if (outmax < olen) return BUFOVER;

  blah = (char *) out;
  while (inlen >= 3)
  {
/* user provided max buffer size; make sure we don't go over it */
      *out++ = basis_64[in[0] >> 2];
      *out++ = basis_64[((in[0] << 4) & 0x30) | (in[1] >> 4)];
      *out++ = basis_64[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
      *out++ = basis_64[in[2] & 0x3f];
      in += 3;
      inlen -= 3;
  }
  if (inlen > 0)
  {
/* user provided max buffer size; make sure we don't go over it */
      *out++ = basis_64[in[0] >> 2];
      oval = (in[0] << 4) & 0x30;
      if (inlen > 1)
          oval |= in[1] >> 4;
      *out++ = basis_64[oval];
      *out++ = (inlen < 2) ? '=' : basis_64[(in[1] << 2) & 0x3c];
      *out++ = '=';
  }

  if (olen < outmax)
      *out = '\0';

  return OK;

}




int decode64(const char *in, unsigned inlen, char *out, unsigned *outlen) {
  unsigned int    len = 0, lup;
  int             c1, c2, c3, c4;

  if (in[0] == '+' && in[1] == ' ')
      in += 2;

  if (*in == '\0')
      return FAIL;

  for (lup = 0; lup < inlen / 4; lup++)
  {
      c1 = in[0];
      if (CHAR64(c1) == -1) return FAIL;
      c2 = in[1];
      if (CHAR64(c2) == -1) return FAIL;
      c3 = in[2];
      if (c3 != '=' && CHAR64(c3) == -1) return FAIL;
      c4 = in[3];
      if (c4 != '=' && CHAR64(c4) == -1) return FAIL;
      in += 4;
      *out++ = (CHAR64(c1) << 2) | (CHAR64(c2) >> 4);
      ++len;
      if (c3 != '=')
      {
          *out++ = ((CHAR64(c2) << 4) & 0xf0) | (CHAR64(c3) >> 2);
          ++len;
          if (c4 != '=')
          {
              *out++ = ((CHAR64(c3) << 6) & 0xc0) | CHAR64(c4);
              ++len;
          }
      }
  }

  *out = 0;
  *outlen = len;

  return OK;

}
