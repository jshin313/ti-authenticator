#ifndef cursor_include_file
#define cursor_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define cursor_width 17
#define cursor_height 22
#define cursor_size 376
#define cursor ((gfx_sprite_t*)cursor_data)
extern unsigned char cursor_data[376];

#ifdef __cplusplus
}
#endif

#endif
