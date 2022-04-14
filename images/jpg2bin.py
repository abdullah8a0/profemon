def jpg2bin(jpg_file):
    with open(jpg_file, 'rb') as f:
        return f.read()

if __name__ == '__main__':
    file = 'JoeSteinmeyer.jpg'
    print(jpg2bin(file))