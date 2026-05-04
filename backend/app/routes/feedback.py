from fastapi import APIRouter, Depends, Header, HTTPException
from sqlalchemy.orm import Session
from app.db.session import get_db
from app.routes.auth import current_user
from app.db.models import UserFeedback
from app.schemas.common import FeedbackIn
from app.services.user_preferences import ACTION_WEIGHT

router = APIRouter()

@router.post('/feedback')
def add_feedback(payload: FeedbackIn, authorization: str | None = Header(None), db: Session = Depends(get_db)):
    if not authorization or not authorization.lower().startswith('bearer '):
        raise HTTPException(401, 'Missing token')
    user = current_user(authorization.split(' ', 1)[1], db)
    weight = ACTION_WEIGHT.get(payload.action)
    if weight is None: raise HTTPException(400, 'Invalid action')
    row = db.query(UserFeedback).filter_by(user_id=user.id, song_id=payload.song_id, target_song_id=payload.target_song_id).first()
    if not row: row = UserFeedback(user_id=user.id, song_id=payload.song_id, target_song_id=payload.target_song_id, action=payload.action, weight=weight)
    else: row.action=payload.action; row.weight=weight
    db.add(row); db.commit()
    return {"status": "ok"}
