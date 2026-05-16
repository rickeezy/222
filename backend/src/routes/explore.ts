import { Router } from 'express';
import { requireAuth, AuthRequest } from '../middleware/auth';
import { processLocation } from '../services/exploration.service';
import { prisma } from '../utils/db';

export const exploreRouter = Router();
exploreRouter.use(requireAuth);
exploreRouter.post('/checkin', async (req: AuthRequest, res) => {
  try {
    const { lat, lng, accuracyM = 30 } = req.body;
    const result = await processLocation(req.user!.id, req.user!.universityId, lat, lng, accuracyM);
    res.json(result);
  } catch (e: any) {
    res.status(400).json({ error: e.message });
  }
});
exploreRouter.get('/dashboard', async (req: AuthRequest, res) => {
  const [progress, unlocks, captures, quests] = await Promise.all([
    prisma.explorationProgress.findMany({ where: { userId: req.user!.id }, include: { zone: true } }),
    prisma.unlockedContent.findMany({ where: { userId: req.user!.id }, include: { pointOfInterest: true } }),
    prisma.photoCapture.findMany({ where: { userId: req.user!.id } }),
    prisma.quest.findMany({ where: { universityId: req.user!.universityId } })
  ]);
  res.json({ progress, unlocks, captures, quests });
});
