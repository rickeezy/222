import numpy as np

VECTOR_LENGTH = 64


def build_vector(features: dict) -> list[float]:
    raw = []
    raw.extend([features["tempo_bpm"] / 220.0, features["tempo_confidence"], features["rms_energy_mean"], features["spectral_centroid_mean"] / 10000.0])
    raw.extend(features["mfcc_mean_vector"][:20])
    raw.extend(features["mfcc_std_vector"][:20])
    raw.extend(features["chroma_mean_vector"][:12])
    arr = np.array(raw, dtype=np.float32)
    if len(arr) < VECTOR_LENGTH:
      arr = np.pad(arr, (0, VECTOR_LENGTH - len(arr)))
    else:
      arr = arr[:VECTOR_LENGTH]
    norm = np.linalg.norm(arr)
    return (arr / norm if norm > 0 else arr).astype(float).tolist()
