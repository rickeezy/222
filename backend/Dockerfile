FROM node:20-alpine
WORKDIR /app
COPY package.json tsconfig.json ./
RUN npm install
COPY prisma ./prisma
RUN npx prisma generate
COPY src ./src
EXPOSE 8000
CMD ["sh", "-c", "npx prisma migrate deploy && npm run seed && npm run dev"]
