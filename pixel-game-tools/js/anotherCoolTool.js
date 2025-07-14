/**
 * =================================================================
 * 工具: 占位符 (如何添加新工具)
 * Placeholder for a new tool
 * =================================================================
 */

const anotherCoolTool = {
    id: 'another-tool',
    name: '另一个很酷的工具',

    init(container) {
        container.innerHTML = `
            <div class="tool-content">
                <h2>${this.name}</h2>
                <p>这是一个新工具的模板。你可以在这里构建它的UI和功能。</p>
                <p>例如，一个调色板提取器，或是一个精灵动画预览器。</p>
            </div>
        `;
    },

    destroy() {
        // 清理这个工具的资源
    }
};

PixelToolApp.registerTool(anotherCoolTool);
