# WaveMatch AI

## Run
```bash
docker compose up --build
```

Frontend: http://localhost:3000
Backend docs: http://localhost:8000/docs

## Local backend
```bash
cd backend
python -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
uvicorn app.main:app --reload
```

## Tests
```bash
cd backend
pytest
```

## Legal
Uploaded audio is private per uploader and not redistributed.
