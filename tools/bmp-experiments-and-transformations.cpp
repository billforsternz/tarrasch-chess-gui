//
// .bmp experiments and transformations
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(1)
struct header
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
    uint8_t   rgb;
};

static void DrawTextPicture( header *h );

//  #define FIN "../test2.bmp"
#define FIN "../font-22-50.bmp"
#define BUF_SIZE 20000000

int main()
{
    FILE *f = fopen( FIN, "rb" );
    if( !f )
    {
        printf( "Cannot open %s\n", FIN );
        return 0;
    }
    char *buf = (char *)malloc(BUF_SIZE);
    if( !buf )
    {
        printf( "Cannot allocate memory\n" );
        return 0;
    }
    uint32_t len = fread( buf, 1, BUF_SIZE, f );
    if( len < 54 )
    {
        printf( "Ill formed header\n" );
        return 0;
    }
    uint8_t *b = (uint8_t *)buf;
    for( int i=0; i<54; i++ )
    {
        printf( "%02x", *b++ );
        if( (i+1) == 54 )
            printf("\n");
        else if( (i+1) % 16 == 0 )
            printf("\n");
        else
            printf(" ");
    }   
    header *h = (header *)buf;
    bool ok=true;
    if( h->bm != 0x4d42 )
    {
        ok = false;
        printf( "Signature? (%04x)\n", h->bm );
    }
    if( h->file_len != len )       
    {
        ok = false;
        printf( "File len? (%x)\n", h->file_len );
    }
    if( h->hdr_len != 54 )
    {
        ok = false;
        printf( "Header len? (%x)\n", h->hdr_len );
    }
    if( h->hdr_len+ h->data_len != h->file_len )
    {
        ok = false;
        printf( "Data len? (%x)\n", h->data_len );
    }
    if( h->sub_hdr_len != 40 )
    {
        ok = false;
        printf( "Sub header len? (%x)\n", h->sub_hdr_len );
    }
    if( h->nbr_planes != 1 )
    {
        ok = false;
        printf( "Nbr planes? (%d)\n", h->nbr_planes );
    }
    if( h->bits_per_pixel != 24 )
    {
        ok = false;
        printf( "Bits per pixel? (%d)\n", h->bits_per_pixel );
    }
    uint32_t row_bytes = 3*h->width;
    if( row_bytes & 1 )
        row_bytes++;
    if( h->height * row_bytes != h->data_len )
    {
        ok = false;
        printf( "Height(%d) and width(%d) don't match data_len(%d)?\n", h->height, h->width, h->data_len );
    }
    if( ok )
        DrawTextPicture(h);
    fclose(f);
    free(buf);
    printf( "Press enter\n" );
    getchar();
}

// Just to establish confidence, draw a text picture, assumes a magenta background
static void DrawTextPicture( header *h )
{
    printf( "height=%d, width=%d\n", h->height, h->width );
    uint32_t row_bytes = 3*h->width;
    if( row_bytes & 1 )
        row_bytes++;
    FILE *f=fopen("picture.txt","wt");
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
    for( uint32_t y=0; y<h->height; y++ )
    {
        uint8_t *ptr = &h->rgb + h->data_len - (y+1)*row_bytes;
        char *s = buf;
        for( uint32_t x=0; x<h->width; x++ )
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

