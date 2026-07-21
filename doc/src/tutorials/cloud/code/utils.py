import io
import zipfile
import os


class QuietBytes(bytes):
    def __str__(self):
        return str(len(self)) + " bytes"

    def __repr__(self):
        return str(len(self)) + " bytes"


def unzip_bytes(zip_bytes):
    with io.BytesIO(zip_bytes) as bytestream:
        with zipfile.ZipFile(bytestream, "r") as zipf:

            zipf.extractall(".")


def zip_directory(directory, exclude_dirs=[]):
    with io.BytesIO() as bytestream:
        with zipfile.ZipFile(bytestream, "w") as zipf:

            for root, dirs, files in os.walk(directory):
                dirs[:] = [d for d in dirs if d not in exclude_dirs]

                for f in files:
                    zipf.write(os.path.join(root, f))

        bytestream.seek(0)
        zipped = QuietBytes(bytestream.read())

    return zipped
