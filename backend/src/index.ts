import express from 'express';
import cors from 'cors';
import { env } from './config/env';
import { authRouter } from './routes/auth';
import { exploreRouter } from './routes/explore';
import { adminRouter } from './routes/admin';
import { prisma } from './utils/db';

const app = express();
app.use(cors({ origin: env.frontendUrl }));
app.use(express.json({ limit: '10mb' }));
app.get('/health', (_req, res) => res.json({ ok: true }));
app.use('/auth', authRouter);
app.use('/explore', exploreRouter);
app.use('/admin', adminRouter);

app.listen(env.port, () => console.log(`API on ${env.port}`));
process.on('SIGINT', async () => { await prisma.$disconnect(); process.exit(0); });
