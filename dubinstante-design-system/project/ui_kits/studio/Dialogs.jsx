// Dialogs — Personnaliser (track settings), Account, Shortcuts, Global Settings
// All are floating panels anchored to the viewport center, dismissible by overlay click.

function DialogShell({ title, onClose, children, width = 560 }) {
  const css = {
    overlay: {
      position: 'fixed', inset: 0, background: 'rgba(12,14,28,0.48)',
      display: 'flex', alignItems: 'center', justifyContent: 'center', zIndex: 500,
      backdropFilter: 'blur(2px)',
    },
    box: {
      width, maxWidth: 'calc(100vw - 48px)', maxHeight: 'calc(100vh - 80px)',
      background: 'var(--surface)', borderRadius: 14,
      boxShadow: 'var(--shadow-xl, 0 24px 48px rgba(12,14,28,.28))',
      border: '1px solid var(--border)',
      display: 'flex', flexDirection: 'column', overflow: 'hidden',
    },
    head: {
      padding: '14px 18px', borderBottom: '1px solid var(--border)',
      display: 'flex', alignItems: 'center', gap: 12,
    },
    title: { font: '600 15px/1 var(--font-display)', color: 'var(--fg1)', flex: 1 },
    close: {
      width: 28, height: 28, borderRadius: 6, border: 0, background: 'transparent',
      cursor: 'pointer', color: 'var(--fg3)', font: '400 18px/1 var(--font-sans)', padding: 0,
    },
    body: { padding: 18, overflow: 'auto' },
  };
  return (
    <div style={css.overlay} onClick={onClose}>
      <div style={css.box} onClick={(e) => e.stopPropagation()}>
        <div style={css.head}>
          <div style={css.title}>{title}</div>
          <button style={css.close} onClick={onClose} aria-label="Close">×</button>
        </div>
        <div style={css.body}>{children}</div>
      </div>
    </div>
  );
}

