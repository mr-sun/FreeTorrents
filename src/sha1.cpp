/****************************************************************************
* FreeTorrents
* Copyright (C) 2009 Vitaly Kuznetsov <freetorrents@mrsun.ru>
*
* FreeTorrent is a standalone application which fakes upload and download stats
* of a torrent to almost all bittorrent trackers. http://ft.mrsun.ru
*
* This programm is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this software. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <vcl.h>
#pragma hdrstop

/*
SHA-1 in C
By Steve Reid <sreid@sea-to-sky.net>
100% Public Domain

Modified 2002-02 By Svante Seleborg
Code is now reentrant, i.e. thread safe. Fixed by adding
a workspace buffer to the SHA1_CTX struct if SHA1HANDSOFF
is defined, and passing the context pointer instead of
state pointer to SHA1Transfor.

Modified 7/98
By James H. Brown <jbrown@burgoyne.com>
Still 100% Public Domain

Modified 2006-12
By Alexander S. Pristenski
SHA1Printable() added just for my Babber project.
Still 100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

#include "sha1.h"

#define i386

/*************************************************************/

#ifndef  i386   /* For ALPHA  (SAK) */
#define LITTLE_ENDIAN 
typedef          long int int64;
typedef unsigned long int uint64;
typedef          int int32;
typedef unsigned int uint32;
#else  /*i386*/
#define LITTLE_ENDIAN 
#if defined(ORIGINALVERSION) // Svante Seleborg
typedef          long long int int64;
typedef unsigned long long int uint64;
#endif // Svante Seleborg
typedef          long int int32;
typedef unsigned long int uint32;
#endif /*i386*/

/* #include <process.h> */	/* prototype for exit() - JHB */
/* Using return() instead of exit() - SWR */

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#ifdef LITTLE_ENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


#ifdef VERBOSE  /* SAK */
/*----------------------------------------------------------------------------*/
void SHAPrintContext(SHA1_CTX *context, char *msg){
  printf("%s (%d,%d) %x %x %x %x %x\n",
	 msg,
	 context->count[0], context->count[1],
	 context->state[0],
	 context->state[1],
	 context->state[2],
	 context->state[3],
	 context->state[4]);
}
#endif

/*----------------------------------------------------------------------------*/
/* Hash a single 512-bit block. This is the core of the algorithm. */
void SHA1Transform(SHA1_CTX *context, unsigned char buffer[64])
{
    uint32 a, b, c, d, e;
    typedef union
    {
      unsigned char c[64];
      uint32 l[16];
    } CHAR64LONG16;

    CHAR64LONG16* block;

    unsigned long *pState = &context->state[0];

#ifdef SHA1HANDSOFF
    block = static_cast<CHAR64LONG16*>(memcpy(context->workspace, buffer, 64));
#else
    block = (CHAR64LONG16*) buffer;
#endif
    /* Copy pState[] to working vars */
    a = pState[0];
    b = pState[1];
    c = pState[2];
    d = pState[3];
    e = pState[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
    /* Add the working vars back into pState[] */
    pState[0] += a;
    pState[1] += b;
    pState[2] += c;
    pState[3] += d;
    pState[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
}

/*----------------------------------------------------------------------------*/
/* SHA1Init - Initialize new context */
void SHA1Init(SHA1_CTX* context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

/*----------------------------------------------------------------------------*/
/* Run your data through this. */
void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int len)	/* JHB */
{
uint32 i, j;	/* JHB */

#ifdef VERBOSE
    SHAPrintContext(context, "before");
#endif
    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3)) context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1Transform(context, context->buffer);
        for ( ; i + 63 < len; i += 64) {
            SHA1Transform(context, &data[i]);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
#ifdef VERBOSE
    SHAPrintContext(context, "after ");
#endif
}

/*----------------------------------------------------------------------------*/
/* Add padding and return the message digest. */
void SHA1Final(unsigned char digest[20], SHA1_CTX* context)
{
uint32 i;	/* JHB */
unsigned char finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
    SHA1Update(context, (unsigned char *)"\200", 1);
    while ((context->count[0] & 504) != 448) {
        SHA1Update(context, (unsigned char *)"\0", 1);
    }
    SHA1Update(context, finalcount, 8);  /* Should cause a SHA1Transform() */
    for (i = 0; i < 20; i++) {
        digest[i] = (unsigned char)
         ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
    /* Wipe variables */
    i = 0;	/* JHB */
	memset(context, 0, sizeof *context);		/* SS */
    memset(finalcount, 0, sizeof finalcount);	/* SWR & SS */
}

/*************************************************************/
/*
	Name: SHA1Printable()
	Description: makes SHA-1 fingerprint from string.
	Parameters:
				[out] char *printable_digest - SHA-1 fingerprint (41 bytes).
				 [in] char *input_string - various input string.

	Return value: none.
	Revision: 16.01.2007
*/
/*----------------------------------------------------------------------------*/
void SHA1Printable(char *printable_digest, char *input_string)
{
    SHA1_CTX context;
    unsigned char digest[20];

	SHA1Init(&context);
	SHA1Update(&context, (unsigned char *)input_string, lstrlen(input_string));
	SHA1Final(digest, &context);
    wsprintf(printable_digest, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7], digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15], digest[16], digest[17], digest[18], digest[19]);
}

