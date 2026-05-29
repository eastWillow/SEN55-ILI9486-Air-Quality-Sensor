## 2024-05-24 - Optimize SPI Overhead in Text Rendering
**Learning:** In embedded graphics systems, pixel-by-pixel rendering is a significant bottleneck. When drawing characters, iterating over each pixel to draw the background color causes a massive amount of redundant SPI/draw calls (e.g. 384 operations for a 16x24 font).
**Action:** Always prefer filling contiguous regions using block/area fill operations (like `LCD_SetArealColor`) first before drawing the foreground details. This reduces the time complexity of the rendering operation from O(W*H) to O(1) + O(foreground pixels), significantly improving display performance.

## $(date +%Y-%m-%d) - O(N^2) loops in basic rendering primitive
**Learning:** The lowest-level rendering primitives (like `GUI_DrawPoint`) often loop over pixels individually, incurring massive overhead on hardware displays (e.g., repeating SPI row/col set commands per pixel). The display drivers implement `LCD_SetArealColor` which is EXCLUSIVE (`Xend`, `Yend`), while legacy algorithms calculate inclusive upper bounds.
**Action:** When replacing per-pixel loops with block fills (`LCD_SetArealColor`), meticulously map inclusive loop boundaries to exclusive API boundaries (adding 1), explicitly cast unsigned coords to signed to prevent underflow wrapping on coordinate 0, and clamp to screen bounds to prevent array/hardware crashes.
