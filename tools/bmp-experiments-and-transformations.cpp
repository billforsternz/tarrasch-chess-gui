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
    uint16_t  res1;
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

class CompressedXpm
{
public:
    uint32_t width;
    uint32_t height;
    uint32_t background_colour;
    int      nbr_other_colours;
    uint32_t other_colours[16];
    uint32_t binary_len;
    uint8_t *bin;
};


static BmpHeader *ReadFile( char *buf, const char *filename );
static void Convert2Xpm( BmpHeader *in, const char *filename );
static void DrawTextPicture( BmpHeader *in );
static void SliceOutPieces( BmpHeader *in );
static void SpliceFiles( char *buf );

static bool Bmp2CompressedXpm( BmpHeader *in, const char *filename, CompressedXpm &compressed );
static void CompressedXpm2Xpm( CompressedXpm &compressed, const char *filename );

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
#define ONE_FILE
#ifdef ONE_FILE
    const char *filename = "pitch-070-batch2.bmp";
    BmpHeader *in = ReadFile( buf, filename );
    if( in )
    {
        CompressedXpm compressed;
        Bmp2CompressedXpm(in,filename,compressed);
        CompressedXpm2Xpm(compressed,filename);
        free(compressed.bin);
    }
#endif
//#define BULK_FILES
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
        "../font-200b.bmp" */

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
        "pitch-306-batch1.bmp"
    };
    for( int i=0; i<sizeof(files)/sizeof(files[0]); i++ )
    {
        BmpHeader *in = ReadFile( buf, files[i] );
        if( in )
        {
            //DrawTextPicture(in);
            //SliceOutPieces(in);
            Convert2Xpm( in, files[i] );
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
    #define DBG_NBR 12
    #define DBG_NBR_OUT 14
    bool ok=true;   // returns bool ok
    uint8_t *buf = (uint8_t *)malloc(BUF_SIZE/10);
    if( !buf )
    {
        printf( "Cannot allocate memory\n" );
        return false;
    }
    const uint32_t magenta = 0xff00ff;
    compressed.width  = in->width;    
    compressed.height = in->height;
    compressed.background_colour = magenta;    
    std::set<uint32_t> colour_set;
    colour_set.insert(magenta);
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
    bool too_many = ( colour_set.size() > 17 );
    printf( "File %s, %d colours found%s\n", filename, colour_set.size(), too_many?" too many!":"" );
    ok = !too_many;
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
        compressed.nbr_other_colours = colour_set.size()-1;
        int idx=0;
        for( std::set<uint32_t>::iterator it=colour_set.begin(); it!=colour_set.end(); it++ )
        {
            uint32_t colour = *it;
            if( colour != magenta )
                compressed.other_colours[ idx++ ] = colour;
        }
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
                                if( chunk <= 32 )
                                {
                                    uint8_t b = chunk-1;
                                    b |= 0xc0;
                                    *dst++ = b;
                                    if( dbg++ < DBG_NBR )
                                        printf( "Buf <=32 (%d) [%02x]\n", chunk, dat[0] );
                                }
                                else
                                {
                                    uint16_t w = chunk-1;
                                    w |= 0xe000;
                                    *dst++ = (w>>8)&0xff;
                                    *dst++ = w&0xff;
                                    if( dbg++ < DBG_NBR )
                                        printf( "Buf >32 (%d) [%02x]\n", chunk, dat[0], dat[1] );
                                }
                                *dst = 0;
                                dat=dst;
                                for( unsigned int i=0; i<chunk; i++ )
                                {
                                    *dst >>= 4;
                                    *dst |= (buf[src++] << 4);
                                    if( (i&1) )
                                    {
                                        dst++;
                                        *dst = 0;
                                    }
                                }
                                if( dbg++ < DBG_NBR )
                                    printf( "[%02x %02x %02x %02x %02x...]\n", dat[0], dat[1], dat[2], dat[3], dat[4] );
                            }

                            // Account for final pixel at end
                            if( end && rgb==magenta )
                            {
                                // add a single magenta pixel
                                *dst++ = 0xa0;
                                *dst = 0;
                            }

                            // Next state
                            if( rgb == magenta )
                            {
                                state = in_run_magenta;
                                n = 1;
                            }
                            else
                            {
                                state = in_run;
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
                                if( dbg++ < DBG_NBR )
                                    printf( "Magenta (%d) [%02x %02x]\n", chunk, dat[0], dat[1] );
                            }

                            // Account for final pixel at end
                            if( end && rgb!=magenta )
                            {

                                // Need a single non magenta pixel
                                *dst++ = cccc;
                            }

                            // Next state
                            state = search;
                            idx = 0;
                            buf[idx++] = cccc;
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
                                    b |= cccc;
                                    *dst++ = b;
                                    if( dbg++ < DBG_NBR )
                                        printf( "Seq <=8 (%d,%d) [%02x]\n", cccc, chunk, dat[0] );
                                }
                                else //if( chunk <= 512 )
                                {
                                    uint16_t w = chunk-1;
                                    w = w<<4;
                                    w |= cccc;
                                    w |= 0x8000;
                                    *dst++ = (w>>8)&0xff;
                                    *dst++ = w&0xff;
                                    if( dbg++ < DBG_NBR )
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
                                    *dst = 0;
                                }

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


