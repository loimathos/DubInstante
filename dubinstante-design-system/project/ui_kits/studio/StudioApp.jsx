// StudioApp — the complete interactive Studio
function StudioApp() {
  const DURATION = 2 * 3600 * 1000 + 11 * 60 * 1000 + 38 * 1000; // 02:11:38
  const [positionMs, setPositionMs] = React.useState(1 * 3600 * 1000 + 24 * 60 * 1000 + 7 * 1000);
  const [playing, setPlaying] = React.useState(false);
  const [recording, setRecording] = React.useState(false);
  const [speed, setSpeed] = React.useState(100);
  const [whiteText, setWhiteText] = React.useState(true);
  const [activeMenu, setActiveMenu] = React.useState(null);
  const [volume, setVolume] = React.useState(80);
  const [toast, setToast] = React.useState(null);
  const [dialog, setDialog] = React.useState(null); // 'personalize' | 'account' | 'shortcuts' | 'settings'
  const [expert, setExpert] = React.useState(false);
  const [fullscreenRec, setFullscreenRec] = React.useState(false);
  const [exportRythmo, setExportRythmo] = React.useState(false);
  const showToast = (t) => { setToast(t); setTimeout(() => setToast(null), 2000); };
  const addTrack = () => {
    if (tracks.length >= 4) return;
    const idx = tracks.length;
    const colors = ['var(--rythmo-bg)', 'rgba(12,30,70,.72)', 'rgba(70,12,28,.72)', 'rgba(12,60,30,.72)'];
    setTracks(t => [...t, { name: `Piste ${idx + 1}`, device: 'Built-in Microphone', gain: 0, muted: false,
      text: 'Nouveau texte à synchroniser…', speed: 100, bg: colors[idx], color: '#fff' }]);
  };
  const removeTrack = () => { if (tracks.length > 1) setTracks(t => t.slice(0, -1)); };
  const handleAction = (a) => {
    if (a === 'personalize') return setDialog('personalize');
    if (a === 'account') return setDialog('account');
    if (a === 'shortcuts') return setDialog('shortcuts');
    if (a === 'settings') return setDialog('settings');
    if (a === 'expert') { setExpert(v => !v); return showToast('Expert mode ' + (!expert ? 'activé' : 'désactivé')); }
    if (a === 'fullscreen') { setFullscreenRec(v => !v); return showToast('Plein écran enregistrement ' + (!fullscreenRec ? 'activé' : 'désactivé')); }
    if (a === 'export-rythmo') { setExportRythmo(v => !v); return showToast('Export bande rythmo ' + (!exportRythmo ? 'activé' : 'désactivé')); }
    if (a === 'track-add') return addTrack();
    if (a === 'track-remove') return removeTrack();
    const msgs = { 'open-mp4': 'Ouvrir MP4…', 'load-project': 'Charger un projet…', 'save-project': 'Sauvegarder le projet…' };
    showToast(msgs[a] || a);
  };
  const [tracks, setTracks] = React.useState([
    {
      name: 'Piste 1', device: 'Shure SM7B (USB)', gain: 6, muted: false,
      text: '— Tu crois vraiment qu\'il va venir ? Oui, il me l\'a promis hier soir avant de partir. Mais bon, tu sais comment il est, toujours en retard, toujours à se perdre en chemin.',
      speed: 100, bg: 'var(--rythmo-bg)', color: '#fff',
    },
    {
      name: 'Piste 2', device: 'Built-in Microphone', gain: 0, muted: false,
      text: '(off) Elle regarde par la fenêtre, visage tendu. Un camion passe au loin. Le soleil tombe derrière les immeubles.',
      speed: 100, bg: 'rgba(18,14,40,0.72)', color: 'var(--purple-200)',
    },
  ]);

  // simple play loop
  React.useEffect(() => {
    if (!playing) return;
    let raf; let last = performance.now();
    const tick = (t) => {
      setPositionMs((p) => Math.min(DURATION, p + (t - last)));
      last = t;
      raf = requestAnimationFrame(tick);
    };
    raf = requestAnimationFrame(tick);
    return () => cancelAnimationFrame(raf);
  }, [playing]);

  // Space plays/pauses
  React.useEffect(() => {
    const onKey = (e) => {
      if (e.target.tagName === 'INPUT' || e.target.tagName === 'SELECT') return;
      if (e.code === 'Space') { e.preventDefault(); setPlaying(p => !p); }
      if (e.code === 'KeyR' && e.ctrlKey) { e.preventDefault(); setRecording(r => !r); }
    };
    window.addEventListener('keydown', onKey);
    return () => window.removeEventListener('keydown', onKey);
  }, []);

  const saCss = {
    root: { width: '100%', height: '100vh', display: 'flex', flexDirection: 'column', background: 'var(--bg)', overflow: 'hidden' },
    center: { flex: 1, display: 'flex', flexDirection: 'column', minHeight: 0 },
    statusBar: {
      height: 26, flexShrink: 0,
      display: 'flex', alignItems: 'center', gap: 14, padding: '0 14px',
      background: 'var(--surface)', borderTop: '1px solid var(--border)',
      font: '500 11px/1 var(--font-mono)', color: 'var(--fg3)',
      fontVariantNumeric: 'tabular-nums',
    },
    statusItem: { display: 'inline-flex', alignItems: 'center', gap: 6 },
    statusDot: (color) => ({ width: 6, height: 6, borderRadius: 3, background: color }),
  };

  return (
    <div style={saCss.root} data-screen-label="Studio">
      <TopBar activeMenu={activeMenu} setActiveMenu={setActiveMenu} onAction={handleAction}
        trackCount={tracks.length}
        flags={{ expert, fullscreenRec, exportRythmo }} />
      {toast && <div style={{ position: 'fixed', bottom: 40, left: '50%', transform: 'translateX(-50%)', background: 'var(--fg1)', color: '#fff', padding: '10px 16px', borderRadius: 8, font: '500 13px var(--font-sans)', boxShadow: 'var(--shadow-lg)', zIndex: 200 }}>{toast}</div>}
      {dialog === 'personalize' && <PersonalizeDialog tracks={tracks} setTracks={setTracks} onClose={() => setDialog(null)} />}
      {dialog === 'account' && <AccountDialog onClose={() => setDialog(null)} />}
      {dialog === 'shortcuts' && <ShortcutsDialog onClose={() => setDialog(null)} />}
      {dialog === 'settings' && <SettingsDialog onClose={() => setDialog(null)} />}
      <div style={saCss.center}>
        <VideoStage positionMs={positionMs} durationMs={DURATION} tracks={tracks} playing={playing} />
        <TransportBar
          positionMs={positionMs} durationMs={DURATION}
          playing={playing} recording={recording} volume={volume}
          onPlayPause={() => setPlaying(p => !p)}
          onStop={() => { setPlaying(false); if (recording) setRecording(false); }}
          onSeek={(ms) => setPositionMs(ms)}
          onToggleRec={() => { setRecording(r => !r); setPlaying(true); }}
          onVolume={setVolume}
        />
        <TrackStack
          tracks={tracks} setTracks={setTracks}
          recording={recording} speed={speed} setSpeed={setSpeed}
          whiteText={whiteText} setWhiteText={setWhiteText}
        />
      </div>
      <div style={saCss.statusBar}>
        <span style={saCss.statusItem}><span style={saCss.statusDot(recording ? 'var(--danger)' : 'var(--success)')}></span>
          {recording ? 'Recording' : (playing ? 'Playing' : 'Ready')}</span>
        <span style={{ flex: 1 }} />
        <span style={saCss.statusItem}><kbd>Space</kbd> Play/Pause</span>
        <span style={saCss.statusItem}><kbd>Ctrl</kbd>+<kbd>S</kbd> Stop REC</span>
      </div>
    </div>
  );
}
Object.assign(window, { StudioApp });
