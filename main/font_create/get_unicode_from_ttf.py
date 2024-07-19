from fontTools.ttLib import TTFont


def get_unicode_code(char):
    out = ""
    for i in [char for char in char]:
        print(i, hex(ord(i)))
        out += hex(ord(i)) + ","
    return out

## 获取字体里面全部的编码
def get_unicode_from_ttf(font_path):
    font = TTFont(font_path)
    cmap = font.getBestCmap()
    a = ""
    for c in cmap:
        print(c)
        a += str(c) +","
    font.close()
    print(a)

# 使用方法
a = get_unicode_code("少云晴间多阴阵雨强雷伴有冰雹小中大极端降毛/细暴特冻到雪夹天气薄雾霾扬沙浮尘浓度重严热冷未知新月蛾眉上弦盈凸满亏下残霜风台六七级八九十一二")
print(a)