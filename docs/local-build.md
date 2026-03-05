# Documentation Build (MkDocs)

## Local setup

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r docs/requirements.txt
```

## Local preview

```bash
mkdocs serve
```

## Production build

```bash
mkdocs build --strict
```

The generated site is written to `site/` and should only be published from CI to `gh-pages`.
