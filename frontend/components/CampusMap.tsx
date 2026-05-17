'use client';
import { MapContainer, Marker, Polygon, TileLayer, Popup } from 'react-leaflet';
import { motion } from 'framer-motion';
import { useEffect } from 'react';
import { useGameStore } from '../store/gameStore';

const center: [number, number] = [38.9227, -77.0196];
const fogPolygon: [number, number][] = [[38.924, -77.022],[38.924, -77.018],[38.921,-77.018],[38.921,-77.022]];

export default function CampusMap() {
  const { discoveredZones, setPosition, checkIn, pois } = useGameStore();
  useEffect(() => { navigator.geolocation?.getCurrentPosition((p) => { setPosition(p.coords.latitude, p.coords.longitude); checkIn(p.coords.latitude, p.coords.longitude); }); }, [checkIn, setPosition]);
  return <motion.div initial={{ opacity: 0 }} animate={{ opacity: 1 }} className="rounded-xl overflow-hidden border border-slate-700">
    <MapContainer center={center} zoom={17} style={{ height: '70vh' }}>
      <TileLayer url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png" />
      {!discoveredZones.includes('Founders Core') && <Polygon positions={fogPolygon} pathOptions={{ color: '#0f172a', fillOpacity: 0.7 }} />}
      {pois.map((poi) => <Marker key={poi.id} position={[poi.lat, poi.lng]}><Popup>{poi.name}: {poi.message}</Popup></Marker>)}
    </MapContainer>
  </motion.div>;
}
