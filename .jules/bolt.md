## 2024-05-24 - Optimize SPI Overhead in Text Rendering
**Learning:** In embedded graphics systems, pixel-by-pixel rendering is a significant bottleneck. When drawing characters, iterating over each pixel to draw the background color causes a massive amount of redundant SPI/draw calls (e.g. 384 operations for a 16x24 font).
**Action:** Always prefer filling contiguous regions using block/area fill operations (like `LCD_SetArealColor`) first before drawing the foreground details. This reduces the time complexity of the rendering operation from O(W*H) to O(1) + O(foreground pixels), significantly improving display performance.
## 2024-05-24 - Grouped drawing via GUI_DrawLine optimization
**Learning:** In character and bitmap rendering logic, drawing point-by-point has significant overhead due to setup calls per pixel. However, contiguous runs of identical pixels (like consecutive 1s in a font bitmap or a grey image line) can be batched together. The `GUI_DrawLine` correctly falls back to `LCD_SetArealColor` for 1D orthogonal lines, which provides an O(1) SPI block transfer optimization similar to the old `GUI_DrawPoint`. Grouping pixels into horizontal line segments significantly improves average render time.
**Action:** Always seek to detect runs of contiguous filled pixels and submit them via a single line or rectangle block-draw operation, converting O(N pixels) commands to O(Segments) commands.
## 2024-05-24 - Static UI element rendering optimization
**Learning:** Drawing static labels within continuous loop functions (like `App_Loop`) unnecessarily consumes SPI bandwidth and CPU cycles, as the content does not change between sensor updates.
**Action:** Move all static UI element rendering (e.g., fixed labels for sensor values) into screen initialization routines (e.g., `DrawMainScreen`). Only update the dynamic values inside the loop.

## 2026-06-26 - Text Memoization Cache to Prevent Redundant SPI Writes
**Learning:** In embedded graphics systems, redrawing text that hasn't changed consumes significant SPI bus time and CPU cycles. Continually re-rendering static or slowly changing sensor values in the main loop leads to a performance bottleneck.
**Action:** Implement a coordinate-based memoization cache (like `TextCache` in `App.cpp`) that stores the last formatted string rendered at a specific `(x, y)` position. Before drawing, format the new string and compare it against the cache; if it matches, return early to skip the redundant draw call.
