#define ASSET_SIZE(asset)  (uintptr_t)      _binary_assets_ ## asset ## _size
#define ASSET_START(asset) (unsigned char*) _binary_assets_ ## asset ## _start
#define ASSET_END(asset)   (unsigned char*) _binary_assets_ ## asset ## _end

extern const char _binary_assets_font_monospace_size[];
extern const char _binary_assets_font_monospace_end[];
extern const char _binary_assets_font_monospace_start[];
