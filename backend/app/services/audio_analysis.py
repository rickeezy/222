import numpy as np
import librosa


def extract_audio_features(file_path: str) -> dict:
    y, sr = librosa.load(file_path, sr=22050, mono=True, duration=120.0)
    if np.max(np.abs(y)) > 0:
        y = y / np.max(np.abs(y))
    tempo, beats = librosa.beat.beat_track(y=y, sr=sr)
    rms = librosa.feature.rms(y=y)[0]
    sc = librosa.feature.spectral_centroid(y=y, sr=sr)[0]
    contrast = librosa.feature.spectral_contrast(y=y, sr=sr)
    zcr = librosa.feature.zero_crossing_rate(y)[0]
    mfcc = librosa.feature.mfcc(y=y, sr=sr, n_mfcc=20)
    chroma = librosa.feature.chroma_stft(y=y, sr=sr)
    onset = librosa.onset.onset_strength(y=y, sr=sr)
    key_idx = int(np.argmax(chroma.mean(axis=1)))
    key_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    return {
        "tempo_bpm": float(tempo),
        "tempo_confidence": float(min(1.0, len(beats) / max(1, len(y) / sr / 2))),
        "beats_per_minute": float(tempo),
        "rms_energy_mean": float(rms.mean()),
        "rms_energy_std": float(rms.std()),
        "spectral_centroid_mean": float(sc.mean()),
        "spectral_centroid_std": float(sc.std()),
        "spectral_bandwidth_mean": float(librosa.feature.spectral_bandwidth(y=y, sr=sr)[0].mean()),
        "spectral_rolloff_mean": float(librosa.feature.spectral_rolloff(y=y, sr=sr)[0].mean()),
        "spectral_contrast_mean": float(contrast.mean()),
        "zero_crossing_rate_mean": float(zcr.mean()),
        "mfcc_mean_vector": mfcc.mean(axis=1).astype(float).tolist(),
        "mfcc_std_vector": mfcc.std(axis=1).astype(float).tolist(),
        "chroma_mean_vector": chroma.mean(axis=1).astype(float).tolist(),
        "chroma_std_vector": chroma.std(axis=1).astype(float).tolist(),
        "onset_strength_mean": float(onset.mean()),
        "onset_strength_std": float(onset.std()),
        "estimated_key": key_names[key_idx],
        "estimated_mode": "major",
        "duration_seconds": float(librosa.get_duration(y=y, sr=sr)),
        "loudness_proxy": float(20 * np.log10(np.clip(np.sqrt((y**2).mean()), 1e-9, None))),
    }
