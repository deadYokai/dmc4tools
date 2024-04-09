from struct import *
import os

class BinaryStream:
    def __init__(self, base_stream):
        self.base_stream = base_stream

    def clear(self):
        self.base_stream.truncate()

    def readByte(self):
        return self.base_stream.read(1)

    def readBytes(self, length):
        return self.base_stream.read(length)

    def readChar(self):
        return self.unpack('b')

    def readUChar(self):
        return self.unpack('B')

    def readBool(self):
        return self.unpack('?')

    def readInt16(self):
        return self.unpack('h', 2)

    def readUInt16(self):
        return self.unpack('H', 2)

    def readInt32(self):
        return self.unpack('i', 4)

    def readUInt32(self):
        return self.unpack('I', 4)

    def readInt64(self):
        return self.unpack('q', 8)

    def readUInt64(self):
        return self.unpack('Q', 8)

    def readFloat(self):
        return self.unpack('f', 4)

    def readDouble(self):
        return self.unpack('d', 8)

    def readString(self, length):
        return self.unpack(str(length) + 's', length)

    def writeBytes(self, value):
        self.base_stream.write(value)

    def writeChar(self, value):
        self.pack('c', value)

    def writeUChar(self, value):
        self.pack('C', value)

    def writeBool(self, value):
        self.pack('?', value)

    def writeInt16(self, value):
        self.pack('h', value)

    def writeUInt16(self, value):
        self.pack('H', value)

    def writeInt32(self, value):
        self.pack('i', value)

    def writeUInt32(self, value):
        self.pack('I', value)

    def writeInt64(self, value):
        self.pack('q', value)

    def writeUInt64(self, value):
        self.pack('Q', value)

    def writeFloat(self, value):
        self.pack('f', value)

    def writeDouble(self, value):
        self.pack('d', value)

    def writeString(self, value):
        length = len(value)
        self.writeUInt16(length)
        self.pack(str(length) + 's', value)

    def offset(self, asbytes = False):
        if asbytes:
            return int.to_bytes(self.base_stream.tell())
        return self.base_stream.tell()

    def seek(self, value):
        if type(value) is bytes:
            self.base_stream.seek(int.from_bytes(value))
        else:
            self.base_stream.seek(value)

    def pack(self, fmt, data):
        return self.writeBytes(pack(fmt, data))

    def unpack(self, fmt, length = 1):
        return unpack(fmt, self.readBytes(length))[0]

from wand.image import Image
import glob
import argparse

def fixHex(val):
    if len(val) == 3 or len(val) == 1:
        return "0" + val
    return val

def cropDDS(path):
    f = os.path.dirname(os.path.abspath(path))
    baseName = os.path.basename(path).split('.')[0]
    with open(fr"{f}{os.sep}{baseName}.gfd", "rb") as gfdFile:
        ddsFile = Image(filename=glob.glob(fr"{f}{os.sep}{baseName}*.dds")[0])
        ddsW, ddsH = ddsFile.size
        gReader = BinaryStream(gfdFile)
        gReader.seek(0)
        header = gReader.readBytes(4)
        indexPath = os.path.join(f, fr"_{baseName}", ".index")
        if os.path.isfile(indexPath):
            os.remove(indexPath)
        if header == b"GFD\x00":
            os.makedirs(fr"{f}{os.sep}_{baseName}", exist_ok=True)
            gReader.seek(b'\x1c')
            count = gReader.readUInt32()
            gReader.seek(b'\x30')
            gfdNameLen = gReader.readInt32()
            if gfdNameLen == 0:
                gfdNameLen = gReader.readInt32()
            gfdName = gReader.readBytes(gfdNameLen + 1) # plus 1 for zerobyte
            for i in range(count):
                char = gReader.readBytes(2)[::-1]
                hexChar = char.hex()
                gReader.readBytes(3)
                tmpPos = [gReader.readByte().hex(), gReader.readByte().hex(), gReader.readByte().hex()]
                gReader.readByte()
                tmpSize = [gReader.readByte().hex(), gReader.readByte().hex(), gReader.readByte().hex()]
                offset = gReader.readInt16()
                otherData = gReader.readBytes(2).hex()
                x = int.from_bytes(bytes.fromhex(fixHex(tmpPos[1][1] + tmpPos[0])))
                y = int.from_bytes(bytes.fromhex(fixHex(tmpPos[2] + tmpPos[1][0])))
                w = int.from_bytes(bytes.fromhex(fixHex(tmpSize[1][1] + tmpSize[0])))
                h = int.from_bytes(bytes.fromhex(fixHex(tmpSize[2] + tmpSize[1][0])))
                print(str(hexChar) + f": [{x},{y},{w},{h}]")
                zs = False
                if w != 0 and h != 0:
                    with ddsFile[x:w+x, y:h+y] as charImg:
                        imgpath = os.path.join(f, fr"_{baseName}", f"{hexChar}.dds")
                        charImg.save(filename=imgpath)
                else:
                    zs = True
                with open(indexPath, "a", encoding="UTF-8") as indexFile:
                    c = "dy"
                    a = ""
                    if zs:
                        a += "\t0"
                    indexFile.write(f"{c}\t{hexChar}\t{offset}{a}\t{otherData}\n")

