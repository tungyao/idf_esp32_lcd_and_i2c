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

#ifndef LV_ATTRIBUTE_IMG_G110
#define LV_ATTRIBUTE_IMG_G110
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_G110 uint8_t g110_map[] = {
  0x00, 0x00, 0x00, 0x01, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0x3b, 	/*Color of index 1*/
  0x00, 0x00, 0x00, 0x5f, 	/*Color of index 2*/
  0x00, 0x00, 0x00, 0x7e, 	/*Color of index 3*/
  0x00, 0x00, 0x00, 0x9c, 	/*Color of index 4*/
  0x00, 0x00, 0x00, 0xc3, 	/*Color of index 5*/
  0x3c, 0xc7, 0xfe, 0xff, 	/*Color of index 6*/
  0x35, 0xb0, 0xe1, 0xff, 	/*Color of index 7*/
  0x31, 0x9f, 0xcb, 0xff, 	/*Color of index 8*/
  0x2b, 0x8e, 0xb5, 0xff, 	/*Color of index 9*/
  0x24, 0x78, 0x99, 0xff, 	/*Color of index 10*/
  0x1b, 0x58, 0x70, 0xff, 	/*Color of index 11*/
  0x12, 0x3d, 0x4e, 0xff, 	/*Color of index 12*/
  0x0d, 0x2b, 0x37, 0xff, 	/*Color of index 13*/
  0x05, 0x11, 0x16, 0xff, 	/*Color of index 14*/
  0x00, 0x01, 0x01, 0xfa, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x02, 0x45, 0xff, 0x54, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xff, 0xdb, 0xbb, 0xbd, 0xff, 0x20, 0x00, 0x00, 
  0x00, 0x01, 0x4f, 0xb7, 0x66, 0x66, 0x66, 0x8b, 0xf3, 0x10, 0x00, 
  0x00, 0x05, 0xd9, 0x66, 0x66, 0x66, 0x66, 0x66, 0xad, 0x50, 0x00, 
  0x00, 0x4e, 0x76, 0x66, 0x66, 0x66, 0x66, 0x66, 0x68, 0xe3, 0x00, 
  0x03, 0xe9, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xae, 0x20, 
  0x05, 0xb6, 0x66, 0x66, 0x66, 0x66, 0x66, 0x76, 0x66, 0x6c, 0x40, 
  0x2e, 0x86, 0x66, 0xcf, 0x76, 0x66, 0x68, 0xfb, 0x66, 0x69, 0xf1, 
  0x4d, 0x66, 0x66, 0xdf, 0x86, 0x66, 0x69, 0xfc, 0x66, 0x66, 0xe3, 
  0x5b, 0x66, 0x66, 0xdf, 0x76, 0x66, 0x69, 0xfb, 0x66, 0x66, 0xc4, 
  0xfa, 0x66, 0x66, 0x77, 0x66, 0x66, 0x66, 0x86, 0x66, 0x66, 0xb5, 
  0x5a, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xb5, 
  0x5b, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xc4, 
  0x4d, 0x66, 0x66, 0x77, 0x66, 0x66, 0x66, 0x77, 0x66, 0x66, 0xe3, 
  0x2e, 0x96, 0x66, 0xce, 0xb7, 0x76, 0x8b, 0xfb, 0x66, 0x6a, 0xf1, 
  0x05, 0xb6, 0x66, 0x6b, 0xef, 0xff, 0xfe, 0xb6, 0x66, 0x6c, 0x40, 
  0x02, 0xea, 0x66, 0x66, 0x78, 0xaa, 0x86, 0x66, 0x66, 0xaf, 0x10, 
  0x00, 0x4e, 0x76, 0x66, 0x66, 0x66, 0x66, 0x66, 0x68, 0xf3, 0x00, 
  0x00, 0x05, 0xda, 0x66, 0x66, 0x66, 0x66, 0x66, 0xae, 0x40, 0x00, 
  0x00, 0x01, 0x3f, 0xc8, 0x66, 0x66, 0x66, 0x9c, 0xf3, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x5f, 0xec, 0xbb, 0xce, 0xf5, 0x10, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x34, 0x55, 0x43, 0x10, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t g110 = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 22,
  .header.h = 22,
  .data_size = 306,
  .data = g110_map,
};