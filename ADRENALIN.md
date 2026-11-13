# Wine Adrenalin Edition

This repository is the authoritative Wine source used by GooseHUB on Apple Silicon. It retains the local native ARM64/ARM64EC, MSync, FEX, OpenGL batching, installer and peripheral compatibility changes. Reviewed upstream fixes are imported individually.

Keep this checkout beside `GooseHUB`, `fex-adrenalin-edition` and `dxmt-adrenalin-edition`. GooseHUB's first-launch installer fetches the pinned revision, builds out of tree in `~/Library/Caches/GooseHUB/Build/Wine`, and installs the resulting runtime in the app. It does not make another copy of this source tree.

The installer and source lock live in the separate GooseHUB repository. This repository carries source code; generated objects, runtime libraries and app bundles are excluded.

Wine is licensed under the LGPL. See `COPYING.LIB` and the source file notices.
