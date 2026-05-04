from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from app.db.session import Base, engine
from app.routes import auth, songs, feedback

app = FastAPI(title="WaveMatch AI")
app.add_middleware(CORSMiddleware, allow_origins=["*"], allow_methods=["*"], allow_headers=["*"])
Base.metadata.create_all(bind=engine)

app.include_router(auth.router, prefix="/auth", tags=["auth"])
app.include_router(songs.router, prefix="/songs", tags=["songs"])
app.include_router(feedback.router, tags=["feedback"])

@app.get("/health")
def health():
    return {"status": "ok"}
