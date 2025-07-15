const tilemapEditorTool = {
    id: 'tilemap-editor',
    name: '瓦片地图编辑器',

    // --- 内部状态属性 ---
    tilesetImage: null, // 原始图块集图像
    tileWidth: 16,      // 单个瓦片的宽度
    tileHeight: 16,     // 单个瓦片的高度
    mapWidthInTiles: 20, // 地图的宽度（以瓦片为单位）
    mapHeightInTiles: 15,// 地图的高度（以瓦片为单位）

    tiles: [],                // 分割后的瓦片图像数据 (ImageBitmap)
    mapData: [],              // 存储地图数据的二维数组
    currentTileIndex: 1,      // 当前选中的瓦片编号 (从1开始)
    isDrawing: false,         // 鼠标是否正在绘制状态

    // --- DOM 元素引用 ---
    elements: {},
    // --- 事件监听器引用，用于清理 ---
    eventListeners: [],


    /**
     * 初始化工具，创建UI并绑定事件
     * @param {HTMLElement} container - 工具将被注入的容器元素
     */
    init(container) {
        // 1. 构建UI的HTML结构
        container.innerHTML = `
            <div class="tool-content">
                <h2>${this.name}</h2>
                <p>导入图块集，在线绘制瓦片地图，并生成适用于C语言的数组数据。</p>

                <!-- 整个编辑器分为左右两栏，这里用一个自定义的 grid 容器包裹 -->
                <div class="tilemap-editor-grid">

                    <!-- 左侧栏：配置和瓦片选择 -->
                    <div class="tilemap-sidebar">

                        <div class="tool-section">
                            <h3>1. 配置地图</h3>
                            <div class="config-grid">
                                <div>
                                    <label for="tileset-input">图块集 (Tileset):</label>
                                    <input type="file" id="tileset-input" accept="image/*">
                                </div>
                                <div>
                                    <label for="tile-width-input">瓦片尺寸 (W x H):</label>
                                    <!-- 将W和H输入框放在一个容器中以便对齐 -->
                                    <div style="display: flex; gap: 10px;">
                                        <input type="number" id="tile-width-input" value="16" min="1" style="width: 60px;">
                                        <input type="number" id="tile-height-input" value="16" min="1" style="width: 60px;">
                                    </div>
                                </div>
                                <div>
                                    <label for="map-width-input">地图尺寸 (瓦片数):</label>
                                    <div style="display: flex; gap: 10px;">
                                        <input type="number" id="map-width-input" value="20" min="1" style="width: 60px;">
                                        <input type="number" id="map-height-input" value="15" min="1" style="width: 60px;">
                                    </div>
                                </div>
                            </div>
                            <button id="process-button" style="margin-top: 15px;">生成/重置地图</button>
                        </div>

                        <div class="tool-section">
                            <h3>2. 选择瓦片</h3>
                            <!-- 使用与预览图一致的容器和样式 -->
                            <div style="margin-top: 15px; max-width: 100%; overflow: auto;">
                                <canvas id="tile-palette-canvas" style="border: 1px solid #7f8c8d;"></canvas>
                            </div>
                            <div id="current-tile-info" style="margin-top: 10px;">当前选择: 无</div>
                        </div>

                    </div>

                    <!-- 右侧栏：地图绘制和代码导出 -->
                    <div class="tilemap-main">

                        <div class="tool-section">
                            <h3>3. 绘制地图</h3>
                            <div style="margin-top: 15px; max-width: 100%; overflow: auto;">
                                <canvas id="map-canvas" style="border: 1px solid #7f8c8d; cursor: crosshair;"></canvas>
                            </div>
                        </div>

                        <div class="tool-section">
                            <h3>4. 导出代码</h3>
                            <!-- 采用了参考代码中“复制”按钮的样式 -->
                            <button id="export-button" style="float: right;" disabled>复制到剪贴板</button>
                            <!-- 使用 textarea 替代 pre，与参考代码保持一致 -->
                            <textarea id="export-output" readonly placeholder="请先配置并绘制地图，这里将显示生成的C代码..."></textarea>
                        </div>

                    </div>
                </div>

                <!-- 你可能需要添加一些CSS来支持这个两栏布局 -->
                <style>
                    .tilemap-editor-grid {
                        display: grid;
                        grid-template-columns: 350px 1fr; /* 左侧栏固定宽度，右侧自适应 */
                        gap: 20px;
                    }
                    /* 为了和参考代码一致，textarea高度需要设置 */
                    #export-output {
                        height: 200px; /* 或者根据需要调整 */
                    }
                    /* 响应式布局：在小屏幕上变为单栏 */
                    @media (max-width: 900px) {
                        .tilemap-editor-grid {
                            grid-template-columns: 1fr;
                        }
                    }
                </style>
            </div>
        `;

        // 2. 缓存所有需要操作的DOM元素
        this.elements = {
            container,
            tilesetInput: container.querySelector('#tileset-input'),
            tileWidthInput: container.querySelector('#tile-width-input'),
            tileHeightInput: container.querySelector('#tile-height-input'),
            mapWidthInput: container.querySelector('#map-width-input'),
            mapHeightInput: container.querySelector('#map-height-input'),
            processButton: container.querySelector('#process-button'),
            paletteCanvas: container.querySelector('#tile-palette-canvas'),
            paletteCtx: container.querySelector('#tile-palette-canvas').getContext('2d'),
            currentTileInfo: container.querySelector('#current-tile-info'),
            mapCanvas: container.querySelector('#map-canvas'),
            mapCtx: container.querySelector('#map-canvas').getContext('2d'),
            exportButton: container.querySelector('#export-button'),
            exportOutput: container.querySelector('#export-output'),
        };

        // 3. 绑定事件监听器
        this._bindEvents();
    },

    /**
     * 绑定所有事件
     * @private
     */
    _bindEvents() {
        const { processButton, paletteCanvas, mapCanvas, exportButton } = this.elements;

        const handlers = [
            { el: processButton, type: 'click', handler: this._processTileset.bind(this) },
            { el: paletteCanvas, type: 'click', handler: this._selectTile.bind(this) },
            { el: mapCanvas,     type: 'mousedown', handler: this._startDrawing.bind(this) },
            { el: mapCanvas,     type: 'mousemove', handler: this._draw.bind(this) },
            { el: mapCanvas,     type: 'mouseup', handler: this._stopDrawing.bind(this) },
            { el: mapCanvas,     type: 'mouseleave', handler: this._stopDrawing.bind(this) },
            { el: exportButton,  type: 'click', handler: this._exportMapData.bind(this) },
        ];

        handlers.forEach(({ el, type, handler }) => {
            el.addEventListener(type, handler);
            this.eventListeners.push({ el, type, handler }); // 保存引用，以便销毁时移除
        });
    },

    /**
     * 处理图块集，分割并设置画布
     * @private
     */
    _processTileset() {
        const { tilesetInput, tileWidthInput, tileHeightInput, mapWidthInput, mapHeightInput, exportButton } = this.elements;

        const file = tilesetInput.files[0];
        if (!file) {
            alert('请先选择一个图块集图片！');
            return;
        }

        this.tileWidth = parseInt(tileWidthInput.value, 10);
        this.tileHeight = parseInt(tileHeightInput.value, 10);
        this.mapWidthInTiles = parseInt(mapWidthInput.value, 10);
        this.mapHeightInTiles = parseInt(mapHeightInput.value, 10);

        if (!(this.tileWidth > 0 && this.tileHeight > 0 && this.mapWidthInTiles > 0 && this.mapHeightInTiles > 0)) {
            alert('所有尺寸值都必须大于0！');
            return;
        }

        const reader = new FileReader();
        reader.onload = e => {
            const img = new Image();
            img.onload = () => {
                this.tilesetImage = img;
                this._sliceAndDisplayTiles();
                this._setupMapCanvas();
                exportButton.disabled = false;
            };
            img.src = e.target.result;
        };
        reader.readAsDataURL(file);
    },

    /**
     * 分割图块集并显示在瓦片选择区
     * @private
     */
    async _sliceAndDisplayTiles() {
        const { paletteCanvas, paletteCtx } = this.elements;
        const img = this.tilesetImage;

        if (img.width % this.tileWidth !== 0 || img.height % this.tileHeight !== 0) {
            console.warn('警告: 图片尺寸不能被瓦片尺寸整除，部分图片区域可能被忽略。');
        }

        const cols = Math.floor(img.width / this.tileWidth);
        const rows = Math.floor(img.height / this.tileHeight);

        this.tiles = [];
        paletteCanvas.width = img.width;
        paletteCanvas.height = img.height;
        paletteCtx.clearRect(0, 0, paletteCanvas.width, paletteCanvas.height);

        let tileNumber = 1;
        for (let y = 0; y < rows; y++) {
            for (let x = 0; x < cols; x++) {
                const tileX = x * this.tileWidth;
                const tileY = y * this.tileHeight;
                // 使用ImageBitmap以获得更好的性能
                const tileBitmap = await createImageBitmap(img, tileX, tileY, this.tileWidth, this.tileHeight);
                this.tiles.push(tileBitmap);

                // 在调色板上绘制瓦片和编号
                paletteCtx.drawImage(tileBitmap, tileX, tileY);
                paletteCtx.strokeStyle = 'rgba(0,0,0,0.5)';
                paletteCtx.strokeRect(tileX, tileY, this.tileWidth, this.tileHeight);
                paletteCtx.fillStyle = 'white';
                paletteCtx.font = '10px sans-serif';
                paletteCtx.fillText(tileNumber++, tileX + 2, tileY + 10);
            }
        }
        this._selectTileByIndex(1); // 默认选中第一个瓦片
    },

    /**
     * 初始化地图绘制画布
     * @private
     */
    _setupMapCanvas() {
        const { mapCanvas, mapCtx } = this.elements;
        mapCanvas.width = this.mapWidthInTiles * this.tileWidth;
        mapCanvas.height = this.mapHeightInTiles * this.tileHeight;

        // 初始化地图数据为0 (0代表空)
        this.mapData = Array.from({ length: this.mapHeightInTiles }, () =>
            new Array(this.mapWidthInTiles).fill(0)
        );

        // 绘制网格
        mapCtx.clearRect(0, 0, mapCanvas.width, mapCanvas.height);
        mapCtx.strokeStyle = '#ccc';
        for (let x = 0; x <= mapCanvas.width; x += this.tileWidth) {
            mapCtx.beginPath();
            mapCtx.moveTo(x, 0);
            mapCtx.lineTo(x, mapCanvas.height);
            mapCtx.stroke();
        }
        for (let y = 0; y <= mapCanvas.height; y += this.tileHeight) {
            mapCtx.beginPath();
            mapCtx.moveTo(0, y);
            mapCtx.lineTo(mapCanvas.width, y);
            mapCtx.stroke();
        }
    },

    /**
     * 根据点击位置选择瓦片
     * @private
     */
    _selectTile(e) {
        const { paletteCanvas } = this.elements;
        const rect = paletteCanvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        const cols = Math.floor(this.tilesetImage.width / this.tileWidth);
        const tileX = Math.floor(x / this.tileWidth);
        const tileY = Math.floor(y / this.tileHeight);

        const index = tileY * cols + tileX + 1;

        if (index > 0 && index <= this.tiles.length) {
           this._selectTileByIndex(index);
        }
    },

    _selectTileByIndex(index) {
        this.currentTileIndex = index;
        const { currentTileInfo, paletteCtx } = this.elements;

        // 更新提示信息
        currentTileInfo.innerHTML = `当前选择: <b>${this.currentTileIndex}</b> 号瓦片`;

        // 重绘调色板以更新高亮
        this._redrawPaletteHighlight();
    },

    _redrawPaletteHighlight() {
        const { paletteCanvas, paletteCtx } = this.elements;
        const img = this.tilesetImage;
        const cols = Math.floor(img.width / this.tileWidth);

        // 先重绘整个调色板
        paletteCtx.clearRect(0, 0, paletteCanvas.width, paletteCanvas.height);
        this.tiles.forEach((tileBitmap, i) => {
            const x = (i % cols) * this.tileWidth;
            const y = Math.floor(i / cols) * this.tileHeight;
            paletteCtx.drawImage(tileBitmap, x, y);
            paletteCtx.strokeStyle = 'rgba(0,0,0,0.5)';
            paletteCtx.strokeRect(x, y, this.tileWidth, this.tileHeight);
            paletteCtx.fillStyle = 'white';
            paletteCtx.font = '10px sans-serif';
            paletteCtx.fillText(i + 1, x + 2, y + 10);
        });

        // 绘制高亮框
        const selectedIndex = this.currentTileIndex - 1;
        const x = (selectedIndex % cols) * this.tileWidth;
        const y = Math.floor(selectedIndex / cols) * this.tileHeight;
        paletteCtx.strokeStyle = 'red';
        paletteCtx.lineWidth = 2;
        paletteCtx.strokeRect(x + 1, y + 1, this.tileWidth - 2, this.tileHeight - 2);
        paletteCtx.lineWidth = 1; // 恢复默认值
    },

    _startDrawing(e) {
        this.isDrawing = true;
        this._draw(e); // 立即绘制一格，以支持单击绘制
    },

    _stopDrawing() {
        this.isDrawing = false;
    },

    /**
     * 在地图上绘制一个瓦片
     * @private
     */
    _draw(e) {
        if (!this.isDrawing || !this.tiles.length) return;

        const { mapCanvas, mapCtx } = this.elements;
        const rect = mapCanvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        const gridX = Math.floor(x / this.tileWidth);
        const gridY = Math.floor(y / this.tileHeight);

        // 检查是否在画布边界内
        if (gridX < 0 || gridX >= this.mapWidthInTiles || gridY < 0 || gridY >= this.mapHeightInTiles) {
            return;
        }

        // 如果该位置的瓦片没有变化，则不重绘，优化性能
        if (this.mapData[gridY][gridX] === this.currentTileIndex) {
            return;
        }

        // 更新地图数据
        this.mapData[gridY][gridX] = this.currentTileIndex;

        // 绘制瓦片
        const tileToDraw = this.tiles[this.currentTileIndex - 1];
        if (tileToDraw) {
            const canvasX = gridX * this.tileWidth;
            const canvasY = gridY * this.tileHeight;
            // 先清除这个格子，再绘制新瓦片和网格线
            mapCtx.clearRect(canvasX, canvasY, this.tileWidth, this.tileHeight);
            mapCtx.drawImage(tileToDraw, canvasX, canvasY);
            mapCtx.strokeStyle = '#ccc';
            mapCtx.strokeRect(canvasX, canvasY, this.tileWidth, this.tileHeight);
        }
    },

    /**
     * 导出地图数据为C语言数组格式
     * @private
     */
    _exportMapData() {
        if (!this.mapData.length) {
            this.elements.exportOutput.textContent = '没有可导出的数据。';
            return;
        }

        let c_array = `// 地图尺寸: ${this.mapWidthInTiles} x ${this.mapHeightInTiles}\n`;
        c_array += `int mapData[${this.mapHeightInTiles}][${this.mapWidthInTiles}] = {\n`;

        for (let y = 0; y < this.mapHeightInTiles; y++) {
            c_array += '    { ';
            c_array += this.mapData[y].join(', ');
            c_array += ' }';
            if (y < this.mapHeightInTiles - 1) {
                c_array += ',\n';
            }
        }

        c_array += '\n};';
        this.elements.exportOutput.textContent = c_array;
    },

    /**
     * 销毁工具，清理资源和事件监听器
     */
    destroy() {
        // 移除所有事件监听器
        this.eventListeners.forEach(({ el, type, handler }) => {
            el.removeEventListener(type, handler);
        });
        this.eventListeners = [];

        // 清理对象引用，帮助垃圾回收
        this.tilesetImage = null;
        this.tiles.forEach(tile => tile.close()); // 关闭ImageBitmap
        this.tiles = [];
        this.mapData = [];
        this.elements = {};

        // 清空容器内容
        const container = document.getElementById(this.id);
        if (container) container.innerHTML = '';

        console.log(`工具 '${this.name}' 已销毁。`);
    }
};

// 注册这个新工具
PixelToolApp.registerTool(tilemapEditorTool);
