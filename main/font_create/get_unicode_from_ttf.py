from fontTools.ttLib import TTFont
import cairosvg

def get_unicode_from_ttf(font_path, char):
    out = ""
    for i in [char for char in char]:
        print(i, hex(ord(i)))
        out += hex(ord(i)) + ","
    return out

# 使用方法
a = get_unicode_from_ttf('./OPPOSans.ttf', "晴多云阴阵雨雷伴有冰雹夹雪小中大暴特雾冻沙尘到浮扬强浓龙卷风弱高吹轻霾度重严")
print(a)