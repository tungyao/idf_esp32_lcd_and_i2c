#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef IDF_VER
            #define IDF_VER
        #endif
    #endif
#endif

#if defined(IDF_VER)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_G600
#define LV_ATTRIBUTE_IMG_G600
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_G600 uint8_t g600_map[] = {
  0x00, 0x00, 0x00, 0x01, 	/*Color of index 0*/
  0xe7, 0xe7, 0xe7, 0xff, 	/*Color of index 1*/
  0x00, 0x00, 0x00, 0x5a, 	/*Color of index 2*/
  0x80, 0x80, 0x80, 0xff, 	/*Color of index 3*/
  0x55, 0x55, 0x55, 0xff, 	/*Color of index 4*/
  0x00, 0x00, 0x00, 0xae, 	/*Color of index 5*/
  0xfc, 0xfd, 0xfe, 0xff, 	/*Color of index 6*/
  0x83, 0xd9, 0xfb, 0xff, 	/*Color of index 7*/
  0x3c, 0xc7, 0xfe, 0xff, 	/*Color of index 8*/
  0x2e, 0xac, 0xe1, 0xff, 	/*Color of index 9*/
  0x16, 0x94, 0xcf, 0xff, 	/*Color of index 10*/
  0x25, 0x7a, 0x9b, 0xff, 	/*Color of index 11*/
  0x19, 0x51, 0x68, 0xff, 	/*Color of index 12*/
  0x14, 0x32, 0x3e, 0xff, 	/*Color of index 13*/
  0x07, 0x16, 0x1c, 0xff, 	/*Color of index 14*/
  0x00, 0x01, 0x01, 0xf6, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x02, 0x55, 0xff, 0x55, 0x20, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xff, 0xdc, 0xcb, 0xdd, 0xff, 0x20, 0x00, 0x00, 
  0x00, 0x00, 0x5e, 0xc9, 0x88, 0x88, 0x88, 0x9c, 0xf5, 0x00, 0x00, 
  0x00, 0x0f, 0xdb, 0x88, 0x9a, 0x89, 0xa9, 0x88, 0xbd, 0x50, 0x00, 
  0x00, 0x5e, 0x98, 0x9a, 0xa9, 0x88, 0x9a, 0xa9, 0x99, 0xf5, 0x00, 
  0x02, 0xea, 0x8a, 0xa9, 0x88, 0x88, 0x88, 0xaa, 0xa8, 0xbf, 0x20, 
  0x0f, 0xc8, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x8c, 0x50, 
  0x2e, 0xa8, 0x9d, 0xb8, 0x88, 0x88, 0x88, 0x8b, 0xe8, 0x8b, 0xf2, 
  0x5d, 0x88, 0x89, 0xde, 0xb8, 0x88, 0x8b, 0xed, 0x88, 0x88, 0xe2, 
  0x5c, 0x88, 0x88, 0x8c, 0xfe, 0x99, 0xef, 0xb8, 0x88, 0x88, 0xd5, 
  0xfb, 0x88, 0x88, 0x8c, 0xfc, 0x89, 0xdf, 0xc8, 0x88, 0x88, 0xc5, 
  0xfb, 0x88, 0x8a, 0xed, 0x98, 0x87, 0x8a, 0xdd, 0x98, 0x88, 0xc5, 
  0x5c, 0x88, 0x9d, 0x98, 0x87, 0x66, 0x78, 0x8a, 0xd8, 0x88, 0xd5, 
  0x5d, 0x88, 0x88, 0x88, 0x76, 0x66, 0x67, 0x88, 0x88, 0x89, 0xd2, 
  0x2f, 0xa8, 0x88, 0x87, 0x66, 0x66, 0x66, 0x78, 0x88, 0x8b, 0xf2, 
  0x05, 0xc8, 0x88, 0x76, 0x66, 0x66, 0x66, 0x67, 0x88, 0x8d, 0x50, 
  0x02, 0xfa, 0x87, 0x66, 0x16, 0x66, 0x61, 0x66, 0x78, 0xbf, 0x20, 
  0x00, 0x5d, 0x96, 0x66, 0x16, 0x66, 0x61, 0x66, 0x69, 0xe5, 0x00, 
  0x00, 0xf4, 0x66, 0x61, 0x16, 0x66, 0x61, 0x16, 0x66, 0xd5, 0x00, 
  0x00, 0x53, 0x66, 0x43, 0x66, 0x33, 0x61, 0x43, 0x66, 0x45, 0x00, 
  0x00, 0x5f, 0x3d, 0xff, 0x4d, 0xff, 0x4d, 0xff, 0x44, 0xf2, 0x00, 
  0x00, 0x02, 0x55, 0x22, 0x55, 0x22, 0x55, 0x22, 0x55, 0x20, 0x00, 
};

const lv_img_dsc_t g600 = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 22,
  .header.h = 22,
  .data_size = 306,
  .data = g600_map,
};
