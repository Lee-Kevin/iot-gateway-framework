/* POINTER defines a generic pointer type */
typedef unsigned char * POINTER;
 
/* UINT2 defines a two byte word */
//typedef unsigned short int UINT2;
 
/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;
 
 
/* MD5 context. */
typedef struct {
	UINT4 state[4];                                   /* state (ABCD) */
	UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;
 
void MD5Init (MD5_CTX *context);
void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen);
void MD5UpdaterString(MD5_CTX *context,const char *string);
int MD5FileUpdateFile (MD5_CTX *context,char *filename);
void MD5Final (unsigned char digest[16], MD5_CTX *context);
void MDString (char *string,unsigned char digest[16]);
int MD5File (char *filename,unsigned char digest[16]);
char *MD5Sum(const char* filename,unsigned char output[33]);
