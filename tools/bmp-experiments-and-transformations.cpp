//
// .bmp experiments and transformations
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <map>

#define BUF_SIZE 20000000

#pragma pack(1)
struct BmpHeader
{
    uint16_t  bm;               // always 'B', 'M'
    uint32_t  file_len;         // should equal hdr_len + data_len
    uint16_t  res1;             // res = various reserved / don't care fields
    uint16_t  res2;
    uint32_t  hdr_len;          // always 54
    uint32_t  sub_hdr_len;      // always 40
    uint32_t  width;
    uint32_t  height;
    uint16_t  nbr_planes;       // 1
    uint16_t  bits_per_pixel;   // 24
    uint32_t  res3;
    uint32_t  data_len;
    uint32_t  res4;
    uint32_t  res5;
    uint32_t  res6;
    uint32_t  res7;
};

/* struct CompressedXpm is the basis of a simple bespoke bmp compression scheme. We want
    to have a decent set of pre-rendered chess graphic bitmaps and in .xpm form they are
    very big. This simple scheme encodes in about 7 times less space in the executable
    program.
  
    Compression system works for up to 17 colours (a background colour plus up to 16
    other colours) - uses run length encoding and nibble coding for the non-background
    colours.

    Byte by byte coding system
    --------------------------
    up to 2^3  =8    successive codes cccc                0nnncccc
    up to 2^9  =512  successive codes cccc                100nnnnn nnnncccc
    up to 2^13 =8192 successive background codes          101nnnnn nnnnnnnn
    up to 2^5  =32   arbitrary codes                      110nnnnn followed by nibble codes
    up to 2^13 =8192 arbitrary codes                      111nnnnn nnnnnnnn followed by nibble codes

    In this cccc is a nibble code 0-15 and encodes a colour. And nnnn is a run length code
    that encodes from 1 to 2^m for an m bit field. So the code includes an offset of 1, so
    for example nnn=000 encodes 1 not 0, which means single byte 0000cccc encodes one pixel
    of colour cccc. Nibble encoding is least significant nibble first.

    Kludge added later: Support an 18th colour (it turned out we needed to) using a quirk
    of the coding system. Code 110nnnnn is never used for nnnnn=00000 since one code can
    be more efficiently encoded with 0nnncccc for nnn=000. So make use of the spare code
    freed up (=0xc0) to directly represent the 18th colour. I dynamically calculate the least
    frequently encountered colour code to be represented by the kludge code.

*/


struct CompressedXpm
{
    uint32_t width;
    uint32_t height;
    uint32_t background_colour;
    uint32_t kludge_colour;
    int      kludge_idx;
    int      nbr_other_colours;
    uint32_t other_colours[16];
    uint32_t binary_len;
    uint8_t  *bin;
};

// CompressedXpmProcessor decompresses into a single char **xpm pointer that can be used
//  for xpm -> bitmap generation within wxWidgets
class CompressedXpmProcessor
{
public:
    CompressedXpmProcessor( const CompressedXpm *encoded );
    ~CompressedXpmProcessor();
    char **GetXpm()       {return xpm;}
    int    GetXpmLen()    {return nbr_xpm_strings;}
private:
    char **xpm;
    int  nbr_xpm_strings; 
    char *string_storage;
};


static BmpHeader *ReadFile( char *buf, const char *filename );
static void Convert2Xpm( BmpHeader *in, const char *filename );
static void DrawTextPicture( BmpHeader *in );
static void SliceOutPieces( BmpHeader *in );
static void SpliceFiles( char *buf );

static bool Bmp2CompressedXpm( BmpHeader *in, const char *filename, CompressedXpm &compressed );
static void CompressedXpm2Xpm( CompressedXpm &compressed, const char *filename );
static void CompressedXpm2CHeader( CompressedXpm &compressed, const char *filename, bool append );
static void StaticTest();

int main()
{
    char *buf = (char *)malloc(BUF_SIZE);
    if( !buf )
    {
        printf( "Cannot allocate memory\n" );
        return 0;
    }
//#define SPLICE_FILES
#ifdef  SPLICE_FILES
    SpliceFiles( buf );
#endif
//#define STATIC_TEST
#ifdef STATIC_TEST
    StaticTest();
#endif
//#define ONE_FILE
#ifdef ONE_FILE
    const char *filename = "pitch-070-batch2.bmp";
    BmpHeader *in = ReadFile( buf, filename );
    if( in )
    {
        CompressedXpm compressed;
        if( Bmp2CompressedXpm(in,filename,compressed) )
        {
            CompressedXpm2CHeader(compressed,"pitch-070-batch2-compressed-xpm.h",false);
            //CompressedXpm2Xpm(compressed,filename);
            free(compressed.bin);
        }
    }
#endif
//#define ONE_ICON
#ifdef ONE_ICON
    const char *filename = "../icon.bmp";
    BmpHeader *in = ReadFile( buf, filename );
    if( in )
    {
        Convert2Xpm( in, "icon.xmp" );
    }
#endif
#define BULK_FILES
#ifdef BULK_FILES
    const char *files[] =
    {
     /* "../adobe5-14-20.bmp",
        "../adobe5-22-50.bmp",
        "../adobe5-55-95.bmp",
        "../adobe5-100-130.bmp",
        "../adobe5-140-160.bmp",
        "../adobe5-160-180.bmp",
        "../adobe5-190-200.bmp"
        ,
        "../font-14-20.bmp",
        "../font-22-50.bmp",
        "../font-55-95.bmp",
        "../font-100-130.bmp",
        "../font-140-160w.bmp",
        "../font-160b-180w.bmp",
        "../font-180b-200w.bmp",
        "../font-200b.bmp"

        "pitch-015-batch2.bmp",
        "pitch-016-batch2.bmp",
        "pitch-018-2-batch2.bmp",
        "pitch-018-batch2.bmp",
        "pitch-021-batch2.bmp",
        "pitch-023-batch2.bmp",
        "pitch-025-batch2.bmp",
        "pitch-027-batch1.bmp",
        "pitch-027-batch2.bmp",
        "pitch-029-batch1.bmp",
        "pitch-029-batch2.bmp",
        "pitch-030-batch1.bmp",
        "pitch-031-batch2.bmp",
        "pitch-032-batch1.bmp",
        "pitch-033-batch2.bmp",
        "pitch-035-batch1.bmp",
        "pitch-035-batch2.bmp",
        "pitch-037-batch1.bmp",
        "pitch-037-batch2.bmp",
        "pitch-039-batch2.bmp",
        "pitch-040-batch1.bmp",
        "pitch-040-batch2.bmp",
        "pitch-042-batch2.bmp",
        "pitch-043-batch1.bmp",
        "pitch-045-batch2.bmp",
        "pitch-046-batch1.bmp",
        "pitch-046-batch2.bmp",
        "pitch-048-batch2.bmp",
        "pitch-050-batch1.bmp",
        "pitch-050-batch2.bmp",
        "pitch-052-batch1.bmp",
        "pitch-055-batch2.bmp",
        "pitch-056-batch1.bmp",
        "pitch-059-batch1.bmp",
        "pitch-060-batch2.bmp",
        "pitch-062-batch1.bmp",
        "pitch-065-batch1.bmp",
        "pitch-065-batch2.bmp",
        "pitch-068-batch1.bmp",
        "pitch-070-batch2.bmp",
        "pitch-071-batch1.bmp",
        "pitch-074-batch1.bmp",
        "pitch-075-batch2.bmp",
        "pitch-077-batch1.bmp",
        "pitch-080-batch2.bmp",
        "pitch-085-batch1.bmp",
        "pitch-085-batch2.bmp",
        "pitch-090-batch2.bmp",
        "pitch-092-batch1.bmp",
        "pitch-095-batch2.bmp",
        "pitch-100-batch1.bmp",
        "pitch-100-batch2.bmp",
        "pitch-107-batch1.bmp",
        "pitch-110-batch2.bmp",
        "pitch-115-batch1.bmp",
        "pitch-120-batch2.bmp",
        "pitch-123-batch1.bmp",
        "pitch-130-batch1.bmp",
        "pitch-130-batch2.bmp",
        "pitch-138-batch1.bmp",
        "pitch-140-batch2.bmp",
        "pitch-146-batch1.bmp",
        "pitch-150-batch2.bmp",
        "pitch-154-batch1.bmp",
        "pitch-160-batch2.bmp",
        "pitch-169-batch1.bmp",
        "pitch-170-batch2.bmp",
        "pitch-180-batch2.bmp",
        "pitch-185-batch1.bmp",
        "pitch-190-batch2.bmp",
        "pitch-200-batch2.bmp",
        "pitch-215-batch1.bmp",
        "pitch-231-batch1.bmp",
        "pitch-246-batch1.bmp",
        "pitch-261-batch1.bmp",
        "pitch-277-batch1.bmp",
        "pitch-292-batch1.bmp",
        "pitch-306-batch1.bmp" */

        "pitch-015-batch2.bmp",
        "pitch-018-2-batch2.bmp",
        "pitch-021-batch2.bmp",
        "pitch-025-batch2.bmp",
        "pitch-029-batch2.bmp",
        "pitch-033-batch2.bmp",
        "pitch-037-batch2.bmp",
        "pitch-040-batch2.bmp",
        "pitch-042-batch2.bmp",
        "pitch-045-batch2.bmp",
        "pitch-050-batch2.bmp",
        "pitch-055-batch2.bmp",
        "pitch-060-batch2.bmp",
        "pitch-065-batch2.bmp",
        "pitch-070-batch2.bmp",
        "pitch-075-batch2.bmp",
        "pitch-080-batch2.bmp",
        "pitch-085-batch2.bmp",
        "pitch-090-batch2.bmp",
        "pitch-095-batch2.bmp",
        "pitch-100-batch2.bmp",
        "pitch-110-batch2.bmp",
        "pitch-120-batch2.bmp",
        "pitch-130-batch2.bmp",
        "pitch-140-batch2.bmp",
        "pitch-150-batch2.bmp",
        "pitch-160-batch2.bmp",
        "pitch-170-batch2.bmp",
        "pitch-180-batch2.bmp",
        "pitch-190-batch2.bmp",
        "pitch-200-batch2.bmp",
        "pitch-215-batch1.bmp",
        "pitch-231-batch1.bmp",
        "pitch-246-batch1.bmp",
        "pitch-261-batch1.bmp",
        "pitch-277-batch1.bmp",
        "pitch-292-batch1.bmp",
        "pitch-306-batch1.bmp" 

/*
        "pitch-015-batch2.bmp",
        "pitch-023-batch2.bmp",
        "pitch-031-batch2.bmp",
        "pitch-040-batch2.bmp",
        "pitch-050-batch2.bmp",
        "pitch-060-batch2.bmp",
        "pitch-070-batch2.bmp" */

    };
    for( int i=0; i<sizeof(files)/sizeof(files[0]); i++ )
    {
        const char *filename = files[i];
        BmpHeader *in = ReadFile( buf, filename );
        if( in )
        {
            //DrawTextPicture(in);
            //SliceOutPieces(in);
            //Convert2Xpm( in, files[i] );
            CompressedXpm compressed;
            if( Bmp2CompressedXpm(in,filename,compressed) )
            {
                std::string s(filename);
                std::string hdr_filename = "../../../src/bitmaps/" + s.substr(0,s.length()-4) + "-compressed.h";
                CompressedXpm2CHeader(compressed,hdr_filename.c_str(),false);
                //CompressedXpm2Xpm(compressed,filename);
                free(compressed.bin);
            }
        }
    }
#endif
    free(buf);
    printf( "Press enter\n" );
    getchar();
    return 0;
}

// Return bool ok
static BmpHeader *ReadFile( char *buf, const char *filename )
{
    FILE *f = fopen( filename, "rb" );
    if( !f )
    {
        printf( "Cannot open %s\n", filename );
        return 0;
    }
    uint32_t len = fread( buf, 1, BUF_SIZE, f );
    if( len < sizeof(BmpHeader) )
    {
        printf( "Ill formed BmpHeader\n" );
        return 0;
    }
/*  uint8_t *b = (uint8_t *)buf;
    for( int i=0; i<sizeof(BmpHeader); i++ )
    {
        printf( "%02x", *b++ );
        if( (i+1) == sizeof(BmpHeader) )
            printf("\n");
        else if( (i+1) % 16 == 0 )
            printf("\n");
        else
            printf(" ");
    } */  
    BmpHeader *in = (BmpHeader *)buf;
    bool ok=true;
    if( in->bm != 0x4d42 )
    {
        ok = false;
        printf( "Signature? (%04x)\n", in->bm );
    }
    if( in->file_len != len )       
    {
        ok = false;
        printf( "File len? (%x)\n", in->file_len );
    }
    if( in->hdr_len != sizeof(BmpHeader) )
    {
        ok = false;
        printf( "Header len? (%x)\n", in->hdr_len );
    }
    if( in->hdr_len + in->data_len != in->file_len )
    {
        ok = false;
        printf( "Data len? (%x)\n", in->data_len );
    }
    if( in->sub_hdr_len != 40 )
    {
        ok = false;
        printf( "Sub BmpHeader len? (%x)\n", in->sub_hdr_len );
    }
    if( in->nbr_planes != 1 )
    {
        ok = false;
        printf( "Nbr planes? (%d)\n", in->nbr_planes );
    }
    if( in->bits_per_pixel != 24 )
    {
        ok = false;
        printf( "Bits per pixel? (%d)\n", in->bits_per_pixel );
    }
    uint32_t row_bytes = 3*in->width;
    if( (row_bytes&3) != 0 )
        row_bytes = (row_bytes|3) + 1;
    if( in->height * row_bytes != in->data_len )
    {
        ok = false;
        printf( "Height(%d) and width(%d) don't match data_len(%d)?\n", in->height, in->width, in->data_len );
    }
    fclose(f);
    return ok ? in : 0;
}

static uint8_t *Locate( BmpHeader *h, uint32_t y )
{
    uint32_t row_bytes = 3*h->width;
    if( (row_bytes&3) != 0 )
        row_bytes = (row_bytes|3) + 1;
    uint8_t *ptr = (uint8_t *)h + sizeof(BmpHeader) + h->data_len - (y+1)*row_bytes;
    return ptr;
}

static uint8_t *Locate( BmpHeader *h, uint32_t x, uint32_t y )
{
    uint32_t row_bytes = 3*h->width;
    if( (row_bytes&3) != 0 )
        row_bytes = (row_bytes|3) + 1;
    uint8_t *ptr = (uint8_t *)h + sizeof(BmpHeader) + h->data_len - (y+1)*row_bytes + 3*x;
    return ptr;
}


// Just to establish confidence, draw a text picture, assumes a magenta background
static void DrawTextPicture( BmpHeader *in )
{
    printf( "height=%d, width=%d\n", in->height, in->width );
    FILE *f = fopen("picture.txt","wt");
    if( !f )
    {
        printf( "Cannot open picture.txt\n" );
        return;
    }
    char *buf = (char *)malloc(BUF_SIZE/10);
    if( !buf )
    {
        printf( "Cannot allocate memory\n" );
        return;
    }
    for( uint32_t y=0; y<in->height; y++ )
    {
        uint8_t *ptr = Locate(in,y);
        char *s = buf;
        for( uint32_t x=0; x<in->width; x++ )
        {
            uint8_t b = *ptr++;    
            uint8_t g = *ptr++;    
            uint8_t r = *ptr++;
            bool magenta = (r==255 && g==0 && b==255);    
            char c = (magenta?' ':'*');
            *s++ = c;
        }
        *s = '\0';
        //printf( "%s\n", buf );
        fprintf( f, "%s\n", buf );
    }
    free(buf);
    fclose(f);
}

static bool TestRowBlank( BmpHeader *in, uint32_t y )
{
    bool blank=true;
    uint8_t *ptr = Locate(in,y);
    for( uint32_t x=0; x<in->width && blank; x++ )
    {
        uint8_t b = *ptr++;    
        uint8_t g = *ptr++;    
        uint8_t r = *ptr++;
        bool magenta = (r==255 && g==0 && b==255);    
        if( !magenta )
            blank = false;
    }
    return blank;
}

static bool TestColBlank( BmpHeader *in, uint32_t x, uint32_t y_begin, uint32_t y_end )
{
    bool blank=true;
    for( uint32_t y=y_begin; y<y_end && blank; y++ )
    {
        uint8_t *ptr = Locate(in,y);
        ptr += 3*x;
        uint8_t b = *ptr++;    
        uint8_t g = *ptr++;    
        uint8_t r = *ptr++;
        bool magenta = (r==255 && g==0 && b==255);    
        if( !magenta )
            blank = false;
    }
    return blank;
}

struct Piece
{
    uint32_t src_x;
    uint32_t src_y;
    uint32_t dst_x;
    uint32_t dst_y;
    uint32_t w;
    uint32_t h;
};

static void WritePieceLine( BmpHeader *in, uint32_t pitch, int nbr_pieces, Piece *pieces )
{
    static int pitches[1000];
    char fname[100];
    sprintf( fname, "pieces%spitch-%d.bmp", nbr_pieces==12?"-":"-half-", pitch );
    if( pitch < 1000 )
    {
        pitches[pitch]++;
        if( pitches[pitch] > 1 )
            sprintf( fname, "pieces%spitch-%d-%d.bmp", nbr_pieces==12?"-":"-half-", pitch, pitches[pitch] );
    }
    FILE *f = fopen(fname,"wb");
    if( !f )
    {
        printf( "Cannot open %s\n", fname );
        return;
    }

    // Allocate a header plus data
    char *buf = (char *)malloc(BUF_SIZE);
    if( !buf )
    {
        printf( "Cannot allocate memory\n" );
        return;
    }
    memset(buf,0,BUF_SIZE);
    BmpHeader *out = (BmpHeader *)buf;

    // Initialise header
    *out = *in;
    out->width  = nbr_pieces*pitch;
    out->height = pitch;
    uint32_t row_bytes = 3*out->width;
    if( (row_bytes&3) != 0 )
        row_bytes = (row_bytes|3) + 1;
    out->data_len = out->height * row_bytes;
    out->file_len = out->hdr_len + out->data_len;

    // Fill the whole thing with magenta
    for( uint32_t x=0; x<out->width; x++ )
    {
        for( uint32_t y=0; y<out->height; y++ )
        {
            uint8_t *dst = Locate(out,x,y);
            *dst++ = 255;
            *dst++ = 0;
            *dst++ = 255;
        }
    }

    // Copy pieces from src
    for( int i=0; i<nbr_pieces; i++ )
    {
        for( uint32_t y=0; y<pieces->h; y++ )
        {
            for( uint32_t x=0; x<pieces->w; x++ )
            {
                uint8_t *src = Locate(in,pieces->src_x+x,pieces->src_y+y);
                uint8_t *dst = Locate(out,pieces->dst_x+x,pieces->dst_y+y);
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
            }
        }
        pieces++;
    }
    fwrite( buf, 1, out->file_len, f );
    free(buf);
    fclose(f);
}

static int ReadPieceLine( BmpHeader *in, uint32_t y_begin, uint32_t y_end, uint32_t &pitch, Piece *pieces )
{
    int nbr_pieces=0;
    pitch = 0;
    int state = 0;
    uint32_t wk_begin, wk_end;
    uint32_t wq_begin, wq_end;
    uint32_t wr_begin, wr_end;
    uint32_t wb_begin, wb_end;
    uint32_t wn_begin, wn_end;
    uint32_t wp_begin, wp_end;
    uint32_t bk_begin, bk_end;
    uint32_t bq_begin, bq_end;
    uint32_t br_begin, br_end;
    uint32_t bb_begin, bb_end;
    uint32_t bn_begin, bn_end;
    uint32_t bp_begin, bp_end;
    for( uint32_t x=0; x<in->width; x++ )
    {
        bool blank = TestColBlank( in, x, y_begin, y_end );
        switch( state )
        {
            case  0: if(  blank )  { state= 1;             } break;
            case  1: if( !blank )  { state= 2; wk_begin=x; } break;
            case  2: if(  blank )  { state= 3; wk_end  =x; } break;
            case  3: if( !blank )  { state= 4; wq_begin=x; } break;
            case  4: if(  blank )  { state= 5; wq_end  =x; } break;
            case  5: if( !blank )  { state= 6; wr_begin=x; } break;
            case  6: if(  blank )  { state= 7; wr_end  =x; } break;
            case  7: if( !blank )  { state= 8; wb_begin=x; } break;
            case  8: if(  blank )  { state= 9; wb_end  =x; } break;
            case  9: if( !blank )  { state=10; wn_begin=x; } break;
            case 10: if(  blank )  { state=11; wn_end  =x; } break;
            case 11: if( !blank )  { state=12; wp_begin=x; } break;
            case 12: if(  blank )  { state=13; wp_end  =x; } break;
            case 13: if( !blank )  { state=14; bk_begin=x; } break;
            case 14: if(  blank )  { state=15; bk_end  =x; } break;
            case 15: if( !blank )  { state=16; bq_begin=x; } break;
            case 16: if(  blank )  { state=17; bq_end  =x; } break;
            case 17: if( !blank )  { state=18; br_begin=x; } break;
            case 18: if(  blank )  { state=19; br_end  =x; } break;
            case 19: if( !blank )  { state=20; bb_begin=x; } break;
            case 20: if(  blank )  { state=21; bb_end  =x; } break;
            case 21: if( !blank )  { state=22; bn_begin=x; } break;
            case 22: if(  blank )  { state=23; bn_end  =x; } break;
            case 23: if( !blank )  { state=24; bp_begin=x; } break;
            case 24: if(  blank )  { state=25; bp_end  =x; } break;
            case 25: if( !blank )  { state=26;             } break;
        }
    }
    nbr_pieces = 0;
    if( state == 13 )
        nbr_pieces = 6;
    else if( state == 25 )
        nbr_pieces = 12;
    else
        printf( "Cannot split out pieces (%d)\n", state );
    if( nbr_pieces > 0 )
    {
        uint32_t height = y_end-y_begin;
        uint32_t width = wk_end-wk_begin;
        #define max(a,b) ((a)>(b)?(a):(b))
        width = max( wq_end-wq_begin, width ); 
        width = max( wr_end-wr_begin, width ); 
        width = max( wb_end-wb_begin, width ); 
        width = max( wn_end-wn_begin, width ); 
        width = max( wp_end-wp_begin, width ); 
        double fpitch;
        if( nbr_pieces == 6 )
        {
            int span = wp_end-wk_begin;
            fpitch = span * 0.17425;
        }
        else if( nbr_pieces == 12 )
        {
            int span = bp_end-wk_begin;
            fpitch = span * 0.084944;
            width = max( bq_end-bq_begin, width ); 
            width = max( br_end-br_begin, width ); 
            width = max( bb_end-bb_begin, width ); 
            width = max( bn_end-bn_begin, width ); 
            width = max( bp_end-bp_begin, width ); 
        }
        uint32_t hi = static_cast<uint32_t>( ceil (fpitch) );
        uint32_t lo = static_cast<uint32_t>( floor(fpitch) );
        pitch = (hi==lo ? lo : hi);
        if( pitch>100 && (pitch&1) )
            pitch = pitch & (~1);   // eg 101->100
        bool clearance = pitch>(width+1) && pitch>(height+1);
        printf( "%d piece line found, height=%d, width=%d, pitch=%d %s\n", nbr_pieces, height, width, pitch, clearance?"have clearance":"*clearance problem*"  );
        while( !clearance )
        {
            pitch++;
            clearance = pitch>(width+1) && pitch>(height+1);
        }
        //if( pitch == 307 )
        //    pitch = 306;    // ugly repair
        for( int i=0; i<nbr_pieces; i++ )
        {
            uint32_t x, w;
            switch(i)
            {
                case 0:  x=wk_begin; w=wk_end-wk_begin;   break;
                case 1:  x=wq_begin; w=wq_end-wq_begin;   break;
                case 2:  x=wr_begin; w=wr_end-wr_begin;   break;
                case 3:  x=wb_begin; w=wb_end-wb_begin;   break;
                case 4:  x=wn_begin; w=wn_end-wn_begin;   break;
                case 5:  x=wp_begin; w=wp_end-wp_begin;   break;
                case 6:  x=bk_begin; w=bk_end-bk_begin;   break;
                case 7:  x=bq_begin; w=bq_end-bq_begin;   break;
                case 8:  x=br_begin; w=br_end-br_begin;   break;
                case 9:  x=bb_begin; w=bb_end-bb_begin;   break;
                case 10: x=bn_begin; w=bn_end-bn_begin;   break;
                case 11: x=bp_begin; w=bp_end-bp_begin;   break;
            }
            pieces->w = w;
            pieces->h = height;
            pieces->src_x = x;
            pieces->src_y = y_begin;
            pieces->dst_x = i*pitch;
            pieces->dst_y = 0;
            pieces->dst_x += (pitch-w)/2;
            pieces->dst_y += (2*(pitch-height))/3;
            pieces++;
        }
    }
    return nbr_pieces;
}