static void CompressedXpm2Xpm( CompressedXpm &compressed, const char *filename )
{
    int dbg=0;

    std::string s(filename);
    std::string xpm_filename = s.substr(0,s.length()-4) + "reconstituted.xpm";
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

    fprintf( f, "static const char *pitch_%d_xpm[] = {\n", compressed.height  );
    fprintf( f, "\"%d %d %d 1\",\n", compressed.width, compressed.height, compressed.nbr_other_colours+1 );
    fprintf( f, "\"%c c #%06x\",\n", ' ', 0xff00ff );   // magenta background first
    char proxy='!';
    char proxy_colours[16];
    for( int i=0; i<compressed.nbr_other_colours; i++ )
    {
        fprintf( f, "\"%c c #%06x\",\n", proxy, compressed.other_colours[i] ); 
        proxy_colours[i] = proxy;
        proxy++;
        while( proxy == '\'' || proxy=='\"' )
            proxy++; 
    }

    int len = compressed.binary_len;
    uint8_t *src = compressed.bin;

    /*
        Byte by byte coding system
        --------------------------
        up to 2^3  =8    successive codes cccc                0nnncccc
        up to 2^9  =512  successive codes cccc                100nnnnn nnnncccc
        up to 2^13 =8192 successive background codes          101nnnnn nnnnnnnn
        up to 2^5  =32   arbitrary codes                      110nnnnn followed by codes
        up to 2^13 =8192 arbitrary codes                      111nnnnn nnnnnnnn followed by codes
    */

    fprintf( f, "\"" );
    uint32_t offset=0;  // from 0 to width*height
    while( len > 0 )
    {
        uint8_t c = *src++;
        len--;
        bool arbitrary_data=false;
        char proxy=' ';
        uint32_t n=0;
        if( (c&0x80) == 0 )
        {
            // Sequence of one colour (short)
            uint32_t cccc = c&0x0f;
            proxy = proxy_colours[cccc];
            n = ((c>>4)&0x07) + 1;
            if( dbg++ < DBG_NBR_OUT )
                printf( "Seq short\n" );
        }
        else if( (c&0xe0) == 0xc0 )
        {
            // Arbitrary data (short)
            n = (c&0x1f) + 1;
            arbitrary_data = true;
            if( dbg++ < DBG_NBR_OUT )
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
                if( dbg++ < DBG_NBR_OUT )
                    printf( "Seq long\n" );
            }
            else if( (c&0xe0) == 0xa0 )
            {
                // Sequence of background colour
                proxy = ' ';
                n =  d;
                n += ((c&0x1f)<<8);
                n++;
                if( dbg++ < DBG_NBR_OUT )
                    printf( "Magenta\n" );
            }
            else if( (c&0xe0) == 0xe0 )
            {
                // Arbitrary data (long)
                n =  d;
                n += ((c&0x1f)<<8);
                n++;
                arbitrary_data = true;
                if( dbg++ < DBG_NBR_OUT )
                    printf( "Data long\n" );
            }
        }

        // Generate decoded data
        if( !arbitrary_data )
            memset( buf, proxy, n );
        else
        {
            for( unsigned int i=0; i<n; i++ )
            {
                uint8_t dat = *src;
                if( (i&1) == 0 )
                    buf[i] = proxy_colours[ (dat>>4)&0x0f ];
                else
                {
                    buf[i] = proxy_colours[dat&0x0f];
                    src++;
                    len--;
                }
            }
        }
        if( dbg < DBG_NBR_OUT )
        {
            printf( "dat (%d) >", n );
            for( unsigned int i=0; i<n && i<10; i++ )
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
            printf( "\n" );
        }

        for( unsigned int i=0; i<n; i++ )
        {
            fprintf( f, "%c", buf[i] );
            offset++;
            if( offset == compressed.height*compressed.width )
                fprintf( f, "\"\n};\n" );
            else if( (offset%compressed.width) == 0 )
                fprintf( f, "\",\n\"" );
        }
    }
    free(buf);
    fclose(f);
}


