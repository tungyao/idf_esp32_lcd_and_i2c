from fontTools.ttLib import TTFont


def get_unicode_from_ttf(font_path, char):
    out = ""
    for i in [char for char in char]:
        print(i, hex(ord(i)))
        out += hex(ord(i)) + ","
    return out


# 使用方法
a = get_unicode_from_ttf('./OPPOSans.ttf', "°1234567890%.-")
print(a)