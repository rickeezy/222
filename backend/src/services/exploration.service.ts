import { ContentType } from '@prisma/client';
import { prisma } from '../utils/db';

const meters = (aLat: number, aLng: number, bLat: number, bLng: number) => {
  const R = 6371e3;
  const p1 = (aLat * Math.PI) / 180;
  const p2 = (bLat * Math.PI) / 180;
  const dp = ((bLat - aLat) * Math.PI) / 180;
  const dl = ((bLng - aLng) * Math.PI) / 180;
  const x = Math.sin(dp / 2) ** 2 + Math.cos(p1) * Math.cos(p2) * Math.sin(dl / 2) ** 2;
  return 2 * R * Math.asin(Math.sqrt(x));
};

export async function processLocation(userId: string, universityId: string, lat: number, lng: number, accuracyM: number) {
  if (accuracyM > 100) throw new Error('Low GPS accuracy');
  const pois = await prisma.pointOfInterest.findMany({ where: { universityId } });
  const zones = await prisma.campusZone.findMany({ where: { universityId } });
  const unlocks = [] as string[];

  for (const poi of pois) {
    if (meters(lat, lng, poi.lat, poi.lng) <= poi.unlockRadiusMeters) {
      await prisma.unlockedContent.upsert({
        where: { userId_pointOfInterestId_contentType: { userId, pointOfInterestId: poi.id, contentType: ContentType.message } },
        create: { userId, pointOfInterestId: poi.id, contentType: ContentType.message },
        update: {}
      });
      unlocks.push(poi.name);
    }
  }

  for (const zone of zones) {
    const [centerLng, centerLat] = (zone.polygonGeoJson as any).properties?.center || [lng, lat];
    if (meters(lat, lng, centerLat, centerLng) <= zone.revealRadiusMeters) {
      await prisma.explorationProgress.upsert({
        where: { userId_zoneId: { userId, zoneId: zone.id } },
        create: { userId, zoneId: zone.id, percentExplored: 100 },
        update: { percentExplored: 100 }
      });
    }
  }
  return { unlocks };
}
