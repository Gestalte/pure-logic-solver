import cairosvg
from PIL import Image
from io import BytesIO
import os

filepaths = os.listdir()
for filepath in filepaths:

    if os.path.isfile(filepath):
        split = tuple(filepath.rsplit(".", 1))
        sansExtension = split[0]
        extension = split[1]

        if extension == "svg":
            svgFile = open(filepath, 'r')
            svgText = svgFile.read()
            svgFile.close()

            svg = cairosvg.svg2svg(svgText, dpi=(96 / 1))
            bytes = cairosvg.svg2png(svg)
            img = Image.open(BytesIO(bytes))

            newFilepath = filepath.rsplit(".", 1)[0] + ".png"
            img.save(newFilepath)
