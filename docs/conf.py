# Sphinx configuration for colorcpp narrative docs (docs/).
# https://www.sphinx-doc.org/en/master/usage/configuration.html

project = "colorcpp"
copyright = "colorcpp contributors"
author = "colorcpp contributors"
release = "0.1.0"

extensions: list[str] = []

exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

html_theme = "alabaster"
html_static_path: list[str] = []

# docs/index.rst is the master document (Sphinx default).
