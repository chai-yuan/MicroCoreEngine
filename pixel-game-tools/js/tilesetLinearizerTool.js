const tilesetLinearizerTool = {
    id: 'tileset-linearizer',
    name: '瓦片地图集线性化工具',

    // 将属性保存在this上，方便在init和destroy中访问
    container: null,
    sourceImage: null,
    fileInput: null,
    widthInput: null,
    heightInput: null,
    processButton: null,
    outputContainer: null,
    downloadLink: null,

    // 将事件处理函数绑定到 'this'，以便可以正确地添加和移除
    _handleFileSelect: null,
    _processImage: null,

    init(container) {
        this.container = container;

        // --- 1. 构建UI ---
        this.container.innerHTML = `
            <style>
                .tool-content {
                    display: flex;
                    flex-direction: column;
                    gap: 15px;
                }
                .tool-controls {
                    display: flex;
                    flex-wrap: wrap;
                    align-items: center;
                    gap: 10px;
                    padding-bottom: 15px;
                    border-bottom: 1px solid #ccc;
                }
                .tool-controls label {
                    margin-right: 5px;
                }
                .tool-controls input[type="number"] {
                    width: 60px;
                }
                #output-container canvas {
                    border: 1px dashed #999;
                    background-color: #f0f0f0;
                    image-rendering: pixelated; /* 保持像素风格 */
                    image-rendering: -moz-crisp-edges;
                    image-rendering: crisp-edges;
                }
                #download-link {
                    display: none; /* 初始隐藏 */
                    margin-top: 10px;
                    font-weight: bold;
                }
                #image-preview {
                    max-width: 100%;
                    max-height: 200px;
                    border: 1px solid #ddd;
                    margin-top: 10px;
                }
            </style>
            <div class="tool-content">
                <h2>${this.name}</h2>
                <p>上传一个瓦片地图集，输入单个瓦片的尺寸，工具会将其转换为一个垂直的长条图。</p>

                <div class="tool-controls">
                    <input type="file" id="tileset-upload" accept="image/*">
                    <label for="tile-width">瓦片宽度:</label>
                    <input type="number" id="tile-width" value="16" min="1">
                    <label for="tile-height">瓦片高度:</label>
                    <input type="number" id="tile-height" value="16" min="1">
                    <button id="process-button">生成线性图</button>
                </div>

                <div id="preview-area"></div>

                <div id="output-container">
                    <p><em>结果将显示在这里...</em></p>
                </div>

                <a id="download-link" download="linear_tileset.png">下载结果</a>
            </div>
        `;

        // --- 2. 获取DOM元素引用 ---
        this.fileInput = this.container.querySelector('#tileset-upload');
        this.widthInput = this.container.querySelector('#tile-width');
        this.heightInput = this.container.querySelector('#tile-height');
        this.processButton = this.container.querySelector('#process-button');
        this.outputContainer = this.container.querySelector('#output-container');
        this.downloadLink = this.container.querySelector('#download-link');
        this.previewArea = this.container.querySelector('#preview-area');

        // --- 3. 绑定事件处理函数 ---
        // 使用 bind(this) 来确保函数内的 'this' 指向工具对象
        this._handleFileSelect = this.handleFileSelect.bind(this);
        this._processImage = this.processImage.bind(this);

        this.fileInput.addEventListener('change', this._handleFileSelect);
        this.processButton.addEventListener('click', this._processImage);
    },

    handleFileSelect(event) {
        const file = event.target.files[0];
        if (!file) {
            this.sourceImage = null;
            this.previewArea.innerHTML = '';
            return;
        }

        const reader = new FileReader();
        reader.onload = (e) => {
            const img = new Image();
            img.onload = () => {
                this.sourceImage = img;
                // 显示预览图
                this.previewArea.innerHTML = '<h4>原图预览:</h4>';
                const previewImg = new Image();
                previewImg.src = img.src;
                previewImg.id = 'image-preview';
                this.previewArea.appendChild(previewImg);
            };
            img.src = e.target.result;
        };
        reader.readAsDataURL(file);
    },

    processImage() {
        if (!this.sourceImage) {
            alert('请先上传一张图片！');
            return;
        }

        const tileWidth = parseInt(this.widthInput.value, 10);
        const tileHeight = parseInt(this.heightInput.value, 10);

        if (isNaN(tileWidth) || isNaN(tileHeight) || tileWidth <= 0 || tileHeight <= 0) {
            alert('请输入有效的瓦片宽度和高度！');
            return;
        }

        const sourceWidth = this.sourceImage.width;
        const sourceHeight = this.sourceImage.height;

        if (sourceWidth % tileWidth !== 0 || sourceHeight % tileHeight !== 0) {
            if (!confirm('警告：图片尺寸不能被瓦片尺寸整除，可能会导致部分图像被裁切。是否继续？')) {
                return;
            }
        }

        // 计算行列数
        const cols = Math.floor(sourceWidth / tileWidth);
        const rows = Math.floor(sourceHeight / tileHeight);
        const totalTiles = cols * rows;

        if (totalTiles === 0) {
            alert('根据输入的尺寸，无法从图片中提取任何瓦片。');
            return;
        }

        // --- 4. 创建并绘制到新的Canvas ---
        const outputCanvas = document.createElement('canvas');
        outputCanvas.width = tileWidth;
        outputCanvas.height = totalTiles * tileHeight;
        const ctx = outputCanvas.getContext('2d');

        // 禁用图像平滑，保持像素感
        ctx.imageSmoothingEnabled = false;

        let currentTileIndex = 0;
        for (let y = 0; y < rows; y++) {
            for (let x = 0; x < cols; x++) {
                const sourceX = x * tileWidth;
                const sourceY = y * tileHeight;

                const destX = 0;
                const destY = currentTileIndex * tileHeight;

                // 从原图(sourceImage)上切割一块，绘制到新画布(outputCanvas)上
                ctx.drawImage(
                    this.sourceImage,
                    sourceX, sourceY,   // 源图像中的裁剪起始坐标
                    tileWidth, tileHeight, // 源图像中的裁剪尺寸
                    destX, destY,       // 目标画布上的绘制起始坐标
                    tileWidth, tileHeight  // 目标画布上的绘制尺寸
                );

                currentTileIndex++;
            }
        }

        // --- 5. 显示结果并提供下载 ---
        this.outputContainer.innerHTML = '<h4>生成结果:</h4>';
        this.outputContainer.appendChild(outputCanvas);

        this.downloadLink.href = outputCanvas.toDataURL('image/png');
        this.downloadLink.style.display = 'block';
    },

    destroy() {
        // --- 6. 清理资源 ---
        console.log(`销毁工具: ${this.name}`);
        // 移除事件监听器
        if (this.fileInput) {
            this.fileInput.removeEventListener('change', this._handleFileSelect);
        }
        if (this.processButton) {
            this.processButton.removeEventListener('click', this._processImage);
        }

        // 清空容器内容
        if (this.container) {
            this.container.innerHTML = '';
        }

        // 重置所有属性，释放引用
        this.container = null;
        this.sourceImage = null;
        this.fileInput = null;
        this.widthInput = null;
        this.heightInput = null;
        this.processButton = null;
        this.outputContainer = null;
        this.downloadLink = null;
        this._handleFileSelect = null;
        this._processImage = null;
    }
};

PixelToolApp.registerTool(tilesetLinearizerTool);
