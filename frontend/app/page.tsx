'use client';
import dynamic from 'next/dynamic';
const CampusMap = dynamic(() => import('../components/CampusMap'), { ssr: false });

export default function Home() {
  return <main className="min-h-screen p-4 bg-slate-950 text-white"><h1 className="text-2xl font-bold mb-4">Campus Explorer</h1><CampusMap /></main>;
}