// --- Personnaliser (Track Settings Dialog, from v0.9) ------------------------
function PersonalizeDialog({ tracks, setTracks, onClose }) {
  const [active, setActive] = React.useState(0);
  const PRESETS = {
    Classic: { bg: 'rgba(18,14,40,0.72)', color: '#ffffff',   font: 'Inter Tight' },
    Dark:    { bg: 'rgba(8,8,14,0.88)',   color: '#e8e4ff',   font: 'Inter Tight' },
    Blue:    { bg: 'rgba(12,30,70,0.78)', color: '#cfe0ff',   font: 'Inter Tight' },
    Red:     { bg: 'rgba(70,12,28,0.78)', color: '#ffd6d8',   font: 'Inter Tight' },
    Green:   { bg: 'rgba(12,60,30,0.78)', color: '#c8ffd6',   font: 'Inter Tight' },
    Yellow:  { bg: 'rgba(70,58,12,0.78)', color: '#fff6a8',   font: 'Inter Tight' },
  };
  const FONTS = ['Inter Tight', 'JetBrains Mono', 'Georgia', 'Helvetica', 'Courier New'];
  const t = tracks[active];
  const update = (patch) => setTracks(tracks.map((x, i) => i === active ? { ...x, ...patch } : x));

  const c = {
    row: { display: 'flex', gap: 14, marginBottom: 14 },
    tabs: { display: 'flex', gap: 6, marginBottom: 14, borderBottom: '1px solid var(--border)', paddingBottom: 10 },
    tab: (on) => ({
      padding: '6px 12px', borderRadius: 6, cursor: 'pointer',
      font: '600 12px/1 var(--font-sans)',
      background: on ? 'var(--accent-soft)' : 'transparent',
      color: on ? 'var(--accent-press)' : 'var(--fg2)',
      border: '1px solid ' + (on ? 'var(--accent-soft)' : 'transparent'),
    }),
    label: { font: '600 11px/1 var(--font-sans)', color: 'var(--fg3)', letterSpacing: '.04em', textTransform: 'uppercase', marginBottom: 6, display: 'block' },
    field: { flex: 1 },
    select: { width: '100%', height: 32, padding: '0 10px', border: '1px solid var(--border-strong)', borderRadius: 6, background: 'var(--surface)', color: 'var(--fg1)', font: '500 13px var(--font-sans)' },
    swatches: { display: 'flex', gap: 6, flexWrap: 'wrap' },
    sw: (bg, on) => ({ width: 28, height: 28, borderRadius: 6, background: bg, cursor: 'pointer', border: on ? '2px solid var(--accent)' : '1px solid var(--border)' }),
    presets: { display: 'grid', gridTemplateColumns: 'repeat(3,1fr)', gap: 8 },
    preset: (on) => ({
      padding: '10px 12px', borderRadius: 8, cursor: 'pointer',
      border: '1px solid ' + (on ? 'var(--accent)' : 'var(--border)'),
      background: on ? 'var(--accent-soft)' : 'var(--surface)',
      font: '600 12px var(--font-sans)', color: 'var(--fg1)',
      display: 'flex', alignItems: 'center', gap: 8,
    }),
    preview: {
      marginTop: 4, borderRadius: 10, padding: 20, minHeight: 60,
      display: 'flex', alignItems: 'center', justifyContent: 'center',
      background: t.bg, color: t.color,
      font: `500 20px/1.2 '${t.font || 'Inter Tight'}', system-ui`,
      letterSpacing: '0.01em', overflow: 'hidden',
      position: 'relative', border: '1px solid var(--border)',
    },
    cursor: { position: 'absolute', top: 4, bottom: 4, left: '50%', width: 2, background: 'var(--accent)' },
    slider: { width: '100%' },
    num: { font: '600 12px var(--font-mono)', color: 'var(--fg2)' },
  };
  const TEXT_COLORS = ['#ffffff', '#e8e4ff', '#cfe0ff', '#ffd6d8', '#c8ffd6', '#fff6a8', '#222222'];
  const BG_COLORS = ['rgba(18,14,40,0.72)', 'rgba(8,8,14,0.88)', 'rgba(12,30,70,0.78)', 'rgba(70,12,28,0.78)', 'rgba(12,60,30,0.78)', 'rgba(70,58,12,0.78)'];
  const currentPreset = Object.entries(PRESETS).find(([, v]) => v.bg === t.bg && v.color === t.color)?.[0];

  return (
    <DialogShell title="Personnaliser les bandes rythmo" onClose={onClose} width={640}>
      <div style={c.tabs}>
        {tracks.map((tr, i) =>
          <div key={i} style={c.tab(i === active)} onClick={() => setActive(i)}>{tr.name}</div>
        )}
      </div>

      <div style={c.preview}>
        <span>{(t.text || '').slice(0, 60) || 'Aperçu — texte de la bande'}</span>
        <div style={c.cursor} />
      </div>

      <div style={{ height: 14 }} />

      <div style={c.row}>
        <div style={c.field}>
          <label style={c.label}>Préréglage</label>
          <div style={c.presets}>
            {Object.keys(PRESETS).map(name =>
              <div key={name} style={c.preset(currentPreset === name)} onClick={() => update(PRESETS[name])}>
                <span style={{ width: 14, height: 14, borderRadius: 3, background: PRESETS[name].bg, border: '1px solid var(--border)' }} />
                {name}
              </div>
            )}
          </div>
        </div>
      </div>

      <div style={c.row}>
        <div style={c.field}>
          <label style={c.label}>Police</label>
          <select style={c.select} value={t.font || 'Inter Tight'} onChange={(e) => update({ font: e.target.value })}>
            {FONTS.map(f => <option key={f}>{f}</option>)}
          </select>
        </div>
      </div>

      <div style={c.row}>
        <div style={c.field}>
          <label style={c.label}>Couleur du texte</label>
          <div style={c.swatches}>
            {TEXT_COLORS.map(col =>
              <div key={col} style={c.sw(col, t.color === col)} onClick={() => update({ color: col })} />
            )}
          </div>
        </div>
        <div style={c.field}>
          <label style={c.label}>Fond de la bande</label>
          <div style={c.swatches}>
            {BG_COLORS.map(col =>
              <div key={col} style={c.sw(col, t.bg === col)} onClick={() => update({ bg: col })} />
            )}
          </div>
        </div>
      </div>

      <div style={c.row}>
        <div style={c.field}>
          <label style={c.label}>Taille globale <span style={c.num}>{t.fontSize || 20}px</span></label>
          <input type="range" min="14" max="36" value={t.fontSize || 20} style={c.slider}
            onChange={(e) => update({ fontSize: parseInt(e.target.value, 10) })} />
        </div>
      </div>
    </DialogShell>
  );
}

