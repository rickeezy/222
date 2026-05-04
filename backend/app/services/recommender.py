import numpy as np
from sklearn.metrics.pairwise import cosine_similarity
from app.db.models import Song, AudioFeature, SongVector
from app.services.user_preferences import preference_boost


def _key_bonus(k1: str, k2: str) -> float:
    return 1.0 if k1 == k2 else 0.6


def recommend(db, user_id: str, song_id: str, limit: int = 10):
    base_vec = db.query(SongVector).filter_by(song_id=song_id).first()
    if not base_vec:
        return []
    base_features = db.query(AudioFeature).filter_by(song_id=song_id).first().features
    others = db.query(SongVector).join(Song, Song.id == SongVector.song_id).filter(Song.user_id == user_id, Song.id != song_id).all()
    results = []
    for ov in others:
        tf = db.query(AudioFeature).filter_by(song_id=ov.song_id).first().features
        vector_sim = float(cosine_similarity([base_vec.vector], [ov.vector])[0][0])
        bpm_penalty = max(0.0, 1 - abs(base_features["tempo_bpm"] - tf["tempo_bpm"]) / 80)
        key = _key_bonus(base_features["estimated_key"], tf["estimated_key"])
        energy = max(0.0, 1 - abs(base_features["rms_energy_mean"] - tf["rms_energy_mean"]))
        timbre = max(0.0, 1 - np.linalg.norm(np.array(base_features["mfcc_mean_vector"]) - np.array(tf["mfcc_mean_vector"])) / 500)
        feature_score = 0.12 * bpm_penalty + 0.10 * key + 0.08 * energy + 0.22 * timbre
        pref = preference_boost(db, user_id, song_id, ov.song_id)
        final = (vector_sim * 0.65 + feature_score * 0.25 + pref * 0.10) * 100
        results.append({"song_id": ov.song_id, "score": round(max(0,min(100,final)),2), "explanation": {"bpm_difference": round(abs(base_features['tempo_bpm'] - tf['tempo_bpm']),2), "key_compatibility": key, "energy_similarity": round(energy,3), "timbre_similarity": round(timbre,3)}})
    return sorted(results, key=lambda x: x["score"], reverse=True)[:limit]
