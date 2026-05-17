import { Router } from 'express';
import { requireAuth, AuthRequest } from '../middleware/auth';
import { prisma } from '../utils/db';

export const adminRouter = Router();
adminRouter.use(requireAuth);
adminRouter.use((req: AuthRequest, res, next) => req.user?.role === 'admin' ? next() : res.status(403).json({ error: 'Admin only' }));
adminRouter.post('/zones', async (req: AuthRequest, res) => res.json(await prisma.campusZone.create({ data: { ...req.body, universityId: req.user!.universityId } })));
adminRouter.post('/pois', async (req: AuthRequest, res) => res.json(await prisma.pointOfInterest.create({ data: { ...req.body, universityId: req.user!.universityId } })));
adminRouter.post('/quests', async (req: AuthRequest, res) => res.json(await prisma.quest.create({ data: { ...req.body, universityId: req.user!.universityId } })));
adminRouter.get('/analytics', async (req: AuthRequest, res) => {
  const [students, discovered] = await Promise.all([
    prisma.user.count({ where: { universityId: req.user!.universityId, role: 'student' } }),
    prisma.explorationProgress.count()
  ]);
  res.json({ students, discovered });
});
