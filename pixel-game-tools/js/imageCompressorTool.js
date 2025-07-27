const imageCompressorTool = {
    id: 'image-compressor',
    name: '图像压缩工具 (RLE)',

    // 用于在 destroy 方法中移除事件监听器
    compressHandler: null,
    compressButton: null,

    init(container) {
        // --- 1. 构建UI (更新了规则描述) ---
        container.innerHTML = `
            <div class="tool-content">
                <h2>${this.name}</h2>
                <p>输入C语言风格的十六进制数组 (例如: <code>{0xFF, 0x00, 0x00, ...}</code>)</p>
                <p><strong>压缩规则:</strong><br>
                  1. <code>[COUNT, VALUE]</code>: 表示 <code>VALUE</code> 重复了 <code>COUNT</code> 次。<strong>(仅当重复次数 >= 3 时启用)</strong><br>
                  2. <code>[128+LENGTH, V1, V2...]</code>: 表示后面跟着 <code>LENGTH</code> 个不进行压缩的原始字节。<br>
                  <small>注意: 长度为1或2的重复序列会被视为不压缩序列处理，以获得更优的压缩率。</small>
                </p>

                <textarea id="compressor-input" class="code-area" rows="8" placeholder="在此处粘贴原始数据...">{
    // 长度为4的重复序列 (会压缩)
    0xAA, 0xAA, 0xAA, 0xAA,
    // 长度为2的重复序列 (不压缩)
    0xBB, 0xBB,
    // 不重复序列 (不压缩)
    0x11, 0x22, 0x33,
    // 另一个长度为3的重复序列 (会压缩)
    0xCC, 0xCC, 0xCC,
    // 单个字节 (不压缩)
    0xDD
}</textarea>

                <button id="compressor-button" class="tool-button">压缩</button>

                <h3>压缩结果:</h3>
                <textarea id="compressor-output" class="code-area" rows="8" readonly></textarea>
                <p id="compressor-ratio"></p>
            </div>
            <style>
                .tool-content { padding: 15px; }
                .code-area {
                    width: 100%;
                    font-family: monospace;
                    margin-bottom: 10px;
                    box-sizing: border-box; /* 确保 padding 不会影响总宽度 */
                }
                .tool-button {
                    padding: 8px 15px;
                    margin-bottom: 15px;
                    cursor: pointer;
                }
                #compressor-ratio { font-weight: bold; }
            </style>
        `;

        // --- 2. 获取DOM元素 (无变化) ---
        const inputArea = container.querySelector('#compressor-input');
        const outputArea = container.querySelector('#compressor-output');
        const ratioDisplay = container.querySelector('#compressor-ratio');
        this.compressButton = container.querySelector('#compressor-button');

        // --- 3. 核心压缩逻辑 (已重写) ---
        this.compressHandler = () => {
            const inputText = inputArea.value;

            // 解析输入 (无变化)
            let originalData;
            try {
                const hexMatches = inputText.match(/0x[0-9a-fA-F]+/g);
                if (!hexMatches) throw new Error("未找到有效的十六进制数据。");
                originalData = hexMatches.map(hex => parseInt(hex));
            } catch (e) {
                outputArea.value = `输入错误: ${e.message}`;
                ratioDisplay.textContent = '';
                return;
            }

            if (originalData.length === 0) {
                outputArea.value = "{}";
                ratioDisplay.textContent = '原始数据为空。';
                return;
            }

            const compressedData = [];
            let i = 0;
            const n = originalData.length;

            while (i < n) {
                // 查找重复序列 (run)
                let runLength = 1;
                while (i + runLength < n && originalData[i] === originalData[i + runLength] && runLength < 127) {
                    runLength++;
                }

                // **核心决策点：只有长度大于等于3才进行RLE压缩**
                if (runLength >= 3) {
                    // 编码为 [COUNT, VALUE]
                    compressedData.push(runLength);
                    compressedData.push(originalData[i]);
                    i += runLength;
                } else {
                    // **处理不压缩序列 (literal)**
                    // 这段序列包含所有不重复的字节，以及长度为2的重复字节

                    // 扫描直到找到一个长度>=3的重复序列的开头，或者数据结束
                    let literalStart = i;
                    let scanPos = i;
                    while (scanPos < n && (scanPos - literalStart) < 127) {
                        // 向前看，检查从当前位置开始是否存在一个长度>=3的重复
                        if (scanPos + 2 < n &&
                            originalData[scanPos] === originalData[scanPos + 1] &&
                            originalData[scanPos + 1] === originalData[scanPos + 2])
                        {
                            // 找到了一个可压缩的序列，在此之前停止本次不压缩序列
                            break;
                        }
                        scanPos++;
                    }

                    const literalLength = scanPos - literalStart;
                    if (literalLength > 0) {
                        // 编码为 [128 + LENGTH, V1, V2, ...]
                        compressedData.push(128 + literalLength);
                        // 从原始数据中拷贝不压缩的部分
                        compressedData.push(...originalData.slice(literalStart, scanPos));
                        // 更新主指针
                        i = scanPos;
                    }
                }
            }

            // --- 4. 格式化输出 (无变化) ---
            const formatAsHex = (byte) => '0x' + byte.toString(16).padStart(2, '0').toUpperCase();

            let outputString = "{\n    ";
            const hexItems = compressedData.map(formatAsHex);

            for (let j = 0; j < hexItems.length; j++) {
                outputString += hexItems[j];
                if (j < hexItems.length - 1) {
                    outputString += ", ";
                    if ((j + 1) % 12 === 0) {
                        outputString += "\n    ";
                    }
                }
            }
            outputString += "\n};";
            outputArea.value = outputString;

            // --- 5. 计算并显示压缩率 (无变化) ---
            const ratio = (compressedData.length / originalData.length);
            ratioDisplay.textContent = `压缩率: ${(ratio * 100).toFixed(2)}% (原始大小: ${originalData.length}字节, 压缩后: ${compressedData.length}字节)`;
            if(ratio > 1) {
                 ratioDisplay.textContent += " (提示: 数据已膨胀)";
            }
        };

        // --- 6. 绑定事件 (无变化) ---
        this.compressButton.addEventListener('click', this.compressHandler);
    },

    destroy() {
        // --- 7. 清理资源 (无变化) ---
        if (this.compressButton && this.compressHandler) {
            this.compressButton.removeEventListener('click', this.compressHandler);
        }
        this.compressHandler = null;
        this.compressButton = null;
        console.log(`工具 "${this.name}" 已销毁。`);
    }
};

PixelToolApp.registerTool(imageCompressorTool);
