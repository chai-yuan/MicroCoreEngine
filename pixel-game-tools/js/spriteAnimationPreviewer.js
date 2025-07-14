const spriteAnimationPreviewer = {
    id: 'sprite-animation-previewer',
    name: '精灵动画预览器',

    // --- 私有属性，用于存储工具状态 ---
    image: null,
    animationFrameId: null,
    lastTime: 0,
    currentFrame: 0,
    cols: 0, // 新增：用于存储精灵图的列数
    totalFrames: 0, // 新增：用于存储总帧数

    // --- DOM元素引用 ---
    fileInput: null,
    widthInput: null,
    heightInput: null,
    fpsInput: null,
    canvas: null,
    ctx: null,
    statusMessage: null,

    init(container) {
        // 1. 构建UI界面 (UI部分无需改动)
        container.innerHTML = `
            <div class="tool-content">
                <h2>${this.name}</h2>
                <p>上传一张包含连续帧的精灵图（spritesheet），然后设置单个精灵的尺寸以预览动画。</p>

                <div class="controls" style="display: flex; flex-direction: column; gap: 10px; margin-bottom: 20px;">
                    <div>
                        <label for="sprite-file-input">上传精灵图: </label>
                        <input type="file" id="sprite-file-input" accept="image/*">
                    </div>
                    <div>
                        <label for="sprite-width-input">精灵宽度 (px): </label>
                        <input type="number" id="sprite-width-input" value="32" min="1">
                    </div>
                    <div>
                        <label for="sprite-height-input">精灵高度 (px): </label>
                        <input type="number" id="sprite-height-input" value="32" min="1">
                    </div>
                    <div>
                        <label for="sprite-fps-input">动画帧率 (FPS): </label>
                        <input type="number" id="sprite-fps-input" value="10" min="1">
                    </div>
                </div>

                <div class="preview-area">
                    <h3>预览</h3>
                    <canvas id="sprite-preview-canvas" style="border: 1px solid #ccc; background-color: #f0f0f0; image-rendering: pixelated; image-rendering: -moz-crisp-edges; image-rendering: crisp-edges;"></canvas>
                    <p id="sprite-status-message" style="color: #666; font-size: 14px; min-height: 20px;">请上传一张精灵图。</p>
                </div>
            </div>
        `;

        // 2. 获取DOM元素引用并存储
        this.fileInput = container.querySelector('#sprite-file-input');
        this.widthInput = container.querySelector('#sprite-width-input');
        this.heightInput = container.querySelector('#sprite-height-input');
        this.fpsInput = container.querySelector('#sprite-fps-input');
        this.canvas = container.querySelector('#sprite-preview-canvas');
        this.statusMessage = container.querySelector('#sprite-status-message');
        this.ctx = this.canvas.getContext('2d');
        // 防止图像缩放时模糊
        this.ctx.imageSmoothingEnabled = false;

        // 3. 绑定事件监听器
        this.fileInput.addEventListener('change', this.handleFileChange.bind(this));
        this.widthInput.addEventListener('input', this.handleSettingsChange.bind(this));
        this.heightInput.addEventListener('input', this.handleSettingsChange.bind(this));
        this.fpsInput.addEventListener('input', this.handleSettingsChange.bind(this));
    },

    handleFileChange(event) {
        const file = event.target.files[0];
        if (!file) {
            this.image = null;
            this.updateStatus('请上传一张精灵图。');
            this.stopAnimation();
            return;
        }

        const reader = new FileReader();
        reader.onload = (e) => {
            const img = new Image();
            img.onload = () => {
                this.image = img;
                this.updateStatus(''); // 清空状态信息
                this.startOrUpdateAnimation();
            };
            img.onerror = () => {
                this.image = null;
                this.updateStatus('错误：无法加载图片文件。');
                this.stopAnimation();
            };
            img.src = e.target.result;
        };
        reader.readAsDataURL(file);
    },

    handleSettingsChange() {
        if (this.image) {
            this.startOrUpdateAnimation();
        }
    },

    startOrUpdateAnimation() {
        this.stopAnimation(); // 停止任何正在运行的动画

        const spriteWidth = parseInt(this.widthInput.value, 10);
        const spriteHeight = parseInt(this.heightInput.value, 10);

        if (!this.image || !spriteWidth || !spriteHeight || spriteWidth <= 0 || spriteHeight <= 0) {
            this.updateStatus('请输入有效的精灵尺寸。');
            return;
        }

        // 【改进】计算行列数和总帧数
        const cols = Math.floor(this.image.width / spriteWidth);
        const rows = Math.floor(this.image.height / spriteHeight);
        const totalFrames = cols * rows;

        if (totalFrames === 0) {
            this.updateStatus('错误：图片尺寸小于指定的精灵尺寸。');
            return;
        }

        // 将计算结果存入实例属性，方便animate函数使用
        this.cols = cols;
        this.totalFrames = totalFrames;

        // 更新Canvas尺寸以匹配单个精灵
        this.canvas.width = spriteWidth;
        this.canvas.height = spriteHeight;
        this.ctx.imageSmoothingEnabled = false; // 每次调整尺寸后重设

        // 【改进】更新状态信息，显示网格布局
        this.updateStatus(`共找到 ${totalFrames} 帧 (${cols} x ${rows} 网格)。正在播放...`);

        // 重置动画状态并开始新的循环
        this.currentFrame = 0;
        this.lastTime = 0;
        this.animationFrameId = requestAnimationFrame(this.animate.bind(this));
    },

    animate(timestamp) {
        const fps = parseInt(this.fpsInput.value, 10) || 10;
        const frameInterval = 1000 / fps;

        if (!this.lastTime) {
            this.lastTime = timestamp;
        }

        const deltaTime = timestamp - this.lastTime;

        if (deltaTime > frameInterval) {
            this.lastTime = timestamp - (deltaTime % frameInterval);

            const spriteWidth = this.canvas.width;
            const spriteHeight = this.canvas.height;

            // 【核心改进】根据当前帧号计算在精灵图网格中的行和列
            // 列索引: 当前帧号 % 总列数
            const colIndex = this.currentFrame % this.cols;
            // 行索引: Math.floor(当前帧号 / 总列数)
            const rowIndex = Math.floor(this.currentFrame / this.cols);

            // 根据行列索引计算在源图片上的裁剪位置 (sx, sy)
            const sourceX = colIndex * spriteWidth;
            const sourceY = rowIndex * spriteHeight;

            // 清除画布
            this.ctx.clearRect(0, 0, spriteWidth, spriteHeight);

            // 绘制当前帧
            // drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight)
            this.ctx.drawImage(
                this.image,
                sourceX, sourceY, spriteWidth, spriteHeight, // 源图像的裁剪区域
                0, 0, spriteWidth, spriteHeight             // 在Canvas上的绘制区域
            );

            // 移动到下一帧，并用总帧数循环
            this.currentFrame = (this.currentFrame + 1) % this.totalFrames;
        }

        // 请求下一帧动画
        this.animationFrameId = requestAnimationFrame(this.animate.bind(this));
    },

    stopAnimation() {
        if (this.animationFrameId) {
            cancelAnimationFrame(this.animationFrameId);
            this.animationFrameId = null;
        }
    },

    updateStatus(message) {
        if(this.statusMessage) {
            this.statusMessage.textContent = message;
        }
    },

    destroy() {
        // 清理这个工具的资源，防止内存泄漏
        this.stopAnimation();

        // 清理对象引用
        this.image = null;
        this.fileInput = null;
        this.widthInput = null;
        this.heightInput = null;
        this.fpsInput = null;
        this.canvas = null;
        this.ctx = null;
        this.statusMessage = null;

        console.log(`工具 "${this.name}" 已销毁。`);
    }
};

PixelToolApp.registerTool(spriteAnimationPreviewer);
