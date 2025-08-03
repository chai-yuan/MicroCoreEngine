const imageToCArrayTool = {
    id: 'img-to-c-array',
    name: '图像转C数组 (含索引/单色模式)',

    // 用于存储当前上传的图像对象
    sourceImage: null,
    // 用于存储对事件监听器的引用
    eventListeners: [],
    // 存储解析后的调色板
    currentPalette: null,

    // 当工具被激活时调用
    init(container) {
        this.cleanup();

        // 1. 创建工具的HTML内容，更新了配置选项
        const content = `
            <div class="tool-content">
                <h2>${this.name}</h2>
                <p>上传图片，配置格式，生成适用于C语言的像素数据数组。</p>

                <div class="tool-section">
                    <h3>1. 上传图片</h3>
                    <input type="file" id="img-uploader" accept="image/png, image/jpeg, image/bmp">
                    <div style="margin-top: 15px; max-width: 100%; overflow: auto;">
                        <canvas id="img-preview" style="display:none; border: 1px solid #7f8c8d;"></canvas>
                    </div>
                </div>

                <div class="tool-section">
                    <h3>2. 配置选项</h3>
                    <div class="config-grid">
                        <div>
                            <label for="var-name">C数组变量名:</label>
                            <input type="text" id="var-name" value="my_sprite_data">
                        </div>
                        <div>
                            <label for="data-format">数据格式:</label>
                            <select id="data-format">
                                <option value="rgb565" selected>RGB565 (16-bit)</option>
                                <option value="rgba4444">RGBA4444 (16-bit)</option>
                                <option value="a8">Alpha (8-bit Grayscale)</option>
                                <option value="indexed4">Indexed (4-bit, 16 Colors)</option>
                                <!-- 新增：1-bit 单色模式 -->
                                <option value="mono1bit">Monochrome (1-bit)</option>
                            </select>
                        </div>
                        <div>
                            <label for="endian-mode">字节/位序 (Endianness):</label>
                            <select id="endian-mode">
                                <option value="little" selected>小端 (LSB First)</option>
                                <option value="big">大端 (MSB First)</option>
                            </select>
                        </div>
                    </div>

                    <!-- 调色板配置区域 (默认隐藏) -->
                    <div id="palette-config" style="display: none; margin-top: 20px; padding-top: 15px; border-top: 1px solid #ccc;">
                        <label for="palette-input">调色板 (16个RGB565 Hex值, 逗号分隔):</label>
                        <textarea id="palette-input" rows="3" style="width: 100%; margin-top: 5px;" placeholder="例如: 0x0000, 0xFFFF, 0xF800, 0x07E0, 0x001F, ... (共16个)"></textarea>
                        <div id="palette-preview" style="margin-top: 10px; display: flex;"></div>
                    </div>
                </div>

                <div class="tool-section">
                    <h3>3. 生成的代码</h3>
                    <button id="copy-btn" style="float: right;">复制到剪贴板</button>
                    <textarea id="c-output" readonly placeholder="上传图片并配置选项后，这里将显示生成的C代码..."></textarea>
                </div>
            </div>
            <style>
                .config-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; }
                .palette-swatch { width: 20px; height: 20px; margin-right: 2px; border: 1px solid #000; }
            </style>
        `;
        container.innerHTML = content;

        // 2. 绑定事件
        this.bindEvents();
        // 初始化调色板区域可见性
        this.togglePaletteVisibility();
    },

    // 集中管理事件绑定
    bindEvents() {
        const uploader = document.getElementById('img-uploader');
        const dataFormatSelect = document.getElementById('data-format');
        const options = [
            document.getElementById('var-name'),
            dataFormatSelect,
            document.getElementById('endian-mode'),
            document.getElementById('palette-input') // 添加调色板输入框
        ];
        const copyBtn = document.getElementById('copy-btn');
        const outputTextArea = document.getElementById('c-output');

        // --- 事件处理函数定义 ---
        const handleUpload = (e) => {
            const file = e.target.files[0];
            if (!file) return;

            const reader = new FileReader();
            reader.onload = (event) => {
                const img = new Image();
                img.onload = () => {
                    this.sourceImage = img;
                    this.generateCode();
                };
                img.src = event.target.result;
            };
            reader.readAsDataURL(file);
        };

        const handleOptionChange = () => {
            this.generateCode();
        };

        const handleFormatChange = () => {
            this.togglePaletteVisibility();
            this.generateCode();
        };

        const handleCopy = () => {
            outputTextArea.select();
            document.execCommand('copy');
            copyBtn.textContent = '已复制!';
            setTimeout(() => { copyBtn.textContent = '复制到剪贴板'; }, 2000);
        };

        // --- 添加事件监听器，并保存引用 ---
        uploader.addEventListener('change', handleUpload);
        this.eventListeners.push({ element: uploader, type: 'change', handler: handleUpload });

        dataFormatSelect.removeEventListener('input', handleOptionChange); // 移除通用处理
        dataFormatSelect.addEventListener('input', handleFormatChange); // 使用特殊处理
        this.eventListeners.push({ element: dataFormatSelect, type: 'input', handler: handleFormatChange });

        options.forEach(opt => {
            if (opt.id === 'data-format') return;
            opt.addEventListener('input', handleOptionChange);
            this.eventListeners.push({ element: opt, type: 'input', handler: handleOptionChange });
        });

        copyBtn.addEventListener('click', handleCopy);
        this.eventListeners.push({ element: copyBtn, type: 'click', handler: handleCopy });
    },

    // 切换调色板输入区域的可见性
    togglePaletteVisibility() {
        const dataFormat = document.getElementById('data-format').value;
        const paletteConfig = document.getElementById('palette-config');
        paletteConfig.style.display = (dataFormat === 'indexed4') ? 'block' : 'none';
    },

    // 辅助函数：将 RGB565 转换为 RGB888 (用于颜色比较)
    rgb565ToRgb888(color565) {
        const r5 = (color565 >> 11) & 0x1F;
        const g6 = (color565 >> 5) & 0x3F;
        const b5 = color565 & 0x1F;
        const r8 = (r5 * 255) / 31;
        const g8 = (g6 * 255) / 63;
        const b8 = (b5 * 255) / 31;
        return { r: Math.round(r8), g: Math.round(g8), b: Math.round(b8) };
    },

    // 辅助函数：解析用户输入的调色板字符串
    parsePalette(paletteString) {
        const hexValues = paletteString.split(',').map(s => s.trim()).filter(s => s.length > 0);
        if (hexValues.length !== 16) return null;

        const palette = [];
        for (const hex of hexValues) {
            const value = parseInt(hex, 16);
            if (isNaN(value) || value < 0 || value > 0xFFFF) return null;
            palette.push({
                rgb565: value,
                rgb888: this.rgb565ToRgb888(value)
            });
        }
        return palette;
    },

    // 辅助函数：查找图像像素在调色板中最接近的颜色的索引
    findClosestColorIndex(r, g, b, palette) {
        let minDistanceSquared = Infinity;
        let closestIndex = 0;

        for (let i = 0; i < palette.length; i++) {
            const pColor = palette[i].rgb888;
            const distanceSquared = Math.pow(r - pColor.r, 2) + Math.pow(g - pColor.g, 2) + Math.pow(b - pColor.b, 2);
            if (distanceSquared < minDistanceSquared) {
                minDistanceSquared = distanceSquared;
                closestIndex = i;
            }
            if (minDistanceSquared === 0) break;
        }
        return closestIndex;
    },

    // 更新调色板预览
    updatePalettePreview(palette) {
        const previewContainer = document.getElementById('palette-preview');
        if (!previewContainer) return;
        previewContainer.innerHTML = '';
        if (!palette) {
            previewContainer.innerHTML = '<span style="color: red;">调色板无效</span>';
            return;
        }
        palette.forEach(color => {
            const swatch = document.createElement('div');
            swatch.className = 'palette-swatch';
            const { r, g, b } = color.rgb888;
            swatch.style.backgroundColor = `rgb(${r}, ${g}, ${b})`;
            swatch.title = `0x${color.rgb565.toString(16).padStart(4, '0')}`;
            previewContainer.appendChild(swatch);
        });
    },

    // 核心函数：处理图像并生成代码
    generateCode() {
        // 获取配置
        const varName = document.getElementById('var-name').value || 'image_data';
        const format = document.getElementById('data-format').value;
        const isLittleEndian = document.getElementById('endian-mode').value === 'little';
        const outputTextArea = document.getElementById('c-output');
        const canvas = document.getElementById('img-preview');
        const ctx = canvas.getContext('2d');

        // --- 调色板处理 ---
        if (format === 'indexed4') {
            const paletteString = document.getElementById('palette-input').value;
            this.currentPalette = this.parsePalette(paletteString);
            this.updatePalettePreview(this.currentPalette);
            if (!this.currentPalette) {
                outputTextArea.value = paletteString.trim() !== ""
                    ? "错误: 调色板输入无效。请确保输入了16个逗号分隔的RGB565 Hex值 (例如 0xFFFF)。"
                    : "请在上方输入框中提供16色调色板。";
                return;
            }
        } else {
            this.currentPalette = null;
        }

        // --- 图像处理 ---
        if (!this.sourceImage) return;

        canvas.width = this.sourceImage.width;
        canvas.height = this.sourceImage.height;
        ctx.drawImage(this.sourceImage, 0, 0);
        canvas.style.display = 'block';

        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
        const rawData = imageData.data;

        const byteArray = [];
        const indices = []; // 用于 indexed4
        const bits = [];    // 用于 mono1bit
        let formatDescription = '';
        const dataType = 'uint8_t';

        // 像素处理循环
        for (let i = 0; i < rawData.length; i += 4) {
            const r = rawData[i];
            const g = rawData[i + 1];
            const b = rawData[i + 2];
            const a = rawData[i + 3];

            switch (format) {
                case 'rgb565': {
                    formatDescription = '16-bit RGB (5-6-5)';
                    const r5 = (r >> 3) & 0x1F;
                    const g6 = (g >> 2) & 0x3F;
                    const b5 = (b >> 3) & 0x1F;
                    const pixelValue = (r5 << 11) | (g6 << 5) | b5;
                    const byte1 = (pixelValue >> 8) & 0xFF; // MSB
                    const byte2 = pixelValue & 0xFF;        // LSB
                    byteArray.push(isLittleEndian ? byte2 : byte1, isLittleEndian ? byte1 : byte2);
                    break;
                }
                case 'rgba4444': {
                    formatDescription = '16-bit RGBA (4-4-4-4)';
                    const r4 = (r >> 4) & 0x0F;
                    const g4 = (g >> 4) & 0x0F;
                    const b4 = (b >> 4) & 0x0F;
                    const a4 = (a >> 4) & 0x0F;
                    const pixelValue = (r4 << 12) | (g4 << 8) | (b4 << 4) | a4;
                    const byte1 = (pixelValue >> 8) & 0xFF; // MSB
                    const byte2 = pixelValue & 0xFF;        // LSB
                    byteArray.push(isLittleEndian ? byte2 : byte1, isLittleEndian ? byte1 : byte2);
                    break;
                }
                case 'a8':
                    formatDescription = '8-bit Alpha (Grayscale)';
                    byteArray.push(a);
                    break;
                case 'indexed4':
                    formatDescription = '4-bit Indexed (16 colors)';
                    const index = this.findClosestColorIndex(r, g, b, this.currentPalette);
                    indices.push(index);
                    break;
                // 新增: 1-bit 单色模式处理
                case 'mono1bit':
                    formatDescription = '1-bit Monochrome (1=white, 0=black)';
                    // 根据亮度计算0或1
                    const luminance = 0.299 * r + 0.587 * g + 0.114 * b;
                    const bit = (luminance >= 128) ? 1 : 0;
                    bits.push(bit);
                    break;
            }
        }

        // --- 数据打包阶段 ---

        // 1-bit 单色模式打包
        if (format === 'mono1bit') {
            for (let i = 0; i < bits.length; i += 8) {
                let packedByte = 0;
                for (let j = 0; j < 8; j++) {
                    if (i + j < bits.length) {
                        const bit = bits[i + j];
                        if (isLittleEndian) { // LSB-first: 第一个像素是 bit 0
                            packedByte |= (bit << j);
                        } else { // MSB-first: 第一个像素是 bit 7
                            packedByte |= (bit << (7 - j));
                        }
                    }
                }
                byteArray.push(packedByte);
            }
        }

        // 4-bit 索引模式打包
        if (format === 'indexed4') {
            for (let i = 0; i < indices.length; i += 2) {
                const index1 = indices[i];
                const index2 = (i + 1 < indices.length) ? indices[i + 1] : 0;
                // 大端 (MSB first): 第一个像素在高4位, 如 0x12
                // 小端 (LSB first): 第一个像素在低4位, 如 0x21
                const packedByte = isLittleEndian ? ((index2 << 4) | index1) : ((index1 << 4) | index2);
                byteArray.push(packedByte);
            }
        }

        // --- 生成C代码字符串 ---
        let c_code = `// Generated by Pixel Game Web Tools\n`;
        c_code += `// Image Dimensions: ${canvas.width} x ${canvas.height}\n`;
        let endiannessDesc = isLittleEndian ? 'Little-Endian' : 'Big-Endian';
        c_code += `// Format: ${formatDescription}, ${endiannessDesc}\n`;

        // 为 1-bit 模式添加额外说明
        if (format === 'mono1bit') {
            c_code += `// Note for 1-bit mode: Endianness controls bit order within a byte.\n`;
            c_code += `//  - Big-Endian (MSB First): 1st pixel is bit 7, ..., 8th pixel is bit 0.\n`;
            c_code += `//  - Little-Endian (LSB First): 1st pixel is bit 0, ..., 8th pixel is bit 7.\n`;
        }
        c_code += `\n`;

        c_code += `#ifndef ${varName.toUpperCase()}_H\n`;
        c_code += `#define ${varName.toUpperCase()}_H\n\n`;
        c_code += `#include <stdint.h>\n\n`;

        if (format === 'indexed4' && this.currentPalette) {
            c_code += `// Palette in RGB565 format\n`;
            c_code += `const uint16_t ${varName}_palette[16] = {\n    `;
            const paletteHex = this.currentPalette.map(c => `0x${c.rgb565.toString(16).padStart(4, '0')}`);
            c_code += paletteHex.join(', ');
            c_code += `\n};\n\n`;
        }

        c_code += `const uint32_t ${varName}_width = ${canvas.width};\n`;
        c_code += `const uint32_t ${varName}_height = ${canvas.height};\n\n`;
        c_code += `// Pixel Data (Total bytes: ${byteArray.length})\n`;
        c_code += `const ${dataType} ${varName}_data[${byteArray.length}] = {\n    `;

        const hexArray = byteArray.map(b => `0x${b.toString(16).padStart(2, '0')}`);
        let lineBytes = [];
        const bytesPerLine = 16;
        for (let i = 0; i < hexArray.length; i++) {
            lineBytes.push(hexArray[i]);
            if ((i + 1) % bytesPerLine === 0 && i < hexArray.length - 1) {
                c_code += lineBytes.join(', ') + ',\n    ';
                lineBytes = [];
            }
        }
        c_code += lineBytes.join(', ');

        c_code += `\n};\n\n`;
        c_code += `#endif // ${varName.toUpperCase()}_H\n`;

        outputTextArea.value = c_code;
    },

    // 清理函数
    cleanup() {
        // console.log(`Cleaning up listeners for ${this.name}`);
        this.eventListeners.forEach(listener => {
            listener.element.removeEventListener(listener.type, listener.handler);
        });
        this.eventListeners = [];
        this.sourceImage = null;
        this.currentPalette = null;
    },

    // 框架会调用这个方法
    destroy() {
        // console.log(`${this.name} is being destroyed.`);
        this.cleanup();
    }
};

// 注册工具 (保持不变)
PixelToolApp.registerTool(imageToCArrayTool);
