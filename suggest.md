## 结论

colorcpp 的功能面已经很全：14 种以上色彩空间，按代价自动选择转换路径的 `color_cast`，CSS Color 4/5 解析，ΔE、色域映射、色觉模拟、APCA，还有 7 条 CI 任务覆盖 GCC、Clang、MSVC 和 macOS。下一阶段不建议继续加新功能，应该先把正确性、发布一致性和包装质量收好，然后再投入批量 API 和性能。

我发现了两个应该尽快修的问题：一个是色域映射默认路径的计算错误，另一个是 CMake 把严格的警告选项传给了下游用户。

（这次终端不可用，没跑 `git log`、构建和测试，下面的结论都是直接读代码得出的。）

---

## 一、高优先级：正确性和包装问题

### 1. `gamut_clip` 的默认方法 `css_color4` 把线性 RGB 当成 sRGB 返回

`details::oklab_to_linrgb` 输出的是**线性** sRGB，但结果直接装进了 `rgbaf_t`。`rgbaf_t` 在整个库里表示经过伽马编码的 sRGB（hub 链是 sRGB → linear RGB → XYZ）：

```124:134:include/colorcpp/algorithms/gamut/srgb.hpp
        // ΔE ≈ JND: the clipped version is the answer.
        return color_cast<Color>(core::rgbaf_t{r_c, g_c, b_c, alpha});
      }
    }

    // Bisection converged: clip the final candidate.
    float r, g, b;
    details::oklab_to_linrgb(L, min_C * cos_h, min_C * sin_h, r, g, b);
    return color_cast<Color>(
        core::rgbaf_t{std::clamp(r, 0.0f, 1.0f), std::clamp(g, 0.0f, 1.0f), std::clamp(b, 0.0f, 1.0f), alpha});
```

后果是映射结果会明显偏暗，明度对不上。现有测试只检查 `is_in_srgb_gamut(result)`，结果偏暗照样在色域内，所以测试发现不了。修法是改用 `linear_rgbaf_t` 构造，或者先做 sRGB 编码。还要补一条测试：映射后的 OkLab 明度 L 应该接近输入值。

另外 `chroma_reduce` 分支经过 `oklab_t`（没有 alpha 通道）再构造 `rgbaf_t`，会丢掉原来的 alpha，需要确认是不是有意为之。

### 2. CMake 把 `-Werror` 和 `/WX` 作为 INTERFACE 选项导出

```66:74:CMakeLists.txt
if(MSVC)
  # /utf-8: treat sources as UTF-8 (avoids C4819 on non-ASCII comments/strings under CP936).
  target_compile_options(colorcpp INTERFACE /W4 /WX /utf-8)
else()
  target_compile_options(colorcpp INTERFACE 
    -Wall -Wextra -Wpedantic -Werror
    -Wno-unused-parameter
    -Wno-unused-variable
  )
endif()
```

这些选项会写进安装后的 `colorcppTargets.cmake`。结果是任何 `target_link_libraries(... colorcpp::colorcpp)` 的下游项目，**自己的代码**也会被强制加上 `-Werror -Wpedantic`。对 header-only 库来说这个问题很严重。建议把警告选项挪到 tests/examples 的目标上，或者放进一个内部的 `colorcpp_warnings` 目标。INTERFACE 上最多保留 `/utf-8`。

### 3. 版本号在四个地方互相对不上

| 位置 | 版本 |
|---|---|
| `CMakeLists.txt` | 1.0.3 |
| `CHANGELOG.md` 最新条目 | 1.0.1 |
| `docs/release-notes`、`release-process` | 只有 v0.3.0、v1.0.1 |
| `debian/changelog` | 0.3.0-1 |

需要补上 1.0.2 和 1.0.3 的条目，或者把版本号改回去。可以考虑生成一个 `colorcpp/version.hpp`（由 CMake 的 `configure_file` 生成），并在 CI 里检查这几处是否一致。

### 4. 文档已经落后于代码

