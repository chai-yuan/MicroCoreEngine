// js/framework.js

const PixelToolApp = {
    // DOM 元素引用
    nodes: {
        nav: null,
        container: null,
    },

    // 存储所有已注册的工具
    tools: {},

    // 当前激活的工具ID
    activeToolId: null,

    /**
     * 初始化应用
     */
    init() {
        console.log("Pixel Tool App initializing...");
        this.nodes.nav = document.getElementById('tool-list');
        this.nodes.container = document.getElementById('tool-container');

        if (!this.nodes.nav || !this.nodes.container) {
            console.error("Framework init failed: missing required DOM elements.");
            return;
        }

        this.buildNav();

        // 默认加载第一个工具
        const firstToolId = Object.keys(this.tools)[0];
        if (firstToolId) {
            this.switchTool(firstToolId);
        } else {
            this.nodes.container.innerHTML = "<h2>没有可用的工具</h2><p>请在 'js/tools.js' 文件中注册工具。</p>";
        }
    },

    /**
     * 注册一个新工具
     * @param {object} tool - 工具对象，必须包含 id, name, 和 init 方法
     */
    registerTool(tool) {
        if (!tool.id || !tool.name || !tool.init) {
            console.error("Failed to register tool: missing required properties (id, name, init).", tool);
            return;
        }
        this.tools[tool.id] = tool;
        console.log(`Tool registered: ${tool.name}`);
    },

    /**
     * 根据已注册的工具构建导航栏
     */
    buildNav() {
        this.nodes.nav.innerHTML = ''; // 清空导航
        for (const toolId in this.tools) {
            const tool = this.tools[toolId];
            const li = document.createElement('li');
            const a = document.createElement('a');
            a.href = `#${tool.id}`;
            a.textContent = tool.name;
            a.dataset.toolId = tool.id;

            a.onclick = (e) => {
                e.preventDefault();
                this.switchTool(tool.id);
            };

            li.appendChild(a);
            this.nodes.nav.appendChild(li);
        }
    },

    /**
     * 切换到指定的工具
     * @param {string} toolId - 要切换到的工具的ID
     */
    switchTool(toolId) {
        if (this.activeToolId === toolId || !this.tools[toolId]) {
            return; // 如果是当前工具或工具不存在，则不执行任何操作
        }

        // 1. (可选) 销毁旧工具，用于清理定时器、事件监听等
        if (this.activeToolId && this.tools[this.activeToolId].destroy) {
            this.tools[this.activeToolId].destroy();
        }

        // 2. 清空工具容器
        this.nodes.container.innerHTML = '';

        // 3. 更新当前激活的工具ID
        this.activeToolId = toolId;

        // 4. 初始化新工具，并将容器元素传递给它
        this.tools[toolId].init(this.nodes.container);

        // 5. 更新导航栏的 active 状态
        this.updateNavActiveState();

        console.log(`Switched to tool: ${this.tools[toolId].name}`);
    },

    /**
     * 更新导航链接的 'active' 类
     */
    updateNavActiveState() {
        const links = this.nodes.nav.querySelectorAll('a');
        links.forEach(link => {
            if (link.dataset.toolId === this.activeToolId) {
                link.classList.add('active');
            } else {
                link.classList.remove('active');
            }
        });
    }
};
