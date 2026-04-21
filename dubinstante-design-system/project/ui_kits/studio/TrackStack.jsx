// TrackStack — lower section with track panels + speed controls
// (track count +/− lives in the "Bande Rythmo" menu, not duplicated here)
function TrackStack({ tracks, setTracks, recording, speed, setSpeed, whiteText, setWhiteText }) {
  const updateTrack = (i, t) => setTracks(tracks.map((x, j) => j === i ? t : x));

  const tsCss = {
    wrap: { flexShrink: 0, padding: '10px 16px 12px', background: 'var(--neutral-50)', borderTop: '1px solid var(--border)', display: 'flex', flexDirection: 'column', gap: 8 },
    header: { display: 'flex', alignItems: 'center', gap: 16 },
    title: { font: '600 11px/1 var(--font-sans)', letterSpacing: '0.06em', textTransform: 'uppercase', color: 'var(--fg3)' },
    spacer: { flex: 1 },
    speed: { display: 'flex', alignItems: 'center', gap: 8 },
    speedLabel: { font: '500 11px/1 var(--font-sans)', letterSpacing: '0.04em', textTransform: 'uppercase', color: 'var(--fg3)' },
    speedInput: {
      width: 70, height: 30, padding: '0 8px',
      border: '1px solid var(--border-strong)', borderRadius: 6,
      background: 'var(--surface)', color: 'var(--fg1)',
      font: '600 12px var(--font-mono)', textAlign: 'right', outline: 'none',
    },
    check: { display: 'flex', alignItems: 'center', gap: 8, font: '500 12px/1 var(--font-sans)', color: 'var(--fg1)', cursor: 'pointer' },
    checkBox: {
      appearance: 'none', width: 16, height: 16, border: '1.5px solid var(--border-strong)',
      borderRadius: 4, background: whiteText ? 'var(--accent)' : 'var(--surface)',
      borderColor: whiteText ? 'var(--accent)' : 'var(--border-strong)',
      position: 'relative', cursor: 'pointer',
    },
    list: { display: 'flex', flexDirection: 'row', gap: 8, flexWrap: 'wrap' },
  };
  return (
    <div style={tsCss.wrap}>
      <div style={tsCss.header}>
        <span style={tsCss.title}>Bandes Rythmo · {tracks.length} active{tracks.length > 1 ? 's' : ''}</span>
        <div style={tsCss.spacer} />
        <div style={tsCss.speed}>
          <span style={tsCss.speedLabel}>Vitesse</span>
          <input style={tsCss.speedInput} value={`${speed}%`}
            onChange={(e) => {
              const n = parseInt(e.target.value.replace(/\D/g, ''), 10) || 100;
              setSpeed(Math.max(1, Math.min(400, n)));
            }}
          />
        </div>
        <label style={tsCss.check} onClick={() => setWhiteText(!whiteText)}>
          <span style={tsCss.checkBox}>{whiteText && <span style={{ position: 'absolute', left: 4, top: 1, width: 5, height: 9, border: 'solid #fff', borderWidth: '0 2px 2px 0', transform: 'rotate(45deg)' }} />}</span>
          Texte Blanc
        </label>
      </div>
      <div style={tsCss.list}>
        {tracks.map((t, i) => (
          <TrackPanel key={i} idx={i} track={t} recording={recording}
            onUpdate={(nt) => updateTrack(i, nt)} />
        ))}
      </div>
    </div>
  );
}
Object.assign(window, { TrackStack });