/*************************************************************/

#if defined(ORIGINALVERSION) // Svante Seleborg

#if 1
/*----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
int i, j;
SHA1_CTX context;
unsigned char digest[20], buffer[16384];
FILE* file;

    if (argc > 2) {
        puts("Public domain SHA-1 implementation - by Steve Reid
<sreid@sea-to-sky.net>");
        puts("Modified for 16 bit environments 7/98 - by James H. Brown
<jbrown@burgoyne.com>");	/* JHB */
        puts("Produces the SHA-1 hash of a file, or stdin if no file is
specified.");
        return(0);
    }
    if (argc < 2) {
        file = stdin;
    }
    else {
        if (!(file = fopen(argv[1], "rb"))) {
            fputs("Unable to open file.", stderr);
            return(-1);
        }
    }
    SHA1Init(&context);
    while (!feof(file)) {  /* note: what if ferror(file) */
        i = fread(buffer, 1, 16384, file);
        SHA1Update(&context, buffer, i);
    }
    SHA1Final(digest, &context);
    fclose(file);
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 4; j++) {
            printf("%02X", digest[i*4+j]);
        }
        putchar(' ');
    }
    putchar('\n');
    return(0);	/* JHB */
}

#else

/*----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
int i, j,k;
SHA1_CTX context;
unsigned char digest[20], buffer[16384];
char output[80];
FILE* file;

 for(k = 0; k < 3; k++){ 
    SHA1Init(&context);
    SHA1Update(&context, test_data[k].data, strlen(test_data[k].data));
    SHA1Final(digest, &context);
    strcpy(output,"0x");
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 4; j++) {
	  char buf[4];
	  int offset = 2 + 2*(i*4+j);
            sprintf(buf,"%02X", digest[i*4+j]);
	    fprintf(stderr,"* offset = %d  buf = %s\n", offset, buf);
	    strcpy(output + offset, buf);
        }
    }
      fprintf(stderr,"* %d right %s (%d) , %s output (%d)\n",
	      k, test_data[k].hexkey, strlen(test_data[k].hexkey), output,
strlen(output));
    if(strcmp(test_data[k].hexkey, output)){
      fprintf(stderr,"* %d right %s != %s output\n",
	      k, test_data[k].hexkey, output);
    }else{
      fprintf(stderr,"* %d right %s == %s output\n",
	      k, test_data[k].hexkey, output);
    }      
 }
    return(0);	/* JHB */
}
#endif

#endif // Svante Seleborg


