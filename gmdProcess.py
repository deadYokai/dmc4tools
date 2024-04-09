from struct import *
import os
import argparse

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

    def seek(self, value, v = None):
        if type(value) is bytes:
            self.base_stream.seek(int.from_bytes(value))
        else:
            if v is not None:
                self.base_stream.seek(value, v)
            else:
                self.base_stream.seek(value)

    def pack(self, fmt, data):
        return self.writeBytes(pack(fmt, data))

    def unpack(self, fmt, length = 1):
        return unpack(fmt, self.readBytes(length))[0]

import glob

def text(path, pack):
    f = os.path.dirname(os.path.abspath(path))
    baseName = os.path.basename(path).split('.')[0]
    if pack:
        mode1 = "rb+"
        mode2 = "r"
    else:
        mode1 = "rb"
        mode2 = "w"
    with open(fr"{f}{os.sep}{baseName}.gmd", mode1) as gfdFile:
        gReader = BinaryStream(gfdFile)
        gReader.seek(0, os.SEEK_END)
        size = gReader.offset()
        gReader.seek(32)
        count = gReader.readUInt32()
        diff = size - count
        if diff > size or diff < 0:
            diff = size
        print(diff)
        gReader.seek(diff)
        with open(fr"{f}{os.sep}{baseName}.txt", mode2, encoding="utf-8") as out:
            if not pack:
                data = gReader.readBytes(count).replace(b'\x00', b'FFFFFFFF').decode("UTF-8")
                text = data.split('FFFFFFFF')
                for a in text:
                    out.write("---STARTSTRING---\n")
                    out.write(a)
                    out.write("\n---ENDSTRING---\n\n")
            else:
                gReader.clear()
                text = out.read().replace("---STARTSTRING---\n", '').split("\n---ENDSTRING---\n\n")
                c = 0
                for a in text:
                    data = a.replace('\n', '\r\n').encode("UTF-8") + b'\x00'
                    print(data)
                    gReader.writeBytes(data)
                    c += len(data)
                o = gReader.offset()
                gReader.seek(32)
                gReader.writeUInt32(c)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = 'GMD Extractor (DMC4SE)', epilog = 'Work in progress')
    parser.add_argument("filename", help = "File")
    parser.add_argument("-p", "--pack", help = "Pack file", default=False, action = argparse.BooleanOptionalAction)
    args = parser.parse_args()

    text(args.filename, args.pack)