// Cut out and size chess piece bitmaps
static void SliceOutPieces( BmpHeader *in )
{
    char *buf = (char *)malloc(BUF_SIZE/10);
    if( !buf )
    {
        printf( "Cannot allocate memory\n" );
        return;
    }
    int state=0;
    uint32_t y_begin=0;
    uint32_t y_end=0;
    for( uint32_t y=0; y<in->height; y++ )
    {
        bool blank = TestRowBlank(in,y);
        switch( state )
        {
            case 0:
            {
                if( blank )
                    state=1;    
                break;
            }
            case 1:
            {
                if( !blank )
                {
                    y_begin = y;
                    state=2;
                }
                break;
            }
            case 2:
            {
                if( blank )
                {
                    state=1;    
                    y_end = y;
                    Piece pieces[12];
                    uint32_t pitch;
                    int nbr_pieces = ReadPieceLine( in, y_begin, y_end, pitch, pieces );
                    if( nbr_pieces )
                        WritePieceLine( in, pitch, nbr_pieces, pieces );
                }
                break;
            }
        }
    }
    free(buf);
}

static void SpliceFiles( char *buf )
{
    const char *files[] =
    {
        "pieces-half-pitch-100.bmp",
        "pieces-half-pitch-100-2.bmp",
        "pieces-pitch-100.bmp",
        "pieces-half-pitch-110.bmp",
        "pieces-half-pitch-110-2.bmp",
        "pieces-pitch-110.bmp",
        "pieces-half-pitch-120.bmp",
        "pieces-half-pitch-120-2.bmp",
        "pieces-pitch-120.bmp",
        "pieces-half-pitch-130.bmp",
        "pieces-half-pitch-130-2.bmp",
        "pieces-pitch-130.bmp",
        "pieces-half-pitch-140.bmp",
        "pieces-half-pitch-140-2.bmp",
        "pieces-pitch-140.bmp",
        "pieces-half-pitch-150.bmp",
        "pieces-half-pitch-150-2.bmp",
        "pieces-pitch-150.bmp",
        "pieces-half-pitch-160.bmp",
        "pieces-half-pitch-160-2.bmp",
        "pieces-pitch-160.bmp",
        "pieces-half-pitch-170.bmp",
        "pieces-half-pitch-170-2.bmp",
        "pieces-pitch-170.bmp",
        "pieces-half-pitch-180.bmp",
        "pieces-half-pitch-180-2.bmp",
        "pieces-pitch-180.bmp",
        "pieces-half-pitch-190.bmp",
        "pieces-half-pitch-190-2.bmp",
        "pieces-pitch-190.bmp",
        "pieces-half-pitch-200.bmp",
        "pieces-half-pitch-200-2.bmp",
        "pieces-pitch-200.bmp"
    };

    char *buf2 = (char *)malloc(BUF_SIZE);
    if( !buf2 )
    {
        printf( "Cannot allocate memory\n" );
        return;
    }
    char *buf3 = (char *)malloc(BUF_SIZE);
    if( !buf3 )
    {
        printf( "Cannot allocate memory\n" );
        return;
    }
    for( int i=0; i<sizeof(files)/sizeof(files[0]); i+=3 )
    {
        BmpHeader *in1 = ReadFile( buf, files[i] );
        if( in1 )
        {
            BmpHeader *in2 = ReadFile( buf2, files[i+1] );
            if( in2 )
            {
                FILE *fout = fopen(files[i+2],"wb");
                if( fout )
                {
                    
                    // Initialise header
                    memset(buf3,0,BUF_SIZE);
                    BmpHeader *out = (BmpHeader *)buf3;
                    *out = *in1;
                    out->width  = in1->width*2;
                    uint32_t row_bytes = 3*out->width;
                    if( (row_bytes&3) != 0 )
                        row_bytes = (row_bytes|3) + 1;
                    out->data_len = out->height * row_bytes;
                    out->file_len = out->hdr_len + out->data_len;

                    // Copy left pieces from src
                    for( uint32_t y=0; y<in1->height; y++ )
                    {
                        for( uint32_t x=0; x<in1->width; x++ )
                        {
                            uint8_t *src = Locate(in1,x,y);
                            uint8_t *dst = Locate(out,x,y);
                            *dst++ = *src++;
                            *dst++ = *src++;
                            *dst++ = *src++;
                        }
                    }

                    // Copy right pieces from src
                    for( uint32_t y=0; y<in2->height; y++ )
                    {
                        for( uint32_t x=0; x<in2->width; x++ )
                        {
                            uint8_t *src = Locate(in2,x,y);
                            uint8_t *dst = Locate(out,x+in1->width,y);
                            *dst++ = *src++;
                            *dst++ = *src++;
                            *dst++ = *src++;
                        }
                    }
                    fwrite( buf3, 1, out->file_len, fout );
                    fclose(fout);
                }
            }
        }
    }
    free(buf3);
    free(buf2);
}

static void Convert2Xpm( BmpHeader *in, const char *filename )
{
    std::string s(filename);
    std::string xpm_filename = s.substr(0,s.length()-4) + ".xpm";
    std::set<uint32_t> colour_set;
    colour_set.insert(0xff00ff);    // some of the code below assumes magenta present, so insist it is represented
    for( uint32_t y=0; y<in->height; y++ )
    {
        uint8_t *ptr = Locate(in,y);
        for( uint32_t x=0; x<in->width; x++ )
        {
            uint8_t b = *ptr++;    
            uint8_t g = *ptr++;    
            uint8_t r = *ptr++;
            uint32_t rgb = r;
            rgb <<= 8;
            rgb |= g;
            rgb <<= 8;
            rgb |= b;
            colour_set.insert(rgb);
        }
    }
    bool too_many = ( colour_set.size() > 50 );
    printf( "File %s, %d colours found%s\n", filename, colour_set.size(), too_many?" too many!":"" );
    if( !too_many )
    {
        char colour_proxy_character = '!';    
        std::map<uint32_t,char> colour_map;
        for( std::set<uint32_t>::iterator it=colour_set.begin(); it!=colour_set.end(); it++ )
        {
            colour_map[*it] = (*it==0xff00ff ? ' ' : colour_proxy_character++); // magenta background gets ' '
            while( colour_proxy_character == '\'' || colour_proxy_character=='\"' )
                colour_proxy_character++;
        }
        FILE *f = fopen(xpm_filename.c_str(),"wt");
        if( !f )
        {
            printf( "Cannot open %s\n", xpm_filename.c_str() );
            return;
        }
        char *buf = (char *)malloc(BUF_SIZE/10);
        if( !buf )
        {
            fclose(f);
            printf( "Cannot allocate memory\n" );
            return;
        }

        /*
            // Format we are going for;
            static const char *bitmap_xpm[] = {
                "6 2 3 1",      // width 6, height 2, 3 colours
                "M c #ff00ff",
                "B c #0000ff",
                "R c #ff0000",
                "RRBBRR",
                "RRMMRR"
            };

        */
        fprintf( f, "static const char *pitch_%d_xpm[] = {\n", in->height  );
        fprintf( f, "\"%d %d %d 1\",\n", in->width, in->height, colour_set.size() );
        fprintf( f, "\"%c c #%06x\",\n", ' ', 0xff00ff );   // magenta background first
        for( std::map<uint32_t,char>::iterator it=colour_map.begin(); it!=colour_map.end(); it++ )
        {
            if( it->first != 0xff00ff ) //we've already listed magenta
                fprintf( f, "\"%c c #%06x\",\n", it->second, it->first ); 
        }
        for( uint32_t y=0; y<in->height; y++ )
        {
            char *s = buf;
            uint8_t *ptr = Locate(in,y);
            for( uint32_t x=0; x<in->width; x++ )
            {
                uint8_t b = *ptr++;    
                uint8_t g = *ptr++;    
                uint8_t r = *ptr++;
                uint32_t rgb = r;
                rgb <<= 8;
                rgb |= g;
                rgb <<= 8;
                rgb |= b;
                *s++ = colour_map[rgb];
            }
            fprintf(f,"\"%s\"%s\n",buf, y+1<in->height?",":"");
        }
        fprintf(f,"};\n");
        free(buf);
        fclose(f);
    }
}


static bool Bmp2CompressedXpm( BmpHeader *in, const char *filename, CompressedXpm &compressed )
{
    int dbg=0;
    bool dbg_enable=false;
    #define DBG_NBR_IN  16
    #define DBG_NBR_OUT 30
    bool ok=true;   // returns bool ok
    uint8_t *buf = (uint8_t *)malloc(BUF_SIZE/10);
    if( !buf )
    {
        printf( "Cannot allocate memory\n" );
        return false;
    }
    const uint32_t magenta = 0xff00ff;
    uint32_t kludge_rgb = 0;
    compressed.width  = in->width;    
    compressed.height = in->height;
    compressed.background_colour = magenta;    
    std::map<uint32_t,uint32_t> colour_map;
    colour_map[magenta] = 0;
    for( uint32_t y=0; y<in->height; y++ )
    {
        uint8_t *ptr = Locate(in,y);
        for( uint32_t x=0; x<in->width; x++ )
        {
            uint8_t b = *ptr++;    
            uint8_t g = *ptr++;    
            uint8_t r = *ptr++;
            uint32_t rgb = r;
            rgb <<= 8;
            rgb |= g;
            rgb <<= 8;
            rgb |= b;
            int count = colour_map.count(rgb);
            if( count == 0 )
                colour_map[rgb] = 0;
            else
            {
                uint32_t val = colour_map[rgb];
                val++;
                colour_map[rgb] = val;
            }
        }
    }
    int nbr_colours = colour_map.size();
    bool too_many = (nbr_colours > 18);

    // Set kudge_rgb to be a colour that doesn't otherwise exist
    kludge_rgb = 0;
    while( colour_map.count(kludge_rgb) > 0 )
        kludge_rgb++;  // try something else

    // But if we have 18 colours, we need kludge_rgb - set it to least frequent colour
    if( nbr_colours == 18 )
    {
        too_many = true;
        uint32_t min = 1000000000;
        for( std::map<uint32_t,uint32_t>::iterator it=colour_map.begin(); it!=colour_map.end(); it++ )
        {
            if( it->second < min )
            {
                if( it->first != magenta )
                {
                    min = it->second;
                    kludge_rgb = it->first;
                    too_many = false;
                }
            }
        }
        if( !too_many )
            printf( "18 colours accommodated with kludge colour #%06x\n", kludge_rgb );
    }
    ok = !too_many;
    printf( "File %s, %d colours found%s\n", filename, nbr_colours, too_many?" too many!":"" );
    if( ok )
    {
        compressed.bin = (uint8_t *)malloc( compressed.width * compressed.height * 2 ); // *2 = safety margin for some weird edge case where compressed bigger than uncompressed
        if( !compressed.bin )
        {
            printf( "Cannot allocate memory\n" );
            ok = false;
        }
    }
    if( ok )
    {
        int idx=0;
        compressed.kludge_colour = kludge_rgb;
        compressed.kludge_idx = -1;
        for( std::map<uint32_t,uint32_t>::iterator it=colour_map.begin(); it!=colour_map.end(); it++ )
        {
            uint32_t colour = it->first;
            if( colour == kludge_rgb )
                compressed.kludge_idx = idx;
            else if( colour != magenta )
                compressed.other_colours[ idx++ ] = colour;
        }
        compressed.nbr_other_colours = idx;
        uint8_t *dst = compressed.bin;

        /*
            Byte by byte coding system
            --------------------------
            up to 2^3  =8    successive codes cccc                0nnncccc
            up to 2^9  =512  successive codes cccc                100nnnnn nnnncccc
            up to 2^13 =8192 successive background codes          101nnnnn nnnnnnnn
            up to 2^5  =32   arbitrary codes                      110nnnnn followed by codes
            up to 2^13 =8192 arbitrary codes                      111nnnnn nnnnnnnn followed by codes
        */

        uint32_t prev_rgb=0;
        uint8_t  cccc=0;
        uint8_t  cccc_in_run=0;
        uint32_t n=0;
        enum{ init, search, in_run, in_run_magenta } state=init;
        for( uint32_t y=0; y<in->height; y++ )
        {
            uint8_t *ptr = Locate(in,y);
            for( uint32_t x=0; x<in->width; x++ )
            {
                uint8_t b = *ptr++;    
                uint8_t g = *ptr++;    
                uint8_t r = *ptr++;
                uint32_t rgb = r;
                rgb <<= 8;
                rgb |= g;
                rgb <<= 8;
                rgb |= b;
                cccc = 0;    // if not found it should be magenta or kludge_rgb
                for( int i=0; i<compressed.nbr_other_colours; i++ )
                {
                    if( rgb == compressed.other_colours[i] )
                    {
                        cccc = i;
                        break;
                    }
                }

                // Last pixel ?
                bool end = ((y+1)==in->height && (x+1)==in->width);

                // Encoding state machine begin
                switch( state )
                {

                    // First pixel
                    case init:
                    {
                        if( rgb == magenta )
                        {
                            state = in_run_magenta;
                            n = 1;
                        }
                        else if( rgb == kludge_rgb )
                        {
                            state = init;
                            *dst++ = 0xc0;  // 0xc0 represents a kludge pixel - it's otherwise unneeded since
                                            //  it specifies a len=1 buffer which can be more efficiently
                                            //  represented as a one byte code 0000cccc
                        }
                        else
                        {
                            state = search;
                            idx = 0;
                            buf[idx++] = cccc;
                        }
                        break;
                    }
                    
                    // Buffering arbitrary pixels, waiting for magenta or runs of same colour
                    case search:
                    {
                        bool write_out_buf = false;
                        if( rgb == magenta )
                        {
                            write_out_buf = true;
                        }
                        else if( rgb == kludge_rgb )
                        {
                            write_out_buf = true;
                        }
                        else if( end )
                        {
                            write_out_buf = true;
                            buf[idx++] = cccc;
                        }
                        else if( rgb == prev_rgb )
                        {
                            write_out_buf = true;
                            idx--;  // remove last from buf, it will be encoded as first of run
                        }
                        else if( rgb != prev_rgb )
                        {
                            buf[idx++] = cccc;  // buffer and keep searching
                        }

                        // Write out buf at end or at start of run
                        if( write_out_buf )
                        {

                            unsigned int nbr_remaining = idx;
                            unsigned int src = 0;
                            while( nbr_remaining > 0 )
                            {
                                unsigned int chunk = nbr_remaining;
                                if( chunk > 8192 )
                                    chunk = 8192;
                                nbr_remaining -= chunk;
                                uint8_t *dat=dst;
                                if( chunk == 1 )
                                {
                                    // no prefix required 0000cccc in buf encodes as 0nnncccc nnn is range 1-8, 000 encodes as 1
                                    if( dbg_enable && dbg<DBG_NBR_IN )
                                        printf( "Buf (%d)\n", chunk );
                                }
                                else if( chunk <= 32 )
                                {
                                    uint8_t b = chunk-1;
                                    b |= 0xc0;
                                    *dst++ = b;
                                    if( dbg_enable && dbg<DBG_NBR_IN )
                                        printf( "Buf <=32 (%d) [%02x]\n", chunk, dat[0] );
                                }
                                else
                                {
                                    uint16_t w = chunk-1;
                                    w |= 0xe000;
                                    *dst++ = (w>>8)&0xff;
                                    *dst++ = w&0xff;
                                    if( dbg_enable && dbg<DBG_NBR_IN )
                                        printf( "Buf >32 (%d) [%02x] [%02x]\n", chunk, dat[0], dat[1] );
                                }
                                dat=dst;
                                int x = src;      //  chunk=3, src-> 09 00 03 -> 90 00 ??
                                uint8_t byt;
                                if( dbg_enable && buf[src]==9 && buf[src+1]==0 && buf[src+2]==3 && chunk==3 )
                                    printf("trigger\n");
                                for( unsigned int i=0; i<chunk; i++ )
                                {
                                    if( (i&1) == 0 )
                                    {
                                        byt = (buf[src++] & 0x0f);
                                        *dst = byt;
                                        if( i+1 == chunk )
                                            dst++;
                                    }
                                    else
                                    {
                                        byt |= (buf[src++]<<4)&0xf0;
                                        *dst++ = byt;
                                    }
                                }
                                if( dbg_enable && dbg<DBG_NBR_IN )
                                {
                                    printf( "[%02x %02x %02x %02x %02x...]\n", buf[x], buf[x+1], buf[x+2], buf[x+3], buf[x+4] );
                                    printf( "[%02x %02x %02x %02x %02x...]\n", dat[0], dat[1], dat[2], dat[3], dat[4] );
                                    dbg_enable = false;
                                    dbg++;
                                }
                            }

                            // Account for final pixel at end
                            if( end && rgb==magenta )
                            {
                                // add a single magenta pixel
                                *dst++ = 0xa0;
                                *dst++ = 0;
                            }

                            // Next state
                            if( rgb == magenta )
                            {
                                state = in_run_magenta;
                                n = 1;
                            }
                            else if( rgb == kludge_rgb )
                            {
                                state = init;
                                *dst++ = 0xc0;
                            }
                            else
                            {
                                state = in_run;
                                cccc_in_run = cccc;
                                n = 2;
                            }
                        }
                        break;
                    }
                    
                    // In a run of magenta pixels
                    case in_run_magenta:
                    {
                        if( rgb == magenta )
                            n++;

                        // Write out buf at end or end of run
                        if( end || rgb!=magenta )
                        {
                            unsigned int nbr_remaining = n;
                            while( nbr_remaining > 0 )
                            {
                                unsigned int chunk = nbr_remaining;
                                if( chunk > 8192 )
                                    chunk = 8192;
                                nbr_remaining -= chunk;
                                uint16_t w = chunk-1;
                                w |= 0xa000;
                                uint8_t *dat=dst;
                                *dst++ = (w>>8)&0xff;
                                *dst++ = w&0xff;
                                if( dbg_enable && dbg<DBG_NBR_IN )
                                    printf( "Magenta (%d) [%02x %02x]\n", chunk, dat[0], dat[1] );
                            }


                            // Account for final pixel at end
                            if( end && rgb!=magenta && rgb!=kludge_rgb )
                            {

                                // Need a single non magenta pixel
                                *dst++ = cccc;
                            }

                            // Next state
                            if( rgb == kludge_rgb )
                            {
                                state = init;
                                *dst++ = 0xc0;
                            }
                            else
                            {
                                state = search;
                                idx = 0;
                                buf[idx++] = cccc;
                            }
                        }
                        break;
                    }

                    // In a run of non magenta pixels
                    case in_run:
                    {
                        if( rgb == prev_rgb )
                            n++;

                        // Write out sequence at end or at end of run
                        if( end || rgb!=prev_rgb )
                        {
                            unsigned int nbr_remaining = n;
                            while( nbr_remaining > 0 )
                            {
                                unsigned int chunk = nbr_remaining;
                                if( chunk > 512 )
                                    chunk = 512;
                                nbr_remaining -= chunk;
                                uint8_t *dat=dst;
                                if( chunk <= 8 )
                                {
                                    uint8_t b = chunk-1;
                                    b = b<<4;
                                    b |= cccc_in_run;
                                    *dst++ = b;
                                    if( dbg_enable && dbg<DBG_NBR_IN )
                                        printf( "Seq <=8 (%d,%d) [%02x]\n", cccc, chunk, dat[0] );
                                }
                                else //if( chunk <= 512 )
                                {
                                    uint16_t w = chunk-1;
                                    w = w<<4;
                                    w |= cccc_in_run;
                                    w |= 0x8000;
                                    *dst++ = (w>>8)&0xff;
                                    *dst++ = w&0xff;
                                    if( dbg_enable && dbg<DBG_NBR_IN )
                                        printf( "Seq >8 (%d,%d) [%02x %02x]\n", cccc, chunk, dat[0], dat[1] );
                                }
                            }

                            // Account for final pixel at end
                            if( end )
                            {
                                // May need a single magenta pixel
                                if( rgb == magenta )
                                {
                                    *dst++ = 0xa0;
                                    *dst++ = 0;
                                }

                                // Or a single kludge pixel
                                else if( rgb == kludge_rgb )
                                    *dst++ = 0xc0;

                                // Or a single non magenta pixel
                                else if( rgb != prev_rgb )
                                    *dst++ = cccc;
                            }

                            // Next state
                            if( rgb == magenta )
                            {
                                state = in_run_magenta;
                                n = 1;
                            }
                            else if( rgb == kludge_rgb )
                            {
                                state = init;
                                if( !end )
                                    *dst++ = 0xc0;
                            }
                            else
                            {
                                state = search;
                                idx = 0;
                                buf[idx++] = cccc;
                            }
                        }
                        break;
                    }
                }

                // After encoding state machine
                prev_rgb  = rgb;
            }
        }
        compressed.binary_len = dst-compressed.bin;
        compressed.bin = (uint8_t *)realloc( (void *)compressed.bin, compressed.binary_len );
    }
    return ok;
}

