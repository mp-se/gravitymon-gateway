# Sphinx configuration for GravityMon Gateway documentation.

project = 'GravityMon Gateway'
copyright = '2021-2026, Magnus Persson'
author = 'Magnus Persson'
release = '0.9.1'

extensions = [
    'sphinx_copybutton',
    'sphinx_design',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'shibuya'
html_static_path = ['_static']
html_css_files = ['custom.css']
html_logo = '_static/gateway-icon.png'
html_favicon = '_static/gateway-icon.png'

