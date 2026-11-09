# GTAngel v15 RE3-Informed Model Boundary

`re3all.zip` contains RE3 source, shaders, configuration, and selected TXD interface assets, but **no DFF mesh files** and no populated `librw` submodule. The native GTAngel executable therefore does not embed or redistribute GTA geometry or textures.

GTAngel v15 uses an original, self-contained model graph whose organization is informed by RE3/RenderWare concepts:

| Concept | GTAngel v15 implementation |
|---|---|
| Clump | One articulated avatar model graph |
| Frame hierarchy | 17 named frame nodes bound to the existing `AvatarSkeleton` joints |
| Atomics | 15 tapered procedural body segments with independent material values |
| Geometry | Original triangles emitted into the 768×768 colour/depth/object-ID rasterizer |
| Materials | Original GTAngel palette; no TXD textures embedded |
| Skinning | The existing live 17-joint pose drives the frame graph each render |

The fallback is intentionally distributable and dependency-free. A future user-owned DFF/TXD converter may populate the same frame/atomic interface at runtime, but proprietary assets must stay outside the executable and outside Arc Angel Echo storage.
