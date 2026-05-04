from fastapi import APIRouter, Depends, UploadFile, File, Header, HTTPException
from sqlalchemy.orm import Session
from app.db.session import get_db
from app.db.models import Song, AudioFeature, SongVector, Artist
from app.routes.auth import current_user
from app.services.storage import save_upload
from app.services.audio_analysis import extract_audio_features
from app.services.vectorizer import build_vector
from app.services.recommender import recommend

router = APIRouter()

def get_user(authorization: str | None, db: Session):
    if not authorization or not authorization.lower().startswith("bearer "):
        raise HTTPException(401, "Missing token")
    return current_user(authorization.split(" ", 1)[1], db)

@router.post('/upload')
def upload_song(title: str, artist: str | None = None, genre: str | None = None, file: UploadFile = File(...), authorization: str | None = Header(None), db: Session = Depends(get_db)):
    user = get_user(authorization, db)
    file_path = save_upload(file)
    artist_obj = None
    if artist:
        artist_obj = db.query(Artist).filter_by(name=artist).first() or Artist(name=artist)
        db.add(artist_obj); db.flush()
    song = Song(user_id=user.id, artist_id=artist_obj.id if artist_obj else None, title=title, genre=genre, file_path=file_path)
    db.add(song); db.commit(); db.refresh(song)
    return {"song_id": song.id}

@router.post('/{song_id}/analyze')
def analyze(song_id: str, authorization: str | None = Header(None), db: Session = Depends(get_db)):
    user = get_user(authorization, db)
    song = db.query(Song).filter_by(id=song_id, user_id=user.id).first()
    if not song: raise HTTPException(404, 'Song not found')
    f = extract_audio_features(song.file_path)
    vec = build_vector(f)
    db.merge(AudioFeature(song_id=song.id, features=f))
    db.merge(SongVector(song_id=song.id, vector=vec))
    db.commit()
    return {"song_id": song.id, "vector_length": len(vec), "features": f}

@router.get('')
def list_songs(authorization: str | None = Header(None), db: Session = Depends(get_db)):
    user = get_user(authorization, db)
    songs = db.query(Song).filter_by(user_id=user.id).all()
    return [{"id": s.id, "title": s.title, "genre": s.genre} for s in songs]

@router.get('/{song_id}')
def song_detail(song_id: str, authorization: str | None = Header(None), db: Session = Depends(get_db)):
    user = get_user(authorization, db)
    song = db.query(Song).filter_by(id=song_id, user_id=user.id).first()
    if not song: raise HTTPException(404, 'Song not found')
    af = db.query(AudioFeature).filter_by(song_id=song.id).first()
    return {"id": song.id, "title": song.title, "features": af.features if af else None}

@router.get('/{song_id}/recommendations')
def get_recs(song_id: str, limit: int = 10, authorization: str | None = Header(None), db: Session = Depends(get_db)):
    user = get_user(authorization, db)
    return {"results": recommend(db, user.id, song_id, limit)}