from shutil import copyfile
def createDDS(path):
    f = os.path.dirname(os.path.abspath(path))
    baseName = os.path.basename(path).split('.')[0]
    size = [512, 512]
    ddsGlob = glob.glob(fr"{f}{os.sep}_{baseName}{os.sep}*.dds")
    origDDS = glob.glob(fr"{f}{os.sep}{baseName}*.dds")[0]
    ddsGlob.sort()
    newDDS = Image(width=size[0], height=size[1])
    X = 0
    Y = 0
    chars = {}
    for file in ddsGlob:
        charHex = os.path.basename(file).split('.')[0]
        print(f"Packing {charHex} to {origDDS}")
        charBytes = bytes.fromhex(charHex)
        with Image(filename=file) as charTex:
            texW = charTex.width
            texH = charTex.height

            if (X+texW) > 512:
                X = 0
                Y += texH
            newDDS.composite(charTex, left=X, top=Y)
            chars[charHex] = {"x": X, "y": Y, "w": texW, "h": texH, "offset": texW, "somedata": "0000"}
            X += texW
    if not os.path.isfile(origDDS + ".bak"):
        copyfile(origDDS, origDDS + ".bak")
    newDDS.save(filename=origDDS)
    with open(fr"{f}{os.sep}_{baseName}{os.sep}.index", "r", encoding="UTF-8") as indexFile:
        line = indexFile.readline()
        while line:
            if line != "":
                l = line.replace('\n', '').split('\t')
                if l[1] not in chars:
                    if len(l) < 5:
                        print(f"{l[1]}.dds not found, skipping")
                    else:
                        chars[l[1]] = {"x": 0, "y": 0, "w": 0, "h": 0, "offset": int(l[2]), "somedata": str(l[4])}
                else:
                    chars[l[1]]["offset"] = int(l[2])
                    chars[l[1]]["somedata"] = l[3]
            line = indexFile.readline()

    with open(fr"{f}{os.sep}{baseName}.gfd", "rb+") as gfdFile:
        gReader = BinaryStream(gfdFile)
        if gReader.readBytes(4) == b'GFD\x00':
            gReader.seek(b'\x1c')
            gReader.writeUInt32(len(chars))
            gReader.seek(b'\x30')
            gfdNameLen = gReader.readInt32()
            if gfdNameLen == 0:
                gfdNameLen = gReader.readInt32()
            name = gReader.readBytes(gfdNameLen + 1)
            if not os.path.isfile(fr"{f}{os.sep}{baseName}.gfd.bak"):
                copyfile(fr"{f}{os.sep}{baseName}.gfd", fr"{f}{os.sep}{baseName}.gfd.bak")
            gReader.clear()
            for charHex, charData in chars.items():
                if len(charHex) == 3:
                    charHex = "0" + charHex
                char = bytes.fromhex(charHex)[::-1]
                y = pack('<H', charData["y"]).hex()
                y = y[1:] + y[0]
                h = pack('<H', charData["h"]).hex()
                h = h[1:] + h[0]
                x = pack('<H', charData["x"]).hex()
                w = pack('<H', charData["w"]).hex()

                posBytes = bytes.fromhex(x[:2] + y[0] + x[3] + y[2:])
                sizeBytes = bytes.fromhex(w[:2] + h[0] + w[3] + h[2:])

                gReader.writeBytes(char)
                gReader.writeBytes(b'\x00\x00\x00')
                gReader.writeBytes(posBytes)
                gReader.writeBytes(b'\x00')
                gReader.writeBytes(sizeBytes)
                gReader.writeInt16(charData["offset"])
                gReader.writeBytes(bytes.fromhex(charData["somedata"]))
            print("Done.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = 'GFD Repack (DMC4SE)', epilog = 'Work in progress')
    parser.add_argument("filename", help = "File")
    parser.add_argument("-p", "--pack", help = "Pack file", default=False, action = argparse.BooleanOptionalAction)
    args = parser.parse_args()

    if args.pack:
        createDDS(args.filename)
    else:
        cropDDS(args.filename)
