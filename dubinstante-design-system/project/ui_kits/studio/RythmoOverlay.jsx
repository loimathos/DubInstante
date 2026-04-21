// RythmoOverlay — stack of scrolling Rythmo bands at the bottom of the video
function RythmoOverlay({ positionMs, tracks, playing }) {
  const overlayCss = {
    wrap: {
      position: 'absolute', left: 0, right: 0, bottom: 16,
      display: 'flex', flexDirection: 'column', gap: 2,
    },
    cursor: {
      position: 'absolute', left: '46%', top: 0, bottom: 0,
      width: 2, background: 'var(--rythmo-cursor)',
      boxShadow: '0 0 10px rgba(156,124,255,.7)', pointerEvents: 'none',
      borderRadius: 2,
    },
  };
  return (
    <div style={overlayCss.wrap}>
      {tracks.map((t, i) => (
        <RythmoBand key={i} track={t} positionMs={positionMs} playing={playing}
          rounded={i === 0 ? 'top' : (i === tracks.length - 1 ? 'bottom' : 'none')} />
      ))}
      <div style={overlayCss.cursor} />
    </div>
  );
}

function RythmoBand({ track, positionMs, playing, rounded }) {
  // Simple scroll model: cursor is at 46%, text scrolls left as time advances.
  // charDurationMs defaults to 180ms (speed 100%); adjust by track.speed
  const charMs = 180 * (100 / (track.speed || 100));
  const cursorIdx = Math.max(0, Math.min(track.text.length, Math.floor(positionMs / charMs)));
  const PX_PER_CHAR = 14;
  const offset = -cursorIdx * PX_PER_CHAR;
  const radius = rounded === 'top' ? '10px 10px 0 0'
                : rounded === 'bottom' ? '0 0 10px 10px'
                : rounded === 'full' ? '10px' : '0';
  const bandCss = {
    band: {
      position: 'relative', height: 56, overflow: 'hidden',
      background: track.bg || 'var(--rythmo-bg)',
      borderRadius: radius,
      display: 'flex', alignItems: 'center',
    },
    label: {
      position: 'absolute', left: 10, top: 6,
      font: '600 9px/1 var(--font-sans)', color: 'rgba(255,255,255,0.5)',
      letterSpacing: '0.08em', textTransform: 'uppercase',
    },
    textLane: {
      position: 'relative', width: '100%', height: '100%',
      display: 'flex', alignItems: 'center',
    },
    text: {
      position: 'absolute', left: '46%', whiteSpace: 'nowrap',
      font: `500 22px/1 ${track.font || 'var(--font-sans)'}`,
      color: track.color || '#fff',
      letterSpacing: '0.01em',
      transform: `translateX(${offset}px)`,
      transition: playing ? 'transform 120ms linear' : 'none',
    },
    spoken: { opacity: 0.45, color: 'var(--purple-200)' },
  };
  const spoken = track.text.slice(0, cursorIdx);
  const ahead = track.text.slice(cursorIdx);
  return (
    <div style={bandCss.band}>
      <div style={bandCss.label}>{track.name}</div>
      <div style={bandCss.textLane}>
        <div style={bandCss.text}>
          <span style={bandCss.spoken}>{spoken}</span>
          <span>{ahead}</span>
        </div>
      </div>
    </div>
  );
}
Object.assign(window, { RythmoOverlay, RythmoBand });
