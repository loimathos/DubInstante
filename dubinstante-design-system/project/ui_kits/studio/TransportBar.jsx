// TransportBar — playback controls below the video (now with video volume)
function TransportBar({ positionMs, durationMs, playing, recording, volume,
                       onPlayPause, onStop, onSeek, onToggleRec, onVolume }) {
  const pct = durationMs ? (positionMs / durationMs) * 100 : 0;
  const muted = volume === 0;
  const tbCss = {
    wrap: {
      flexShrink: 0, padding: '10px 16px', background: 'var(--surface)',
      borderTop: '1px solid var(--border)', display: 'flex', alignItems: 'center', gap: 12,
    },
    iconBtn: {
      width: 36, height: 36, borderRadius: 8,
      border: '1px solid var(--border-strong)', background: 'var(--surface)',
      display: 'flex', alignItems: 'center', justifyContent: 'center',
      cursor: 'pointer', color: 'var(--fg1)', padding: 0,
    },
    playBtn: {
      width: 40, height: 40, borderRadius: 10,
      background: 'var(--fg1)', border: 0, color: '#fff',
      display: 'flex', alignItems: 'center', justifyContent: 'center', cursor: 'pointer',
    },
    tc: { font: '500 14px/1 var(--font-mono)', color: 'var(--fg1)', fontVariantNumeric: 'tabular-nums', minWidth: 170 },
    tcDim: { color: 'var(--fg3)' },
    scrub: { flex: 1, height: 6, background: 'var(--neutral-200)', borderRadius: 3, position: 'relative', cursor: 'pointer' },
    scrubFill: { position: 'absolute', inset: 0, width: `${pct}%`, background: 'var(--accent)', borderRadius: 3 },
    scrubHandle: {
      position: 'absolute', left: `${pct}%`, top: '50%',
      width: 14, height: 14, borderRadius: '50%', background: 'var(--accent)',
      transform: 'translate(-50%,-50%)', boxShadow: '0 2px 6px rgba(124,86,245,.4)',
    },
    volGroup: { display: 'flex', alignItems: 'center', gap: 8, paddingLeft: 8, paddingRight: 8, borderLeft: '1px solid var(--border)', borderRight: '1px solid var(--border)' },
    volBtn: {
      width: 28, height: 28, borderRadius: 6, border: 0, background: 'transparent',
      display: 'flex', alignItems: 'center', justifyContent: 'center', cursor: 'pointer',
      color: muted ? 'var(--danger)' : 'var(--fg2)', padding: 0,
    },
    volSlider: { width: 90, height: 4, background: 'var(--neutral-200)', borderRadius: 2, position: 'relative', cursor: 'pointer' },
    volFill: { position: 'absolute', inset: 0, width: `${volume}%`, background: 'var(--fg2)', borderRadius: 2 },
    volHandle: { position: 'absolute', left: `${volume}%`, top: '50%', width: 10, height: 10, borderRadius: '50%', background: 'var(--fg1)', transform: 'translate(-50%,-50%)' },
    volNum: { font: '600 11px/1 var(--font-mono)', color: 'var(--fg2)', minWidth: 36, textAlign: 'right', fontVariantNumeric: 'tabular-nums' },
    recBtn: (active) => ({
      padding: '0 16px', height: 36, border: 0, borderRadius: 8,
      background: 'var(--danger)', color: '#fff',
      font: '700 12px/1 var(--font-sans)', letterSpacing: '0.08em', cursor: 'pointer',
      display: 'inline-flex', alignItems: 'center', gap: 8,
      boxShadow: active ? '0 0 0 4px rgba(226,52,77,.2)' : 'none',
      animation: active ? 'recpulse 1.4s infinite' : 'none',
    }),
    dot: { width: 9, height: 9, borderRadius: '50%', background: '#fff' },
  };
  const handleScrubClick = (e) => {
    const r = e.currentTarget.getBoundingClientRect();
    const p = (e.clientX - r.left) / r.width;
    onSeek(Math.max(0, Math.min(1, p)) * durationMs);
  };
  const handleVolClick = (e) => {
    const r = e.currentTarget.getBoundingClientRect();
    const p = (e.clientX - r.left) / r.width;
    onVolume(Math.round(Math.max(0, Math.min(1, p)) * 100));
  };
  const volIcon = muted
    ? <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor"><path d="M3 10v4h4l5 5V5L7 10H3zm13.5 2a4.5 4.5 0 00-2.5-4v2.17l2.45 2.45c.03-.2.05-.41.05-.62zM14 4.44v2.06a6.98 6.98 0 014.95 9.94l1.45 1.45A9 9 0 0014 4.44zM4.27 3L3 4.27 7.73 9H3v6h4l5 5v-6.73l4.25 4.25a7 7 0 01-2.25.98v2.06a9 9 0 003.69-1.58L19.73 21 21 19.73 12 10.73 4.27 3z"/></svg>
    : volume < 50
      ? <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor"><path d="M7 9v6h4l5 5V4l-5 5H7z"/></svg>
      : <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor"><path d="M3 10v4h4l5 5V5L7 10H3zm13.5 2A4.5 4.5 0 0014 8.03v7.94A4.5 4.5 0 0016.5 12zM14 3.23v2.06a7 7 0 010 13.42v2.06a9 9 0 000-17.54z"/></svg>;

  return (
    <div style={tbCss.wrap}>
      <style>{`@keyframes recpulse{50%{box-shadow:0 0 0 8px rgba(226,52,77,0)}}`}</style>
      <button style={tbCss.playBtn} onClick={onPlayPause} title="Play/Pause (Space)">
        {playing
          ? <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor"><rect x="6" y="5" width="4" height="14" rx="1"/><rect x="14" y="5" width="4" height="14" rx="1"/></svg>
          : <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor"><path d="M8 5v14l11-7z"/></svg>}
      </button>
      <button style={tbCss.iconBtn} onClick={onStop} title="Stop">
        <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor"><rect x="6" y="6" width="12" height="12" rx="1"/></svg>
      </button>
      <div style={tbCss.tc}>{fmt(positionMs)} <span style={tbCss.tcDim}>/ {fmt(durationMs)}</span></div>
      <div style={tbCss.scrub} onClick={handleScrubClick}>
        <div style={tbCss.scrubFill} />
        <div style={tbCss.scrubHandle} />
      </div>
      <div style={tbCss.volGroup}>
        <button style={tbCss.volBtn} onClick={() => onVolume(muted ? 100 : 0)} title="Mute">{volIcon}</button>
        <div style={tbCss.volSlider} onClick={handleVolClick}>
          <div style={tbCss.volFill} />
          <div style={tbCss.volHandle} />
        </div>
        <div style={tbCss.volNum}>{volume}%</div>
      </div>
      <button style={tbCss.recBtn(recording)} onClick={onToggleRec}>
        <span style={tbCss.dot} />
        {recording ? 'STOP' : 'REC'}
      </button>
    </div>
  );
}
Object.assign(window, { TransportBar });
