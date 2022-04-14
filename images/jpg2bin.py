from PIL import Image
import io

def jpg2bin(jpg_file):
    with open(jpg_file, 'rb') as f:
        return f.read()

def jpg2bin_small(jpg_file, width=32, height=40):
    img = Image.open(jpg_file)
    img.thumbnail((width, height))
    output = io.BytesIO()
    img.save(output, format='JPEG')
    return output.getvalue()

if __name__ == '__main__':
    file = 'JoeSteinmeyer.jpg'
    print(len(jpg2bin(file)))
    print(len(jpg2bin_small(file)))