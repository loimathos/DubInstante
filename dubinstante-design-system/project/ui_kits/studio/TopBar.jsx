// TopBar — menu bar with functional dropdowns
function TopBar({ activeMenu, setActiveMenu, onAction, trackCount = 1, flags = {} }) {
  const barRef = React.useRef(null);
  React.useEffect(() => {
    const onDoc = (e) => { if (barRef.current && !barRef.current.contains(e.target)) setActiveMenu(null); };
    document.addEventListener('mousedown', onDoc);
    return () => document.removeEventListener('mousedown', onDoc);
  }, [setActiveMenu]);

  const css = {
    bar: {
      height: 48, flexShrink: 0, display: 'flex', alignItems: 'center',
      padding: '0 14px', borderBottom: '1px solid var(--border)',
      background: 'var(--surface)', gap: 8, position: 'relative', zIndex: 100,
    },
    brand: { display: 'flex', alignItems: 'center', gap: 10, paddingRight: 14, marginRight: 8, borderRight: '1px solid var(--border)' },
    mark: { width: 26, height: 26, borderRadius: 7, overflow: 'hidden', flexShrink: 0 },
    word: { font: '700 15px/1 var(--font-display)', letterSpacing: '-0.015em', color: 'var(--fg1)' },
    menu: (active) => ({
      padding: '6px 10px', borderRadius: 6, cursor: 'pointer',
      font: '500 13px/1 var(--font-sans)',
      color: active ? 'var(--accent-press)' : 'var(--fg2)',
      background: active ? 'var(--accent-soft)' : 'transparent',
      position: 'relative',
    }),
    dd: {
      position: 'absolute', top: 'calc(100% + 4px)', left: 0,
      minWidth: 260, background: 'var(--surface)',
      border: '1px solid var(--border)', borderRadius: 10,
      boxShadow: 'var(--shadow-lg)', padding: 6,
    },
    ddRight: { right: 0, left: 'auto' },
    item: {
      display: 'flex', alignItems: 'center', justifyContent: 'space-between',
      padding: '8px 10px', borderRadius: 6,
      font: '500 13px/1 var(--font-sans)', color: 'var(--fg1)', cursor: 'pointer', gap: 12,
    },
    sc: { color: 'var(--fg3)', font: '500 11px var(--font-mono)' },
    check: { color: 'var(--accent)', font: '600 12px/1 var(--font-sans)' },
    divider: { height: 1, background: 'var(--border)', margin: '4px 2px' },
    status: {
      display: 'inline-flex', alignItems: 'center', gap: 6,
      padding: '4px 10px', borderRadius: 999,
      background: 'var(--success-bg)', color: 'var(--success)',
      font: '600 10px/1 var(--font-sans)', letterSpacing: '0.06em', textTransform: 'uppercase',
    },
    account: {
      width: 30, height: 30, borderRadius: '50%',
      background: 'linear-gradient(135deg,var(--purple-400),var(--purple-600))',
      color: '#fff', display: 'flex', alignItems: 'center', justifyContent: 'center',
      font: '700 12px/1 var(--font-sans)', cursor: 'pointer', marginLeft: 8,
      border: '2px solid transparent',
    },
    accountActive: { border: '2px solid var(--accent)' },
    counter: { display: 'flex', alignItems: 'center', gap: 6, padding: '6px 8px', background: 'var(--neutral-50)', borderRadius: 6, marginBottom: 4 },
    cBtn: { width: 24, height: 24, borderRadius: 5, border: '1px solid var(--border-strong)', background: 'var(--surface)', cursor: 'pointer', color: 'var(--fg1)', font: '600 14px/1 var(--font-sans)', padding: 0 },
    cLabel: { flex: 1, textAlign: 'center', font: '600 12px/1 var(--font-mono)', color: 'var(--fg1)' },
  };

  const renderItem = (label, opts = {}) => (
    <div style={css.item}
      onMouseEnter={(e) => e.currentTarget.style.background = 'var(--surface-hover)'}
      onMouseLeave={(e) => e.currentTarget.style.background = 'transparent'}
      onClick={() => { opts.action && onAction(opts.action); if (!opts.keepOpen) setActiveMenu(null); }}>
      <span>{label}</span>
      {opts.sc && <span style={css.sc}>{opts.sc}</span>}
      {opts.toggle != null && <span style={opts.toggle ? css.check : css.sc}>{opts.toggle ? '✓' : ' '}</span>}
    </div>
  );

  return (
    <div style={css.bar} ref={barRef}>
      <div style={css.brand}>
        <div style={css.mark}><img src="../../assets/logo.png" style={{ width: '100%', height: '100%', display: 'block' }} alt="" /></div>
        <div style={css.word}>Dub<span style={{ color: 'var(--accent)' }}>Instante</span></div>
      </div>

      {/* Files */}
      <div style={css.menu(activeMenu === 'Files')} onClick={() => setActiveMenu(activeMenu === 'Files' ? null : 'Files')}>
        Files
        {activeMenu === 'Files' && (
          <div style={css.dd} onClick={(e) => e.stopPropagation()}>
            {renderItem('Open MP4', { action: 'open-mp4', sc: '⌘O' })}
            {renderItem('Open save file', { action: 'load-project' })}
            {renderItem('Save .dbi / .zip', { action: 'save-project', sc: '⌘S' })}
          </div>
        )}
      </div>

      {/* Application */}
      <div style={css.menu(activeMenu === 'Application')} onClick={() => setActiveMenu(activeMenu === 'Application' ? null : 'Application')}>
        Application
        {activeMenu === 'Application' && (
          <div style={css.dd} onClick={(e) => e.stopPropagation()}>
            {renderItem('Expert mode', { action: 'expert', toggle: flags.expert, keepOpen: true })}
            {renderItem('Fullscreen mode', { action: 'fullscreen', toggle: flags.fullscreenRec, keepOpen: true })}
            <div style={css.divider} />
            {renderItem('Changer raccourcis', { action: 'shortcuts' })}
            {renderItem('Paramètre global', { action: 'settings' })}
          </div>
        )}
      </div>

      {/* Bande Rythmo */}
      <div style={css.menu(activeMenu === 'Bande Rythmo')} onClick={() => setActiveMenu(activeMenu === 'Bande Rythmo' ? null : 'Bande Rythmo')}>
        Bande Rythmo
        {activeMenu === 'Bande Rythmo' && (
          <div style={css.dd} onClick={(e) => e.stopPropagation()}>
            <div style={css.counter}>
              <button style={css.cBtn} onClick={() => onAction('track-remove')} disabled={trackCount <= 1}>−</button>
              <span style={css.cLabel}>{trackCount} bande{trackCount > 1 ? 's' : ''} rythmo</span>
              <button style={css.cBtn} onClick={() => onAction('track-add')} disabled={trackCount >= 4}>+</button>
            </div>
            <div style={css.divider} />
            {renderItem('Personnaliser…', { action: 'personalize' })}
            {renderItem('Exporter la bande rythmo', { action: 'export-rythmo', toggle: flags.exportRythmo, keepOpen: true })}
          </div>
        )}
      </div>

      <div style={{ flex: 1 }} />
      <span style={css.status}>● Synced</span>

      {/* Account */}
      <div style={{ position: 'relative' }}>
        <div style={{ ...css.account, ...(activeMenu === 'Account' ? css.accountActive : {}) }}
          title="Compte"
          onClick={() => { setActiveMenu(null); onAction('account'); }}>MR</div>
      </div>
    </div>
  );
}
Object.assign(window, { TopBar });
