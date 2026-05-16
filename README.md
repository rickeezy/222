# Campus Exploration Game (Howard University Example)

## Phase 1: Architecture & Setup
- Frontend: Next.js + TypeScript + Tailwind + Leaflet + Zustand + Framer Motion
- Backend: Express + Prisma + PostgreSQL + Redis + JWT auth
- Infra: Docker Compose for `postgres`, `redis`, `backend`, `frontend`

## File Tree
- `backend/` API, Prisma schema, seed, auth/exploration/admin services
- `frontend/` map UI, fog reveal, dashboard/admin pages
- `docker-compose.yml` one-command startup

## Quick Start
```bash
docker compose up --build
```

## URLs
- Frontend: `http://localhost:3000`
- Backend: `http://localhost:8000/health`

## Seed Credentials
- Admin: `admin@howard.edu` / `Password123!`

## Phase 2: Backend
- `/auth/register`, `/auth/login`
- `/explore/checkin`, `/explore/dashboard`
- `/admin/zones`, `/admin/pois`, `/admin/quests`, `/admin/analytics`
- GPS spoofing mitigation: check-in requires acceptable GPS accuracy

## Phase 3: Frontend
- Interactive Leaflet map centered at Howard
- Fog-of-war polygon revealed after discovery
- Geolocation check-in and POI marker/messages
- Student dashboard page scaffold

## Phase 4: Admin
- Admin routes and initial UI pages scaffolded for zone/POI/quest management

## Phase 5: Deployment + Testing
- Run `docker compose up --build`
- Backend checks: `curl http://localhost:8000/health`
- Frontend checks: open localhost:3000 and verify map + fog reveal

## Privacy & Safety Copy (UI requirement)
- Location is processed only while app is open.
- The app stores discovery events, not continuous movement history.
- Exact location is never shared with other students.
