import { Router } from 'express';
import bcrypt from 'bcryptjs';
import jwt from 'jsonwebtoken';
import { body, validationResult } from 'express-validator';
import { prisma } from '../utils/db';
import { env } from '../config/env';

export const authRouter = Router();
authRouter.post('/register', body('email').isEmail(), body('password').isLength({ min: 8 }), async (req, res) => {
  const err = validationResult(req);
  if (!err.isEmpty()) return res.status(400).json({ errors: err.array() });
  const { name, email, password, universityId } = req.body;
  if (env.studentDomain && !email.endsWith(`@${env.studentDomain}`)) return res.status(400).json({ error: 'Must use student email' });
  const passwordHash = await bcrypt.hash(password, 10);
  const user = await prisma.user.create({ data: { name, email, passwordHash, universityId } });
  res.json({ id: user.id });
});
authRouter.post('/login', async (req, res) => {
  const { email, password } = req.body;
  const user = await prisma.user.findUnique({ where: { email } });
  if (!user || !(await bcrypt.compare(password, user.passwordHash))) return res.status(401).json({ error: 'Invalid credentials' });
  const token = jwt.sign({ id: user.id, role: user.role, universityId: user.universityId }, env.jwtSecret, { expiresIn: '7d' });
  res.json({ token });
});
