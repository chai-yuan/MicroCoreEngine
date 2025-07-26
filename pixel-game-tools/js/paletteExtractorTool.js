const paletteExtractorTool = {
    id: 'palette-extractor',
    name: '16色调色板提取器',

    eventListeners: [],

    init(container) {
        container.innerHTML = `
            <div class="tool-content">
                <h2>${this.name}</h2>
                <p>上传图片，工具将自动提取16色调色板，生成C代码，并预览应用新调色板后的效果。</p>

                <div class="palette-extractor-grid">

                    <!-- 左侧栏：上传和原始预览 -->
                    <div class="palette-sidebar">
                        <div class="tool-section">
                            <h3>1. 上传图片</h3>
                            <input type="file" id="palette-image-input" accept="image/*">
                        </div>

                        <div class="tool-section">
                            <h3>2. 图片预览 (原始)</h3>
                            <div class="image-container">
                                <canvas id="original-image-canvas"></canvas>
                                <span id="image-placeholder" class="placeholder-text">请先上传图片</span>
                            </div>
                        </div>
                    </div>

                    <!-- 右侧栏：调色板、效果预览和代码输出 -->
                    <div class="palette-main">
                        <div class="tool-section">
                            <h3>3. 提取的16色调色板</h3>
                            <p class="small-text">0号颜色固定为纯黑 (0x0000)，通常用作透明色。</p>
                            <div id="palette-display" class="palette-container">
                                <!-- 调色板颜色块将在这里生成 -->
                            </div>
                        </div>

                        <!-- 新增：应用调色板后的预览 -->
                        <div class="tool-section" id="quantized-preview-section" style="display: none;">
                             <h3>4. 应用调色板后预览</h3>
                             <div class="image-container">
                                <canvas id="quantized-image-canvas"></canvas>
                                <span id="quantized-placeholder" class="placeholder-text">正在生成预览...</span>
                             </div>
                        </div>

                        <div class="tool-section">
                            <h3>5. 导出C语言代码 (RGB565)</h3>
                            <button id="palette-copy-button" style="float: right;" disabled>复制到剪贴板</button>
                            <textarea id="palette-export-output" readonly placeholder="上传图片后，这里将显示生成的C代码..."></textarea>
                        </div>
                    </div>
                </div>

                <!-- 工具专属的CSS样式 -->
                <style>
                    .palette-extractor-grid {
                        display: grid;
                        grid-template-columns: 350px 1fr;
                        gap: 20px;
                        margin-top: 20px;
                    }
                    .image-container {
                        margin-top: 15px;
                        border: 1px solid #7f8c8d;
                        min-height: 200px;
                        display: flex;
                        align-items: center;
                        justify-content: center;
                        background: #2c3e50;
                        position: relative;
                    }
                    #original-image-canvas, #quantized-image-canvas {
                        max-width: 100%;
                        max-height: 400px;
                        object-fit: contain;
                        display: none; /* 默认隐藏，加载后显示 */
                    }
                    .placeholder-text {
                        color: #95a5a6;
                    }
                    .small-text {
                        font-size: 0.9em;
                        color: #bdc3c7;
                        margin-top: -10px;
                        margin-bottom: 10px;
                    }
                    .palette-container {
                        display: grid;
                        grid-template-columns: repeat(auto-fill, minmax(30px, 1fr));
                        gap: 5px;
                        margin-top: 10px;
                        border: 1px solid #7f8c8d;
                        padding: 10px;
                        background: #2c3e50;
                    }
                    .color-swatch {
                        width: 100%;
                        padding-bottom: 100%;
                        border: 1px solid #ecf0f1;
                        position: relative;
                        cursor: help;
                    }
                    .color-swatch .tooltip {
                        visibility: hidden;
                        width: 120px;
                        background-color: #34495e;
                        color: #fff;
                        text-align: center;
                        border-radius: 6px;
                        padding: 5px 0;
                        position: absolute;
                        z-index: 1;
                        bottom: 125%;
                        left: 50%;
                        margin-left: -60px;
                        opacity: 0;
                        transition: opacity 0.3s;
                    }
                    .color-swatch:hover .tooltip {
                        visibility: visible;
                        opacity: 1;
                    }
                    #palette-export-output {
                        height: 150px;
                    }
                    @media (max-width: 900px) {
                        .palette-extractor-grid {
                            grid-template-columns: 1fr;
                        }
                    }
                </style>
            </div>
        `;

        this.setupEventListeners();
    },

    setupEventListeners() {
        // ... (这部分代码与之前完全相同)
        const imageInput = document.getElementById('palette-image-input');
        const copyButton = document.getElementById('palette-copy-button');

        const handleImageUpload = (event) => this.processImage(event.target.files[0]);
        const handleCopy = () => this.copyToClipboard();

        imageInput.addEventListener('change', handleImageUpload);
        copyButton.addEventListener('click', handleCopy);

        this.eventListeners.push({ element: imageInput, type: 'change', handler: handleImageUpload });
        this.eventListeners.push({ element: copyButton, type: 'click', handler: handleCopy });
    },

    processImage(file) {
        if (!file) return;

        // 重置UI状态，隐藏上次的结果
        document.getElementById('quantized-preview-section').style.display = 'none';
        document.getElementById('quantized-image-canvas').style.display = 'none';
        document.getElementById('quantized-placeholder').style.display = 'block';


        const reader = new FileReader();
        reader.onload = (e) => {
            const img = new Image();
            img.onload = () => {
                const canvas = document.getElementById('original-image-canvas');
                const ctx = canvas.getContext('2d');

                canvas.width = img.width;
                canvas.height = img.height;
                ctx.drawImage(img, 0, 0);

                document.getElementById('image-placeholder').style.display = 'none';
                canvas.style.display = 'block';

                const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
                const pixels = this.getPixels(imageData);

                // 异步处理，防止UI卡顿
                setTimeout(() => {
                    const k = 15;
                    const colorPalette = this.kmeans(pixels, k);
                    const finalPalette = [{ r: 0, g: 0, b: 0 }, ...colorPalette];

                    this.displayPalette(finalPalette);
                    this.generateCCode(finalPalette);

                    // --- 新增功能调用 ---
                    this.applyPaletteAndPreview(imageData, finalPalette);
                }, 10); // 短暂延迟让UI先响应
            };
            img.src = e.target.result;
        };
        reader.readAsDataURL(file);
    },

    /**
     * 新增功能：应用调色板并显示预览图
     * @param {ImageData} originalImageData - 原始图像数据
     * @param {Array<Object>} palette - 16色调色板
     */
    applyPaletteAndPreview(originalImageData, palette) {
        const previewSection = document.getElementById('quantized-preview-section');
        const previewCanvas = document.getElementById('quantized-image-canvas');
        const previewCtx = previewCanvas.getContext('2d');
        const placeholder = document.getElementById('quantized-placeholder');

        // 显示预览区域
        previewSection.style.display = 'block';

        const { width, height } = originalImageData;
        previewCanvas.width = width;
        previewCanvas.height = height;

        const newImageData = previewCtx.createImageData(width, height);
        const originalData = originalImageData.data;
        const newData = newImageData.data;

        for (let i = 0; i < originalData.length; i += 4) {
            const pixel = {
                r: originalData[i],
                g: originalData[i + 1],
                b: originalData[i + 2]
            };

            // 找到调色板中最接近的颜色
            const closestColor = this.findClosestColor(pixel, palette);

            newData[i]     = closestColor.r;
            newData[i + 1] = closestColor.g;
            newData[i + 2] = closestColor.b;
            newData[i + 3] = 255; // Alpha通道设为不透明
        }

        // 绘制新图像
        previewCtx.putImageData(newImageData, 0, 0);

        // 显示Canvas，隐藏占位符
        placeholder.style.display = 'none';
        previewCanvas.style.display = 'block';
    },

    /**
     * 辅助函数：在调色板中查找最接近的颜色
     * @param {Object} pixel - {r, g, b}
     * @param {Array<Object>} palette - 调色板 [{r, g, b}, ...]
     * @returns {Object} 调色板中最近的颜色 {r, g, b}
     */
    findClosestColor(pixel, palette) {
        let minDistance = Infinity;
        let closestColor = palette[0];

        for (const color of palette) {
            // 使用欧氏距离的平方来比较，可以避免开方运算，提高性能
            const distance = Math.pow(pixel.r - color.r, 2) +
                             Math.pow(pixel.g - color.g, 2) +
                             Math.pow(pixel.b - color.b, 2);

            if (distance < minDistance) {
                minDistance = distance;
                closestColor = color;
            }
        }
        return closestColor;
    },

    getPixels(imageData) {
        const pixels = [];
        const data = imageData.data;
        const sampleRate = 5;
        for (let i = 0; i < data.length; i += 4 * sampleRate) {
            if (data[i + 3] > 128) {
                pixels.push({ r: data[i], g: data[i + 1], b: data[i + 2] });
            }
        }
        return pixels.length > 0 ? pixels : [{r:0, g:0, b:0}]; // 避免空数组
    },

    kmeans(pixels, k) {
        // 1. 随机选择k个初始中心点
        // 打乱数组以获得更随机的初始点
        const shuffled = pixels.sort(() => 0.5 - Math.random());
        let centroids = shuffled.slice(0, k).map(p => ({...p}));

        const maxIterations = 20;
        for (let i = 0; i < maxIterations; i++) {
            const clusters = Array.from({ length: k }, () => []);
            pixels.forEach(pixel => {
                let minDistance = Infinity;
                let closestCentroidIndex = 0;
                centroids.forEach((centroid, index) => {
                    const distance = Math.pow(pixel.r - centroid.r, 2) +
                                     Math.pow(pixel.g - centroid.g, 2) +
                                     Math.pow(pixel.b - centroid.b, 2);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestCentroidIndex = index;
                    }
                });
                clusters[closestCentroidIndex].push(pixel);
            });

            let converged = true;
            const newCentroids = centroids.map((oldCentroid, index) => {
                const cluster = clusters[index];
                if (cluster.length > 0) {
                    const sum = cluster.reduce((acc, p) => ({ r: acc.r + p.r, g: acc.g + p.g, b: acc.b + p.b }), { r: 0, g: 0, b: 0 });
                    const newCentroid = {
                        r: Math.round(sum.r / cluster.length),
                        g: Math.round(sum.g / cluster.length),
                        b: Math.round(sum.b / cluster.length)
                    };
                    // 检查是否收敛
                    if (newCentroid.r !== oldCentroid.r || newCentroid.g !== oldCentroid.g || newCentroid.b !== oldCentroid.b) {
                        converged = false;
                    }
                    return newCentroid;
                } else {
                    return oldCentroid;
                }
            });

            centroids = newCentroids;
            if (converged) break; // 如果中心点不再变化，提前结束
        }

        return centroids.map(c => ({
            r: Math.round(c.r), g: Math.round(c.g), b: Math.round(c.b)
        }));
    },

    rgbToRgb565({ r, g, b }) {
        const r5 = (r >> 3) & 0x1F;
        const g6 = (g >> 2) & 0x3F;
        const b5 = (b >> 3) & 0x1F;
        return (r5 << 11) | (g6 << 5) | b5;
    },

    displayPalette(palette) {
        const paletteContainer = document.getElementById('palette-display');
        paletteContainer.innerHTML = '';

        palette.forEach(({ r, g, b }) => {
            const hexColor = `#${r.toString(16).padStart(2, '0')}${g.toString(16).padStart(2, '0')}${b.toString(16).padStart(2, '0')}`;
            const rgb565 = this.rgbToRgb565({r, g, b});
            const hex565 = `0x${rgb565.toString(16).padStart(4, '0').toUpperCase()}`;

            const swatch = document.createElement('div');
            swatch.className = 'color-swatch';
            swatch.style.backgroundColor = hexColor;
            swatch.innerHTML = `<span class="tooltip">RGB: ${r},${g},${b}<br>RGB565: ${hex565}</span>`;

            paletteContainer.appendChild(swatch);
        });
    },

    generateCCode(palette) {
        const outputTextarea = document.getElementById('palette-export-output');
        const copyButton = document.getElementById('palette-copy-button');

        let code = `// ${this.name}生成的16色调色板\n`;
        code += 'const uint16_t palette[16] = {\n    ';

        const hexCodes = palette.map(color => {
            const rgb565 = this.rgbToRgb565(color);
            return `0x${rgb565.toString(16).padStart(4, '0')}`;
        });

        for (let i = 0; i < hexCodes.length; i++) {
            code += hexCodes[i] + ', ';
            if ((i + 1) % 8 === 0 && i < hexCodes.length - 1) {
                code += '\n    ';
            }
        }

        code = code.trim().slice(0, -1);
        code += '\n};';

        outputTextarea.value = code;
        copyButton.disabled = false;
    },

    copyToClipboard() {
        const outputTextarea = document.getElementById('palette-export-output');
        const copyButton = document.getElementById('palette-copy-button');

        navigator.clipboard.writeText(outputTextarea.value).then(() => {
            const originalText = copyButton.textContent;
            copyButton.textContent = '已复制!';
            copyButton.style.backgroundColor = '#27ae60';
            setTimeout(() => {
                copyButton.textContent = originalText;
                copyButton.style.backgroundColor = '';
            }, 2000);
        }).catch(err => {
            console.error('无法复制到剪贴板: ', err);
            alert('复制失败，请手动复制。');
        });
    },

    destroy() {
        this.eventListeners.forEach(({ element, type, handler }) => {
            if (element) {
                element.removeEventListener(type, handler);
            }
        });
        this.eventListeners = [];
        console.log(`Tool '${this.name}' destroyed and cleaned up.`);
    }
};

// 注册工具
PixelToolApp.registerTool(paletteExtractorTool);
//
