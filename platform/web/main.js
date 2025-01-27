// 初始化终端
const terminal = new Terminal({
    fontSize: 14,
    cursorBlink: true,
    theme: {
        background: '#1E1E1E',  // 暗色背景
        foreground: '#FFFFFF'   // 白色文字
    }
});
const inputChar = Module.cwrap('input_char', null, ['number']);
const checkOutputSize = Module.cwrap('check_output_size', 'number', []);
const outputChar = Module.cwrap('output_char', 'number', []);
const machineInit = Module.cwrap('machine_init', null, ['array', 'number']);
const machineStep = Module.cwrap('machine_step', null, []);

terminal.open(document.getElementById('terminal'));

terminal.writeln('Hello! 这是一个简单的RISCV模拟器');
terminal.writeln('传入预编译的二进制文件，点击运行！');

// 处理终端输入
terminal.onData(data => {
    inputChar(data.charCodeAt());
});

// 文件上传按钮事件处理
const uploadButton = document.getElementById('upload-button');
uploadButton.addEventListener('change', (event) => {
    const file = event.target.files[0];
    if (file) {
        terminal.writeln(`已选择文件: ${file.name}`);

        const reader = new FileReader();
        reader.onload = () => {
            const arrayBuffer = reader.result;
            const uint8Array = new Uint8Array(arrayBuffer);

            // 将文件内容传递给 C 函数
            machineInit(uint8Array,uint8Array.length);
        };
        reader.readAsArrayBuffer(file);
    }
});

// 运行按钮事件处理
const runButton = document.getElementById('run-button');
runButton.addEventListener('click', () => {
    terminal.writeln('启动运行...');

    (function step() {
        machineStep();
        if(checkOutputSize()){
            data = String.fromCharCode(outputChar());
            if (data === '\n') { 
                terminal.writeln('');
            } else {
                terminal.write(data);
            }
        }
        setTimeout(step, 0);  // 立即安排下一次执行
    })();
});
