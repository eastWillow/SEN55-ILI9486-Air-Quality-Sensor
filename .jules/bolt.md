## 2024-05-24 - Optimize SPI Overhead in Text Rendering
**Learning:** In embedded graphics systems, pixel-by-pixel rendering is a significant bottleneck. When drawing characters, iterating over each pixel to draw the background color causes a massive amount of redundant SPI/draw calls (e.g. 384 operations for a 16x24 font).
**Action:** Always prefer filling contiguous regions using block/area fill operations (like `LCD_SetArealColor`) first before drawing the foreground details. This reduces the time complexity of the rendering operation from O(W*H) to O(1) + O(foreground pixels), significantly improving display performance.
## 2024-05-24 - Optimize Orthogonal Line Rendering
**Learning:** In embedded graphics systems, pixel-by-pixel rendering is a significant bottleneck. When drawing orthogonal lines (horizontal or vertical), iterating over each pixel to draw causes O(N) SPI/draw calls.
**Action:** Always prefer filling contiguous regions using block/area fill operations (like `LCD_SetArealColor`) for lines parallel to axes. This reduces the time complexity of the rendering operation from O(N) to O(1), significantly improving display performance.