- README 的色彩空间列表缺少 Adobe RGB、ProPhoto RGB、Rec.2020，也没提色适应（`chromatic_adaptation.hpp`）。这几项都已经有实现、测试和示例。
- README 的 Examples 和 Benchmarks 列表也不全，缺 harmony、vision、palette、accessibility、compare 等。
- `blend()` 和 CHANGELOG 里都有一句“不应理解为线性 sRGB 合成”的免责说明。这说明合成语义还没定下来，见第三部分。

---

## 二、中优先级：工程质量

1. **CI 覆盖的缺口**：
   - 没有 ASan/UBSan 任务。CSS 解析器、`.cube` 读取都是处理外部输入的代码，最需要这个。
   - 没有 C++20/23 的构建矩阵，但 README 写着支持 C++20 增强特性，`core/concepts.hpp` 也在。
   - 没有覆盖率统计。
   - 两个实验性 SIMD 开关（delta_e_ok、vision machado）从来没在 CI 里打开过。
   - 没有 ARM 平台（macOS runner 是 arm64，但没开 SIMD，而 SIMD 只实现了 SSE2）。
2. **模糊测试**：给 `parse_css_color*` 和 `cube::read_3d` 写 libFuzzer 测试目标，投入小、收益高。
3. **测试质量**：参考第一部分的色域映射问题，很多测试只验证“在合法范围内”，不验证数值。可以引入 [colorjs.io](https://colorjs.io) 或 CSS WPT 的参考向量，做对照测试，尤其是 `color-mix`、相对颜色语法和色域映射。
4. **打包**：`colorcpp.pc` 只在 `COLORCPP_INSTALL_HEADERS=ON` 时才安装，安装默认又是 OFF，和常见做法相反。另外还没上 vcpkg 和 Conan。
5. **编译时间**：图路由加上大量模板，包含 umbrella header 的代价值得测一下（比如 `-ftime-trace`），然后在 README 里推荐按模块包含。

---

## 三、下一阶段功能方向（按价值排序）

1. **批量 / span API（价值最高）**：现在所有接口都是单个颜色。可以加 `color_cast_n(span<const From>, span<To>)`、`blend_n`、`delta_e_n`，并支持 SoA 布局。`simd.rst` 自己也写了 AoS 布局限制了 SIMD 收益，`benchmark_simd_candidates.cpp` 正是为这个方向准备的。同时补上 NEON 后端（现在只有 SSE2）。
2. **把合成语义定下来**：给 `blend()` 加一个 `blend_space` 参数（sRGB 或 linear）。顺便明确预乘 alpha 的约定，这样就能删掉那句免责说明。
3. **完成 CSS Color 5**：README 里写着还没做完的相对颜色语法，即 `hsl(from …)`、`oklch(from …)`、`lab(from …)` 等。再加 `contrast-color()`。另外把格式化输出补完整，做到“解析 → 输出 CSS 字符串”能完全往返。
4. **HDR 和视频色彩空间**：已经有 Rec.2020，自然的下一步是 PQ/HLG 传递函数、ICtCp、JzAzBz、CAM16/HCT。HCT 是 Material You 用的，和现有 `material_*` 调色板可以连起来。
5. **其他适配**：`std::format`/`fmt` 格式化器（C++20），`constexpr` 覆盖面审计，以及一个下游自定义色彩空间的完整示例，用来检验 `COLORCPP_REGISTER_CONVERSION` 的扩展性承诺。

---

## 建议的里程碑

- **v1.0.4（补丁版本）**：修复色域映射的线性/sRGB 混淆，移除 INTERFACE 上的 `-Werror` 和 `/WX`，统一版本号和 CHANGELOG，同步 README。
- **v1.1**：CI 加 sanitizer、模糊测试、C++20 矩阵；引入参考向量对照测试；确定 `blend` 的合成语义；补完 CSS 相对颜色语法。
- **v1.2**：批量 span API、SoA 布局和 NEON；HDR 色彩空间（PQ/HLG/ICtCp）；HCT。

你可以先选一个方向，我再给出具体的设计或改动清单。如果要直接改代码，需要切换到 Agent 模式。