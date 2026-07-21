Vendored KaTeX assets (v0.16.22) for self-contained client-side math rendering.

Contents: katex.min.css and fonts/*.woff2 from the KaTeX v0.16.22 distribution
(https://github.com/KaTeX/KaTeX/releases/tag/v0.16.22). Only the woff2 fonts are
kept; browsers pick woff2 first, so the missing woff/ttf are never requested.
The katex.min.js and auto-render.min.js are provided by the sphinxcontrib-katex
package (matching version), not vendored here.

KaTeX is distributed under the MIT License, Copyright (c) 2013-2020 Khan Academy
and other contributors. See https://github.com/KaTeX/KaTeX/blob/main/LICENSE.
