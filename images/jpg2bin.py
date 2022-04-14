from PIL import Image
import io

def jpg2bin(jpg_file, width=96, height=120):
    img = Image.open(jpg_file)
    img = img.resize((width, height))
    output = io.BytesIO()
    img.save(output, format='JPEG')
    return output.getvalue()

def jpg2bin_small(jpg_file, width=32, height=40):
    img = Image.open(jpg_file)
    img = img.resize((width, height))
    output = io.BytesIO()
    img.save(output, format='JPEG')
    return output.getvalue()

if __name__ == '__main__':
    file = 'JoeSteinmeyer.jpg'
    print(len(jpg2bin(file)))
    print(len(jpg2bin_small(file)))