static void CompressedXpm2CHeader( CompressedXpm &compressed, const char *filename, bool append )
{
    FILE *f = fopen(filename,append?"at":"wt");
    if( !f )
    {
        printf( "Cannot open %s\n", filename );
        return;
    }
    fprintf( f, "// Bespoke bitmap compression, generated with tools/bmp-experiments-and-transformations.cpp\n"  );
    fprintf( f, "static uint8_t pitch_%03d_compressed_xpm_data[] =\n{\n    ",  compressed.height  );
    for( uint32_t i=0; i<compressed.binary_len; i++ )
    {
        fprintf( f, "0x%02x", compressed.bin[i] );
        if( i+1 == compressed.binary_len )
            fprintf( f, "\n};\n" );
        else if( (i+1) % 8 == 0 )
            fprintf( f, ",\n    " );
        else
            fprintf( f, ", " );
    }
    fprintf( f, "\n" );
    fprintf( f, "static CompressedXpm pitch_%03d_compressed_xpm =\n{\n",  compressed.height  );
    fprintf( f, "    %d,  // width\n"                                  , compressed.width   );
    fprintf( f, "    %d,  // height\n"                                 , compressed.height  );
    fprintf( f, "    0x%06x,  // background_colour\n"                  , compressed.background_colour  );
    fprintf( f, "    0x%06x,  // kludge_colour\n"                      , compressed.kludge_colour  );
    fprintf( f, "    %d,  // kludge_idx\n"                            , compressed.kludge_idx  );
    fprintf( f, "    %d,  // nbr_other_colours\n    {"                , compressed.nbr_other_colours  );
    for( int i=0; i<compressed.nbr_other_colours; i++ )
        fprintf( f, "0x%06x%s", compressed.other_colours[i], i+1<compressed.nbr_other_colours?",":"},\n" );
    fprintf( f, "    %d,  // binary_len\n"                            , compressed.binary_len  );
    fprintf( f, "    pitch_%03d_compressed_xpm_data //bin\n"          , compressed.height   );
    fprintf( f, "};\n" );
    fclose(f);
}

static void CompressedXpm2Xpm( CompressedXpm &compressed, const char *filename )
{
    CompressedXpmProcessor processor( &compressed );
    std::string s(filename);
    std::string xpm_filename = "reconstituted/" + s.substr(0,s.length()-4) + ".xpm";
    FILE *f = fopen(xpm_filename.c_str(),"wt");
    if( !f )
    {
        printf( "Cannot open %s\n", xpm_filename.c_str() );
        return;
    }

    fprintf( f, "static const char *pitch_%d_xpm[] = {\n", compressed.height  );
    int nbr_strings  = processor.GetXpmLen();
    char **xpm = processor.GetXpm();
    for( int i=0; i<nbr_strings; i++ )
    {
        const char *line = xpm[i];
        fprintf( f, "\"%s\"%s\n", line, i+1==nbr_strings?"":"," );
    }
    fprintf( f, "};\n" );
    fclose(f);
}

CompressedXpmProcessor::CompressedXpmProcessor( const CompressedXpm *encoded )
{
    int dbg=0;
    bool dbg_enable=false;

    /*
        // Format we are going for;
        static const char *bitmap_xpm[] = {
            "6 2 3 1",      // width 6, height 2, 3 colours
            "M c #ff00ff",
            "B c #0000ff",
            "R c #ff0000",
            "RRBBRR",
            "RRMMRR"
        };

    */

    // Calculate storage requirements    
    int nbr_colours = 1 /*background*/ + encoded->nbr_other_colours;
    if( encoded->kludge_idx != -1 )
        nbr_colours++;
    nbr_xpm_strings = 1 /*header*/ + nbr_colours + encoded->height;
    char hdr[100];
    sprintf( hdr, "%d %d %d 1", encoded->width, encoded->height, nbr_colours );
    size_t string_storage_required = strlen(hdr)+1;
    string_storage_required += (nbr_colours * (11 + 1));  // strlen("M c #ff00ff") == 11
    string_storage_required += (encoded->height * (encoded->width+1)); // height * width C strings with +1 for trailing '\0'

    // Dynamic allocation
//#define C_STYLE
#ifdef C_STYLE
    string_storage = (char *) malloc( string_storage_required );
    xpm = (char **) malloc( nbr_xpm_strings * sizeof( char **) );
#else
    string_storage = new char[ string_storage_required + 100 ];     // note my silly little safety margin
    xpm = new char *[nbr_xpm_strings];
#endif
    char *s = string_storage;
    int line_idx=0;
    strcpy( s , hdr );
    xpm[line_idx++] = s;
    s += (strlen(s) + 1);
    sprintf( s, "%c c #%06x", ' ', 0xffffff & encoded->background_colour );   // background first
    xpm[line_idx++] = s;
    s += (strlen(s) + 1);
    char proxy='!';
    char proxy_kludge='z';
    char proxy_colours[16];
    for( int i=0; i<encoded->nbr_other_colours; i++ )
    {
        if( i == encoded->kludge_idx )
        {
            sprintf( s, "%c c #%06x", proxy, 0xffffff & encoded->kludge_colour ); 
            xpm[line_idx++] = s;
            s += (strlen(s) + 1);
            proxy_kludge = proxy;
            proxy++;
            while( proxy == '\'' || proxy=='\"' )
                proxy++; 
        }
        sprintf( s, "%c c #%06x", proxy, 0xffffff & encoded->other_colours[i] ); 
        xpm[line_idx++] = s;
        s += (strlen(s) + 1);
        proxy_colours[i] = proxy;
        proxy++;
        while( proxy == '\'' || proxy=='\"' )
            proxy++; 
    }

    int len = encoded->binary_len;
    const uint8_t *src = encoded->bin;

    /*
        Byte by byte coding system
        --------------------------
        up to 2^3  =8    successive codes cccc                0nnncccc
        up to 2^9  =512  successive codes cccc                100nnnnn nnnncccc
        up to 2^13 =8192 successive background codes          101nnnnn nnnnnnnn
        up to 2^5  =32   arbitrary codes                      110nnnnn followed by codes
        up to 2^13 =8192 arbitrary codes                      111nnnnn nnnnnnnn followed by codes
    */

    xpm[line_idx++] = s;
    uint32_t offset=0;  // from 0 to width*height
    while( len > 0 )
    {
        uint8_t c = *src++;
        len--;
        bool arbitrary_data=false;
        char proxy=' ';
        uint32_t n=0;
        if( c == 0xc0 )   // This is the kludge code - search for "kludge" for explanation
        {
            proxy = proxy_kludge;
            n = 1;
        }
        else if( (c&0x80) == 0 )
        {
            // Sequence of one colour (short)
            uint32_t cccc = c&0x0f;
            proxy = proxy_colours[cccc];
            n = ((c>>4)&0x07) + 1;
            if( dbg_enable && dbg++ < DBG_NBR_OUT )
                printf( "Seq short\n" );
        }
        else if( (c&0xe0) == 0xc0 )
        {
            // Arbitrary data (short)
            n = (c&0x1f) + 1;
            arbitrary_data = true;
            if( dbg_enable && dbg++ < DBG_NBR_OUT )
                printf( "Data short\n" );
        }
        else
        {
            uint8_t d = *src++;
            len--;
            if( (c&0xe0) == 0x80 )
            {
                // Sequence of one colour (long)
                uint32_t cccc = d&0x0f;
                proxy = proxy_colours[cccc];
                n =  ((d>>4)&0x0f);
                n += ((c&0x1f)<<4);
                n++;
                if( dbg_enable && dbg++ < DBG_NBR_OUT )
                    printf( "Seq long\n" );
            }
            else if( (c&0xe0) == 0xa0 )
            {
                // Sequence of background colour
                proxy = ' ';
                n =  d;
                n += ((c&0x1f)<<8);
                n++;
                if( dbg_enable && dbg++ < DBG_NBR_OUT )
                    printf( "Magenta\n" );
            }
            else if( (c&0xe0) == 0xe0 )
            {
                // Arbitrary data (long)
                n =  d;
                n += ((c&0x1f)<<8);
                n++;
                arbitrary_data = true;
                if( dbg_enable && dbg++ < DBG_NBR_OUT )
                    printf( "Data long\n" );
            }
        }

        // Generate decoded data at end of allowed storage space, if you
        //  think about it carefully, you realise that it can't be
        //  overwritten before it is used
        char *buf = string_storage + string_storage_required - n;
        if( !arbitrary_data )
            memset( buf, proxy, n );
        else
        {
            for( unsigned int i=0; i<n; i++ )
            {
                uint8_t dat = *src;
                if( (i&1) == 0 )
                {
                    buf[i] = proxy_colours[dat&0x0f];
                    if( i+1 == n )
                    {        
                        src++;
                        len--;
                    }
                }
                else
                {
                    buf[i] = proxy_colours[(dat>>4)&0x0f];
                    src++;
                    len--;
                }
            }
        }
        if( dbg_enable && dbg < DBG_NBR_OUT )
        {
            printf( "dat (%d) >", n );
            if( arbitrary_data )
            {
                for( uint32_t i=0; i<n && i<10; i++ )
                {
                    char c = buf[i];
                    if( c == ' ' )
                        printf( "magenta " );
                    else
                    {
                        for( int j=0; j<16; j++ )
                        {
                            if( c == proxy_colours[j] )
                            {
                                printf( "%02x ", j );
                                break;
                            }
                        }
                    }
                }
            }
            printf( "\n" );
        }

        // Move generated data into final resting place, interspercing end of string after each line (=row of pixels)
        for( unsigned int i=0; i<n; i++ )
        {
            *s++ = buf[i];
            offset++;
            if( offset == encoded->height*encoded->width )
            {
                *s++ = '\0';
                printf( "Check 1: %s\n", len == 0 ? "Pass" : "Fail" );
                printf( "Check 2: %s\n", s == (string_storage+string_storage_required) ? "Pass" : "Fail" );
                printf( "Check 3: %s\n", line_idx == nbr_xpm_strings ? "Pass" : "Fail" );
            }
            else if( (offset%encoded->width) == 0 )
            {
                *s++ = '\0';
                xpm[line_idx++] = s;
            }
        }
    }
}

CompressedXpmProcessor::~CompressedXpmProcessor()
{
#ifdef C_STYLE
    free(xpm);
    free(string_storage);
#else
    delete[](xpm);
    delete[](string_storage);
#endif
}

