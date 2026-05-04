import uuid
from datetime import datetime
from sqlalchemy import String, DateTime, ForeignKey, Float, Integer, Boolean, JSON, UniqueConstraint
from sqlalchemy.orm import Mapped, mapped_column, relationship
from pgvector.sqlalchemy import Vector
from app.db.session import Base


class User(Base):
    __tablename__ = "users"
    id: Mapped[str] = mapped_column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    email: Mapped[str] = mapped_column(String, unique=True, index=True)
    password_hash: Mapped[str] = mapped_column(String)
    created_at: Mapped[datetime] = mapped_column(DateTime, default=datetime.utcnow)


class Artist(Base):
    __tablename__ = "artists"
    id: Mapped[str] = mapped_column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    name: Mapped[str] = mapped_column(String, unique=True, index=True)


class Song(Base):
    __tablename__ = "songs"
    id: Mapped[str] = mapped_column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    user_id: Mapped[str] = mapped_column(ForeignKey("users.id"), index=True)
    artist_id: Mapped[str | None] = mapped_column(ForeignKey("artists.id"), nullable=True, index=True)
    title: Mapped[str] = mapped_column(String, index=True)
    genre: Mapped[str | None] = mapped_column(String, nullable=True)
    file_path: Mapped[str] = mapped_column(String, unique=True)
    uploaded_at: Mapped[datetime] = mapped_column(DateTime, default=datetime.utcnow, index=True)
    is_demo: Mapped[bool] = mapped_column(Boolean, default=False)


class AudioFeature(Base):
    __tablename__ = "audio_features"
    id: Mapped[str] = mapped_column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    song_id: Mapped[str] = mapped_column(ForeignKey("songs.id"), unique=True, index=True)
    features: Mapped[dict] = mapped_column(JSON)


class SongVector(Base):
    __tablename__ = "song_vectors"
    id: Mapped[str] = mapped_column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    song_id: Mapped[str] = mapped_column(ForeignKey("songs.id"), unique=True, index=True)
    vector: Mapped[list[float]] = mapped_column(Vector(64))


class UserFeedback(Base):
    __tablename__ = "user_feedback"
    __table_args__ = (UniqueConstraint("user_id", "song_id", "target_song_id", name="uq_feedback"),)
    id: Mapped[str] = mapped_column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    user_id: Mapped[str] = mapped_column(ForeignKey("users.id"), index=True)
    song_id: Mapped[str] = mapped_column(ForeignKey("songs.id"), index=True)
    target_song_id: Mapped[str] = mapped_column(ForeignKey("songs.id"), index=True)
    action: Mapped[str] = mapped_column(String)
    weight: Mapped[float] = mapped_column(Float, default=0.0)


class RecommendationEvent(Base):
    __tablename__ = "recommendation_events"
    id: Mapped[str] = mapped_column(String, primary_key=True, default=lambda: str(uuid.uuid4()))
    user_id: Mapped[str] = mapped_column(ForeignKey("users.id"), index=True)
    song_id: Mapped[str] = mapped_column(ForeignKey("songs.id"), index=True)
    count: Mapped[int] = mapped_column(Integer, default=1)
    created_at: Mapped[datetime] = mapped_column(DateTime, default=datetime.utcnow)
