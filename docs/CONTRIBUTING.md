# Docs Contributing Guide

## Page template

Each page should include:

1. Purpose
2. When to use
3. Minimal examples
4. Pitfalls / failure modes

## Writing rules

- Prefer short, direct paragraphs.
- Keep one canonical page per API topic.
- Avoid duplicate function declarations across pages.
- Use fenced code blocks with language tags.

## Link policy

- Use absolute URLs for external links.
- Use relative links for internal docs links.
- Prefer canonical docs URL `https://chipsncode.com/SPIMemory/` when sharing.

## Validation

Run before submitting:

```bash
mkdocs build --strict
```