// --- Account -----------------------------------------------------------------
function AccountDialog({ onClose }) {
  const [tab, setTab] = React.useState('login');
  const c = {
    tabs: { display: 'flex', gap: 2, marginBottom: 16, background: 'var(--neutral-100)', padding: 3, borderRadius: 8 },
    tab: (on) => ({
      flex: 1, padding: '8px 12px', borderRadius: 6, border: 0, cursor: 'pointer',
      background: on ? 'var(--surface)' : 'transparent',
      color: on ? 'var(--fg1)' : 'var(--fg3)',
      font: '600 12px/1 var(--font-sans)',
      boxShadow: on ? '0 1px 2px rgba(0,0,0,.06)' : 'none',
    }),
    field: { marginBottom: 12 },
    label: { display: 'block', font: '600 11px/1 var(--font-sans)', color: 'var(--fg3)', letterSpacing: '.04em', textTransform: 'uppercase', marginBottom: 6 },
    input: { width: '100%', height: 36, padding: '0 12px', border: '1px solid var(--border-strong)', borderRadius: 8, background: 'var(--surface)', color: 'var(--fg1)', font: '500 13px var(--font-sans)', boxSizing: 'border-box' },
    cta: { width: '100%', height: 38, borderRadius: 8, border: 0, background: 'var(--accent)', color: '#fff', font: '600 13px/1 var(--font-sans)', cursor: 'pointer', marginTop: 6 },
    link: { display: 'block', textAlign: 'center', marginTop: 14, font: '500 12px var(--font-sans)', color: 'var(--accent-press)', cursor: 'pointer' },
    hint: { font: '500 11px var(--font-sans)', color: 'var(--fg3)', marginTop: -6, marginBottom: 12 },
  };
  return (
    <DialogShell title="Votre compte DubInstante" onClose={onClose} width={420}>
      <div style={c.tabs}>
        <button style={c.tab(tab === 'login')} onClick={() => setTab('login')}>Se connecter</button>
        <button style={c.tab(tab === 'signup')} onClick={() => setTab('signup')}>Créer un compte</button>
      </div>
      {tab === 'signup' && (
        <div style={c.field}>
          <label style={c.label}>Nom</label>
          <input style={c.input} placeholder="Marie Rouvier" />
        </div>
      )}
      <div style={c.field}>
        <label style={c.label}>Email</label>
        <input style={c.input} type="email" placeholder="marie@studio.fr" />
      </div>
      <div style={c.field}>
        <label style={c.label}>Mot de passe</label>
        <input style={c.input} type="password" placeholder="••••••••" />
      </div>
      {tab === 'login' && <div style={c.hint}>Connexion chiffrée · Vos projets restent locaux.</div>}
      <button style={c.cta} onClick={onClose}>{tab === 'login' ? 'Se connecter' : 'Créer mon compte'}</button>
      <span style={c.link} onClick={() => setTab(tab === 'login' ? 'signup' : 'login')}>
        {tab === 'login' ? 'Pas encore de compte ? Créer un compte' : 'Déjà inscrit ? Se connecter'}
      </span>
    </DialogShell>
  );
}

// --- Shortcuts ---------------------------------------------------------------
function ShortcutsDialog({ onClose }) {
  const items = [
    ['Space', 'Lecture / Pause'],
    ['Ctrl+S', 'Arrêter l\'enregistrement'],
    ['Esc', 'Insérer espace + lecture (ou sortir du plein écran)'],
    ['← / →', 'Image par image'],
    ['Backspace', 'Supprimer caractère avant le curseur'],
    ['Delete', 'Supprimer caractère après le curseur'],
    ['Ctrl+O', 'Ouvrir MP4'],
    ['Ctrl+R', 'Démarrer / arrêter l\'enregistrement'],
  ];
  const c = {
    table: { width: '100%', borderCollapse: 'collapse' },
    row: { borderBottom: '1px solid var(--border)' },
    key: { padding: '10px 8px 10px 0', width: 160 },
    kbd: {
      display: 'inline-block', padding: '3px 8px',
      border: '1px solid var(--border-strong)', borderRadius: 5,
      background: 'var(--neutral-50)', color: 'var(--fg1)',
      font: '600 12px/1 var(--font-mono)',
    },
    label: { padding: '10px 0', font: '500 13px/1.3 var(--font-sans)', color: 'var(--fg1)' },
  };
  return (
    <DialogShell title="Raccourcis clavier" onClose={onClose} width={480}>
      <table style={c.table}>
        <tbody>
          {items.map(([k, l], i) =>
            <tr key={i} style={c.row}>
              <td style={c.key}><span style={c.kbd}>{k}</span></td>
              <td style={c.label}>{l}</td>
            </tr>
          )}
        </tbody>
      </table>
    </DialogShell>
  );
}

