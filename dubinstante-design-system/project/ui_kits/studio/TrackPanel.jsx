// TrackPanel — compact vertical card (one per track, laid out horizontally)
function TrackPanel({ idx, track, recording, onUpdate }) {
  const [meter, setMeter] = React.useState(0.15);
  React.useEffect(() => {
    if (!recording) { setMeter(0.12); return; }
    const id = setInterval(() => setMeter(0.25 + Math.random() * 0.65), 90);
    return () => clearInterval(id);
  }, [recording]);

  const css = {
    card: {
      flex: '1 1 0', minWidth: 180,
      background: 'var(--surface)', border: '1px solid var(--border)',
      borderRadius: 10, padding: '10px 12px',
      display: 'flex', flexDirection: 'column', gap: 8,
    },
    head: { display: 'flex', alignItems: 'center', gap: 8 },
    idx: {
      width: 22, height: 22, borderRadius: 6,
      background: 'var(--accent-soft)', color: 'var(--accent-press)',
      display: 'flex', alignItems: 'center', justifyContent: 'center',
      font: '700 11px/1 var(--font-sans)', flexShrink: 0,
    },
    name: { font: '600 12px/1.2 var(--font-sans)', color: 'var(--fg1)', flex: 1, overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' },
    mute: {
      width: 22, height: 22, borderRadius: 5,
      border: '1px solid var(--border-strong)', background: 'var(--surface)',
      cursor: 'pointer', font: '700 10px var(--font-sans)',
      color: track.muted ? 'var(--danger)' : 'var(--fg2)',
      borderColor: track.muted ? 'var(--danger)' : 'var(--border-strong)',
      flexShrink: 0, padding: 0,
    },
    select: {
      height: 26, padding: '0 8px', border: '1px solid var(--border-strong)',
      borderRadius: 6, background: 'var(--surface)', color: 'var(--fg1)',
      font: '500 11px var(--font-sans)', width: '100%',
    },
    bot: { display: 'flex', alignItems: 'center', gap: 8 },
    meterBox: { flex: 1, height: 6, background: 'var(--neutral-100)', borderRadius: 3, overflow: 'hidden' },
    meterFill: (v) => ({
      height: '100%', width: `${Math.max(5, v * 100)}%`,
      background: v > 0.85 ? 'var(--danger)' : (v > 0.7 ? 'var(--warning)' : 'var(--success)'),
      transition: 'width 80ms linear, background 80ms linear',
    }),
    gain: { font: '600 11px/1 var(--font-mono)', color: 'var(--fg2)', fontVariantNumeric: 'tabular-nums', minWidth: 42, textAlign: 'right' },
  };
  return (
    <div style={css.card}>
      <div style={css.head}>
        <div style={css.idx}>{idx + 1}</div>
        <div style={css.name}>{track.name}</div>
        <button style={css.mute} onClick={() => onUpdate({ ...track, muted: !track.muted })} title={track.muted ? 'Unmute' : 'Mute'}>M</button>
      </div>
      <select style={css.select} defaultValue={track.device}>
        <option>{track.device}</option>
        <option>Built-in Microphone</option>
        <option>Focusrite Scarlett 2i2</option>
      </select>
      <div style={css.bot}>
        <div style={css.meterBox}><div style={css.meterFill(meter)} /></div>
        <div style={css.gain}>{track.gain >= 0 ? '+' : ''}{track.gain} dB</div>
      </div>
    </div>
  );
}
Object.assign(window, { TrackPanel });
