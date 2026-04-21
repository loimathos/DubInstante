// Studio — shared styles (imports CSS vars, supplies shared module styles)
const studioStyles = {
  app: {
    width: '100%',
    minHeight: '100vh',
    background: 'var(--bg)',
    color: 'var(--fg1)',
    fontFamily: 'var(--font-sans)',
    display: 'flex',
    flexDirection: 'column',
  },
};
window.studioStyles = studioStyles;
