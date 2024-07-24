from fontTools.ttLib import TTFont
import cairosvg

def get_unicode_from_ttf(font_path, char):
    out = ""
    for i in [char for char in char]:
        print(i, hex(ord(i)))
        out += hex(ord(i)) + ","
    return out

# 使用方法
a = get_unicode_from_ttf('./MiSansLatin-Light.ttf', "0123456789:")
print(a)