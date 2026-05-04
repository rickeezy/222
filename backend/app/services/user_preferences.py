from app.db.models import UserFeedback

ACTION_WEIGHT = {"like": 1.0, "save": 0.8, "skip": -0.4, "dislike": -1.0}


def preference_boost(db, user_id: str, song_id: str, target_song_id: str) -> float:
    f = db.query(UserFeedback).filter_by(user_id=user_id, song_id=song_id, target_song_id=target_song_id).first()
    return f.weight if f else 0.0
