import dotenv from 'dotenv';
dotenv.config();

export const env = {
  port: Number(process.env.PORT || 8000),
  dbUrl: process.env.DATABASE_URL || '',
  jwtSecret: process.env.JWT_SECRET || 'dev-secret',
  frontendUrl: process.env.FRONTEND_URL || 'http://localhost:3000',
  redisUrl: process.env.REDIS_URL || 'redis://redis:6379',
  studentDomain: process.env.STUDENT_EMAIL_DOMAIN || ''
};
