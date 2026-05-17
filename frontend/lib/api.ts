const base = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8000';
export const api = {
  post: async (path: string, body: unknown) => {
    const token = typeof window !== 'undefined' ? localStorage.getItem('token') : null;
    const res = await fetch(`${base}${path}`, { method: 'POST', headers: { 'Content-Type': 'application/json', ...(token ? { Authorization: `Bearer ${token}` } : {}) }, body: JSON.stringify(body) });
    if (!res.ok) throw new Error('Request failed');
    return res.json();
  }
};
