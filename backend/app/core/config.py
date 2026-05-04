from pydantic_settings import BaseSettings


class Settings(BaseSettings):
    app_name: str = "WaveMatch AI"
    secret_key: str = "dev-secret"
    algorithm: str = "HS256"
    access_token_expire_minutes: int = 60 * 24
    database_url: str = "postgresql+psycopg2://wavematch:wavematch@postgres:5432/wavematch"
    upload_dir: str = "backend/uploads"
    max_upload_mb: int = 40


settings = Settings()
