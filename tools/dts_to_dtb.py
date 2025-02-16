import subprocess

def compile_dts_to_dtb(dts_file):
    """
    使用 dtc 工具将DTS文件编译为DTB文件。
    """
    dtb_file = dts_file.replace('.dts', '.dtb')
    command = ['dtc', '-I', 'dts', '-O', 'dtb', '-o', dtb_file, dts_file]
    
    try:
        subprocess.run(command, check=True)
        print(f"成功编译: {dts_file} -> {dtb_file}")
        return dtb_file
    except subprocess.CalledProcessError as e:
        print(f"编译失败: {e}")
        return None

def convert_dtb_to_c_array(dtb_file):
    """
    将DTB文件转换为C语言数组格式。
    """
    try:
        with open(dtb_file, 'rb') as f:
            data = f.read()
            
        # 转换为C数组格式
        c_array = ', '.join(f'0x{byte:02x}' for byte in data)
        c_array = f"const unsigned char dtb[] = {{\n    {c_array}\n}};"
        return c_array
    except Exception as e:
        print(f"读取DTB文件失败: {e}")
        return None

def main(dts_file):
    # 编译DTS文件为DTB
    dtb_file = compile_dts_to_dtb(dts_file)
    if dtb_file:
        # 将DTB文件转换为C语言数组
        c_array = convert_dtb_to_c_array(dtb_file)
        if c_array:
            print("\n生成的C语言数组如下：\n")
            print(c_array)

if __name__ == "__main__":
    # 在此指定DTS文件路径
    dts_file = '/home/charain/Project/RISCV-Emulator/src/machine/spike.dts'  # 替换为你自己的文件路径
    main(dts_file)
