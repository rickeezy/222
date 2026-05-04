from jose import jwt
from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from app.db.session import get_db
from app.db.models import User
from app.schemas.common import RegisterIn, LoginIn, TokenOut
from app.core.security import hash_password, verify_password, create_access_token
from app.core.config import settings

router = APIRouter()

@router.post('/register', response_model=TokenOut)
def register(payload: RegisterIn, db: Session = Depends(get_db)):
    if db.query(User).filter_by(email=payload.email).first():
        raise HTTPException(400, 'Email already exists')
    user = User(email=payload.email, password_hash=hash_password(payload.password))
    db.add(user); db.commit(); db.refresh(user)
    return TokenOut(access_token=create_access_token(user.id))

@router.post('/login', response_model=TokenOut)
def login(payload: LoginIn, db: Session = Depends(get_db)):
    user = db.query(User).filter_by(email=payload.email).first()
    if not user or not verify_password(payload.password, user.password_hash):
        raise HTTPException(401, 'Invalid credentials')
    return TokenOut(access_token=create_access_token(user.id))

def current_user(token: str, db: Session):
    try:
      uid = jwt.decode(token, settings.secret_key, algorithms=[settings.algorithm])["sub"]
      user = db.query(User).filter_by(id=uid).first()
      if not user: raise Exception()
      return user
    except Exception:
      raise HTTPException(401, 'Invalid token')
