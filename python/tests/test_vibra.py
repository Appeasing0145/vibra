import struct
import unittest

from vibra import (
    Vibra,
    get_signature_from_float_pcm,
    get_signature_from_signed_pcm,
    get_signature_from_wav_data,
)


def make_wav(samples):
    data = b"".join(struct.pack("<h", sample) for sample in samples)
    return b"".join(
        [
            b"RIFF",
            struct.pack("<I", 36 + len(data)),
            b"WAVE",
            b"fmt ",
            struct.pack("<I", 16),
            struct.pack("<H", 1),
            struct.pack("<H", 1),
            struct.pack("<I", 16000),
            struct.pack("<I", 16000 * 2),
            struct.pack("<H", 2),
            struct.pack("<H", 16),
            b"data",
            struct.pack("<I", len(data)),
            data,
        ]
    )


class VibraTest(unittest.TestCase):
    def test_generates_signature_from_signed_pcm(self):
        pcm = struct.pack("<128h", *([0] * 128))

        signature = get_signature_from_signed_pcm(
            pcm,
            sample_rate=16000,
            sample_width=16,
            channel_count=1,
        )

        self.assertEqual(signature.sample_ms, 8)
        self.assertTrue(signature.uri.startswith("data:audio/vnd.shazam.sig;base64,"))

    def test_generates_same_signature_for_equivalent_wav_and_pcm(self):
        samples = [index % 64 for index in range(512)]
        pcm = struct.pack("<512h", *samples)
        wav_signature = get_signature_from_wav_data(make_wav(samples))
        pcm_signature = Vibra().get_signature_from_signed_pcm(
            pcm,
            sample_rate=16000,
            sample_width=16,
            channel_count=1,
        )

        self.assertEqual(wav_signature, pcm_signature)

    def test_generates_signature_from_float_pcm(self):
        pcm = struct.pack("<128f", *([0.0] * 128))

        signature = get_signature_from_float_pcm(
            pcm,
            sample_rate=16000,
            sample_width=32,
            channel_count=1,
        )

        self.assertEqual(signature.sample_ms, 8)


if __name__ == "__main__":
    unittest.main()