static uint8_t pitch_070_compressed_xpm_data[] =
{
    '\xae', '\x4f', '\xc1', '\xc4', '\xa1', '\xa1', '\x0e', '\xa0',
    '\x8b', '\x0d', '\x81', '\x18', '\x0d', '\xa0', '\xbf', '\xc1',
    '\x3b', '\x10', '\xc1', '\xc4', '\xa0', '\x04', '\xc1', '\x4c',
    '\x10', '\xc1', '\xb3', '\xa0', '\x30', '\xc2', '\x09', '\x03',
    '\xa0', '\x8a', '\x0d', '\x81', '\x18', '\x0d', '\xa0', '\xbf',
    '\xc5', '\x4c', '\x10', '\xe6', '\xa0', '\x04', '\xc1', '\x5c',
    '\x10', '\xc1', '\xc4', '\xa0', '\x31', '\xc1', '\xe5', '\xa0',
    '\x8a', '\x0b', '\x81', '\x10', '\x0b', '\xa0', '\x2f', '\xc4',
    '\x8b', '\x84', '\x0b', '\xa0', '\x0d', '\xc5', '\x8d', '\x65',
    '\xe9', '\xa0', '\x23', '\x78', '\x0d', '\xa0', '\x05', '\x0b',
    '\x80', '\x98', '\x0b', '\xa0', '\x06', '\x78', '\xa0', '\x27',
    '\x0c', '\x40', '\xc1', '\xd1', '\xa0', '\x02', '\xc1', '\x1c',
    '\x40', '\x0b', '\xa0', '\x2f', '\x03', '\x10', '\x06', '\xa0',
    '\x89', '\x0b', '\x81', '\x10', '\x0b', '\xa0', '\x2f', '\xc4',
    '\x59', '\x64', '\x0b', '\xa0', '\x0d', '\xc4', '\x6b', '\x54',
    '\x09', '\xa0', '\x24', '\x78', '\x0d', '\xa0', '\x05', '\x0b',
    '\x80', '\x98', '\x0b', '\xa0', '\x05', '\x0d', '\x78', '\xa0',
    '\x27', '\xc1', '\x1c', '\x30', '\xc1', '\xe3', '\xa0', '\x02',
    '\xc1', '\x1c', '\x30', '\xc1', '\xc1', '\xa0', '\x30', '\xc1',
    '\x54', '\xa0', '\x8a', '\x0b', '\x81', '\x10', '\x0b', '\xa0',
    '\x2e', '\x07', '\x40', '\x06', '\xa0', '\x0b', '\xc1', '\x19',
    '\x30', '\xc1', '\xc2', '\xa0', '\x22', '\x70', '\x0b', '\xa0',
    '\x05', '\x08', '\x80', '\x90', '\x08', '\xa0', '\x06', '\x70',
    '\xa0', '\x27', '\xc7', '\x04', '\xd5', '\x1c', '\x60', '\xa0',
    '\x02', '\xc2', '\x05', '\x02', '\x1d', '\xc2', '\x03', '\x05',
    '\xa0', '\x2e', '\x0d', '\x30', '\x09', '\xa0', '\x49', '\xc1',
    '\xae', '\x18', '\xc1', '\xea', '\xa0', '\x38', '\x0b', '\x20',
    '\xc2', '\xb3', '\x03', '\x50', '\xc2', '\xb3', '\x03', '\x20',
    '\x0b', '\xa0', '\x2d', '\xc1', '\x3e', '\x40', '\x09', '\xa0',
    '\x0b', '\x06', '\x40', '\xc1', '\xe3', '\xa0', '\x22', '\x70',
    '\x0b', '\xa0', '\x05', '\x08', '\x80', '\x90', '\x08', '\xa0',
    '\x05', '\x0b', '\x70', '\xa0', '\x27', '\x04', '\x50', '\x08',
    '\xa0', '\x02', '\x04', '\x50', '\x05', '\xa0', '\x2a', '\x1c',
    '\xa0', '\x03', '\xc2', '\x02', '\x09', '\xa0', '\x4a', '\xc1',
    '\xae', '\x18', '\xc1', '\xea', '\xa0', '\x38', '\x0b', '\x20',
    '\xc1', '\x81', '\x7b', '\xc1', '\x18', '\x20', '\x0b', '\xa0',
    '\x2d', '\x08', '\x10', '\xc5', '\x84', '\x17', '\x80', '\xa0',
    '\x09', '\xc3', '\x1d', '\x40', '\x18', '\xc2', '\x02', '\x02',
    '\xa0', '\x22', '\x70', '\x0b', '\xa0', '\x05', '\x08', '\x80',
    '\x90', '\x08', '\xa0', '\x06', '\x70', '\xa0', '\x27', '\x10',
    '\x0a', '\x1f', '\xc2', '\x04', '\x04', '\xa0', '\x02', '\xc2',
    '\x01', '\x08', '\x1f', '\x08', '\x10', '\xa0', '\x29', '\xc1',
    '\x8b', '\xa0', '\x02', '\x09', '\x40', '\x09', '\xa0', '\x47',
    '\xc1', '\x18', '\x30', '\xc1', '\x81', '\xa0', '\x37', '\x0b',
    '\x10', '\xc4', '\xe3', '\xef', '\x05', '\x30', '\xc4', '\xe5',
    '\xef', '\x03', '\x10', '\x0b', '\xa0', '\x2d', '\x04', '\x60',
    '\x0b', '\xa0', '\x09', '\x08', '\x60', '\x04', '\xa0', '\x22',
    '\x70', '\x0b', '\xa0', '\x05', '\x08', '\x80', '\x90', '\x08',
    '\xa0', '\x05', '\x0b', '\x70', '\xa0', '\x27', '\x60', '\x04',
    '\xa0', '\x02', '\x70', '\xa0', '\x2a', '\xc2', '\x1b', '\x0c',
    '\xa0', '\x02', '\x20', '\x0c', '\xa0', '\x48', '\xc1', '\x18',
    '\x30', '\xc1', '\x81', '\xa0', '\x37', '\x0b', '\x10', '\xc3',
    '\x03', '\xc1', '\x5f', '\xc3', '\x1c', '\x30', '\x10', '\x0b',
    '\xa0', '\x2d', '\xc2', '\x02', '\x05', '\x2f', '\xc2', '\x09',
    '\x02', '\xa0', '\x09', '\xc2', '\x08', '\x02', '\x2f', '\xc3',
    '\x1e', '\xa0', '\xa0', '\x21', '\x10', '\x04', '\x1f', '\x08',
    '\x10', '\x0b', '\xa0', '\x05', '\x08', '\x10', '\x0b', '\x3f',
    '\x0b', '\x10', '\x08', '\xa0', '\x06', '\x10', '\x04', '\x1f',
    '\x04', '\x10', '\xa0', '\x27', '\x10', '\x07', '\x1f', '\xc2',
    '\x03', '\x04', '\xa0', '\x02', '\xc2', '\x03', '\x04', '\x1f',
    '\x05', '\x10', '\xa0', '\x29', '\xc3', '\x0b', '\xe5', '\xa0',
    '\x00', '\x07', '\x10', '\x04', '\x20', '\x09', '\xa0', '\x44',
    '\xc1', '\x4e', '\x70', '\xc1', '\xe4', '\xa0', '\x35', '\x0b',
    '\x10', '\x09', '\x3f', '\x06', '\x10', '\x06', '\x3f', '\x09',
    '\x10', '\x0b', '\xa0', '\x2c', '\x0d', '\x70', '\x06', '\xa0',
    '\x09', '\x02', '\x70', '\x0d', '\xa0', '\x21', '\x70', '\x0b',
    '\xa0', '\x05', '\x08', '\x80', '\x90', '\x08', '\xa0', '\x05',
    '\x0b', '\x70', '\xa0', '\x27', '\x60', '\x04', '\xa0', '\x02',
    '\x01', '\x60', '\xa0', '\x2a', '\xc3', '\x0b', '\xc1', '\xa0',
    '\x01', '\x20', '\xc1', '\xc1', '\xa0', '\x45', '\xc1', '\x3e',
    '\x70', '\xc1', '\xe3', '\xa0', '\x35', '\x0b', '\x10', '\xc4',
    '\x6b', '\x10', '\x0c', '\x3f', '\xc4', '\x1c', '\x60', '\x0b',
    '\x10', '\x0b', '\xa0', '\x2d', '\x10', '\x0b', '\x2f', '\x0e',
    '\x10', '\xa0', '\x09', '\xc2', '\x04', '\x07', '\x3f', '\xc2',
    '\x04', '\x08', '\xa0', '\x21', '\x10', '\x04', '\x1f', '\x08',
    '\x10', '\x0b', '\xa0', '\x05', '\x08', '\x10', '\x0b', '\x3f',
    '\x0b', '\x10', '\x08', '\xa0', '\x06', '\x10', '\x04', '\x1f',
    '\x04', '\x10', '\xa0', '\x27', '\x05', '\x10', '\xc1', '\x23',
    '\x10', '\x08', '\xa0', '\x02', '\x08', '\x10', '\xc1', '\x32',
    '\x10', '\x05', '\xa0', '\x29', '\x0b', '\x10', '\xc6', '\xc3',
    '\x04', '\xf3', '\x06', '\x20', '\x34', '\xc3', '\x85', '\xc9',
    '\xa0', '\x3c', '\x07', '\x10', '\xc1', '\xa3', '\x1f', '\xc1',
    '\x3a', '\x10', '\x07', '\xa0', '\x35', '\x0b', '\x10', '\xc1',
    '\xc1', '\x3f', '\x17', '\x3f', '\xc1', '\x1c', '\x10', '\x0b',
    '\xa0', '\x2c', '\x0b', '\x70', '\x04', '\xa0', '\x09', '\x80',
    '\x80', '\x0b', '\xa0', '\x21', '\x70', '\x0b', '\xa0', '\x05',
    '\x08', '\x80', '\x90', '\x08', '\xa0', '\x05', '\x0b', '\x70',
    '\xa0', '\x27', '\x03', '\x50', '\x09', '\xa0', '\x02', '\x07',
    '\x50', '\x05', '\xa0', '\x2a', '\x0b', '\x10', '\xc2', '\xa1',
    '\x0b', '\x40', '\x08', '\xa0', '\x44', '\x07', '\x80', '\x90',
    '\x07', '\xa0', '\x35', '\x0b', '\x10', '\xc5', '\xfb', '\x06',
    '\xc1', '\x1f', '\xc5', '\x1c', '\x60', '\xbf', '\x10', '\x0b',
    '\xa0', '\x2d', '\x10', '\x0a', '\x2f', '\xc2', '\x0c', '\x01',
    '\xa0', '\x09', '\xc2', '\x06', '\x05', '\x3f', '\xc2', '\x03',
    '\x08', '\xa0', '\x21', '\x10', '\x04', '\x1f', '\x08', '\x10',
    '\x0b', '\xa0', '\x05', '\x08', '\x10', '\x0b', '\x3f', '\x0b',
    '\x10', '\x08', '\xa0', '\x06', '\x10', '\x04', '\x1f', '\x04',
    '\x10', '\xa0', '\x27', '\xc1', '\x1c', '\x30', '\xc1', '\xe4',
    '\xa0', '\x03', '\x05', '\x30', '\xc1', '\xe4', '\xa0', '\x29',
    '\x0b', '\x20', '\xc3', '\x21', '\x80', '\x1f', '\x06', '\x80',
    '\x90', '\xc3', '\x51', '\xe9', '\xa0', '\x37', '\x0d', '\x10',
    '\xc1', '\xe2', '\x3f', '\xc1', '\x2e', '\x10', '\x0d', '\xa0',
    '\x34', '\x0b', '\x20', '\xc1', '\xb1', '\x7f', '\xc1', '\x1b',
    '\x20', '\x0b', '\xa0', '\x2c', '\x0c', '\x70', '\x05', '\xa0',
    '\x09', '\x02', '\x70', '\x0c', '\xa0', '\x21', '\x70', '\x0b',
    '\xa0', '\x05', '\x08', '\x80', '\x90', '\x08', '\xa0', '\x05',
    '\x0b', '\x70', '\xa0', '\x27', '\x0b', '\x40', '\x06', '\xa0',
    '\x03', '\xc1', '\x2e', '\x30', '\xc1', '\xe5', '\xa0', '\x2a',
    '\x0b', '\x80', '\xa0', '\xc1', '\xa2', '\xa0', '\x41', '\x0d',
    '\x80', '\xb0', '\x0d', '\xa0', '\x34', '\x0b', '\x10', '\x0b',
    '\x1f', '\xc2', '\x06', '\x01', '\x1c', '\xc2', '\x01', '\x06',
    '\x1f', '\x0b', '\x10', '\x0b', '\xa0', '\x2d', '\xc8', '\x05',
    '\xd3', '\xef', '\x05', '\x06', '\xa0', '\x09', '\x0b', '\x10',
    '\x0b', '\x1f', '\x08', '\x10', '\x0d', '\xa0', '\x21', '\x10',
    '\x04', '\x1f', '\x08', '\x80', '\xb0', '\x0b', '\x3f', '\x0b',
    '\x80', '\xb0', '\x04', '\x1f', '\x04', '\x10', '\xa0', '\x28',
    '\x07', '\x30', '\x0c', '\xa0', '\x03', '\xc1', '\x2e', '\x30',
    '\xc1', '\xe2', '\xa0', '\x2a', '\x50', '\x08', '\x2f', '\xc1',
    '\x18', '\x80', '\xa0', '\xc2', '\x61', '\x0d', '\xa0', '\x35',
    '\x08', '\x10', '\x09', '\x5f', '\x09', '\x10', '\x08', '\xa0',
    '\x34', '\x0b', '\x40', '\x09', '\x5f', '\x09', '\x40', '\x0b',
    '\xa0', '\x2d', '\x02', '\x60', '\x09', '\xa0', '\x09', '\x08',
    '\x60', '\x02', '\xa0', '\x22', '\x82', '\x90', '\xa0', '\x28',
    '\x05', '\x30', '\x0e', '\xa0', '\x03', '\xc1', '\x1e', '\x30',
    '\x03', '\xa0', '\x2b', '\x0d', '\x80', '\xd0', '\xc3', '\x63',
    '\xc8', '\xa0', '\x3c', '\x08', '\x80', '\xb0', '\x08', '\xa0',
    '\x34', '\x0b', '\x10', '\x0b', '\x2f', '\x06', '\x30', '\x06',
    '\x2f', '\x0b', '\x10', '\x0b', '\xa0', '\x2d', '\xc1', '\x1c',
    '\x40', '\xc1', '\xe1', '\xa0', '\x0a', '\x05', '\x50', '\x08',
    '\xa0', '\x22', '\x10', '\x04', '\x1f', '\x08', '\x80', '\xb0',
    '\x0b', '\x3f', '\x0b', '\x80', '\xb0', '\x04', '\x1f', '\x04',
    '\x10', '\xa0', '\x27', '\x09', '\x10', '\x01', '\x10', '\xc1',
    '\xe3', '\xa0', '\x02', '\x04', '\x10', '\xc1', '\x86', '\x10',
    '\x04', '\xa0', '\x2a', '\x10', '\xc4', '\xeb', '\x04', '\x05',
    '\x3f', '\xc7', '\x6e', '\x74', '\xda', '\x2f', '\x60', '\xc1',
    '\xd6', '\xa0', '\x33', '\x08', '\x10', '\x0b', '\x5f', '\x0b',
    '\x10', '\x08', '\xa0', '\x34', '\x0b', '\x40', '\xc1', '\xe2',
    '\x3f', '\xc1', '\x2e', '\x40', '\x0b', '\xa0', '\x2d', '\x0a',
    '\x50', '\x03', '\xa0', '\x0b', '\x02', '\x50', '\x0b', '\xa0',
    '\x22', '\x82', '\x90', '\xa0', '\x27', '\x09', '\x40', '\xc1',
    '\xe3', '\xa0', '\x01', '\xc1', '\x3e', '\x50', '\x06', '\xa0',
    '\x2b', '\x01', '\x81', '\x10', '\xc2', '\x84', '\x0d', '\xa0',
    '\x38', '\x08', '\x80', '\xb0', '\x08', '\xa0', '\x34', '\x0b',
    '\x10', '\x0b', '\x3f', '\x04', '\x10', '\x04', '\x3f', '\x0b',
    '\x10', '\x0b', '\xa0', '\x2e', '\xc1', '\x6d', '\x40', '\x0b',
    '\xa0', '\x0a', '\x02', '\x30', '\xc1', '\xa3', '\xa0', '\x23',
    '\x10', '\x04', '\x1f', '\x0b', '\x80', '\xb8', '\x0d', '\x3f',
    '\x0d', '\x80', '\xb8', '\x09', '\x1f', '\x04', '\x10', '\xa0',
    '\x26', '\xc5', '\x1c', '\x30', '\x9c', '\x10', '\x03', '\x1e',
    '\x05', '\x10', '\x06', '\x1f', '\x04', '\x10', '\x08', '\xa0',
    '\x29', '\xc1', '\x41', '\x1f', '\x0a', '\x10', '\x0b', '\x80',
    '\x9f', '\x09', '\x70', '\xc1', '\x81', '\xa0', '\x32', '\x09',
    '\x10', '\x0a', '\x5f', '\x0a', '\x10', '\x08', '\xa0', '\x34',
    '\x0b', '\x30', '\xc1', '\xe3', '\x5f', '\xc1', '\x3e', '\x30',
    '\x0b', '\xa0', '\x2e', '\xc1', '\x4c', '\x40', '\x0d', '\xa0',
    '\x09', '\x0d', '\x40', '\xc1', '\xc4', '\xa0', '\x23', '\x82',
    '\x90', '\xa0', '\x26', '\x0c', '\x60', '\x03', '\x1e', '\x03',
    '\x70', '\x0a', '\xa0', '\x2a', '\x06', '\x50', '\x03', '\x80',
    '\xd0', '\xc1', '\xa4', '\xa0', '\x36', '\x09', '\x80', '\xb0',
    '\x08', '\xa0', '\x26', '\xc1', '\x8a', '\x34', '\xc2', '\x97',
    '\x0d', '\xa0', '\x04', '\x0b', '\x10', '\x0b', '\x2f', '\x09',
    '\x30', '\x09', '\x2f', '\x0b', '\x10', '\x0b', '\xa0', '\x04',
    '\xc2', '\x9d', '\x07', '\x34', '\xc1', '\xa8', '\xa0', '\x21',
    '\x0d', '\x10', '\xc3', '\x65', '\x40', '\xa0', '\x09', '\xc6',
    '\x08', '\x85', '\x01', '\x08', '\xa0', '\x24', '\x10', '\x04',
    '\x82', '\x3f', '\x04', '\x10', '\xa0', '\x25', '\xc4', '\x3e',
    '\x30', '\x0e', '\x1f', '\x09', '\x10', '\xc1', '\x31', '\x10',
    '\x06', '\x2f', '\xc1', '\x1d', '\x10', '\x0c', '\xa0', '\x28',
    '\xc4', '\x04', '\xfe', '\x0b', '\x10', '\xc1', '\xe2', '\x80',
    '\x9f', '\x1b', '\xc2', '\xfe', '\x01', '\x40', '\xc1', '\xd3',
    '\xa0', '\x30', '\xc3', '\x1e', '\x60', '\x5f', '\x04', '\x10',
    '\x0c', '\xa0', '\x26', '\xc1', '\x9d', '\x48', '\x0b', '\xa0',
    '\x05', '\x0b', '\x20', '\xc1', '\xe5', '\x2f', '\x1e', '\x2f',
    '\xc1', '\x5e', '\x20', '\x0b', '\xa0', '\x05', '\x0b', '\x48',
    '\xc1', '\xd9', '\xa0', '\x21', '\x0a', '\x40', '\x06', '\xa0',
    '\x09', '\x06', '\x40', '\x0a', '\xa0', '\x24', '\x82', '\x90',
    '\xa0', '\x25', '\xc1', '\x1e', '\x70', '\xc1', '\x32', '\x80',
    '\x80', '\xc1', '\xd1', '\xa0', '\x29', '\x0a', '\x40', '\xc1',
    '\xa6', '\x70', '\xc4', '\xb8', '\x68', '\x01', '\x20', '\xc1',
    '\x92', '\xa0', '\x35', '\x02', '\x80', '\xa0', '\x0c', '\xa0',
    '\x23', '\xc2', '\x8e', '\x01', '\x80', '\x80', '\xc1', '\x93',
    '\xa0', '\x02', '\x0b', '\x10', '\x0b', '\x1f', '\x09', '\x10',
    '\x18', '\x10', '\x09', '\x1f', '\x0b', '\x10', '\x0b', '\xa0',
    '\x02', '\xc1', '\x39', '\x80', '\x80', '\xc2', '\x81', '\x0e',
    '\xa0', '\x1e', '\x09', '\x10', '\xc4', '\xef', '\x01', '\x0b',
    '\xa0', '\x07', '\xc7', '\x1e', '\xd0', '\x7f', '\x50', '\xa0',
    '\x24', '\x10', '\x04', '\x82', '\x3f', '\x04', '\x10', '\xa0',
    '\x25', '\xc3', '\x05', '\xd1', '\x3f', '\x09', '\x30', '\x06',
    '\x4f', '\x08', '\x10', '\x03', '\xa0', '\x28', '\xc5', '\x05',
    '\x85', '\x13', '\x10', '\x05', '\x80', '\xdf', '\xc3', '\x06',
    '\x14', '\x20', '\xc1', '\xa1', '\xa0', '\x30', '\x09', '\x10',
    '\xc1', '\xe8', '\x1f', '\xc1', '\x6e', '\x10', '\x06', '\xa0',
    '\x25', '\xc1', '\x39', '\x70', '\xc2', '\x61', '\x0b', '\xa0',
    '\x02', '\x0b', '\x10', '\x06', '\x3f', '\x0e', '\x13', '\x0e',
    '\x3f', '\x06', '\x10', '\x0b', '\xa0', '\x02', '\xc1', '\x6b',
    '\x80', '\x80', '\xc1', '\x92', '\xa0', '\x1f', '\x07', '\x50',
    '\x0d', '\xa0', '\x07', '\x0d', '\x50', '\x07', '\xa0', '\x24',
    '\x82', '\x90', '\xa0', '\x25', '\x04', '\x81', '\x40', '\x04',
    '\xa0', '\x2a', '\x02', '\x30', '\xc2', '\xfd', '\x01', '\x50',
    '\x04', '\x4f', '\xc1', '\x29', '\x20', '\xc1', '\xb3', '\xa0',
    '\x33', '\x0a', '\x80', '\x90', '\x08', '\xa0', '\x23', '\xc1',
    '\x1a', '\x80', '\xc0', '\xc1', '\x92', '\xa0', '\x00', '\x0b',
    '\x10', '\xc2', '\xfb', '\x09', '\x10', '\x09', '\x1f', '\x09',
    '\x10', '\xc2', '\xf9', '\x0b', '\x10', '\x0b', '\xa0', '\x00',
    '\xc1', '\x29', '\x80', '\xc0', '\xc1', '\xa1', '\xa0', '\x1d',
    '\xc2', '\x06', '\x05', '\x1f', '\xc2', '\x08', '\x04', '\xa0',
    '\x07', '\xc2', '\x08', '\x06', '\x1f', '\xc2', '\x0a', '\x01',
    '\xa0', '\x24', '\x10', '\x04', '\x82', '\x3f', '\x03', '\x10',
    '\xa0', '\x24', '\x08', '\x10', '\x0b', '\x5f', '\x07', '\x10',
    '\x03', '\x6f', '\x04', '\x10', '\x08', '\xa0', '\x27', '\x08',
    '\x20', '\xc2', '\xe5', '\x05', '\x10', '\x0d', '\x80', '\xcf',
    '\x0e', '\x1f', '\x07', '\x40', '\x09', '\xa0', '\x2e', '\xc1',
    '\x5d', '\x10', '\xc1', '\xe2', '\x1f', '\xc1', '\x3e', '\x10',
    '\x09', '\xa0', '\x23', '\xc1', '\x3c', '\x80', '\xc0', '\xc1',
    '\xb4', '\xa0', '\x00', '\x0b', '\x10', '\x0a', '\x2f', '\xc1',
    '\x1c', '\x10', '\xc1', '\xc1', '\x2f', '\x0a', '\x10', '\x0b',
    '\xa0', '\x00', '\xc1', '\x4b', '\x80', '\xc0', '\xc1', '\xc3',
    '\xa0', '\x1d', '\x03', '\x50', '\x06', '\xa0', '\x07', '\x05',
    '\x50', '\x04', '\xa0', '\x24', '\x82', '\x90', '\xa0', '\x24',
    '\x08', '\x80', '\xa0', '\x03', '\x1b', '\xc1', '\x28', '\x60',
    '\x0a', '\xa0', '\x29', '\x08', '\x20', '\x03', '\x1f', '\xc1',
    '\x19', '\x40', '\xc1', '\xe2', '\x5f', '\xc1', '\x18', '\x20',
    '\xc1', '\xe7', '\xa0', '\x30', '\xc1', '\x4b', '\x80', '\x90',
    '\x08', '\xa0', '\x22', '\x09', '\x20', '\xc3', '\x61', '\xda',
    '\x1f', '\xc3', '\xbe', '\x58', '\x30', '\xc1', '\x84', '\x10',
    '\xc1', '\x9b', '\x10', '\x09', '\x3f', '\x09', '\x10', '\xc1',
    '\xb9', '\x10', '\xc1', '\x49', '\x30', '\xc3', '\x85', '\xeb',
    '\x1f', '\xc3', '\xad', '\x16', '\x20', '\x09', '\xa0', '\x1c',
    '\xc2', '\x02', '\x08', '\x1f', '\xc3', '\x1e', '\xb0', '\xa0',
    '\x05', '\xc3', '\x1e', '\xd0', '\x1f', '\x0e', '\x10', '\x0c',
    '\xa0', '\x23', '\x03', '\x10', '\x08', '\x82', '\x0f', '\xc1',
    '\x6e', '\x10', '\x05', '\xa0', '\x23', '\xc3', '\x1c', '\x80',
    '\x7f', '\xc3', '\x06', '\xc1', '\x5f', '\x0c', '\x10', '\xc1',
    '\xd1', '\xa0', '\x26', '\x08', '\x10', '\xc1', '\xd1', '\x1f',
    '\xc2', '\x1a', '\x0b', '\x80', '\xff', '\x0c', '\x50', '\x09',
    '\xa0', '\x2b', '\xc1', '\x6d', '\x20', '\xc1', '\xc1', '\x3f',
    '\xc1', '\x1c', '\x10', '\xc1', '\xd6', '\xa0', '\x20', '\xc1',
    '\x19', '\x80', '\xf0', '\xc1', '\xa5', '\x10', '\xc4', '\xd1',
    '\xcf', '\x01', '\x30', '\xc4', '\xc1', '\xdf', '\x01', '\x10',
    '\xc1', '\x5a', '\x80', '\xf0', '\xc1', '\x91', '\xa0', '\x1b',
    '\x0d', '\x70', '\x0d', '\xa0', '\x05', '\x0b', '\x70', '\xa0',
    '\x24', '\x06', '\x82', '\x70', '\x06', '\xa0', '\x23', '\x0c',
    '\x80', '\xb0', '\x09', '\x2f', '\xc1', '\x3e', '\x50', '\xc1',
    '\xe2', '\xa0', '\x27', '\xc1', '\x1c', '\x20', '\x04', '\x3f',
    '\x04', '\x40', '\xc2', '\x71', '\x0d', '\x4f', '\xc1', '\x3d',
    '\x20', '\xc1', '\xc3', '\xa0', '\x2d', '\xc1', '\x4c', '\x80',
    '\xc0', '\xc1', '\xb4', '\xa0', '\x1f', '\x09', '\x20', '\xc1',
    '\xe5', '\x80', '\x8f', '\xc2', '\x8e', '\x01', '\x40', '\x06',
    '\x10', '\x09', '\x5f', '\x09', '\x10', '\x06', '\x40', '\xc2',
    '\x81', '\x0e', '\x80', '\x8f', '\xc1', '\x5e', '\x20', '\x09',
    '\xa0', '\x1a', '\x0c', '\x10', '\x0c', '\x2f', '\xc2', '\x08',
    '\x04', '\xa0', '\x05', '\xc2', '\x08', '\x06', '\x3f', '\xc2',
    '\x03', '\x08', '\xa0', '\x23', '\xc1', '\x3e', '\x10', '\x06',
    '\x81', '\xef', '\xc1', '\x3e', '\x10', '\xc1', '\xe3', '\xa0',
    '\x23', '\xc2', '\x03', '\x02', '\x80', '\x9f', '\xc3', '\x04',
    '\xd1', '\x5f', '\x07', '\x10', '\x06', '\xa0', '\x26', '\x0b',
    '\x10', '\x08', '\x3f', '\x0e', '\x81', '\x1f', '\xc1', '\x14',
    '\x40', '\x09', '\xa0', '\x28', '\xc2', '\x7c', '\x01', '\x20',
    '\xc1', '\xc2', '\x5f', '\xc1', '\x3e', '\x20', '\xc2', '\x83',
    '\x0c', '\xa0', '\x1c', '\x0a', '\x81', '\x60', '\xc1', '\x73',
    '\x70', '\xc1', '\x37', '\x81', '\x60', '\x0b', '\xa0', '\x1a',
    '\x09', '\x70', '\x08', '\xa0', '\x05', '\x04', '\x70', '\x0b',
    '\xa0', '\x24', '\x06', '\x82', '\x50', '\x06', '\xa0', '\x23',
    '\xc1', '\x2e', '\x80', '\xb0', '\x04', '\x3f', '\xc1', '\x3e',
    '\x50', '\x08', '\xa0', '\x26', '\xc1', '\x3e', '\x30', '\x04',
    '\x3f', '\x08', '\x70', '\xc1', '\xd6', '\x3f', '\xc1', '\x6e',
    '\x20', '\xc1', '\xa1', '\xa0', '\x2a', '\xc1', '\x6a', '\x81',
    '\x00', '\xc2', '\x62', '\x0a', '\xa0', '\x1b', '\x0c', '\x20',
    '\x08', '\x80', '\xcf', '\xc1', '\x4d', '\x50', '\x09', '\x7f',
    '\x09', '\x50', '\xc1', '\xd5', '\x80', '\xcf', '\x06', '\x10',
    '\xc1', '\xd1', '\xa0', '\x09', '\xc2', '\xbd', '\x0d', '\xa0',
    '\x0c', '\xc2', '\x08', '\x02', '\x3f', '\xc3', '\x1e', '\xb0',
    '\xa0', '\x03', '\xc3', '\x1e', '\xd0', '\x3f', '\xc2', '\x07',
    '\x05', '\xa0', '\x0c', '\x0e', '\x1b', '\xa0', '\x14', '\xc1',
    '\x3e', '\x10', '\x06', '\x81', '\xcf', '\xc1', '\x3e', '\x10',
    '\xc1', '\xe3', '\xa0', '\x23', '\x08', '\x10', '\x0b', '\x80',
    '\x9f', '\xc4', '\x3e', '\x30', '\x0e', '\x4f', '\xc1', '\x1e',
    '\x10', '\x0d', '\xa0', '\x25', '\xc3', '\x09', '\xe1', '\x81',
    '\x9f', '\xc1', '\x3c', '\x20', '\x0a', '\xa0', '\x25', '\xc1',
    '\x4a', '\x30', '\xc2', '\x81', '\x0e', '\x80', '\x8f', '\xc1',
    '\x29', '\x30', '\xc1', '\x92', '\xa0', '\x19', '\xc1', '\x1d',
    '\x83', '\x90', '\xc1', '\xe1', '\xa0', '\x09', '\xc2', '\xbc',
    '\x0e', '\xa0', '\x0c', '\x06', '\x70', '\xc1', '\xe1', '\xa0',
    '\x03', '\x0b', '\x80', '\x80', '\x08', '\xa0', '\x0c', '\xc2',
    '\xbe', '\x0c', '\xa0', '\x15', '\x06', '\x82', '\x30', '\x06',
    '\xa0', '\x24', '\x08', '\x80', '\xd0', '\x08', '\x3f', '\xc1',
    '\x1d', '\x40', '\xc1', '\xe1', '\xa0', '\x25', '\x07', '\x40',
    '\x03', '\x3f', '\x0a', '\x80', '\x90', '\xc1', '\xe7', '\x3f',
    '\x07', '\x30', '\x09', '\xa0', '\x27', '\xc1', '\x28', '\x81',
    '\x50', '\xc2', '\x71', '\x0e', '\xa0', '\x18', '\x04', '\x10',
    '\x05', '\x80', '\xff', '\x08', '\x81', '\x30', '\x08', '\x80',
    '\xff', '\x05', '\x10', '\x06', '\xa0', '\x07', '\xc1', '\x4e',
    '\x20', '\xc1', '\xe5', '\xa0', '\x0a', '\xc2', '\x05', '\x06',
    '\x4f', '\xc2', '\x08', '\x04', '\xa0', '\x03', '\xc2', '\x06',
    '\x06', '\x4f', '\xc2', '\x0a', '\x01', '\xa0', '\x0b', '\xc1',
    '\x19', '\x10', '\xc1', '\xc2', '\xa0', '\x14', '\x06', '\x10',
    '\xc1', '\xe4', '\x81', '\x9f', '\xc1', '\x3e', '\x10', '\xc1',
    '\xe3', '\xa0', '\x23', '\xc3', '\x1d', '\x40', '\x80', '\xbf',
    '\xc3', '\x1c', '\x30', '\x5f', '\x08', '\x10', '\x06', '\xa0',
    '\x24', '\x09', '\x10', '\x09', '\x81', '\xbf', '\x08', '\x20',
    '\xc1', '\xc1', '\xa0', '\x22', '\xc1', '\x4e', '\x20', '\xc2',
    '\x61', '\x0a', '\x80', '\xdf', '\xc2', '\x6a', '\x01', '\x20',
    '\xc1', '\xb2', '\xa0', '\x17', '\x06', '\x83', '\xb0', '\x07',
    '\xa0', '\x07', '\xc1', '\x3c', '\x20', '\x08', '\xa0', '\x0b',
    '\x02', '\x80', '\x80', '\x08', '\xa0', '\x03', '\x04', '\x80',
    '\x80', '\x04', '\xa0', '\x0b', '\x08', '\x20', '\xc1', '\xc3',
    '\xa0', '\x14', '\x06', '\x30', '\x0b', '\x81', '\x8f', '\x30',
    '\xc1', '\xe3', '\xa0', '\x23', '\x0d', '\x80', '\xe0', '\xc1',
    '\xd1', '\x3f', '\x0a', '\x50', '\x08', '\xa0', '\x24', '\x0d',
    '\x60', '\x0d', '\x2f', '\x08', '\x80', '\xa0', '\xc1', '\xb1',
    '\x3f', '\x09', '\x30', '\x09', '\xa0', '\x24', '\xc1', '\x3c',
    '\x81', '\x90', '\xc1', '\x81', '\xa0', '\x16', '\x0d', '\x10',
    '\xc1', '\xe1', '\x81', '\x0f', '\x09', '\x81', '\x10', '\x09',
    '\x81', '\x0f', '\xc1', '\x1d', '\x10', '\x0e', '\xa0', '\x06',
    '\xc6', '\x04', '\x42', '\x01', '\x05', '\xa0', '\x0a', '\xc2',
    '\x01', '\x0a', '\x4f', '\x0e', '\x10', '\x0b', '\xa0', '\x01',
    '\x0d', '\x10', '\x0d', '\x4f', '\x0e', '\x10', '\x0c', '\xa0',
    '\x09', '\x0a', '\x40', '\xc1', '\xd1', '\xa0', '\x14', '\x06',
    '\x81', '\xf0', '\xc1', '\xe3', '\xa0', '\x24', '\x05', '\x10',
    '\x0d', '\x80', '\xcf', '\x0a', '\x10', '\x06', '\x4f', '\xc4',
    '\x1e', '\x10', '\x0e', '\xa0', '\x22', '\x09', '\x10', '\x09',
    '\x81', '\xcf', '\x0a', '\x30', '\xc1', '\xe3', '\xa0', '\x20',
    '\xc1', '\x2c', '\x10', '\xc1', '\x92', '\x81', '\x3f', '\xc1',
    '\x29', '\x20', '\x08', '\xa0', '\x15', '\x0d', '\x81', '\xc0',
    '\x01', '\x14', '\x01', '\x81', '\xb0', '\xc1', '\xe1', '\xa0',
    '\x06', '\x02', '\x40', '\x08', '\xa0', '\x09', '\x0d', '\x80',
    '\x90', '\xc1', '\xe1', '\xa0', '\x01', '\x0b', '\x80', '\xa0',
    '\xa0', '\x0a', '\x08', '\x40', '\x02', '\xa0', '\x15', '\x06',
    '\x20', '\x0b', '\x81', '\x8f', '\x20', '\xc1', '\xe3', '\xa0',
    '\x24', '\x04', '\x80', '\xf0', '\x03', '\x4f', '\x04', '\x40',
    '\x02', '\xa0', '\x24', '\x06', '\x60', '\xc4', '\x93', '\x9b',
    '\x01', '\x80', '\xc0', '\x09', '\x3f', '\x09', '\x30', '\x09',
    '\xa0', '\x22', '\xc1', '\x1c', '\x81', '\xc0', '\x06', '\xa0',
    '\x15', '\x08', '\x10', '\x07', '\x81', '\x2f', '\x09', '\x10',
    '\x03', '\x80', '\x98', '\x03', '\x10', '\x09', '\x81', '\x2f',
    '\x06', '\x10', '\x09', '\xa0', '\x05', '\xc2', '\x0c', '\x03',
    '\x1f', '\xc3', '\x1e', '\xd0', '\xa0', '\x08', '\x0c', '\x10',
    '\x0e', '\x5f', '\xc2', '\x06', '\x04', '\xa0', '\x01', '\xc2',
    '\x06', '\x06', '\x6f', '\xc2', '\x03', '\x08', '\xa0', '\x09',
    '\xc7', '\x04', '\xf8', '\x5e', '\x80', '\xa0', '\x15', '\x05',
    '\x81', '\xd0', '\xc1', '\xe2', '\xa0', '\x24', '\x0b', '\x10',
    '\x06', '\x80', '\xef', '\x07', '\x10', '\x09', '\x4f', '\x07',
    '\x10', '\x09', '\xa0', '\x21', '\x0c', '\x10', '\x08', '\x81',
    '\xdf', '\x0c', '\x40', '\x06', '\xa0', '\x1f', '\xc1', '\x2e',
    '\x10', '\xc1', '\xe6', '\x81', '\x6f', '\x08', '\x20', '\x08',
    '\xa0', '\x14', '\x09', '\x81', '\xb0', '\x08', '\x3f', '\x08',
    '\x81', '\xb0', '\x0a', '\xa0', '\x05', '\x09', '\x50', '\x02',
    '\xa0', '\x09', '\x09', '\x80', '\xa0', '\x08', '\xa0', '\x01',
    '\x04', '\x80', '\xa0', '\x0b', '\xa0', '\x09', '\x02', '\x50',
    '\x09', '\xa0', '\x15', '\x05', '\x10', '\x0b', '\x81', '\x8f',
    '\x10', '\xc1', '\xe2', '\xa0', '\x24', '\x0b', '\x81', '\x10',
    '\x08', '\x3f', '\x0d', '\x50', '\x0a', '\xa0', '\x22', '\x0d',
    '\x81', '\xa0', '\x09', '\x3f', '\x07', '\x20', '\xc1', '\xc1',
    '\xa0', '\x20', '\xc1', '\x1d', '\x81', '\xe0', '\x07', '\xa0',
    '\x14', '\x06', '\x10', '\x0b', '\x81', '\x3f', '\x07', '\x10',
    '\x0c', '\x7f', '\x0c', '\x10', '\x07', '\x81', '\x3f', '\x0b',
    '\x10', '\x08', '\xa0', '\x05', '\xc2', '\x0b', '\x08', '\x2f',
    '\xc2', '\x04', '\x0b', '\xa0', '\x08', '\xc2', '\x08', '\x03',
    '\x6f', '\x0d', '\x10', '\x1d', '\x10', '\x0d', '\x6f', '\xc2',
    '\x08', '\x06', '\xa0', '\x09', '\x10', '\x2f', '\xc2', '\x09',
    '\x04', '\xa0', '\x15', '\x08', '\x10', '\x06', '\x81', '\x88',
    '\x10', '\x04', '\xa0', '\x25', '\x04', '\x10', '\x0d', '\x80',
    '\xff', '\xc3', '\x03', '\xd1', '\x3f', '\x0d', '\x10', '\x03',
    '\xa0', '\x21', '\xc2', '\x02', '\x05', '\x81', '\xff', '\xc3',
    '\x8b', '\x46', '\x10', '\x0a', '\xa0', '\x1e', '\x05', '\x10',
    '\x08', '\x81', '\x9f', '\x0c', '\x20', '\x0d', '\xa0', '\x13',
    '\x07', '\x80', '\xa0', '\xc4', '\x42', '\x46', '\x01', '\x80',
    '\xa0', '\x05', '\x5f', '\x05', '\x80', '\xa0', '\xc4', '\x41',
    '\x46', '\x02', '\x80', '\xa0', '\x08', '\xa0', '\x05', '\x08',
    '\x60', '\xa0', '\x09', '\x05', '\x80', '\xa0', '\xc2', '\xe1',
    '\x0a', '\x80', '\xb0', '\x08', '\xa0', '\x09', '\x60', '\x08',
    '\xa0', '\x15', '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10',
    '\x04', '\xa0', '\x25', '\x04', '\x81', '\x20', '\x0c', '\x3f',
    '\x06', '\x40', '\x06', '\xa0', '\x22', '\x07', '\x81', '\xb0',
    '\x09', '\x3f', '\x06', '\x20', '\xc1', '\xe2', '\xa0', '\x1f',
    '\x04', '\x82', '\x00', '\x0b', '\xa0', '\x13', '\x04', '\x10',
    '\x81', '\x5f', '\xc3', '\x04', '\xe2', '\x5f', '\xc3', '\x2e',
    '\x40', '\x81', '\x5f', '\x10', '\x04', '\xa0', '\x05', '\xc2',
    '\x0c', '\x04', '\x1f', '\xc3', '\x1e', '\xd0', '\xa0', '\x08',
    '\xc2', '\x04', '\x08', '\x7f', '\xc1', '\x06', '\x16', '\xc1',
    '\x60', '\x7f', '\xc2', '\x0b', '\x02', '\xa0', '\x09', '\xc2',
    '\x03', '\x0d', '\x1f', '\xc2', '\x08', '\x07', '\xa0', '\x15',
    '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0',
    '\x24', '\x0b', '\x10', '\x05', '\x81', '\x0f', '\x0c', '\x10',
    '\x04', '\x4f', '\x03', '\x10', '\x0e', '\xa0', '\x1f', '\xc3',
    '\x0b', '\xd1', '\x4f', '\x1b', '\x0c', '\x81', '\xbf', '\xc3',
    '\x01', '\xe2', '\xa0', '\x1c', '\x0b', '\x10', '\x04', '\x81',
    '\xbf', '\x07', '\x10', '\x06', '\xa0', '\x13', '\x04', '\x80',
    '\x80', '\xc1', '\xd5', '\x4f', '\xc1', '\x6c', '\x80', '\x80',
    '\x0b', '\x5f', '\x0b', '\x80', '\x80', '\xc1', '\xb6', '\x4f',
    '\xc1', '\x5d', '\x80', '\x80', '\x05', '\xa0', '\x05', '\x09',
    '\x50', '\x01', '\xa0', '\x09', '\x01', '\x80', '\xb0', '\xc1',
    '\x28', '\x80', '\xb0', '\x04', '\xa0', '\x09', '\x01', '\x50',
    '\x08', '\xa0', '\x15', '\x08', '\x10', '\x08', '\x81', '\x8b',
    '\x10', '\x04', '\xa0', '\x24', '\x0b', '\x81', '\x30', '\x03',
    '\x3f', '\x0d', '\x40', '\x01', '\xa0', '\x22', '\x02', '\x81',
    '\xc0', '\x09', '\x3f', '\x03', '\x20', '\x06', '\xa0', '\x1e',
    '\x0b', '\x82', '\x10', '\x05', '\xa0', '\x13', '\x04', '\x10',
    '\x81', '\x5f', '\xc3', '\x1d', '\x70', '\x5f', '\xc3', '\x07',
    '\xd1', '\x81', '\x5f', '\x10', '\x04', '\xa0', '\x06', '\xc7',
    '\x03', '\x43', '\x02', '\xe1', '\xa0', '\x08', '\x10', '\x0b',
    '\x7f', '\x0d', '\x30', '\x0c', '\x80', '\x8f', '\x10', '\x0d',
    '\xa0', '\x08', '\xc7', '\x05', '\x83', '\x17', '\xd0', '\xa0',
    '\x15', '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04',
    '\xa0', '\x24', '\x06', '\x10', '\x0b', '\x81', '\x1f', '\x08',
    '\x10', '\x09', '\x3f', '\x08', '\x10', '\x09', '\xa0', '\x1f',
    '\xc2', '\x08', '\x07', '\x4f', '\x03', '\x20', '\xc2', '\x71',
    '\x0d', '\x81', '\x7f', '\x0c', '\x20', '\x08', '\xa0', '\x1c',
    '\x06', '\x10', '\x0a', '\x81', '\xbf', '\x0b', '\x10', '\x01',
    '\xa0', '\x13', '\x04', '\x70', '\x08', '\x80', '\x8f', '\xc1',
    '\x5d', '\x60', '\x0b', '\x5f', '\x0b', '\x60', '\xc1', '\xc3',
    '\x80', '\x8f', '\x08', '\x70', '\x04', '\xa0', '\x05', '\xc1',
    '\x1e', '\x40', '\x04', '\xa0', '\x08', '\x0c', '\x81', '\xb0',
    '\xa0', '\x08', '\xc1', '\x4e', '\x40', '\xc1', '\xe1', '\xa0',
    '\x15', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x24', '\x05',
    '\x81', '\x40', '\x09', '\x3f', '\x06', '\x40', '\x0b', '\xa0',
    '\x20', '\x0d', '\x10', '\xc1', '\x84', '\x2b', '\xc1', '\x17',
    '\x81', '\x50', '\x09', '\x2f', '\xc1', '\x1e', '\x20', '\x0b',
    '\xa0', '\x1d', '\x05', '\x82', '\x20', '\xa0', '\x13', '\x05',
    '\x10', '\x81', '\x6f', '\x08', '\x10', '\x0b', '\x3f', '\x0b',
    '\x10', '\x08', '\x81', '\x6f', '\x10', '\x07', '\xa0', '\x06',
    '\xc1', '\x3e', '\x20', '\xc3', '\x01', '\xc2', '\xa0', '\x06',
    '\xc2', '\x0a', '\x01', '\x80', '\x9f', '\x06', '\x10', '\x04',
    '\x80', '\x9f', '\xc2', '\x04', '\x09', '\xa0', '\x06', '\xc1',
    '\x5e', '\x40', '\xc1', '\x91', '\xa0', '\x16', '\x08', '\x10',
    '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0', '\x23', '\x0e',
    '\x10', '\x02', '\x81', '\x3f', '\xc3', '\x02', '\xe1', '\x2f',
    '\x0c', '\x10', '\x07', '\xa0', '\x1f', '\xc2', '\x05', '\x0d',
    '\x3f', '\x09', '\x50', '\xc1', '\xd2', '\x81', '\x6f', '\x08',
    '\x20', '\xc1', '\xd1', '\xa0', '\x1b', '\x01', '\x10', '\x06',
    '\x81', '\xb8', '\x06', '\x20', '\x0c', '\xa0', '\x12', '\x05',
    '\x60', '\x02', '\x80', '\xbf', '\x08', '\x50', '\x09', '\x5f',
    '\x09', '\x50', '\x06', '\x80', '\xbf', '\x02', '\x60', '\x06',
    '\xa0', '\x06', '\xc1', '\x1c', '\x40', '\xc1', '\xe3', '\xa0',
    '\x06', '\x08', '\x81', '\xb0', '\x0c', '\xa0', '\x06', '\xc1',
    '\x3d', '\x40', '\xc1', '\xc1', '\xa0', '\x16', '\x08', '\x81',
    '\xd0', '\x04', '\xa0', '\x23', '\x0e', '\x81', '\x50', '\xc1',
    '\xe1', '\x2f', '\x0b', '\x40', '\x08', '\xa0', '\x20', '\xc2',
    '\x0b', '\x08', '\x5f', '\xc1', '\x3d', '\x81', '\x50', '\x0b',
    '\x2f', '\x0b', '\x20', '\x02', '\xa0', '\x1d', '\x82', '\x30',
    '\x0b', '\xa0', '\x12', '\x08', '\x10', '\x0c', '\x81', '\x6f',
    '\xc2', '\x02', '\x03', '\x3f', '\xc2', '\x04', '\x02', '\x81',
    '\x6f', '\x0c', '\x10', '\x08', '\xa0', '\x08', '\xc7', '\x5b',
    '\x50', '\x03', '\x91', '\xa0', '\x05', '\xc2', '\x07', '\x05',
    '\x80', '\x9f', '\x0d', '\x10', '\x0b', '\x80', '\x9f', '\xc2',
    '\x08', '\x06', '\xa0', '\x05', '\xc1', '\x2c', '\x10', '\xc4',
    '\x02', '\xa2', '\x0e', '\xa0', '\x17', '\x08', '\x10', '\x0b',
    '\x81', '\x8f', '\x10', '\x04', '\xa0', '\x23', '\x09', '\x10',
    '\x08', '\x81', '\x3f', '\x0b', '\x10', '\x08', '\x3f', '\x10',
    '\x04', '\xa0', '\x1e', '\xc2', '\x0e', '\x03', '\x4f', '\x08',
    '\x30', '\xc1', '\x92', '\x81', '\x8f', '\x05', '\x30', '\x06',
    '\xa0', '\x1a', '\x0c', '\x82', '\x30', '\x0b', '\xa0', '\x12',
    '\x08', '\x60', '\x07', '\x80', '\xcf', '\x09', '\x40', '\xc1',
    '\xe2', '\x3f', '\xc1', '\x2e', '\x40', '\x09', '\x80', '\xcf',
    '\x07', '\x60', '\x08', '\xa0', '\x07', '\xc2', '\x9e', '\x03',
    '\x30', '\xc1', '\xc1', '\xa0', '\x05', '\x05', '\x81', '\xb0',
    '\x08', '\xa0', '\x05', '\xc1', '\x1a', '\x30', '\xc2', '\x83',
    '\x0e', '\xa0', '\x17', '\x08', '\x81', '\xd0', '\x04', '\xa0',
    '\x23', '\x09', '\x81', '\x60', '\x09', '\x3f', '\x02', '\x30',
    '\x05', '\xa0', '\x20', '\xc2', '\x08', '\x08', '\x6f', '\x0d',
    '\x81', '\x50', '\xc1', '\xc1', '\x2f', '\x08', '\x20', '\x08',
    '\xa0', '\x1b', '\x0b', '\x82', '\x30', '\x0b', '\xa0', '\x12',
    '\x0a', '\x10', '\x09', '\x81', '\x6f', '\x0b', '\x10', '\x09',
    '\x1f', '\x09', '\x10', '\x0b', '\x81', '\x6f', '\x09', '\x10',
    '\x0b', '\xa0', '\x09', '\x0e', '\x10', '\xc1', '\x6d', '\x10',
    '\x07', '\xa0', '\x04', '\xc2', '\x03', '\x09', '\x80', '\xaf',
    '\x14', '\x80', '\xaf', '\xc2', '\x0b', '\x02', '\xa0', '\x04',
    '\x09', '\x10', '\xc4', '\x91', '\x01', '\x09', '\xa0', '\x19',
    '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0',
    '\x23', '\x06', '\x10', '\x0d', '\x81', '\x4f', '\xc3', '\x05',
    '\xe1', '\x2f', '\xc2', '\x04', '\x01', '\xa0', '\x1e', '\xc2',
    '\x07', '\x07', '\x4f', '\xc4', '\x2a', '\x20', '\x09', '\x81',
    '\xaf', '\xc1', '\x3c', '\x30', '\x0d', '\xa0', '\x19', '\x0b',
    '\x82', '\x30', '\x0b', '\xa0', '\x12', '\x09', '\x60', '\x08',
    '\x3f', '\x0c', '\x14', '\xc1', '\xd8', '\x4f', '\x09', '\x40',
    '\xc1', '\x92', '\x1e', '\xc1', '\x29', '\x40', '\x08', '\x4f',
    '\xc1', '\x7b', '\x14', '\x0c', '\x3f', '\x08', '\x60', '\x0a',
    '\xa0', '\x09', '\x0b', '\x50', '\x09', '\xa0', '\x04', '\x81',
    '\xc0', '\x05', '\xa0', '\x04', '\x07', '\x50', '\x0b', '\xa0',
    '\x19', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x23', '\x05',
    '\x81', '\x60', '\x04', '\x3f', '\x07', '\x30', '\x04', '\xa0',
    '\x20', '\x07', '\x10', '\xc1', '\x82', '\x5f', '\x04', '\x81',
    '\x50', '\xc1', '\xe2', '\x2f', '\x02', '\x10', '\xc1', '\xe1',
    '\xa0', '\x1a', '\x0c', '\x80', '\xb4', '\x02', '\x80', '\x90',
    '\x02', '\x80', '\xb4', '\x0c', '\xa0', '\x12', '\x0e', '\x10',
    '\x06', '\x81', '\x7f', '\xc2', '\x04', '\x02', '\x1f', '\xc2',
    '\x02', '\x04', '\x81', '\x7f', '\x06', '\x10', '\x0e', '\xa0',
    '\x0a', '\xc4', '\x06', '\xf6', '\x08', '\x10', '\xc1', '\xe4',
    '\xa0', '\x01', '\x0e', '\x10', '\x0d', '\x80', '\xaf', '\x1b',
    '\x80', '\xbf', '\x10', '\x0d', '\xa0', '\x01', '\xc1', '\x6e',
    '\x10', '\xc4', '\xc3', '\x08', '\x01', '\xa0', '\x1a', '\x08',
    '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0', '\x23',
    '\xc2', '\x02', '\x02', '\x81', '\x5f', '\x0d', '\x10', '\x08',
    '\x2f', '\x04', '\x10', '\xa0', '\x1d', '\x0b', '\x10', '\x0a',
    '\x82', '\x6f', '\xc1', '\x18', '\x10', '\x07', '\xa0', '\x26',
    '\x08', '\x10', '\x0b', '\x3f', '\x0b', '\x10', '\x08', '\xa0',
    '\x1f', '\x0d', '\x60', '\x08', '\x3f', '\x04', '\x30', '\xc1',
    '\xe7', '\x3f', '\x07', '\x80', '\xd0', '\x05', '\x3f', '\xc1',
    '\x4e', '\x30', '\x04', '\x3f', '\x08', '\x60', '\x0e', '\xa0',
    '\x0a', '\x02', '\x50', '\x06', '\xa0', '\x02', '\x0b', '\x81',
    '\xc0', '\x01', '\xa0', '\x02', '\xc1', '\x4e', '\x50', '\x03',
    '\xa0', '\x1a', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x23',
    '\x01', '\x81', '\x70', '\x3f', '\x0a', '\x40', '\xa0', '\x1f',
    '\x0c', '\x40', '\xc2', '\xfc', '\x0e', '\x2b', '\x04', '\x81',
    '\x60', '\x07', '\x2f', '\x0b', '\x20', '\x08', '\xa0', '\x27',
    '\x08', '\x80', '\x90', '\x08', '\xa0', '\x20', '\xc3', '\x04',
    '\xe1', '\x81', '\x6f', '\x09', '\x10', '\x19', '\x10', '\x09',
    '\x81', '\x7f', '\xc2', '\x01', '\x04', '\xa0', '\x0b', '\xc9',
    '\x0c', '\xe1', '\xaf', '\x01', '\xc2', '\xa0', '\x00', '\xc2',
    '\x0a', '\x02', '\x81', '\x9f', '\xc2', '\x05', '\x09', '\xa0',
    '\x00', '\xc1', '\x3c', '\x10', '\xc5', '\xe6', '\x2f', '\x80',
    '\xa0', '\x1a', '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10',
    '\x04', '\xa0', '\x23', '\x10', '\x06', '\x81', '\x6f', '\xc2',
    '\x06', '\x02', '\x2f', '\x08', '\x10', '\xa0', '\x1c', '\xc3',
    '\x1e', '\x20', '\x82', '\x8f', '\xc4', '\x4d', '\x10', '\x0e',
    '\xa0', '\x25', '\x08', '\x10', '\x0b', '\x3f', '\x0b', '\x10',
    '\x08', '\xa0', '\x20', '\x03', '\x50', '\x05', '\x3f', '\x04',
    '\x40', '\xc1', '\xe3', '\x3f', '\x03', '\x80', '\xb0', '\xc1',
    '\xe2', '\x2f', '\xc1', '\x1c', '\x40', '\x04', '\x3f', '\x05',
    '\x50', '\x03', '\xa0', '\x0b', '\x09', '\x60', '\xc1', '\xe3',
    '\xa0', '\x00', '\x08', '\x81', '\xd0', '\x0c', '\xa0', '\x00',
    '\xc1', '\x1b', '\x60', '\x09', '\xa0', '\x1a', '\x08', '\x81',
    '\xd0', '\x04', '\xa0', '\x22', '\x0e', '\x81', '\x80', '\x0a',
    '\x2f', '\x0e', '\x40', '\xa0', '\x1f', '\x04', '\x40', '\xc1',
    '\x57', '\x81', '\xc0', '\x0c', '\x2f', '\x04', '\x10', '\x02',
    '\xa0', '\x27', '\x08', '\x80', '\x90', '\x08', '\xa0', '\x20',
    '\x09', '\x10', '\x09', '\x81', '\x7f', '\xc1', '\x02', '\x14',
    '\xc1', '\x20', '\x81', '\x7f', '\x09', '\x10', '\x09', '\xa0',
    '\x0c', '\xc2', '\x04', '\x08', '\x1f', '\xc7', '\x2c', '\x10',
    '\x69', '\x70', '\x81', '\x9f', '\xc4', '\x08', '\xa6', '\x01',
    '\x10', '\x08', '\x1f', '\xc3', '\x09', '\xe1', '\xa0', '\x1a',
    '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0',
    '\x22', '\x0c', '\x10', '\x08', '\x81', '\x6f', '\x0d', '\x10',
    '\x0a', '\x1f', '\x08', '\x10', '\xa0', '\x1b', '\xc1', '\x3e',
    '\x10', '\x0b', '\x82', '\x9f', '\x09', '\x10', '\x09', '\xa0',
    '\x25', '\x08', '\x10', '\x0b', '\x3f', '\x0b', '\x10', '\x08',
    '\xa0', '\x20', '\x08', '\x50', '\x01', '\x3f', '\x08', '\x50',
    '\xc1', '\xe3', '\x2f', '\x0b', '\x80', '\xb0', '\x0b', '\x2f',
    '\xc1', '\x2e', '\x50', '\x08', '\x3f', '\x01', '\x50', '\x08',
    '\xa0', '\x0c', '\x01', '\x60', '\xc2', '\xc1', '\x04', '\x81',
    '\xd0', '\x18', '\x70', '\x02', '\xa0', '\x1b', '\x08', '\x81',
    '\xd0', '\x04', '\xa0', '\x22', '\x0b', '\x81', '\x80', '\x07',
    '\x3f', '\x03', '\x30', '\xa0', '\x1e', '\x08', '\x82', '\x40',
    '\x04', '\x2f', '\x0b', '\x20', '\x0a', '\xa0', '\x26', '\x08',
    '\x80', '\x90', '\x08', '\xa0', '\x21', '\xc2', '\x01', '\x02',
    '\x81', '\x7f', '\x08', '\x30', '\x08', '\x81', '\x7f', '\xc3',
    '\x04', '\xe1', '\xa0', '\x0c', '\xc3', '\x09', '\xe1', '\x1f',
    '\xc1', '\x3e', '\x10', '\xc2', '\x01', '\x0a', '\x81', '\x9f',
    '\xc2', '\x0c', '\x01', '\x10', '\xc1', '\xb1', '\x2f', '\xc2',
    '\x02', '\x06', '\xa0', '\x1b', '\x08', '\x10', '\x0b', '\x81',
    '\x8f', '\x10', '\x04', '\xa0', '\x22', '\x0b', '\x10', '\x0b',
    '\x81', '\x7f', '\xc2', '\x04', '\x06', '\x1f', '\x08', '\x10',
    '\xa0', '\x1a', '\xc1', '\x3e', '\x10', '\x08', '\x82', '\xaf',
    '\x04', '\x10', '\x03', '\xa0', '\x25', '\x08', '\x10', '\x0b',
    '\x3f', '\x0b', '\x10', '\x08', '\xa0', '\x20', '\xc1', '\x1e',
    '\x50', '\x0a', '\x2f', '\xc1', '\x1e', '\x50', '\x07', '\x3f',
    '\x06', '\x80', '\x90', '\x04', '\x3f', '\x05', '\x50', '\xc1',
    '\xe1', '\x2f', '\x0a', '\x50', '\xc1', '\xe1', '\xa0', '\x0c',
    '\x08', '\x82', '\x70', '\x01', '\x80', '\x80', '\x08', '\xa0',
    '\x1b', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x22', '\x0b',
    '\x81', '\x80', '\x02', '\x3f', '\x04', '\x30', '\xa0', '\x1d',
    '\xc1', '\x1e', '\x82', '\x50', '\x0c', '\x2f', '\x04', '\x10',
    '\x05', '\xa0', '\x26', '\x08', '\x80', '\x90', '\x08', '\xa0',
    '\x21', '\x08', '\x10', '\x08', '\x81', '\x6f', '\x0d', '\x30',
    '\x0d', '\x81', '\x6f', '\x09', '\x10', '\x08', '\xa0', '\x0e',
    '\xc2', '\x02', '\x09', '\x3f', '\x06', '\x20', '\x0e', '\x81',
    '\xaf', '\x01', '\x10', '\xc1', '\xe3', '\x2f', '\x09', '\x10',
    '\x0d', '\xa0', '\x1b', '\x08', '\x10', '\x0b', '\x81', '\x8f',
    '\x10', '\x04', '\xa0', '\x22', '\x0b', '\x10', '\x0b', '\x81',
    '\x7f', '\xc2', '\x09', '\x02', '\x1f', '\x08', '\x10', '\xa0',
    '\x1a', '\x06', '\x10', '\x06', '\x82', '\xaf', '\x0d', '\x30',
    '\x0c', '\xa0', '\x24', '\x07', '\x10', '\x0b', '\x3f', '\x0b',
    '\x10', '\x07', '\xa0', '\x21', '\x08', '\x50', '\x05', '\x3f',
    '\x0a', '\x60', '\x0b', '\x2f', '\x0d', '\x80', '\x90', '\x0b',
    '\x2f', '\x0a', '\x60', '\x0a', '\x3f', '\x05', '\x50', '\x06',
    '\xa0', '\x0d', '\x0d', '\x83', '\x00', '\xc1', '\xe1', '\xa0',
    '\x1b', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x22', '\x0b',
    '\x81', '\x90', '\x0d', '\x2f', '\x08', '\x30', '\xa0', '\x1d',
    '\x05', '\x82', '\x60', '\x06', '\x2f', '\x09', '\x20', '\x0e',
    '\xa0', '\x25', '\x06', '\x80', '\x90', '\x06', '\xa0', '\x21',
    '\xc4', '\x2e', '\x10', '\x0e', '\x81', '\x6f', '\x03', '\x10',
    '\x03', '\x81', '\x6f', '\xc4', '\x1e', '\x10', '\x0e', '\xa0',
    '\x0e', '\xc2', '\x08', '\x02', '\x4f', '\xc2', '\x08', '\x04',
    '\x81', '\xbf', '\xc2', '\x05', '\x06', '\x4f', '\xc2', '\x03',
    '\x06', '\xa0', '\x1c', '\x08', '\x10', '\x0b', '\x81', '\x8f',
    '\x10', '\x04', '\xa0', '\x22', '\x0e', '\x10', '\x0b', '\x81',
    '\x8f', '\xc6', '\x01', '\xfc', '\x04', '\x01', '\xa0', '\x19',
    '\x06', '\x10', '\x06', '\x82', '\xbf', '\x07', '\x30', '\x07',
    '\xa0', '\x23', '\xc4', '\x1e', '\x10', '\x0e', '\x3f', '\xc4',
    '\x1e', '\x10', '\x0e', '\xa0', '\x20', '\xc1', '\x1e', '\x50',
    '\x0c', '\x3f', '\x06', '\x50', '\x02', '\x3f', '\x05', '\x70',
    '\x04', '\x3f', '\x02', '\x50', '\x07', '\x3f', '\x0c', '\x50',
    '\xc1', '\xe1', '\xa0', '\x0e', '\x06', '\x82', '\xf0', '\x08',
    '\xa0', '\x1c', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x22',
    '\x0c', '\x81', '\x90', '\x09', '\x2f', '\x08', '\x20', '\x03',
    '\xa0', '\x1c', '\x08', '\x82', '\x70', '\xc1', '\xe1', '\x2f',
    '\x01', '\x10', '\x09', '\xa0', '\x24', '\x0d', '\x80', '\xb0',
    '\x0d', '\xa0', '\x21', '\x09', '\x10', '\x04', '\x81', '\x6f',
    '\x08', '\x10', '\x08', '\x81', '\x6f', '\x05', '\x10', '\x09',
    '\xa0', '\x0f', '\x0e', '\x10', '\x09', '\x4f', '\xc1', '\x9a',
    '\x81', '\xbf', '\x19', '\x4f', '\x0b', '\x10', '\x0c', '\xa0',
    '\x1c', '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04',
    '\xa0', '\x23', '\x10', '\x08', '\x81', '\x8f', '\xc6', '\x05',
    '\xf9', '\x04', '\x04', '\xa0', '\x18', '\x09', '\x10', '\x06',
    '\x81', '\x8f', '\x0e', '\x81', '\x2f', '\x04', '\x30', '\x02',
    '\xa0', '\x23', '\x06', '\x10', '\x08', '\x5f', '\x08', '\x10',
    '\x08', '\xa0', '\x21', '\x08', '\x50', '\x04', '\x4f', '\x03',
    '\x50', '\x09', '\x2f', '\x0b', '\x70', '\x0b', '\x2f', '\x09',
    '\x50', '\x03', '\x4f', '\x04', '\x50', '\x08', '\xa0', '\x0f',
    '\x0b', '\x82', '\xf0', '\x0d', '\xa0', '\x1c', '\x08', '\x81',
    '\xd0', '\x04', '\xa0', '\x23', '\x81', '\x90', '\x05', '\x2f',
    '\x08', '\x20', '\x04', '\xa0', '\x1b', '\x0b', '\x81', '\x70',
    '\x06', '\x81', '\x00', '\x09', '\x2f', '\x05', '\x10', '\x06',
    '\xa0', '\x24', '\x06', '\x80', '\xb0', '\x06', '\xa0', '\x22',
    '\x04', '\x10', '\x08', '\x81', '\x5f', '\x0d', '\x10', '\x0d',
    '\x81', '\x5f', '\x08', '\x10', '\x04', '\xa0', '\x11', '\xc2',
    '\x06', '\x03', '\x82', '\x9f', '\xc2', '\x04', '\x04', '\xa0',
    '\x1d', '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04',
    '\xa0', '\x23', '\xc2', '\x04', '\x07', '\x81', '\x8f', '\xc6',
    '\x08', '\xf7', '\x01', '\x05', '\xa0', '\x17', '\xc3', '\x1d',
    '\x60', '\x81', '\x9f', '\x06', '\x81', '\x3f', '\x08', '\x30',
    '\x0c', '\xa0', '\x21', '\x0d', '\x10', '\x02', '\x6f', '\xc4',
    '\x1e', '\x10', '\x0e', '\xa0', '\x21', '\x03', '\x50', '\x08',
    '\x3f', '\xc1', '\x1e', '\x40', '\x04', '\x3f', '\x02', '\x50',
    '\x02', '\x3f', '\x04', '\x40', '\xc1', '\xe2', '\x3f', '\x08',
    '\x50', '\x03', '\xa0', '\x11', '\x03', '\x82', '\xd0', '\x06',
    '\xa0', '\x1d', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x23',
    '\x03', '\x81', '\x90', '\x0e', '\x1f', '\x08', '\x20', '\x07',
    '\xa0', '\x1a', '\xc1', '\x1c', '\x81', '\x60', '\xc1', '\xb9',
    '\x81', '\x00', '\x05', '\x2f', '\x09', '\x10', '\x01', '\xa0',
    '\x23', '\x0e', '\x80', '\xd0', '\x0d', '\xa0', '\x21', '\xc1',
    '\x1d', '\x10', '\x09', '\x80', '\xcf', '\xc1', '\xab', '\x18',
    '\x44', '\x10', '\x34', '\x05', '\x18', '\xc1', '\xcb', '\x80',
    '\xcf', '\x09', '\x10', '\xc1', '\xe1', '\xa0', '\x11', '\x0d',
    '\x10', '\x0b', '\x80', '\xbf', '\xc2', '\xbe', '\x0a', '\x28',
    '\x34', '\x38', '\xc1', '\xcb', '\x80', '\xbf', '\x0b', '\x10',
    '\x0b', '\xa0', '\x1d', '\x08', '\x10', '\x0b', '\x81', '\x8f',
    '\x10', '\x04', '\xa0', '\x23', '\xc2', '\x07', '\x02', '\x81',
    '\x8f', '\xc3', '\x0b', '\xd4', '\x10', '\x08', '\xa0', '\x17',
    '\xc2', '\x06', '\x05', '\x80', '\xbf', '\xc1', '\xc7', '\x80',
    '\xaf', '\xc2', '\x4a', '\x03', '\x81', '\x4f', '\xc1', '\x3c',
    '\x10', '\x08', '\xa0', '\x21', '\x07', '\x10', '\x08', '\x7f',
    '\x07', '\x10', '\x09', '\xa0', '\x21', '\x0c', '\x60', '\x0a',
    '\x3f', '\xc1', '\x1c', '\x40', '\x0d', '\x2f', '\x08', '\x50',
    '\x07', '\x2f', '\x0d', '\x40', '\xc1', '\xc1', '\x3f', '\x0a',
    '\x50', '\xc1', '\xc1', '\xa0', '\x11', '\x09', '\x82', '\xd0',
    '\x0d', '\xa0', '\x1d', '\x08', '\x81', '\xd0', '\x04', '\xa0',
    '\x23', '\x06', '\x81', '\x90', '\x09', '\x1f', '\x06', '\x20',
    '\x08', '\xa0', '\x19', '\xc1', '\x1c', '\x81', '\x60', '\x09',
    '\xa0', '\x00', '\x0a', '\x81', '\x00', '\x01', '\x2f', '\x0c',
    '\x20', '\x0c', '\xa0', '\x22', '\x08', '\x80', '\xd0', '\x06',
    '\xa0', '\x22', '\x0a', '\x20', '\x09', '\x6f', '\xc3', '\x8d',
    '\x25', '\x81', '\x50', '\xc3', '\x73', '\xe9', '\x6f', '\x09',
    '\x20', '\x0b', '\xa0', '\x13', '\xc2', '\x04', '\x04', '\x6f',
    '\xc4', '\xae', '\x48', '\x01', '\x81', '\x00', '\xc3', '\x63',
    '\xd9', '\x6f', '\xc2', '\x04', '\x02', '\xa0', '\x1e', '\x08',
    '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0', '\x23',
    '\x0a', '\x10', '\x0b', '\x81', '\x7f', '\xc3', '\x0b', '\x82',
    '\x10', '\x0b', '\xa0', '\x16', '\xc3', '\x0d', '\xe1', '\x80',
    '\xbf', '\xc4', '\x01', '\x41', '\x05', '\x48', '\xc1', '\x45',
    '\x20', '\x06', '\x81', '\x6f', '\xc2', '\x08', '\x04', '\xa0',
    '\x20', '\xc4', '\x1e', '\x10', '\x0e', '\x7f', '\x0d', '\x10',
    '\x02', '\xa0', '\x22', '\x08', '\x50', '\xc1', '\xa1', '\x3f',
    '\xc6', '\x0b', '\x53', '\xb8', '\x0d', '\x2f', '\x0e', '\x5b',
    '\x0e', '\x2f', '\xc6', '\xbd', '\x68', '\x03', '\x0c', '\x3f',
    '\xc1', '\x1b', '\x50', '\x09', '\xa0', '\x13', '\x01', '\x80',
    '\xe0', '\x03', '\x14', '\x06', '\x58', '\x24', '\x02', '\x80',
    '\xe0', '\x04', '\xa0', '\x1e', '\x08', '\x81', '\xd0', '\x04',
    '\xa0', '\x23', '\x09', '\x81', '\x90', '\xc3', '\xe2', '\x2f',
    '\x20', '\x0b', '\xa0', '\x18', '\xc1', '\x1e', '\x20', '\xc1',
    '\x43', '\x81', '\x10', '\x09', '\xa0', '\x01', '\x08', '\x81',
    '\x10', '\x0b', '\x2f', '\x20', '\x08', '\xa0', '\x22', '\x02',
    '\x80', '\xe0', '\x0e', '\xa0', '\x22', '\x08', '\x20', '\x09',
    '\x2f', '\xc2', '\x8e', '\x03', '\x80', '\xa0', '\x03', '\x54',
    '\x03', '\x80', '\xa0', '\xc2', '\x93', '\x0e', '\x2f', '\x09',
    '\x20', '\x09', '\xa0', '\x14', '\x0a', '\x10', '\x0c', '\x2f',
    '\xc2', '\x7c', '\x03', '\x70', '\x80', '\x94', '\x70', '\xc2',
    '\x62', '\x0a', '\x2f', '\x0d', '\x10', '\x09', '\xa0', '\x1e',
    '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0',
    '\x24', '\xc2', '\x01', '\x04', '\x81', '\x7f', '\x0b', '\x10',
    '\x02', '\x10', '\x0e', '\xa0', '\x16', '\xc2', '\x08', '\x07',
    '\x2f', '\xc2', '\x49', '\x07', '\x6f', '\x07', '\x80', '\xd0',
    '\x09', '\x81', '\x6f', '\x08', '\x10', '\xa0', '\x20', '\x09',
    '\x10', '\x07', '\x80', '\x9f', '\x04', '\x10', '\x0c', '\xa0',
    '\x22', '\x06', '\x60', '\x08', '\x82', '\x1f', '\x08', '\x60',
    '\x07', '\xa0', '\x14', '\x08', '\x80', '\x80', '\xc4', '\x62',
    '\xb8', '\x0e', '\x80', '\xff', '\xc4', '\xbd', '\x58', '\x01',
    '\x80', '\x80', '\x0b', '\xa0', '\x1e', '\x08', '\x81', '\xd0',
    '\x04', '\xa0', '\x23', '\x0e', '\x81', '\xa0', '\xc1', '\x54',
    '\x30', '\xa0', '\x19', '\x08', '\x10', '\xc1', '\xd3', '\x1f',
    '\xc1', '\x7e', '\x80', '\xd0', '\xc1', '\xb1', '\xa0', '\x02',
    '\x08', '\x81', '\x10', '\x08', '\x2f', '\x04', '\x10', '\x06',
    '\xa0', '\x21', '\x0c', '\x80', '\xf0', '\x08', '\xa0', '\x23',
    '\x06', '\x20', '\xc3', '\xe8', '\x18', '\x50', '\xc4', '\x63',
    '\xb8', '\x0c', '\x80', '\xbf', '\xc4', '\xbc', '\x68', '\x03',
    '\x50', '\xc3', '\x81', '\x9e', '\x20', '\x06', '\xa0', '\x16',
    '\xc5', '\x02', '\xd6', '\x27', '\x40', '\xc3', '\x74', '\xb9',
    '\x80', '\xdf', '\xc3', '\x9b', '\x47', '\x40', '\xc5', '\x71',
    '\x6d', '\x20', '\xa0', '\x1f', '\x08', '\x10', '\x0b', '\x81',
    '\x8f', '\x10', '\x04', '\xa0', '\x24', '\x06', '\x10', '\x0b',
    '\x81', '\x6f', '\x0b', '\x30', '\x03', '\xa0', '\x17', '\xc2',
    '\x05', '\x0a', '\x1f', '\xc3', '\x08', '\x81', '\x6f', '\xc1',
    '\x3e', '\x80', '\x80', '\x12', '\x10', '\x0d', '\x81', '\x5f',
    '\xc1', '\x1d', '\x10', '\x0b', '\xa0', '\x1f', '\x05', '\x10',
    '\x0c', '\x80', '\x9f', '\x09', '\x10', '\x07', '\xa0', '\x23',
    '\x04', '\x60', '\x0a', '\x81', '\xff', '\x0a', '\x60', '\x06',
    '\xa0', '\x15', '\x0d', '\x50', '\xc2', '\x83', '\x0d', '\x81',
    '\x9f', '\xc2', '\x7c', '\x02', '\x40', '\x04', '\xa0', '\x1f',
    '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x24', '\x05', '\x70',
    '\xc3', '\x61', '\xb9', '\x7f', '\xc3', '\xab', '\x27', '\x60',
    '\x04', '\xa0', '\x19', '\x04', '\x10', '\x0a', '\x3f', '\x09',
    '\x10', '\xc2', '\x43', '\x01', '\x60', '\xc2', '\x71', '\x0e',
    '\xa0', '\x03', '\x05', '\x81', '\x10', '\x05', '\x2f', '\x06',
    '\x10', '\x03', '\xa0', '\x21', '\x07', '\x80', '\xf0', '\x03',
    '\xa0', '\x24', '\x06', '\x20', '\x01', '\x30', '\xc3', '\x61',
    '\xe9', '\x81', '\x5f', '\xc3', '\x9e', '\x16', '\x30', '\x01',
    '\x20', '\x06', '\xa0', '\x17', '\x08', '\x50', '\xc3', '\x61',
    '\xea', '\x81', '\x6f', '\xc2', '\x6a', '\x01', '\x50', '\x08',
    '\xa0', '\x1f', '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10',
    '\x04', '\xa0', '\x24', '\x0b', '\x10', '\xc1', '\xe2', '\x4f',
    '\xc1', '\xbd', '\x78', '\x1b', '\x4f', '\x09', '\x30', '\x07',
    '\xa0', '\x17', '\xc2', '\x04', '\x0b', '\x1f', '\xc2', '\xbe',
    '\x0e', '\x7f', '\xc2', '\x1e', '\x00', '\x1d', '\x3b', '\x0d',
    '\xa0', '\x01', '\xc2', '\x05', '\x03', '\x81', '\x6f', '\x06',
    '\x20', '\x08', '\xa0', '\x1f', '\x10', '\x02', '\x80', '\xaf',
    '\x0e', '\x10', '\x03', '\xa0', '\x23', '\xc1', '\x5e', '\x40',
    '\x09', '\x80', '\xcf', '\x0d', '\x5b', '\x0d', '\x80', '\xcf',
    '\x09', '\x40', '\x06', '\xa0', '\x17', '\x06', '\x20', '\xc1',
    '\xc4', '\x81', '\xff', '\xc1', '\x19', '\x20', '\x09', '\xa0',
    '\x1f', '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x24', '\x0a',
    '\x50', '\xc1', '\xa4', '\x80', '\xff', '\xc1', '\x3b', '\x40',
    '\x08', '\xa0', '\x19', '\x04', '\x10', '\x3f', '\xc3', '\x19',
    '\xc2', '\x1f', '\x0a', '\x30', '\xc2', '\xd7', '\x0b', '\xa0',
    '\x06', '\x02', '\x81', '\x10', '\x02', '\x2f', '\x08', '\x20',
    '\xa0', '\x21', '\x03', '\x81', '\x00', '\x0e', '\xa0', '\x24',
    '\x08', '\x40', '\xc1', '\x94', '\x81', '\xdf', '\xc1', '\x49',
    '\x40', '\x09', '\xa0', '\x18', '\x0e', '\x30', '\xc1', '\x94',
    '\x81', '\xdf', '\xc1', '\x4a', '\x30', '\x0e', '\xa0', '\x1f',
    '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0',
    '\x25', '\x02', '\x10', '\xc5', '\xf6', '\x8d', '\x15', '\x80',
    '\xc0', '\xc4', '\x83', '\xfd', '\x05', '\x30', '\x0a', '\xa0',
    '\x17', '\xc2', '\x04', '\x0b', '\x5f', '\xc1', '\x37', '\x4f',
    '\xc2', '\x05', '\x06', '\xa0', '\x07', '\x0c', '\x10', '\x09',
    '\x81', '\x5f', '\x0b', '\x30', '\x05', '\xa0', '\x1e', '\x0b',
    '\x10', '\x06', '\x80', '\xbf', '\x03', '\x10', '\xa0', '\x25',
    '\x06', '\x20', '\x04', '\x6f', '\xc4', '\xad', '\x58', '\x04',
    '\x80', '\xb0', '\xc4', '\x54', '\xa8', '\x0d', '\x6f', '\x04',
    '\x20', '\x07', '\xa0', '\x18', '\x0c', '\x10', '\x03', '\x80',
    '\x9f', '\x0e', '\x1b', '\x0a', '\x78', '\x1b', '\x0c', '\x80',
    '\x9f', '\x0c', '\x10', '\x02', '\xa0', '\x20', '\x08', '\x81',
    '\xd0', '\x04', '\xa0', '\x25', '\x02', '\x30', '\x08', '\x81',
    '\x3f', '\x08', '\x30', '\x0c', '\xa0', '\x19', '\x05', '\x10',
    '\x1f', '\xc4', '\x4e', '\x10', '\x0c', '\x2f', '\x0b', '\x20',
    '\xc1', '\xe1', '\xa0', '\x07', '\x0d', '\x81', '\x30', '\x2f',
    '\x08', '\x20', '\x0c', '\xa0', '\x20', '\x81', '\x10', '\x0b',
    '\xa0', '\x25', '\x0c', '\x10', '\xc1', '\xd2', '\x82', '\x1f',
    '\xc1', '\x2d', '\x10', '\x0c', '\xa0', '\x1a', '\x04', '\x10',
    '\x0c', '\x82', '\x1f', '\x0d', '\x10', '\x04', '\xa0', '\x20',
    '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04', '\xa0',
    '\x25', '\x09', '\x20', '\x03', '\x81', '\x30', '\x05', '\x30',
    '\x01', '\xa0', '\x18', '\xc2', '\x06', '\x08', '\x3f', '\xc3',
    '\x3e', '\x80', '\x3f', '\xc3', '\x08', '\xe1', '\xa0', '\x06',
    '\xc3', '\x3e', '\x20', '\x81', '\x6f', '\x0a', '\x30', '\x02',
    '\xa0', '\x1e', '\x08', '\x10', '\x08', '\x80', '\xbf', '\x05',
    '\x10', '\x0c', '\xa0', '\x25', '\x0a', '\x10', '\x03', '\x2f',
    '\xc3', '\x8c', '\x15', '\x81', '\x50', '\xc3', '\x51', '\xc8',
    '\x2f', '\x03', '\x10', '\x0a', '\xa0', '\x1a', '\x10', '\x08',
    '\x4f', '\xc3', '\x8c', '\x46', '\x81', '\x00', '\xc4', '\x41',
    '\xa8', '\x0d', '\x4f', '\x10', '\x05', '\xa0', '\x20', '\x08',
    '\x10', '\x03', '\x81', '\x84', '\x10', '\x04', '\xa0', '\x25',
    '\x09', '\x20', '\x07', '\x6f', '\xc1', '\xbc', '\x28', '\xc2',
    '\xb9', '\x0e', '\x6f', '\x06', '\x10', '\x02', '\xa0', '\x1a',
    '\x09', '\x10', '\xc2', '\xf9', '\x06', '\x10', '\x08', '\x3f',
    '\x09', '\x20', '\x0a', '\xa0', '\x08', '\x08', '\x81', '\x30',
    '\x0b', '\x1f', '\x0b', '\x20', '\x0b', '\xa0', '\x1f', '\x0b',
    '\x81', '\x10', '\x08', '\xa0', '\x26', '\x10', '\x02', '\x80',
    '\xdf', '\xc1', '\x7d', '\x34', '\xc1', '\xc6', '\x80', '\xdf',
    '\x02', '\x10', '\xa0', '\x1b', '\x04', '\x10', '\x80', '\xef',
    '\x0b', '\x38', '\x0b', '\x80', '\xef', '\x10', '\x04', '\xa0',
    '\x20', '\x08', '\x10', '\x06', '\x81', '\x88', '\x10', '\x04',
    '\xa0', '\x26', '\x02', '\x50', '\xc2', '\x74', '\x08', '\x80',
    '\x9b', '\xc2', '\x78', '\x04', '\x50', '\x06', '\xa0', '\x18',
    '\xc3', '\x09', '\xc1', '\x2f', '\x04', '\x10', '\x08', '\x2f',
    '\x08', '\x10', '\x0a', '\xa0', '\x07', '\x03', '\x10', '\x0a',
    '\x81', '\x7f', '\x09', '\x30', '\xa0', '\x1e', '\x08', '\x10',
    '\x0b', '\x80', '\xbf', '\x08', '\x10', '\x0b', '\xa0', '\x26',
    '\x20', '\x15', '\x01', '\x81', '\xd0', '\x01', '\x15', '\x20',
    '\xa0', '\x1b', '\xc7', '\x02', '\xf2', '\x8d', '\x15', '\x81',
    '\x90', '\xc4', '\x62', '\xea', '\x0c', '\x10', '\x08', '\xa0',
    '\x20', '\x08', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x04',
    '\xa0', '\x26', '\x02', '\x10', '\x09', '\x2f', '\xc2', '\x7c',
    '\x03', '\x80', '\x80', '\xc3', '\x62', '\xe9', '\x2f', '\x08',
    '\x10', '\x08', '\xa0', '\x1a', '\xc1', '\x2e', '\x50', '\x0e',
    '\x3f', '\x04', '\x10', '\x05', '\xa0', '\x09', '\x03', '\x81',
    '\x30', '\x0b', '\x1f', '\x0b', '\x20', '\x09', '\xa0', '\x1f',
    '\x0b', '\x81', '\x10', '\x08', '\xa0', '\x26', '\x03', '\x10',
    '\x80', '\xdf', '\x01', '\x50', '\x01', '\x80', '\xdf', '\x10',
    '\x04', '\xa0', '\x1b', '\x08', '\x10', '\x80', '\xdf', '\x04',
    '\x50', '\x04', '\x80', '\xdf', '\x10', '\x06', '\xa0', '\x20',
    '\x08', '\x81', '\xd0', '\x04', '\xa0', '\x26', '\x0b', '\x20',
    '\xc2', '\x92', '\x0e', '\x80', '\xff', '\xc2', '\x9e', '\x02',
    '\x20', '\x0c', '\xa0', '\x18', '\xc6', '\x2e', '\x10', '\xb7',
    '\x06', '\x20', '\xc3', '\xb5', '\x39', '\x10', '\x08', '\xa0',
    '\x07', '\x06', '\x10', '\x05', '\x81', '\x9f', '\x09', '\x20',
    '\x0b', '\xa0', '\x1d', '\x08', '\x10', '\x08', '\x80', '\xbf',
    '\x08', '\x10', '\x0b', '\xa0', '\x26', '\x03', '\x80', '\xf0',
    '\xc2', '\x91', '\x0d', '\x1f', '\xc2', '\x8c', '\x01', '\x80',
    '\xf0', '\x04', '\xa0', '\x1b', '\x04', '\x81', '\x00', '\xc1',
    '\x73', '\x18', '\xc1', '\x16', '\x81', '\x00', '\x08', '\xa0',
    '\x20', '\x07', '\x10', '\x0b', '\x81', '\x8f', '\x10', '\x03',
    '\xa0', '\x26', '\x09', '\x10', '\xc3', '\xf5', '\x29', '\x50',
    '\x03', '\x14', '\x02', '\x50', '\xc3', '\xa5', '\x2e', '\x10',
    '\x0d', '\xa0', '\x1b', '\x0b', '\x40', '\x03', '\x2f', '\xc1',
    '\x5e', '\x10', '\xc1', '\xe2', '\xa0', '\x08', '\x0b', '\x81',
    '\x40', '\x08', '\x1f', '\x0b', '\x20', '\x08', '\xa0', '\x1f',
    '\x0b', '\x81', '\x10', '\x08', '\xa0', '\x26', '\x04', '\x10',
    '\x80', '\xdf', '\x01', '\x50', '\x02', '\x80', '\xdf', '\x10',
    '\x04', '\xa0', '\x1b', '\x08', '\x10', '\x0c', '\x80', '\xcf',
    '\x04', '\x50', '\x03', '\x80', '\xcf', '\x0c', '\x10', '\x08',
    '\xa0', '\x1f', '\xc1', '\x1c', '\x81', '\xe0', '\x09', '\xa0',
    '\x26', '\x05', '\x10', '\x0b', '\x81', '\x3f', '\x0b', '\x10',
    '\x06', '\xa0', '\x1a', '\xc1', '\x1c', '\x80', '\xb0', '\x09',
    '\xa0', '\x06', '\xc1', '\x3e', '\x10', '\xc1', '\xe3', '\x81',
    '\xaf', '\x08', '\x10', '\x0b', '\xa0', '\x1d', '\x0a', '\x10',
    '\x06', '\x80', '\xbf', '\x04', '\x10', '\x0d', '\xa0', '\x26',
    '\x04', '\x80', '\xf0', '\x07', '\x5f', '\x07', '\x80', '\xf0',
    '\x04', '\xa0', '\x1b', '\x06', '\x80', '\xf0', '\x08', '\x4f',
    '\xc1', '\x3e', '\x80', '\xf0', '\x09', '\xa0', '\x1f', '\x09',
    '\x20', '\x0b', '\x81', '\x8f', '\x20', '\x06', '\xa0', '\x26',
    '\x04', '\x80', '\x90', '\x0c', '\x3f', '\x08', '\x80', '\x90',
    '\x07', '\xa0', '\x1d', '\x09', '\x20', '\xc1', '\xc1', '\x1f',
    '\xc1', '\x1a', '\x10', '\xc1', '\xc1', '\xa0', '\x09', '\x02',
    '\x81', '\x40', '\x08', '\x2f', '\x20', '\x08', '\xa0', '\x1f',
    '\x0e', '\x81', '\x10', '\x0a', '\xa0', '\x26', '\x04', '\x10',
    '\x80', '\xdf', '\xc2', '\x8e', '\x07', '\x14', '\xc2', '\x98',
    '\x0e', '\x80', '\xdf', '\x10', '\x04', '\xa0', '\x1b', '\x0a',
    '\x10', '\x0b', '\x80', '\xcf', '\xc1', '\x9e', '\x38', '\xc1',
    '\xe9', '\x80', '\xcf', '\x0b', '\x10', '\x08', '\xa0', '\x1e',
    '\xc3', '\x1c', '\x60', '\x81', '\xbf', '\x06', '\x10', '\x0b',
    '\xa0', '\x25', '\x08', '\x10', '\x0b', '\x6f', '\xc1', '\x6e',
    '\x14', '\xc1', '\xe6', '\x6f', '\x0b', '\x10', '\x08', '\xa0',
    '\x1b', '\xc2', '\x8e', '\x03', '\x10', '\xc1', '\x81', '\x30',
    '\xc1', '\xb4', '\xa0', '\x06', '\xc1', '\x3e', '\x10', '\xc1',
    '\xc1', '\x81', '\xbf', '\x02', '\x10', '\x09', '\xa0', '\x1d',
    '\xc3', '\x1e', '\x10', '\x80', '\xbf', '\xc2', '\x01', '\x02',
    '\xa0', '\x27', '\x04', '\x80', '\xf0', '\x07', '\x5f', '\x07',
    '\x80', '\xf0', '\x04', '\xa0', '\x1b', '\x08', '\x80', '\xf0',
    '\x0b', '\x5f', '\x08', '\x80', '\xf0', '\x0b', '\xa0', '\x1e',
    '\x09', '\x30', '\x0b', '\x81', '\x8f', '\x30', '\x07', '\xa0',
    '\x25', '\x06', '\x10', '\xc2', '\x84', '\x03', '\x40', '\xc1',
    '\xe8', '\x1f', '\xc1', '\x6d', '\x40', '\xc2', '\x84', '\x02',
    '\x10', '\x08', '\xa0', '\x1e', '\xc3', '\x1a', '\xc2', '\xa0',
    '\x00', '\xc1', '\x5e', '\x20', '\xc1', '\xc1', '\xa0', '\x09',
    '\x07', '\x81', '\x50', '\x08', '\x2f', '\x20', '\x08', '\xa0',
    '\x20', '\x03', '\x80', '\xf0', '\xc1', '\xe1', '\xa0', '\x26',
    '\x04', '\x40', '\x06', '\x81', '\xbf', '\x06', '\x40', '\x04',
    '\xa0', '\x1b', '\x0b', '\x40', '\xc1', '\xe3', '\x81', '\xaf',
    '\x03', '\x40', '\x0a', '\xa0', '\x1d', '\xc3', '\x1c', '\x60',
    '\x81', '\xdf', '\xc3', '\x06', '\xc1', '\xa0', '\x24', '\x07',
    '\x10', '\xc1', '\xa2', '\x5f', '\x09', '\x30', '\x09', '\x5f',
    '\xc1', '\x2a', '\x10', '\x07', '\xa0', '\x1e', '\xc2', '\xbe',
    '\x0e', '\xa0', '\x00', '\x0d', '\x1b', '\x0e', '\xa0', '\x07',
    '\xc1', '\x3c', '\x10', '\xc1', '\xc1', '\x81', '\xbf', '\x08',
    '\x20', '\x08', '\xa0', '\x1e', '\x09', '\x10', '\x06', '\x80',
    '\x9f', '\x08', '\x10', '\x0a', '\xa0', '\x27', '\x04', '\x20',
    '\xc3', '\x96', '\x3b', '\x80', '\x90', '\x06', '\x3b', '\x06',
    '\x80', '\x90', '\xc3', '\xb3', '\x69', '\x20', '\x04', '\xa0',
    '\x1b', '\x08', '\x40', '\x03', '\x80', '\x90', '\xc1', '\xe5',
    '\x3f', '\xc1', '\x1b', '\x80', '\x90', '\x04', '\x40', '\x0b',
    '\xa0', '\x1d', '\x09', '\x40', '\x03', '\x81', '\x84', '\x40',
    '\x09', '\xa0', '\x24', '\x04', '\x10', '\x1d', '\x02', '\x80',
    '\xf0', '\xc2', '\xd5', '\x0a', '\x10', '\x08', '\xa0', '\x23',
    '\x06', '\x20', '\xc1', '\xe6', '\xa0', '\x09', '\x09', '\x81',
    '\x60', '\x07', '\x2f', '\x20', '\x08', '\xa0', '\x20', '\x0a',
    '\x80', '\xf0', '\x09', '\xa0', '\x27', '\x06', '\x30', '\xc1',
    '\xe3', '\x81', '\xbf', '\xc1', '\x3e', '\x30', '\x07', '\xa0',
    '\x1b', '\x0e', '\x30', '\xc1', '\xe3', '\x81', '\xbf', '\xc1',
    '\x3e', '\x30', '\x0b', '\xa0', '\x1c', '\xc3', '\x1c', '\x60',
    '\x81', '\xff', '\xc3', '\x06', '\xc1', '\xa0', '\x23', '\x04',
    '\x20', '\xc1', '\xe6', '\x4f', '\xc1', '\x8e', '\x14', '\xc1',
    '\xe8', '\x4f', '\xc1', '\x6e', '\x20', '\x04', '\xa0', '\x2d',
    '\xc1', '\x1b', '\x10', '\xc1', '\xc1', '\x81', '\xbf', '\xc1',
    '\x1d', '\x20', '\x08', '\xa0', '\x1f', '\x08', '\x10', '\x08',
    '\x7f', '\x08', '\x10', '\x08', '\xa0', '\x28', '\x05', '\x10',
    '\x08', '\x2f', '\x07', '\x81', '\x90', '\x07', '\x2f', '\x08',
    '\x10', '\x06', '\xa0', '\x1b', '\x0b', '\x10', '\xc1', '\xb3',
    '\x1f', '\x07', '\x80', '\xa0', '\x03', '\x14', '\x02', '\x80',
    '\xa0', '\x09', '\x1f', '\xc1', '\x1a', '\x10', '\xa0', '\x1d',
    '\x09', '\x82', '\x50', '\x09', '\xa0', '\x23', '\x04', '\x81',
    '\x90', '\x08', '\xa0', '\x23', '\xc3', '\x8a', '\xe9', '\xa0',
    '\x0a', '\x09', '\x81', '\x70', '\x04', '\x2f', '\x20', '\x08',
    '\xa0', '\x21', '\x08', '\x80', '\xd0', '\x09', '\xa0', '\x28',
    '\x08', '\x20', '\xc1', '\xe2', '\x81', '\xdf', '\xc1', '\x2e',
    '\x20', '\x08', '\xa0', '\x1c', '\x20', '\xc1', '\xd1', '\x81',
    '\xdf', '\xc1', '\x1e', '\x20', '\x0c', '\xa0', '\x1c', '\xc2',
    '\x01', '\x03', '\x82', '\x1f', '\xc3', '\x03', '\xe1', '\xa0',
    '\x22', '\x05', '\x10', '\x06', '\x81', '\x3f', '\x06', '\x10',
    '\x05', '\xa0', '\x2c', '\x08', '\x20', '\xc1', '\xc1', '\x81',
    '\xcf', '\x04', '\x30', '\x08', '\xa0', '\x1f', '\x0b', '\x10',
    '\x0a', '\x7f', '\x0a', '\x10', '\x0b', '\xa0', '\x28', '\x08',
    '\x10', '\xc3', '\xb6', '\x6a', '\x81', '\xb0', '\xc3', '\xa6',
    '\x6b', '\x10', '\x08', '\xa0', '\x1b', '\x0c', '\x10', '\x09',
    '\x1f', '\xc1', '\x3e', '\x81', '\x90', '\xc1', '\xe6', '\x1f',
    '\x08', '\x10', '\xa0', '\x1d', '\x82', '\x70', '\x0c', '\xa0',
    '\x22', '\x04', '\x70', '\xc2', '\x41', '\x06', '\x38', '\xc2',
    '\x47', '\x02', '\x70', '\x08', '\xa0', '\x30', '\xc1', '\x6e',
    '\x81', '\x80', '\x04', '\x2f', '\x20', '\x08', '\xa0', '\x21',
    '\x0b', '\x80', '\xd0', '\x0b', '\xa0', '\x28', '\x08', '\x10',
    '\xc1', '\xc1', '\x7f', '\x0e', '\x2b', '\x78', '\x2b', '\x0e',
    '\x7f', '\xc1', '\x1c', '\x10', '\x08', '\xa0', '\x1c', '\xc3',
    '\x02', '\xc1', '\x80', '\xcf', '\x0c', '\x1b', '\x0c', '\x80',
    '\xef', '\xc1', '\x1c', '\x10', '\xa0', '\x1d', '\x10', '\x04',
    '\x82', '\x1f', '\x04', '\x10', '\xa0', '\x23', '\x08', '\x10',
    '\x0b', '\x81', '\x3f', '\x0b', '\x10', '\x08', '\xa0', '\x2a',
    '\xc1', '\x6e', '\x20', '\xc1', '\xc3', '\x81', '\xcf', '\x0d',
    '\x40', '\x08', '\xa0', '\x1f', '\x0b', '\x10', '\x0c', '\x7f',
    '\x0c', '\x10', '\x09', '\xa0', '\x28', '\x08', '\x82', '\x70',
    '\x08', '\xa0', '\x1c', '\x10', '\xc2', '\x82', '\x05', '\x81',
    '\xc0', '\xc5', '\x51', '\x18', '\x20', '\xa0', '\x1d', '\x82',
    '\x70', '\x0b', '\xa0', '\x22', '\x04', '\x40', '\xc2', '\x83',
    '\x0c', '\x80', '\x9f', '\xc2', '\x9d', '\x04', '\x40', '\x08',
    '\xa0', '\x2f', '\xc1', '\x19', '\x81', '\x90', '\x07', '\x2f',
    '\x20', '\x08', '\xa0', '\x21', '\x0b', '\x80', '\xd0', '\x08',
    '\xa0', '\x28', '\x08', '\x10', '\x04', '\x3f', '\xc3', '\x8d',
    '\x46', '\x81', '\x10', '\xc3', '\x64', '\xd8', '\x3f', '\x04',
    '\x10', '\x08', '\xa0', '\x1c', '\xc2', '\x04', '\x04', '\x4f',
    '\xc5', '\xbe', '\x68', '\x14', '\x80', '\xa0', '\xc5', '\x42',
    '\x85', '\xc9', '\x5f', '\x04', '\x10', '\xa0', '\x1c', '\x0c',
    '\x10', '\x09', '\x82', '\x1f', '\x09', '\x10', '\x0c', '\xa0',
    '\x22', '\x08', '\x10', '\x0b', '\x1f', '\x1b', '\x80', '\xb8',
    '\x1b', '\x1f', '\x0b', '\x10', '\x08', '\xa0', '\x29', '\xc1',
    '\x3e', '\x20', '\xc1', '\xe3', '\x81', '\xef', '\x09', '\x30',
    '\x0b', '\xa0', '\x1a', '\xc2', '\xbe', '\x0a', '\x18', '\x06',
    '\x10', '\x80', '\x9f', '\x10', '\x04', '\x28', '\xc1', '\xdb',
    '\xa0', '\x23', '\x08', '\x80', '\xb0', '\x02', '\x24', '\x06',
    '\x58', '\x07', '\x24', '\x02', '\x80', '\xb0', '\x08', '\xa0',
    '\x1c', '\x80', '\x90', '\xc2', '\x42', '\x07', '\x18', '\x0a',
    '\x7b', '\x28', '\xc2', '\x45', '\x01', '\x80', '\x80', '\x04',
    '\xa0', '\x1c', '\x09', '\x82', '\x70', '\x06', '\xa0', '\x22',
    '\x08', '\x20', '\xc1', '\xc6', '\x80', '\xff', '\xc1', '\x4b',
    '\x20', '\x08', '\xa0', '\x2d', '\xc1', '\x2c', '\x81', '\xb0',
    '\x08', '\x2f', '\x20', '\x0b', '\xa0', '\x1c', '\xc2', '\xbe',
    '\x0a', '\x18', '\x06', '\x80', '\xd0', '\x04', '\x28', '\xc1',
    '\xca', '\xa0', '\x23', '\x08', '\x10', '\xc4', '\xf4', '\x8e',
    '\x02', '\x81', '\x90', '\xc4', '\x82', '\xfe', '\x04', '\x10',
    '\x08', '\xa0', '\x1c', '\xc2', '\x06', '\x04', '\x1f', '\xc2',
    '\x8e', '\x03', '\x81', '\x70', '\xc2', '\x85', '\x0e', '\x1f',
    '\xc2', '\x04', '\x03', '\xa0', '\x1b', '\xc3', '\x1c', '\x90',
    '\x82', '\x3f', '\xc3', '\x09', '\xc1', '\xa0', '\x21', '\x09',
    '\x10', '\xc1', '\x24', '\x81', '\x10', '\xc1', '\x42', '\x10',
    '\x09', '\xa0', '\x28', '\xc1', '\x3e', '\x20', '\x06', '\x82',
    '\x1f', '\x09', '\x20', '\x0b', '\xa0', '\x17', '\xc2', '\x7c',
    '\x03', '\x60', '\x02', '\x80', '\x9f', '\x02', '\x60', '\xc2',
    '\x61', '\x0b', '\xa0', '\x20', '\x08', '\x40', '\xc4', '\x51',
    '\xa8', '\x0b', '\x81', '\x3f', '\xc4', '\xab', '\x48', '\x01',
    '\x40', '\x08', '\xa0', '\x1c', '\x04', '\x40', '\xc2', '\x82',
    '\x0b', '\x81', '\x4f', '\xc3', '\xad', '\x16', '\x40', '\x04',
    '\xa0', '\x1b', '\x06', '\x82', '\x90', '\x06', '\xa0', '\x21',
    '\x08', '\x10', '\x08', '\x6f', '\x0c', '\x3b', '\x0d', '\x6f',
    '\x05', '\x10', '\x0b', '\xa0', '\x2c', '\xc1', '\x1c', '\x81',
    '\xc0', '\x08', '\x2f', '\x20', '\x0b', '\xa0', '\x19', '\xc2',
    '\x7c', '\x03', '\x81', '\xa0', '\xc1', '\xa5', '\xa0', '\x20',
    '\x0b', '\x10', '\xc2', '\xa4', '\x01', '\x40', '\xc1', '\x64',
    '\x18', '\x2b', '\x0e', '\x3f', '\x0d', '\x2b', '\x18', '\xc1',
    '\x45', '\x40', '\xc2', '\xa1', '\x04', '\x10', '\x0b', '\xa0',
    '\x1c', '\xc4', '\x08', '\xd4', '\x06', '\x30', '\xc4', '\x41',
    '\x86', '\x09', '\x1b', '\x0e', '\x5f', '\x0e', '\x1b', '\xc3',
    '\x89', '\x46', '\x40', '\xc4', '\xd6', '\x04', '\x04', '\xa0',
    '\x1a', '\xc3', '\x19', '\x90', '\x82', '\x5f', '\xc3', '\x09',
    '\x91', '\xa0', '\x20', '\x0c', '\x50', '\xc2', '\x42', '\x06',
    '\x78', '\xc2', '\x46', '\x02', '\x50', '\x0c', '\xa0', '\x28',
    '\x08', '\x20', '\x08', '\x82', '\x3f', '\x04', '\x10', '\x0b',
    '\xa0', '\x15', '\xc1', '\x4c', '\x80', '\x80', '\xc1', '\x71',
    '\x80', '\x9f', '\x04', '\x80', '\x90', '\xc1', '\xc4', '\xa0',
    '\x1e', '\x0b', '\x20', '\xc1', '\xa3', '\x81', '\xdf', '\xc1',
    '\x3a', '\x20', '\x0b', '\xa0', '\x1c', '\x04', '\x20', '\xc1',
    '\xb1', '\x81', '\xaf', '\xc1', '\x8e', '\x30', '\x07', '\xa0',
    '\x1a', '\x06', '\x82', '\xb0', '\x06', '\xa0', '\x20', '\x0b',
    '\x10', '\x0b', '\x2f', '\xc2', '\x7c', '\x03', '\x70', '\xc2',
    '\x73', '\x0d', '\x2f', '\x08', '\x10', '\x0d', '\xa0', '\x2b',
    '\xc1', '\x2e', '\x81', '\xd0', '\x08', '\x1f', '\x0d', '\x20',
    '\x0d', '\xa0', '\x17', '\xc1', '\x4c', '\x81', '\xf0', '\xc1',
    '\xb2', '\xa0', '\x1e', '\x0b', '\x10', '\x01', '\x10', '\xc3',
    '\x61', '\xd9', '\x81', '\x5f', '\xc3', '\x9d', '\x16', '\x10',
    '\x01', '\x10', '\x0b', '\xa0', '\x1c', '\xc2', '\x08', '\x01',
    '\x10', '\xc3', '\x61', '\xda', '\x81', '\x5f', '\xc3', '\x9d',
    '\x16', '\x10', '\xc2', '\x01', '\x04', '\xa0', '\x19', '\x09',
    '\x10', '\x0b', '\x82', '\x7f', '\x0b', '\x10', '\x09', '\xa0',
    '\x12', '\xc3', '\x9d', '\x58', '\x34', '\x05', '\x18', '\xc2',
    '\xdb', '\x0f', '\x20', '\xc2', '\x81', '\x0e', '\x80', '\xdf',
    '\xc2', '\x8e', '\x01', '\x20', '\xc2', '\xdf', '\x0b', '\x18',
    '\x05', '\x34', '\xc3', '\x85', '\xd9', '\xa0', '\x1a', '\xc4',
    '\x1e', '\x10', '\x0c', '\x82', '\x4f', '\x03', '\x10', '\x0b',
    '\xa0', '\x14', '\x09', '\x30', '\xc4', '\x61', '\xb8', '\x0e',
    '\x81', '\x1f', '\xc3', '\xbd', '\x58', '\x40', '\x09', '\xa0',
    '\x1d', '\x0b', '\x20', '\x0e', '\x5f', '\xc1', '\xbd', '\x28',
    '\xc2', '\x46', '\x06', '\x38', '\xc2', '\x46', '\x06', '\x28',
    '\xc1', '\xdb', '\x5f', '\x0e', '\x20', '\x0b', '\xa0', '\x1c',
    '\x08', '\x20', '\x0c', '\x81', '\xdf', '\x07', '\x20', '\x08',
    '\xa0', '\x19', '\x07', '\x82', '\xd0', '\x07', '\xa0', '\x12',
    '\xc3', '\x9c', '\x58', '\x34', '\x06', '\x18', '\xc2', '\xdb',
    '\x0e', '\x10', '\xc3', '\xf5', '\x39', '\x80', '\xd0', '\xc8',
    '\xb4', '\x4e', '\x10', '\xcf', '\x0b', '\x18', '\x05', '\x34',
    '\xc3', '\x85', '\xd9', '\xa0', '\x1e', '\x05', '\x81', '\xe0',
    '\x08', '\x1f', '\x0b', '\x20', '\xa0', '\x17', '\x09', '\x82',
    '\x30', '\x09', '\xa0', '\x1d', '\x0b', '\x30', '\xc1', '\xe8',
    '\x81', '\xbf', '\xc1', '\x8e', '\x30', '\x0b', '\xa0', '\x1c',
    '\x0b', '\x20', '\xc1', '\xe8', '\x81', '\xbf', '\xc1', '\x7e',
    '\x20', '\x08', '\xa0', '\x19', '\x04', '\x10', '\x82', '\x9f',
    '\x10', '\x04', '\xa0', '\x10', '\xc1', '\x39', '\x80', '\xd0',
    '\x01', '\x10', '\x03', '\x81', '\x1f', '\x03', '\x10', '\x01',
    '\x80', '\xd0', '\xc1', '\x93', '\xa0', '\x18', '\x0a', '\x10',
    '\x09', '\x82', '\x4f', '\x0d', '\x20', '\x0e', '\xa0', '\x13',
    '\x0a', '\x20', '\xc1', '\x92', '\x81', '\xaf', '\xc2', '\x8e',
    '\x01', '\x20', '\x09', '\xa0', '\x1c', '\x0b', '\x20', '\x09',
    '\x1f', '\xc3', '\x8b', '\x14', '\x81', '\x30', '\xc3', '\x41',
    '\xb8', '\x1f', '\x09', '\x20', '\x0b', '\xa0', '\x1c', '\x08',
    '\x20', '\x4f', '\x0b', '\x18', '\x05', '\x14', '\x01', '\x70',
    '\x02', '\x14', '\xc3', '\x86', '\xca', '\x3f', '\x09', '\x20',
    '\x08', '\xa0', '\x19', '\x04', '\x82', '\xd0', '\x04', '\xa0',
    '\x10', '\xc1', '\x29', '\x80', '\xc0', '\x01', '\x81', '\x80',
    '\x01', '\x80', '\xd0', '\xc1', '\x93', '\xa0', '\x1b', '\x09',
    '\x81', '\xf0', '\x0b', '\x1f', '\x0b', '\x10', '\x02', '\xa0',
    '\x16', '\x0a', '\x82', '\x50', '\x09', '\xa0', '\x1c', '\x0b',
    '\x20', '\x07', '\x81', '\xff', '\x07', '\x20', '\x0b', '\xa0',
    '\x1c', '\x0c', '\x10', '\x07', '\x81', '\xff', '\x05', '\x10',
    '\x08', '\xa0', '\x19', '\x04', '\x10', '\x82', '\x9f', '\x10',
    '\x04', '\xa0', '\x0e', '\xc1', '\x4e', '\x30', '\x03', '\x64',
    '\x70', '\xc3', '\x82', '\xcb', '\x80', '\x9f', '\xc3', '\xbc',
    '\x38', '\x70', '\x64', '\x03', '\x30', '\xc1', '\xe3', '\xa0',
    '\x16', '\x08', '\x10', '\x82', '\x5f', '\x09', '\x20', '\xa0',
    '\x13', '\xc1', '\x1e', '\x10', '\xc1', '\xe6', '\x81', '\xdf',
    '\xc1', '\x3d', '\x10', '\xc1', '\xe1', '\xa0', '\x1b', '\x0b',
    '\x30', '\x04', '\x81', '\xd0', '\x04', '\x30', '\x0b', '\xa0',
    '\x1c', '\x0a', '\x20', '\xc3', '\xa8', '\x37', '\x81', '\x60',
    '\xc4', '\x41', '\xb8', '\x05', '\x20', '\x0b', '\xa0', '\x19',
    '\x04', '\x82', '\xd0', '\x04', '\xa0', '\x0e', '\xc1', '\x3d',
    '\x83', '\x90', '\xc1', '\xe4', '\xa0', '\x18', '\xc1', '\x1e',
    '\x81', '\xf0', '\x0b', '\x1f', '\x09', '\x10', '\x04', '\xa0',
    '\x15', '\xc1', '\x1e', '\x82', '\x50', '\xc1', '\xd1', '\xa0',
    '\x1b', '\x0e', '\x20', '\x07', '\x81', '\xff', '\x07', '\x20',
    '\x0e', '\xa0', '\x1d', '\x10', '\x06', '\x81', '\xff', '\x04',
    '\x10', '\x09', '\xa0', '\x19', '\x04', '\x10', '\x82', '\x9f',
    '\x10', '\x04', '\xa0', '\x0e', '\x03', '\x10', '\xc1', '\xb6',
    '\x80', '\xaf', '\xc5', '\xbc', '\x78', '\x14', '\x81', '\x10',
    '\xc5', '\x41', '\x87', '\xcb', '\x80', '\xaf', '\xc1', '\x6b',
    '\x10', '\x03', '\xa0', '\x16', '\xc2', '\x08', '\x04', '\x82',
    '\x5f', '\x06', '\x10', '\x01', '\xa0', '\x13', '\x08', '\x10',
    '\x02', '\x82', '\x0f', '\x0b', '\x20', '\x08', '\xa0', '\x1b',
    '\x0e', '\x82', '\x70', '\x0e', '\xa0', '\x1c', '\x0b', '\x82',
    '\x50', '\x0b', '\xa0', '\x19', '\x04', '\x82', '\xd0', '\x04',
    '\xa0', '\x0d', '\xc1', '\x3e', '\x83', '\xb0', '\x04', '\xa0',
    '\x18', '\x08', '\x82', '\x00', '\x0b', '\x1f', '\x08', '\x10',
    '\x08', '\xa0', '\x15', '\x08', '\x82', '\x70', '\x07', '\xa0',
    '\x1c', '\x06', '\x20', '\xc1', '\xe8', '\x81', '\xbf', '\xc1',
    '\x8e', '\x20', '\x06', '\xa0', '\x1e', '\x03', '\x10', '\xc1',
    '\xd6', '\x81', '\xbf', '\xc1', '\x5d', '\x10', '\xc1', '\xd2',
    '\xa0', '\x19', '\x04', '\x10', '\x82', '\x9f', '\x10', '\x04',
    '\xa0', '\x0d', '\xc3', '\x08', '\xc2', '\x81', '\x2f', '\xc3',
    '\xad', '\x28', '\x80', '\x90', '\xc3', '\x82', '\xda', '\x81',
    '\x2f', '\xc3', '\x2c', '\x80', '\xa0', '\x15', '\x09', '\x10',
    '\x80', '\xcf', '\x0d', '\x80', '\xab', '\x0c', '\x80', '\xbf',
    '\x01', '\x10', '\x04', '\xa0', '\x13', '\x04', '\x10', '\x06',
    '\x82', '\x1b', '\x02', '\x10', '\x02', '\xa0', '\x1c', '\x04',
    '\x82', '\x50', '\x04', '\xa0', '\x1d', '\xc1', '\x1e', '\x82',
    '\x30', '\xc1', '\xe2', '\xa0', '\x19', '\x04', '\x82', '\xd0',
    '\x04', '\xa0', '\x0d', '\x08', '\x83', '\xd0', '\x09', '\xa0',
    '\x17', '\x01', '\x82', '\x00', '\x0b', '\x1f', '\x05', '\x10',
    '\x0a', '\xa0', '\x15', '\x04', '\x82', '\x70', '\x02', '\xa0',
    '\x1d', '\x06', '\x30', '\xc2', '\x85', '\x0b', '\x81', '\x5f',
    '\xc2', '\x8b', '\x05', '\x30', '\x06', '\xa0', '\x20', '\x08',
    '\x20', '\xc2', '\x85', '\x0b', '\x81', '\x4f', '\xc3', '\xbe',
    '\x38', '\x10', '\xc2', '\x81', '\x0e', '\xa0', '\x1a', '\x04',
    '\x10', '\x82', '\x9f', '\x10', '\x04', '\xa0', '\x0d', '\xc2',
    '\x02', '\x0a', '\x81', '\x4f', '\xc5', '\x8d', '\x01', '\x51',
    '\x38', '\xc5', '\x25', '\x10', '\xd8', '\x81', '\x4f', '\xc2',
    '\x0a', '\x02', '\xa0', '\x15', '\x0d', '\x10', '\xc3', '\xe8',
    '\xab', '\x18', '\x05', '\x14', '\x01', '\x81', '\x20', '\x01',
    '\x14', '\x06', '\x18', '\xc2', '\xdb', '\x0c', '\x20', '\x07',
    '\xa0', '\x13', '\x82', '\x90', '\xa0', '\x1c', '\xc1', '\x3e',
    '\x82', '\x30', '\xc1', '\xe3', '\xa0', '\x1e', '\xc1', '\x4e',
    '\x82', '\x10', '\xc1', '\xe8', '\xa0', '\x1a', '\x04', '\x82',
    '\xd0', '\x04', '\xa0', '\x0d', '\x01', '\x81', '\xa0', '\xc1',
    '\x41', '\x38', '\xc1', '\x14', '\x81', '\xa0', '\x03', '\xa0',
    '\x16', '\x0c', '\x82', '\x10', '\x06', '\x1b', '\x01', '\x10',
    '\x0d', '\xa0', '\x15', '\x82', '\x90', '\xa0', '\x1e', '\xc1',
    '\x2a', '\x50', '\xc1', '\x43', '\x18', '\x0a', '\x2b', '\x0e',
    '\x1f', '\x0e', '\x2b', '\x0a', '\x18', '\xc1', '\x34', '\x50',
    '\xc1', '\xa2', '\xa0', '\x22', '\xc1', '\x6d', '\x40', '\xc2',
    '\x42', '\x06', '\x18', '\x80', '\x9b', '\x18', '\xc2', '\x46',
    '\x01', '\x30', '\xc2', '\x81', '\x0d', '\xa0', '\x1c', '\x04',
    '\x82', '\xd0', '\x04', '\xa0', '\x0c', '\x0e', '\x10', '\x80',
    '\xdf', '\x0e', '\x2b', '\x18', '\xc1', '\x24', '\x10', '\xc1',
    '\xb3', '\xa0', '\x07', '\xc1', '\x3b', '\x10', '\xc1', '\x42',
    '\x18', '\x2b', '\x0d', '\x80', '\xdf', '\x10', '\x0e', '\xa0',
    '\x15', '\x04', '\x82', '\x90', '\x0a', '\xa0', '\x13', '\x82',
    '\x90', '\xa0', '\x1e', '\xc1', '\x18', '\x81', '\xf0', '\xc1',
    '\x81', '\xa0', '\x22', '\xc1', '\x29', '\x81', '\xc0', '\xc2',
    '\x71', '\x0d', '\xa0', '\x1c', '\x04', '\x82', '\xd0', '\x04',
    '\xa0', '\x0c', '\x0c', '\x81', '\x90', '\xc1', '\xa3', '\xa0',
    '\x07', '\xc1', '\x29', '\x81', '\x90', '\xa0', '\x16', '\x0b',
    '\x82', '\x60', '\x01', '\xa0', '\x16', '\x82', '\x90', '\xa0',
    '\x20', '\xc1', '\x59', '\x81', '\xb0', '\xc1', '\x95', '\xa0',
    '\x26', '\xc2', '\x8e', '\x04', '\x81', '\x60', '\xc2', '\x62',
    '\x0a', '\xa0', '\x1f', '\x04', '\x82', '\xd0', '\x04', '\xa0',
    '\x0c', '\x0b', '\x10', '\x06', '\x64', '\x80', '\xe0', '\x06',
    '\xa0', '\x0b', '\x06', '\x80', '\xe0', '\x64', '\x06', '\x10',
    '\x0b', '\xa0', '\x15', '\x0c', '\x82', '\x80', '\xc1', '\xe1',
    '\xa0', '\x13', '\x82', '\x9b', '\xa0', '\x1f', '\xc2', '\x8e',
    '\x02', '\x81', '\xb0', '\xc2', '\x82', '\x0e', '\xa0', '\x25',
    '\xc2', '\x6b', '\x02', '\x81', '\x60', '\xc2', '\x51', '\x09',
    '\xa0', '\x1f', '\x04', '\x82', '\xd0', '\x04', '\xa0', '\x0c',
    '\x0b', '\x81', '\x80', '\x06', '\xa0', '\x0a', '\xc1', '\x3e',
    '\x81', '\x80', '\xa0', '\x17', '\x03', '\x82', '\x50', '\x05',
    '\xa0', '\x16', '\x82', '\x9b', '\xa0', '\x22', '\xc5', '\xbe',
    '\x58', '\x14', '\x80', '\xf0', '\xc5', '\x41', '\x85', '\xeb',
    '\xa0', '\x2c', '\xc2', '\x8b', '\x07', '\x14', '\x80', '\xa0',
    '\xc5', '\x41', '\x85', '\xda', '\xa0', '\x22', '\x09', '\x82',
    '\xd8', '\x09', '\xa0', '\x0c', '\x0b', '\x81', '\x70', '\x02',
    '\xa0', '\x0d', '\x02', '\x81', '\x70', '\x0b', '\xa0', '\x16',
    '\x0a', '\x82', '\x78', '\x0a', '\xa0', '\x61', '\xc3', '\x8c',
    '\x46', '\x81', '\x30', '\xc3', '\x64', '\xc8', '\xa0', '\x2b',
    '\xc4', '\x9d', '\x48', '\x03', '\x80', '\xc0', '\xc4', '\x42',
    '\x87', '\x0c', '\xa0', '\x22', '\x09', '\x82', '\xd8', '\x09',
    '\xa0', '\x0c', '\x0b', '\x81', '\x70', '\x03', '\xa0', '\x0c',
    '\xc1', '\x1e', '\x81', '\x70', '\xa0', '\x18', '\x0c', '\x82',
    '\x4b', '\x0d', '\xa0', '\x6b', '\x0d', '\x80', '\x9b', '\x0d',
    '\xa0', '\x3d', '\x0d', '\x1b', '\x0d', '\xa0', '\x69', '\x30',
    '\xc2', '\x74', '\x08', '\x1b', '\x0d', '\xa0', '\x0b', '\x1b',
    '\x0d', '\xa0', '\x0d', '\x0d', '\x1b', '\xa0', '\x0b', '\x0d',
    '\x1b', '\xc2', '\x78', '\x04', '\x30', '\xa0', '\xa9', '\x0c',
    '\x4b', '\x38', '\x4b', '\x0c', '\xa0', '\x38', '\x0d', '\x6b',
    '\x0c', '\xa0', '\x66', '\x0d', '\x30', '\xc2', '\x74', '\x08',
    '\x1b', '\x0c', '\xa0', '\x0a', '\x2b', '\x0d', '\xa0', '\x0d',
    '\x0d', '\x1b', '\x0c', '\xa0', '\x0a', '\x2b', '\xc2', '\x78',
    '\x04', '\x20', '\x01', '\xa1', '\x63', '\xc2', '\x64', '\x0b',
    '\xa0', '\x39', '\xc2', '\x6a', '\x04', '\xa1', '\x63', '\xc2',
    '\x73', '\x0c', '\xa0', '\x39', '\x0a', '\x16', '\xaa', '\x66'
};

static CompressedXpm pitch_070_compressed_xpm =
{
    840,  // width
    70,  // height
    0xff00ff,  // background_colour
    0x000001,  // kludge_colour
    -1,  // kludge_idx
    16,  // nbr_other_colours
    {0x000000,0x393939,0x535353,0x686868,0x7a7a7a,0x8b8b8b,0x999999,0xa7a7a7,0xb4b4b4,0xc0c0c0,0xcccccc,0xd7d7d7,0xe1e1e1,0xececec,0xf5f5f5,0xffffff},
    9104,  // binary_len
    pitch_070_compressed_xpm_data //bin
};

static void StaticTest()
{
    CompressedXpm2Xpm( pitch_070_compressed_xpm, "pitch_070_compressed.xpm" );
}


