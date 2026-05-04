from app.services.vectorizer import build_vector

def test_vector_consistent_length():
    f = {
      'tempo_bpm':120,'tempo_confidence':0.8,'rms_energy_mean':0.2,'spectral_centroid_mean':2400,
      'mfcc_mean_vector':[0.1]*20,'mfcc_std_vector':[0.2]*20,'chroma_mean_vector':[0.3]*12
    }
    v = build_vector(f)
    assert len(v) == 64
