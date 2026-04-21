// VideoStage — the dark video area with rythmo overlay
function VideoStage({ positionMs, durationMs, tracks, playing }) {
  const stageCss = {
    wrap: { flex: 1, background: '#000', position: 'relative', overflow: 'hidden', minHeight: 320 },
    // Placeholder "video": subtle gradient + frame counter
    frame: {
      position: 'absolute', inset: 0,
      background: 'radial-gradient(circle at 30% 40%, #1a1a22 0, #000 70%)',
      display: 'flex', alignItems: 'center', justifyContent: 'center',
    },
    // Black letterbox lines to read as 16:9 even at weird aspect ratios
    subject: {
      width: '40%', aspectRatio: '2/3', borderRadius: 8,
      background: 'linear-gradient(180deg,#2a2530 0,#120f1a 100%)',
      boxShadow: '0 20px 60px rgba(124,86,245,0.12)',
      opacity: 0.9,
    },
    hint: {
      position: 'absolute', top: 16, left: 16,
      font: '500 11px/1 var(--font-mono)', color: 'rgba(255,255,255,.5)',
      letterSpacing: '0.04em', textTransform: 'uppercase',
    },
    tcBadge: {
      position: 'absolute', top: 16, right: 16,
      padding: '6px 10px', borderRadius: 6,
      background: 'rgba(0,0,0,0.5)', color: '#fff',
      font: '500 12px/1 var(--font-mono)', letterSpacing: '0.02em',
      fontVariantNumeric: 'tabular-nums',
    },
  };
  return (
    <div style={stageCss.wrap}>
      <div style={stageCss.frame}>
        <div style={stageCss.subject} />
      </div>
      <div style={stageCss.tcBadge}>{fmt(positionMs)} / {fmt(durationMs)}</div>
      <RythmoOverlay positionMs={positionMs} tracks={tracks} playing={playing} />
    </div>
  );
}

function fmt(ms) {
  const s = Math.max(0, Math.floor(ms / 1000));
  const h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60), x = s % 60;
  return `${String(h).padStart(2,'0')}:${String(m).padStart(2,'0')}:${String(x).padStart(2,'0')}`;
}
window.fmt = fmt;
Object.assign(window, { VideoStage });
