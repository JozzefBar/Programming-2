from PIL import Image
import sys
import numpy


def img2txt(image_path: str, output_txt_path: str):
	with Image.open(image_path).convert('L') as im:
		data = numpy.asarray(im)
		data = data/255.0
		numpy.savetxt(output_txt_path, data, '%.2f')


if len(sys.argv) != 3:
	print('Usage: python img2txt.py image_path output_txt_path')
else:
	img2txt(sys.argv[1], sys.argv[2])
