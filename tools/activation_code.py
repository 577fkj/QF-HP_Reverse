def generate_activation_code(input_bytes: bytes, output_mode: int = 6) -> bytes:
    if len(input_bytes) < 6:
        input_bytes = input_bytes.ljust(6, b'\x00')
    elif len(input_bytes) > 6:
        input_bytes = input_bytes[:6]
    
    buffer1 = bytearray(input_bytes)
    buffer2 = bytearray(input_bytes)
    
    for i in range(6):
        buffer2[i] ^= 0x98
    
    for i in range(6):
        buffer1[i] ^= 0x08
    
    buffer2[1] = (buffer1[5] + buffer2[0] + buffer1[0] + buffer2[1]) & 0xFF
    buffer2[2] = (buffer1[1] + buffer2[2] + buffer2[1]) & 0xFF
    buffer2[3] = (buffer1[2] + buffer2[3] + buffer2[2]) & 0xFF
    buffer2[4] = (buffer1[3] + buffer2[4] + buffer2[3]) & 0xFF
    buffer2[5] = (buffer1[4] + buffer2[5] + buffer2[4]) & 0xFF
    buffer2[0] = (buffer1[5] + buffer2[0] + buffer2[5]) & 0xFF
    
    buffer1[3] = (buffer1[3] + buffer2[2]) & 0xFF
    buffer1[2] = (buffer1[2] + buffer2[1]) & 0xFF
    buffer1[5] = (buffer1[5] + buffer2[4]) & 0xFF
    buffer1[4] = (buffer1[4] + buffer2[3]) & 0xFF
    buffer1[1] = (buffer1[1] + buffer2[0]) & 0xFF
    buffer1[0] = (buffer1[0] + buffer2[5]) & 0xFF
    
    for i in range(6):
        buffer2[i] ^= buffer1[i]
    
    buffer1 = bytearray(buffer2)
    
    for i in range(5):
        buffer2[i] ^= buffer1[i + 1]
    
    buffer2[5] ^= buffer1[0]
    
    if output_mode == 3:
        activation_code = bytearray(3)
        activation_code[0] = buffer2[0] ^ buffer2[3]
        activation_code[1] = buffer2[1] ^ buffer2[4]
        activation_code[2] = buffer2[5] ^ buffer2[2]
        return activation_code
    else:
        return buffer2


def format_hex_bytes(data: bytes) -> str:
    return '-'.join(f"{byte:02X}" for byte in data)


def parse_hex_input(input_str: str) -> bytes:
    hex_str = input_str.replace('-', '').replace(':', '')
    return bytes.fromhex(hex_str)


def main():
    try:
        input_str = input("请输入机器码: ").strip()
        mode = int(input("请选择激活码类型 (3=3字节, 其他=6字节): ") or 6)
        
        input_bytes = parse_hex_input(input_str)
        
        if len(input_bytes) != 6:
            print(f"错误：需要6字节输入，实际收到{len(input_bytes)}字节")
            return
        
        activation_code = generate_activation_code(input_bytes, mode)
    
        formatted_code = format_hex_bytes(activation_code)
        
        print("\n生成的激活码:")
        print(formatted_code)
        
    except ValueError as e:
        print(f"输入错误: {e}")
        print("请确保输入格式正确，如：00-11-22-33-44-55")


if __name__ == "__main__":
    main()