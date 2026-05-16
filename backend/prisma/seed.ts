import { PrismaClient, Role, UnlockType } from '@prisma/client';
import bcrypt from 'bcryptjs';
const prisma = new PrismaClient();

async function main() {
  const university = await prisma.university.upsert({
    where: { slug: 'howard-university' },
    update: {},
    create: {
      name: 'Howard University', slug: 'howard-university', centerLat: 38.9227, centerLng: -77.0196,
      mapBounds: { north: 38.9275, south: 38.918, east: -77.014, west: -77.026 }
    }
  });
  const zone = await prisma.campusZone.create({ data: { universityId: university.id, name: 'Founders Core', description: 'Historic core', revealRadiusMeters: 180, unlockType: UnlockType.location, polygonGeoJson: { type: 'Feature', properties: { center: [-77.0196, 38.9227] }, geometry: { type: 'Polygon', coordinates: [[[-77.022,38.924],[-77.018,38.924],[-77.018,38.921],[-77.022,38.921],[-77.022,38.924]]] } } } });
  await prisma.pointOfInterest.createMany({ data: [
    { universityId: university.id, zoneId: zone.id, name: 'Founders Library', description: 'Iconic campus library', lat: 38.9232, lng: -77.0198, category: 'library', message: 'You found Founders Library.', unlockRadiusMeters: 60 },
    { universityId: university.id, zoneId: zone.id, name: 'The Yard', description: 'Heart of student life', lat: 38.9221, lng: -77.0208, category: 'landmark', message: 'This spot is part of Howard history.', unlockRadiusMeters: 80 }
  ]});
  await prisma.quest.create({ data: { universityId: university.id, title: 'Bison Trail', description: 'Visit iconic Howard landmarks.', requiredPoiIds: [], rewardBadge: 'Bison Explorer' } });
  const hash = await bcrypt.hash('Password123!', 10);
  await prisma.user.upsert({ where: { email: 'admin@howard.edu' }, update: {}, create: { name: 'Admin', email: 'admin@howard.edu', passwordHash: hash, role: Role.admin, universityId: university.id } });
}

main().finally(async () => prisma.$disconnect());
