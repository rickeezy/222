import { create } from 'zustand';
import { api } from '../lib/api';

type POI = { id: string; name: string; lat: number; lng: number; message: string };

interface GameState {
  discoveredZones: string[];
  pois: POI[];
  setPosition: (lat: number, lng: number) => void;
  checkIn: (lat: number, lng: number) => Promise<void>;
}

export const useGameStore = create<GameState>((set) => ({
  discoveredZones: [],
  pois: [
    { id: '1', name: 'Founders Library', lat: 38.9232, lng: -77.0198, message: 'You found Founders Library.' },
    { id: '2', name: 'The Yard', lat: 38.9221, lng: -77.0208, message: 'This spot is part of Howard history.' }
  ],
  setPosition: () => undefined,
  checkIn: async (lat, lng) => {
    try {
      await api.post('/explore/checkin', { lat, lng, accuracyM: 30 });
      set({ discoveredZones: ['Founders Core'] });
    } catch {
      // ignore if not logged in
    }
  }
}));