// --- Global Settings ---------------------------------------------------------
function SettingsDialog({ onClose }) {
  const [section, setSection] = React.useState('General');
  const sections = ['General', 'Audio', 'Export', 'Apparence'];
  const c = {
    wrap: { display: 'flex', gap: 18, minHeight: 320 },
    nav: { width: 140, flexShrink: 0, borderRight: '1px solid var(--border)', paddingRight: 12 },
    navItem: (on) => ({
      padding: '8px 10px', borderRadius: 6, cursor: 'pointer',
      font: '500 13px/1 var(--font-sans)',
      background: on ? 'var(--accent-soft)' : 'transparent',
      color: on ? 'var(--accent-press)' : 'var(--fg2)',
      marginBottom: 2,
    }),
    content: { flex: 1 },
    field: { marginBottom: 14 },
    label: { display: 'block', font: '600 11px/1 var(--font-sans)', color: 'var(--fg3)', letterSpacing: '.04em', textTransform: 'uppercase', marginBottom: 6 },
    input: { width: '100%', height: 32, padding: '0 10px', border: '1px solid var(--border-strong)', borderRadius: 6, background: 'var(--surface)', color: 'var(--fg1)', font: '500 13px var(--font-sans)', boxSizing: 'border-box' },
    toggleRow: { display: 'flex', alignItems: 'center', justifyContent: 'space-between', padding: '10px 0', borderBottom: '1px solid var(--border)' },
    togLabel: { font: '500 13px var(--font-sans)', color: 'var(--fg1)' },
    togSub: { font: '500 11px var(--font-sans)', color: 'var(--fg3)', marginTop: 2 },
  };
  const Toggle = ({ on }) => (
    <span style={{ width: 32, height: 18, borderRadius: 999, background: on ? 'var(--accent)' : 'var(--neutral-300)', position: 'relative', transition: '.15s' }}>
      <span style={{ position: 'absolute', top: 2, left: on ? 16 : 2, width: 14, height: 14, borderRadius: '50%', background: '#fff', transition: '.15s' }} />
    </span>
  );
  return (
    <DialogShell title="Paramètres globaux" onClose={onClose} width={620}>
      <div style={c.wrap}>
        <div style={c.nav}>
          {sections.map(s => <div key={s} style={c.navItem(section === s)} onClick={() => setSection(s)}>{s}</div>)}
        </div>
        <div style={c.content}>
          {section === 'General' && <>
            <div style={c.field}>
              <label style={c.label}>Langue</label>
              <select style={c.input}><option>Français</option><option>English</option></select>
            </div>
            <div style={c.field}>
              <label style={c.label}>Dossier projets</label>
              <input style={c.input} defaultValue="~/Documents/DubInstante" />
            </div>
            <div style={c.toggleRow}>
              <div><div style={c.togLabel}>Auto-save</div><div style={c.togSub}>Sauvegarde toutes les 5 minutes</div></div>
              <Toggle on={true} />
            </div>
          </>}
          {section === 'Audio' && <>
            <div style={c.field}>
              <label style={c.label}>Fréquence d'échantillonnage</label>
              <select style={c.input}><option>48 kHz</option><option>44.1 kHz</option><option>96 kHz</option></select>
            </div>
            <div style={c.field}>
              <label style={c.label}>Taille de buffer</label>
              <select style={c.input}><option>256</option><option>512</option><option>1024</option></select>
            </div>
            <div style={c.toggleRow}>
              <div><div style={c.togLabel}>Monitoring direct</div><div style={c.togSub}>Entendre le micro en temps réel</div></div>
              <Toggle on={true} />
            </div>
          </>}
          {section === 'Export' && <>
            <div style={c.field}>
              <label style={c.label}>Qualité vidéo (CRF)</label>
              <input style={c.input} defaultValue="18" />
            </div>
            <div style={c.field}>
              <label style={c.label}>Chemin FFmpeg</label>
              <input style={c.input} defaultValue="/usr/local/bin/ffmpeg" />
            </div>
          </>}
          {section === 'Apparence' && <>
            <div style={c.field}>
              <label style={c.label}>Thème</label>
              <select style={c.input}><option>Clair (par défaut)</option><option>Sombre</option><option>Système</option></select>
            </div>
            <div style={c.toggleRow}>
              <div><div style={c.togLabel}>Animations</div><div style={c.togSub}>Transitions fluides dans l'interface</div></div>
              <Toggle on={true} />
            </div>
          </>}
        </div>
      </div>
    </DialogShell>
  );
}

Object.assign(window, { DialogShell, PersonalizeDialog, AccountDialog, ShortcutsDialog, SettingsDialog });
