from pydantic import BaseModel, EmailStr


class RegisterIn(BaseModel):
    email: EmailStr
    password: str


class LoginIn(RegisterIn):
    pass


class TokenOut(BaseModel):
    access_token: str
    token_type: str = "bearer"


class FeedbackIn(BaseModel):
    song_id: str
    target_song_id: str
    action: str
