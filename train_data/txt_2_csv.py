file_path = r'd:\Edge caculation\magic_wand\train_data\train_data_other.txt'

# 读取文件内容，去掉空行
with open(file_path, 'r') as f:
    lines = f.readlines()

# 过滤掉空行并保持每行内容
cleaned_lines = [line for line in lines if line.strip()]

# 重新写入文件
with open(file_path, 'w') as f:
    f.writelines(cleaned_lines)

print("已去掉空行。")