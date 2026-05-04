import os, uuid
from pathlib import Path
from fastapi import UploadFile, HTTPException
from app.core.config import settings

ALLOWED = {".wav", ".mp3", ".flac", ".m4a"}

def save_upload(file: UploadFile) -> str:
    ext = Path(file.filename or "").suffix.lower()
    if ext not in ALLOWED:
        raise HTTPException(400, "Unsupported file type")
    content = file.file.read()
    if len(content) > settings.max_upload_mb * 1024 * 1024:
        raise HTTPException(400, "File too large")
    os.makedirs(settings.upload_dir, exist_ok=True)
    name = f"{uuid.uuid4()}{ext}"
    path = os.path.join(settings.upload_dir, name)
    with open(path, "wb") as f:
        f.write(content)
    return path
